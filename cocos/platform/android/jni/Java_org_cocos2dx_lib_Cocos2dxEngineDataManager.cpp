/****************************************************************************
Copyright (c) 2016 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxEngineDataManager.h"
#include "platform/android/jni/JniHelper.h"
#include "platform/CCFileUtils.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"
#include "renderer/CCRenderer.h"
#include "2d/CCParticleSystem.h"
#include "2d/CCActionManager.h"
#include "audio/include/AudioEngine.h"
#include "json/document.h"

#include <android/log.h>

#define LOG_TAG    "EngineDataManager.cpp"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define EDM_DEBUG 1

#define JNI_FUNC_PREFIX(func) Java_org_cocos2dx_lib_Cocos2dxEngineDataManager_##func

using namespace cocos2d;

typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonValue;

namespace {

const char* _className = "org/cocos2dx/lib/Cocos2dxEngineDataManager";

bool _isInitialized = false;
bool _isSupported = false;
bool _isFirstSetNextScene = true;

/* last time frame lost cycle was calculated */
std::chrono::steady_clock::time_point _lastContinuousFrameLostUpdate;
std::chrono::steady_clock::time_point _lastFrameLost100msUpdate;

/* last time low fps cycle was calculated */
std::chrono::steady_clock::time_point _lastLowFpsUpdate;

int _continuousFrameLostCycle = 5000;
int _continuousFrameLostThreshold = 3;
int _continuousFrameLostCount = 0;
int _frameLostCounter = 0;

int _lowFpsCycle = 1000;
float _lowFpsThreshold = 0.3f;
int _lowFpsCounter = 0;

int _oldCpuLevel = -1;
int _oldGpuLevel = -1;

#define CARRAY_SIZE(arr) ((int)(arr.size()))

// CPU Level

struct CpuLevelInfo
{
    unsigned int nodeCount;
    unsigned int particleCount;
    unsigned int actionCount;
    unsigned int audioCount;
};

std::vector<CpuLevelInfo> _cpuLevelArr = {
    {500 , 500,  500,   6},
    {1250, 1500, 2000,  20},
    {1750, 2000, 3000,  32},
    {2750, 2500, 7000,  50},
    {4000, 3500, 10000, 80}
};

// GPU Level

struct GpuLevelInfo
{
    unsigned int vertexCount;
    unsigned int drawCount;
};

std::vector<GpuLevelInfo> _gpuLevelArr = {
    {2000, 400},
    {4000, 800},
    {6000, 1000},
    {8000, 1100},
    {10000, 1200},
    {15000, 1300},
    {22000, 1350},
    {30000, 1400},
    {40000, 1450}
};

// Particle Level
const std::vector<float> _particleLevelArr = {
    0.0f,
    0.2f,
    0.4f,
    0.6f,
    0.8f,
    1.0f
};

#if EDM_DEBUG
float _oldCpuLevelNode = 0;
float _oldCpuLevelParticle = 0;
float _oldCpuLevelAction = 0;
float _oldCpuLevelAudio = 0;

float _oldGpuLevelVertex = 0;
float _oldGpuLevelDraw = 0;
#endif

int cbCpuLevelNode(int i) { return _cpuLevelArr[i].nodeCount; }
int cbCpuLevelParticle(int i) { return _cpuLevelArr[i].particleCount; }
int cbCpuLevelAction(int i) { return _cpuLevelArr[i].actionCount; }
int cbCpuLevelAudio(int i) { return _cpuLevelArr[i].audioCount; }

float toCpuLevelPerFactor(int value, int (*cb)(int i))
{
    int len = CARRAY_SIZE(_cpuLevelArr);
    int prevStep = 0;
    int curStep = 0;
    for (int i = 0; i < len; ++i)
    {
        curStep = cb(i);
        if (value < curStep)
        {
            // The return value should be a float value.
            // Do linear interpolation here
            return i + (1.0f / (curStep - prevStep) * (value - prevStep));
        }
        prevStep = curStep;
    }
    return len;
}

