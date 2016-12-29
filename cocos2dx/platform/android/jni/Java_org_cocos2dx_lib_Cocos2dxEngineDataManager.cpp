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
#include "platform/android/CCApplication.h"
#include "CCDirector.h"
#include "particle_nodes/CCParticleSystem.h"
#include "actions/CCActionManager.h"
#include <android/log.h>
#include <cmath>

#define LOG_TAG    "EngineDataManager.cpp"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define EDM_DEBUG 1

#if EDM_DEBUG
#include "json/document.h"
typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonValue;
#endif

#define JNI_FUNC_PREFIX(func) Java_org_cocos2dx_lib_Cocos2dxEngineDataManager_##func

using namespace cocos2d;

extern unsigned int g_uNumberOfDraws;
extern unsigned int g_uNumberOfVertex;

namespace {

const char* CLASS_NAME_ENGINE_DATA_MANAGER = "org/cocos2dx/lib/Cocos2dxEngineDataManager";
const char* CLASS_NAME_RENDERER = "org/cocos2dx/lib/Cocos2dxRenderer";

bool _isInitialized = false;
bool _isSupported = false;
bool _isFirstSetNextScene = true;
bool _isReplaceScene = false;
bool _isReadFile = false;
bool _isInBackground = false;

uint32_t _drawCountInterval = 0;
const uint32_t _drawCountThreshold = 30;

/* last time frame lost cycle was calculated */ 
struct timespec _lastContinuousFrameLostUpdate = {0, 0};
struct timespec _lastFrameLost100msUpdate = {0, 0};

/* last time low fps cycle was calculated */
struct timespec _lastLowFpsUpdate = {0, 0};

int _continuousFrameLostCycle = 5000;
int _continuousFrameLostThreshold = 3;
int _continuousFrameLostCount = 0;
int _frameLostCounter = 0;

int _lowFpsCycle = 1000;
float _lowFpsThreshold = 0.3f;
int _lowFpsCounter = 0;

int _oldCpuLevel = -1;
int _oldGpuLevel = -1;

float _animationIntervalSetBySystem = -1.0f;
float _animationIntervalWhenSceneChange = -1.0f;

#define CARRAY_SIZE(arr) ((int)(arr.size()))

// CPU Level

struct CpuLevelInfo
{
    unsigned int nodeCount;
    unsigned int particleCount;
    unsigned int actionCount;
    unsigned int audioCount;

    CpuLevelInfo()
    : nodeCount(0)
    , particleCount(0)
    , actionCount(0)
    , audioCount(0)
    {}

    CpuLevelInfo(unsigned int nodeCount_,
                 unsigned int particleCount_,
                 unsigned int actionCount_,
                 unsigned int audioCount_)
    : nodeCount(nodeCount_)
    , particleCount(particleCount_)
    , actionCount(actionCount_)
    , audioCount(audioCount_)
    {}
};

CpuLevelInfo _cpuLevelCArray[] = {
    CpuLevelInfo(500 , 500,  500,   6),
    CpuLevelInfo(1250, 1500, 2000,  20),
    CpuLevelInfo(1750, 2000, 3000,  32),
    CpuLevelInfo(2750, 2500, 7000,  50),
    CpuLevelInfo(4000, 3500, 10000, 80)
};

std::vector<CpuLevelInfo> _cpuLevelArr(_cpuLevelCArray, _cpuLevelCArray + sizeof(_cpuLevelCArray) / sizeof(_cpuLevelCArray[0]));

// GPU Level

struct GpuLevelInfo
{
    unsigned int vertexCount;
    unsigned int drawCount;

    GpuLevelInfo()
    : vertexCount(0)
    , drawCount(0)
    {}