int toCpuLevel(int nodeCount, int particleCount, int actionCount, int audioCount)
{
    float cpuLevelNode = toCpuLevelPerFactor(nodeCount, cbCpuLevelNode);
    float cpuLevelParticle = toCpuLevelPerFactor(particleCount, cbCpuLevelParticle);
    float cpuLevelAction = toCpuLevelPerFactor(actionCount, cbCpuLevelAction);
    float cpuLevelAudio = toCpuLevelPerFactor(audioCount, cbCpuLevelAudio);
    int cpuLevel = std::floor(cpuLevelNode + cpuLevelParticle + cpuLevelAction + cpuLevelAudio);
    cpuLevel = std::min(cpuLevel, CARRAY_SIZE(_cpuLevelArr));

#if EDM_DEBUG
    if (_oldCpuLevel != cpuLevel
        || _oldCpuLevelNode - cpuLevelNode > 1.0f
        || _oldCpuLevelParticle - cpuLevelParticle > 1.0f
        || _oldCpuLevelAction - cpuLevelAction > 1.0f
        || _oldCpuLevelAudio - cpuLevelAudio > 1.0f)
    {
        log("cpu level: %d, node: (%f, %d), particle: (%f, %d), action: (%f, %d), audio: (%f, %d)", 
            cpuLevel, cpuLevelNode, nodeCount, cpuLevelParticle, particleCount, cpuLevelAction, actionCount, cpuLevelAudio, audioCount);
        _oldCpuLevelNode = cpuLevelNode;
        _oldCpuLevelParticle = cpuLevelParticle;
        _oldCpuLevelAction = cpuLevelAction;
        _oldCpuLevelAudio = cpuLevelAudio;
    }
#endif
    return cpuLevel;
}

int cbGpuLevelVertex(int i) { return _gpuLevelArr[i].vertexCount; }
int cbGpuLevelDraw(int i) { return _gpuLevelArr[i].drawCount; }

float toGpuLevelPerFactor(int value, int (*cb)(int i))
{
    int len = CARRAY_SIZE(_gpuLevelArr);
    int prevStep = 0;
    int curStep = 0;

    for (int i = 0; i < len; ++i)
    {
        curStep = cb(i);
        if (value < curStep)
        {
            // The return value should be a float value.
            // Do linear interpolation here
            return i + (1.0f / (curStep - prevStep) * (value - prevStep));
        }

        prevStep = curStep;

    }
    return len;
}

int toGpuLevel(int vertexCount, int drawCount)
{
    float gpuLevelVertex = toGpuLevelPerFactor(vertexCount, cbGpuLevelVertex);
    float gpuLevelDraw = toGpuLevelPerFactor(drawCount, cbGpuLevelDraw);
    int gpuLevel = std::floor(gpuLevelVertex + gpuLevelDraw);
    gpuLevel = std::min(gpuLevel, CARRAY_SIZE(_gpuLevelArr));

#if EDM_DEBUG
    if (_oldGpuLevel != gpuLevel
        || _oldGpuLevelVertex - gpuLevelVertex > 1.0f
        || _oldGpuLevelDraw - gpuLevelDraw > 1.0f)
    {
        log("gpu level: %d, vertex: (%f, %d), draw: (%f, %d)", gpuLevel, gpuLevelVertex, vertexCount, gpuLevelDraw, drawCount);
        _oldGpuLevelVertex = gpuLevelVertex;
        _oldGpuLevelDraw = gpuLevelDraw;
    }
#endif
    return gpuLevel;
}

void resetLastTime()
{
    _lastFrameLost100msUpdate = std::chrono::steady_clock::now();
    _lastContinuousFrameLostUpdate = _lastFrameLost100msUpdate;
    _lastLowFpsUpdate = _lastFrameLost100msUpdate;
}

void parseDebugConfig()
{
#if EDM_DEBUG
    auto fileUtils = FileUtils::getInstance();

    const char* configPath = "/sdcard/cc-res-level.json";

    if (!fileUtils->isFileExist(configPath))
    {
        return;
    }

    LOGD("Using debug level config: %s", configPath);
    std::string resLevelConfig = fileUtils->getStringFromFile(configPath);

    RapidJsonDocument document;
    document.Parse<0>(resLevelConfig.c_str());
    
    {
        assert(document.HasMember("cpu_level"));
        const RapidJsonValue& cpu = document["cpu_level"];
        assert(cpu.IsArray());
        assert(_cpuLevelArr.size() == cpu.Size());

        _cpuLevelArr.clear();
        CpuLevelInfo cpuLevelInfo;
        for (auto i = 0; i < cpu.Size(); ++i)
        {
            assert(cpu[i].IsObject());

            cpuLevelInfo.nodeCount = cpu[i]["node"].GetUint();
            cpuLevelInfo.particleCount = cpu[i]["particle"].GetUint();
            cpuLevelInfo.actionCount = cpu[i]["action"].GetUint();
            cpuLevelInfo.audioCount = cpu[i]["audio"].GetUint();
            
            _cpuLevelArr.push_back(cpuLevelInfo);
        }
    }
    
    {
        assert(document.HasMember("gpu_level"));

        const RapidJsonValue& gpu = document["gpu_level"];
        assert(gpu.IsArray());
        assert(_gpuLevelArr.size() == gpu.Size());
        
        _gpuLevelArr.clear();
        GpuLevelInfo gpuLevelInfo;
        for (auto i = 0; i < gpu.Size(); ++i)
        {
            assert(gpu[i].IsObject());
            
            gpuLevelInfo.vertexCount = gpu[i]["vertex"].GetUint();
            gpuLevelInfo.drawCount = gpu[i]["draw"].GetUint();
            
            _gpuLevelArr.push_back(gpuLevelInfo);
        }
    }
    
    LOGD("-----------------------------------------");
    for (auto&& level : _cpuLevelArr)
    {
        LOGD("cpu level: %u, %u, %u, %u", level.nodeCount, level.particleCount, level.actionCount, level.audioCount);
    }
    LOGD("-----------------------------------------");
    
    LOGD("=========================================");
    for (auto&& level : _gpuLevelArr)
    {
        LOGD("gpu level: %u, %u", level.vertexCount, level.drawCount);
    }
    LOGD("=========================================");
#endif // EDM_DEBUG
}

} // namespace {

namespace cocos2d {

int EngineDataManager::getTotalParticleCount()
{
    auto& particleSystems = ParticleSystem::getAllParticleSystems();
    if (particleSystems.empty())
    {
        return 0;
    }

    int count = 0;
    for (auto&& system : particleSystems)
    {
        count += system->getTotalParticles();
    }

    return count;
}

// calculates frame lost event
// static
void EngineDataManager::calculateFrameLost()
{
    auto director = Director::getInstance();

    if (_lowFpsThreshold > 0 && _continuousFrameLostThreshold > 0)
    {
        float animationInterval = director->getAnimationInterval();
        float frameRate = director->getFrameRate();

        float expectedFPS = 1.0f / animationInterval;
        float frameLostRate = (expectedFPS - frameRate) * animationInterval;
        if (frameLostRate > _lowFpsThreshold)
        {
            ++_frameLostCounter;
            ++_lowFpsCounter;
//            log("_frameLostCounter: %d, _lowFpsCounter=%d", _frameLostCounter, _lowFpsCounter);
        }
        
        auto now = std::chrono::steady_clock::now();
        float interval = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastFrameLost100msUpdate).count() / 1000000.0f;
        if (interval > 0.1f)
        {
            _lastFrameLost100msUpdate = now;
            // check lost frame count
            if (_frameLostCounter > _continuousFrameLostThreshold)
            {
                _frameLostCounter = 0;
                ++_continuousFrameLostCount;
            }
        }
        
        interval = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastContinuousFrameLostUpdate).count() / 1000000.0f;
        if (interval > (_continuousFrameLostCycle / 1000.0f))
        {
            _lastContinuousFrameLostUpdate = now;
            if (_continuousFrameLostCount > 0)
            {
                // notify continuous frame lost event to system
                notifyContinuousFrameLost(_continuousFrameLostCycle, _continuousFrameLostThreshold, _continuousFrameLostCount);

                log("continuous frame lost: %d", _continuousFrameLostCount);
                _continuousFrameLostCount = 0;
            }
        }
        