    GpuLevelInfo(unsigned int vertexCount_, unsigned int drawCount_)
    : vertexCount(vertexCount_)
    , drawCount(drawCount_)
    {}
};

GpuLevelInfo _gpuLevelCArray[] = {
    GpuLevelInfo(2000, 400),
    GpuLevelInfo(4000, 800),
    GpuLevelInfo(6000, 1000),
    GpuLevelInfo(8000, 1100),
    GpuLevelInfo(10000, 1200),
    GpuLevelInfo(15000, 1300),
    GpuLevelInfo(22000, 1350),
    GpuLevelInfo(30000, 1400),
    GpuLevelInfo(40000, 1450)
};

std::vector<GpuLevelInfo> _gpuLevelArr(_gpuLevelCArray, _gpuLevelCArray + sizeof(_gpuLevelCArray) / sizeof(_gpuLevelCArray[0]));

// Particle Level

float _particleLevelCArray[] = {
    0.0f,
    0.2f,
    0.4f,
    0.6f,
    0.8f,
    1.0f
};

const std::vector<float> _particleLevelArr(_particleLevelCArray, _particleLevelCArray + sizeof(_particleLevelCArray) / sizeof(_particleLevelCArray[0]));

#if EDM_DEBUG
float _oldCpuLevelNode = 0;
float _oldCpuLevelParticle = 0;
float _oldCpuLevelAction = 0;
float _oldCpuLevelAudio = 0;

float _oldGpuLevelVertex = 0;
float _oldGpuLevelDraw = 0;
#endif

inline float getInterval(const struct timespec& newTime, const struct timespec& oldTime)
{
    return (newTime.tv_sec + newTime.tv_nsec / 1000000000.0f) - (oldTime.tv_sec + oldTime.tv_nsec / 1000000000.0f);
}

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
        LOGD("cpu level: %d, node: (%f, %d), particle: (%f, %d), action: (%f, %d), audio: (%f, %d)", 
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
        LOGD("gpu level: %d, vertex: (%f, %d), draw: (%f, %d)", gpuLevel, gpuLevelVertex, vertexCount, gpuLevelDraw, drawCount);
        _oldGpuLevelVertex = gpuLevelVertex;
        _oldGpuLevelDraw = gpuLevelDraw;
    }
#endif
    return gpuLevel;
}

void resetLastTime()
{
    clock_gettime(CLOCK_MONOTONIC, &_lastFrameLost100msUpdate);
    _lastContinuousFrameLostUpdate = _lastFrameLost100msUpdate;
    _lastLowFpsUpdate = _lastFrameLost100msUpdate;
}

void parseDebugConfig()
{
#if EDM_DEBUG
    CCFileUtils* fileUtils = CCFileUtils::sharedFileUtils();

    const char* configPath = "/sdcard/cc-res-level.json";

    if (!fileUtils->isFileExist(configPath))
    {
        return;
    }

    LOGD("Using debug level config: %s", configPath);
    unsigned long size = 0;
    unsigned char* resLevelConfig = fileUtils->getFileData(configPath, "rb", &size);

    RapidJsonDocument document;
    document.Parse<0>((char*)resLevelConfig, size);
    delete[] resLevelConfig;
    
    {
        assert(document.HasMember("cpu_level"));
        const RapidJsonValue& cpu = document["cpu_level"];
        assert(cpu.IsArray());
        assert(_cpuLevelArr.size() == cpu.Size());

        _cpuLevelArr.clear();
        CpuLevelInfo cpuLevelInfo;
        for (unsigned int i = 0; i < cpu.Size(); ++i)
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
        for (unsigned int i = 0; i < gpu.Size(); ++i)
        {
            assert(gpu[i].IsObject());
            
            gpuLevelInfo.vertexCount = gpu[i]["vertex"].GetUint();
            gpuLevelInfo.drawCount = gpu[i]["draw"].GetUint();
            
            _gpuLevelArr.push_back(gpuLevelInfo);
        }
    }
    

    {
        LOGD("-----------------------------------------");
        std::vector<CpuLevelInfo>::iterator iter = _cpuLevelArr.begin();
        for (; iter != _cpuLevelArr.end(); ++iter)
        {
            CpuLevelInfo level = *iter;
            LOGD("cpu level: %u, %u, %u, %u", level.nodeCount, level.particleCount, level.actionCount, level.audioCount);
        }
        LOGD("-----------------------------------------");
    }

    {
        LOGD("=========================================");
        std::vector<GpuLevelInfo>::iterator iter = _gpuLevelArr.begin();
        for (; iter != _gpuLevelArr.end(); ++iter)
        {
            GpuLevelInfo level = *iter;
            LOGD("gpu level: %u, %u", level.vertexCount, level.drawCount);
        }
        LOGD("=========================================");
    }
#endif // EDM_DEBUG
}

void setAnimationIntervalSetBySystem(float interval)
{
    if (!_isSupported)
        return;

    _animationIntervalSetBySystem = interval;
    LOGD("Set FPS %f by system", std::ceil(1.0f / interval));

    CCApplication::sharedApplication()->setAnimationIntervalForReason(interval, SET_INTERVAL_REASON_BY_SYSTEM);
}

void setAnimationIntervalWhenSceneChange(float interval)
{
    if (!_isSupported)
        return;

    LOGD("Set FPS %f while changing scene", std::ceil(1.0f / interval));
    _animationIntervalWhenSceneChange = interval;

    CCApplication::sharedApplication()->setAnimationIntervalForReason(interval, SET_INTERVAL_REASON_BY_SCENE_CHANGE);
}

} // namespace {

namespace cocos2d {

int EngineDataManager::getTotalParticleCount()
{
    std::vector<CCParticleSystem*>& particleSystems = CCParticleSystem::getAllParticleSystems();
    if (particleSystems.empty())
    {
        return 0;
    }

    unsigned int count = 0;
    std::vector<CCParticleSystem*>::iterator iter = particleSystems.begin();
    for (; iter != particleSystems.end(); ++iter)
    {
        count += (*iter)->getTotalParticles();
    }

    return count;
}

// calculates frame lost event
// static
void EngineDataManager::calculateFrameLost()
{
    CCDirector* director = CCDirector::sharedDirector();

    if (_lowFpsThreshold > 0 && _continuousFrameLostThreshold > 0)
    {
        float animationInterval = director->getAnimationInterval();
        if (_animationIntervalSetBySystem > 0.0f)
        {
            animationInterval = _animationIntervalSetBySystem;
        }

        float frameRate = director->getFrameRate();

        float expectedFPS = 1.0f / animationInterval;
        float frameLostRate = (expectedFPS - frameRate) * animationInterval;
        if (frameLostRate > _lowFpsThreshold)
        {
            ++_frameLostCounter;
            ++_lowFpsCounter;
//            LOGD("_frameLostCounter: %d, _lowFpsCounter=%d", _frameLostCounter, _lowFpsCounter);
        }
        
        struct timespec now = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &now);

        float interval = getInterval(now, _lastFrameLost100msUpdate);
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
        
        interval = getInterval(now, _lastContinuousFrameLostUpdate);
        if (interval > (_continuousFrameLostCycle / 1000.0f))
        {
            _lastContinuousFrameLostUpdate = now;
            if (_continuousFrameLostCount > 0)
            {
                // notify continuous frame lost event to system
                notifyContinuousFrameLost(_continuousFrameLostCycle, _continuousFrameLostThreshold, _continuousFrameLostCount);

                LOGD("continuous frame lost: %d", _continuousFrameLostCount);
                _continuousFrameLostCount = 0;
            }
        }
        
        interval = getInterval(now, _lastLowFpsUpdate);
        if (interval > (_lowFpsCycle / 1000.0f))
        {
            _lastLowFpsUpdate = now;
            if (_lowFpsCounter > 0)
            {
                // notify low fps event to system
                notifyLowFps(_lowFpsCycle, _lowFpsThreshold, _lowFpsCounter);
                LOGD("low fps frame count: %d", _lowFpsCounter);
                _lowFpsCounter = 0;
            }
        }
    }
}

// static 
void EngineDataManager::onBeforeSetNextScene()
{
    // Reset the old status since we have changed CPU/GPU level manually.
    // If the CPU level isn't 5 and GPU level isn't 0 in the next time of checking CPU/GPU level,
    // Make sure that the changed CPU/GPU level will be notified.
    _oldCpuLevel = -1;
    _oldGpuLevel = -1;

    if (_isFirstSetNextScene)
    {
        _isFirstSetNextScene = false;
        notifyGameStatus(LAUNCH_END, -1, -1);
    }
    else if (_isReplaceScene)
    {
        notifyGameStatus(SCENE_CHANGE_END, -1, -1);
    }

    notifyGameStatus(SCENE_CHANGE_BEGIN, 5, 0);

    // Set _animationIntervalWhenSceneChange to 1.0f/60.0f while there isn't in replacing scene.
    if (!_isReplaceScene)
    {
        // Modify fps to 60 
        setAnimationIntervalWhenSceneChange(1.0f/60.0f);
    }

    _isReplaceScene = true;
}

void EngineDataManager::onBeforeReadFile()
{
    _isReadFile = true;
}

void EngineDataManager::notifyGameStatusIfCpuOrGpuLevelChanged()
{
    // calculate CPU & GPU levels
    int cpuLevel = 0;
    int gpuLevel = 0;
    getCpuAndGpuLevel(&cpuLevel, &gpuLevel);

    if (cpuLevel != _oldCpuLevel || gpuLevel != _oldGpuLevel)
    {
        notifyGameStatus(IN_SCENE, cpuLevel, gpuLevel);

        _oldCpuLevel = cpuLevel;
        _oldGpuLevel = gpuLevel;
    }
}