        interval = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastLowFpsUpdate).count() / 1000000.0f;
        if (interval > (_lowFpsCycle / 1000.0f))
        {
            _lastLowFpsUpdate = now;
            if (_lowFpsCounter > 0)
            {
                // notify low fps event to system
                notifyLowFps(_lowFpsCycle, _lowFpsThreshold, _lowFpsCounter);
                log("low fps frame count: %d", _lowFpsCounter);
                _lowFpsCounter = 0;
            }
        }
    }
}

void EngineDataManager::notifyGameStatusIfCpuOrGpuLevelChanged()
{
    // calculate CPU & GPU levels
    int cpuLevel = 0;
    int gpuLevel = 0;
    getCpuAndGpuLevel(&cpuLevel, &gpuLevel);

    if (cpuLevel != _oldCpuLevel || gpuLevel != _oldGpuLevel)
    {
        notifyGameStatus(GameStatus::IN_SCENE, cpuLevel, gpuLevel);

        _oldCpuLevel = cpuLevel;
        _oldGpuLevel = gpuLevel;
    }
}

// static
void EngineDataManager::onAfterDrawScene(EventCustom* event)
{
    calculateFrameLost();

    notifyGameStatusIfCpuOrGpuLevelChanged();
}

void EngineDataManager::getCpuAndGpuLevel(int* cpuLevel, int* gpuLevel)
{
    if (cpuLevel == nullptr || gpuLevel == nullptr)
        return;

    auto director = Director::getInstance();
    int totalNodeCount = Node::getAttachedNodeCount();
    int totalParticleCount = getTotalParticleCount();
    int totalActionCount = director->getActionManager()->getNumberOfRunningActions();
    int totalPlayingAudioCount = experimental::AudioEngine::getPlayingAudioCount();
    *cpuLevel = toCpuLevel(totalNodeCount, totalParticleCount, totalActionCount, totalPlayingAudioCount);

    auto renderer = director->getRenderer();
    int vertexCount = renderer->getDrawnVertices();
    int batchedCount = renderer->getDrawnBatches();

    *gpuLevel = toGpuLevel(vertexCount, batchedCount);
}

// static
void EngineDataManager::onEnterForeground(EventCustom* event)
{
    // It will not trigger `enter foreground` event when game startup in Cocos2d-x 3.5.
    // So the following check isn't needed.
    // static bool isFirstTime = true;
    // LOGD("onEnterForeground, isFirstTime: %d", isFirstTime);

    // if (isFirstTime)
    // {
    //     isFirstTime = false;
    // }
    // else
    {
        resetLastTime();
        // Reset the old status
        _oldCpuLevel = -1;
        _oldGpuLevel = -1;
        // Notify CPU/GPU level to system since old levels have been changed.
        notifyGameStatusIfCpuOrGpuLevelChanged();  
    }
}

void EngineDataManager::onCreateScene(EventCustom* event)
{
    if (_isFirstSetNextScene)
    {// If it's the first time of setting next scene, a 'START_SCENE' event has been already triggered.
     // So don't notify 'SCENE_CHANGE' event to system.
        _isFirstSetNextScene = false;
    }
    else
    {
        // Reset the old status since we have changed CPU/GPU level manually.
        // If the CPU level isn't 5 and GPU level isn't 0 in the next time of checking CPU/GPU level,
        // Make sure that the changed CPU/GPU level will be notified.
        _oldCpuLevel = -1;
        _oldGpuLevel = -1;

        notifyGameStatus(GameStatus::SCENE_CHANGE, 5, 0);
    }
}

// static
void EngineDataManager::init()
{
    if (!_isSupported)
        return;

    if (_isInitialized)
        return;

    resetLastTime();

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener(Director::EVENT_AFTER_DRAW, std::bind(onAfterDrawScene, std::placeholders::_1));
    dispatcher->addCustomEventListener(EVENT_COME_TO_FOREGROUND, std::bind(onEnterForeground, std::placeholders::_1));

    dispatcher->addCustomEventListener(EVENT_CREATE_SCENE, std::bind(onCreateScene, std::placeholders::_1));

    notifyGameStatus(GameStatus::START, 5, -1);

    parseDebugConfig();

    _isInitialized = true;
}

// static 
void EngineDataManager::destroy()
{
    if (!_isSupported)
        return;
}

// static
void EngineDataManager::notifyGameStatus(GameStatus type, int cpuLevel, int gpuLevel)
{
    if (!_isSupported)
        return;

    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, _className, "notifyGameStatus", "(III)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)type, cpuLevel, gpuLevel);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

// static
void EngineDataManager::notifyContinuousFrameLost(int continueFrameLostCycle, int continueFrameLostThreshold, int times)
{
    if (!_isSupported)
        return;

    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, _className, "notifyContinuousFrameLost", "(III)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, continueFrameLostCycle, continueFrameLostThreshold, times);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

// static
void EngineDataManager::notifyLowFps(int lowFpsCycle, float lowFpsThreshold, int frames)
{
    if (!_isSupported)
        return;

    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, _className, "notifyLowFps", "(IFI)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, lowFpsCycle, lowFpsThreshold, frames);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void EngineDataManager::nativeOnQueryFps(JNIEnv* env, jobject thiz, jintArray arrExpectedFps, jintArray arrRealFps)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnQueryFps");
    jsize arrLenExpectedFps = env->GetArrayLength(arrExpectedFps);
    jsize arrLenRealFps = env->GetArrayLength(arrRealFps);

    if (arrLenExpectedFps > 0 && arrLenRealFps > 0)
    {
        jboolean isCopy = JNI_FALSE;
        jint* expectedFps = env->GetIntArrayElements(arrExpectedFps, &isCopy);
        *expectedFps = (int)std::ceil(1.0f / cocos2d::Director::getInstance()->getAnimationInterval());
        env->ReleaseIntArrayElements(arrExpectedFps, expectedFps, 0);

        jint* realFps = env->GetIntArrayElements(arrRealFps, &isCopy);
        *realFps = (int)cocos2d::Director::getInstance()->getFrameRate();
        env->ReleaseIntArrayElements(arrRealFps, realFps, 0);
    }
    else
    {
        LOGE("Invalid array size, expectedFps.size=%d, realFps.size=%d", arrLenExpectedFps, arrLenRealFps);
    }
}

void EngineDataManager::nativeOnChangeContinuousFrameLostConfig(JNIEnv* env, jobject thiz, jint continueFrameLostCycle, jint continueFrameLostThreshold)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeContinuousFrameLostConfig, continueFrameLostCycle: %d, continueFrameLostThreshold: %d", continueFrameLostCycle, continueFrameLostThreshold);

    _continuousFrameLostCycle = continueFrameLostCycle;
    _continuousFrameLostThreshold = continueFrameLostThreshold;
}

void EngineDataManager::nativeOnChangeLowFpsConfig(JNIEnv* env, jobject thiz, jint lowFpsCycle, jfloat lowFpsThreshold)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeLowFpsConfig, lowFpsCycle: %d, lowFpsThreshold: %f", lowFpsCycle, lowFpsThreshold);
    _lowFpsCycle = lowFpsCycle;
    _lowFpsThreshold = lowFpsThreshold;
}