// static
void EngineDataManager::onAfterDrawScene()
{
    calculateFrameLost();

    if (_isReplaceScene)
    {
        ++_drawCountInterval;

        if (_drawCountInterval > _drawCountThreshold)
        {
            _drawCountInterval = 0;
            _isReplaceScene = false;

            // Reset _animationIntervalWhenSceneChange to -1.0f to
            // make developer's or huawei's FPS setting take effect.
            setAnimationIntervalWhenSceneChange(-1.0f);

            _oldCpuLevel = -1;
            _oldGpuLevel = -1;
            notifyGameStatus(SCENE_CHANGE_END, -1, -1);
        }
        else if (_isReadFile)
        {
            _drawCountInterval = 0;
        }
        _isReadFile = false;
    }
    else
    {
        notifyGameStatusIfCpuOrGpuLevelChanged();
    }
}

void EngineDataManager::getCpuAndGpuLevel(int* cpuLevel, int* gpuLevel)
{
    if (cpuLevel == NULL || gpuLevel == NULL)
        return;

    CCDirector* director = CCDirector::sharedDirector();
    int totalNodeCount = CCNode::getAttachedNodeCount();
    int totalParticleCount = getTotalParticleCount();
    int totalActionCount = director->getActionManager()->numberOfRunningActions();
    int totalPlayingAudioCount = 0;// experimental::AudioEngine::getPlayingAudioCount();
    *cpuLevel = toCpuLevel(totalNodeCount, totalParticleCount, totalActionCount, totalPlayingAudioCount);

    *gpuLevel = toGpuLevel(g_uNumberOfVertex, g_uNumberOfDraws);
}

// static
void EngineDataManager::onEnterForeground()
{
    _isInBackground = false;

    static bool isFirstTime = true;
    LOGD("onEnterForeground, isFirstTime: %d", isFirstTime);

    // Cocos2d-x 2.x will not trigger onEnterForeground when launch.
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

void EngineDataManager::onEnterBackground()
{
    LOGD("EngineDataManager::onEnterBackground ...");
    _isInBackground = true;
}

// static
void EngineDataManager::init()
{
    if (!_isSupported)
        return;

    if (_isInitialized)
        return;

    resetLastTime();


    CCDirector* director = CCDirector::sharedDirector();
    director->setBeforeSetNextSceneHook(onBeforeSetNextScene);
    director->setAfterDrawHook(onAfterDrawScene);
    CCFileUtils::sharedFileUtils()->setBeforeReadFileHook(onBeforeReadFile);

    notifyGameStatus(LAUNCH_BEGIN, 5, -1);

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
    if (JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME_ENGINE_DATA_MANAGER, "notifyGameStatus", "(III)V"))
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
    if (JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME_ENGINE_DATA_MANAGER, "notifyContinuousFrameLost", "(III)V"))
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
    if (JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME_ENGINE_DATA_MANAGER, "notifyLowFps", "(IFI)V"))
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
        CCDirector* director = CCDirector::sharedDirector();
        float animationInterval = director->getAnimationInterval();
        *expectedFps = (int)std::ceil(1.0f / animationInterval);
        env->ReleaseIntArrayElements(arrExpectedFps, expectedFps, 0);

        jint* realFps = env->GetIntArrayElements(arrRealFps, &isCopy);
        *realFps = (int)cocos2d::CCDirector::sharedDirector()->getFrameRate();
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

    CCDirector* director = cocos2d::CCDirector::sharedDirector();
    float defaultAnimationInterval = director->getAnimationInterval();

    int defaultFps = static_cast<int>(std::ceil(1.0f/defaultAnimationInterval));

    if (fps > defaultFps)
    {
        LOGD("nativeOnChangeExpectedFps, fps (%d) is greater than default fps (%d), reset it to default!", fps, defaultFps);
        fps = -1;
    }

    LOGD("nativeOnChangeExpectedFps, set fps: %d, default fps: %d", fps, defaultFps);

    if (fps > 0)
    {
        setAnimationIntervalSetBySystem(1.0f/fps);
        LOGD("nativeOnChangeExpectedFps, fps (%d) was set successfuly!", fps);
    }
    else if (fps == -1) // -1 means to reset to default FPS
    {
        setAnimationIntervalSetBySystem(-1.0f);
        LOGD("nativeOnChangeExpectedFps, fps (%d) was reset successfuly!", defaultFps);
    }
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

    CCParticleSystem::setTotalParticleCountFactor(_particleLevelArr[level]);
}

void EngineDataManager::nativeOnChangeMuteEnabled(JNIEnv* env, jobject thiz, jboolean isMuteEnabled)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeMuteEnabled, isMuteEnabled: %d", isMuteEnabled);
    //cjh cocos2d::experimental::AudioEngine::setEnabled(!isMuteEnabled);
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