void EngineDataManager::nativeOnChangeExpectedFps(JNIEnv* env, jobject thiz, jint fps)
{
    if (!_isSupported)
        return;


    if (fps < -1 || fps > 60)
    {
        LOGE("Setting fps (%d) isn't supported!", fps);
        return;
    }

    auto director = cocos2d::Director::getInstance();
    float defaultAnimationInterval = director->getAnimationInterval();

    int defaultFps = static_cast<int>(std::ceil(1.0f/defaultAnimationInterval));

    LOGD("nativeOnChangeExpectedFps, set fps: %d, default fps: %d", fps, defaultFps);

    if (fps > 0)
    {
        director->setAnimationIntervalByEngineDataManager(1.0f/fps);
        LOGD("nativeOnChangeExpectedFps, fps (%d) was set successfuly!", fps);
    }
    else if (fps == -1) // -1 means to reset to default FPS
    {
        director->setAnimationIntervalByEngineDataManager(-1.0f);
        LOGD("nativeOnChangeExpectedFps, fps (%d) was reset successfuly!", defaultFps);
    }

    // Trigger Director::startAnimation
    director->setAnimationInterval(defaultAnimationInterval);
}

void EngineDataManager::nativeOnChangeSpecialEffectLevel(JNIEnv* env, jobject thiz, jint level)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeSpecialEffectLevel, set level: %d", level);

    if (level < 0 || level >= CARRAY_SIZE(_particleLevelArr))
    {
        LOGE("Pass a wrong level value: %d, only 0 ~ %d is supported!", level, CARRAY_SIZE(_particleLevelArr) - 1);
        return;
    }

    ParticleSystem::setTotalParticleCountFactor(_particleLevelArr[level]);
}

void EngineDataManager::nativeOnChangeMuteEnabled(JNIEnv* env, jobject thiz, jboolean isMuteEnabled)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeMuteEnabled, isMuteEnabled: %d", isMuteEnabled);
    cocos2d::experimental::AudioEngine::setEnabled(!isMuteEnabled);
}

} // namespace cocos2d {

/////////////////////////////
extern "C" {

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeSetSupportOptimization)(JNIEnv* env, jobject thiz, jboolean isSupported)
{
    LOGD("nativeSetSupportOptimization: %d", isSupported);
    _isSupported = (isSupported == JNI_TRUE);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnQueryFps)(JNIEnv* env, jobject thiz, jintArray arrExpectedFps, jintArray arrRealFps)
{
    EngineDataManager::nativeOnQueryFps(env, thiz, arrExpectedFps, arrRealFps);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnChangeContinuousFrameLostConfig)(JNIEnv* env, jobject thiz, jint continueFrameLostCycle, jint continueFrameLostThreshold)
{
    EngineDataManager::nativeOnChangeContinuousFrameLostConfig(env, thiz, continueFrameLostCycle, continueFrameLostThreshold);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnChangeLowFpsConfig)(JNIEnv* env, jobject thiz, jint lowFpsCycle, jfloat lowFpsThreshold)
{
    EngineDataManager::nativeOnChangeLowFpsConfig(env, thiz, lowFpsCycle, lowFpsThreshold);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnChangeExpectedFps)(JNIEnv* env, jobject thiz, jint fps)
{
    EngineDataManager::nativeOnChangeExpectedFps(env, thiz, fps);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnChangeSpecialEffectLevel)(JNIEnv* env, jobject thiz, jint level)
{
    EngineDataManager::nativeOnChangeSpecialEffectLevel(env, thiz, level);
}

JNIEXPORT void JNICALL JNI_FUNC_PREFIX(nativeOnChangeMuteEnabled)(JNIEnv* env, jobject thiz, jboolean enabled)
{
    EngineDataManager::nativeOnChangeMuteEnabled(env, thiz, enabled);
}
/////////////////////////////

} // extern "C" {
