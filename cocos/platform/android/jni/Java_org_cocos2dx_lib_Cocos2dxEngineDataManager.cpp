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
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"
#include "renderer/CCRenderer.h"
#include "2d/CCParticleSystem.h"
#include "2d/CCAction.h"
#include "audio/include/AudioEngine.h"

#include <android/log.h>

#define LOG_TAG    "Java_org_cocos2dx_lib_Cocos2dxEngineDataManager.cpp"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define JNI_FUNC_PREFIX(func) Java_org_cocos2dx_lib_Cocos2dxEngineDataManager_##func

using namespace cocos2d;

namespace {

const char* _className = "org/cocos2dx/lib/Cocos2dxEngineDataManager";

bool _isInitialized = false;
bool _isSupported = false;

float _defaultAnimationInterval = -1.0f;

std::unordered_map<unsigned int /*Ref::_ID*/, int/*particleCount*/> _psIdCountMap;

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

int _oldCpuLevel = 0;
int _oldGpuLevel = 0;

#define CARRAY_SIZE(arr) ((int)(sizeof(arr) / sizeof(arr[0])))

// CPU Level
#define CPU_LEVEL_WEIGHT_NODE (0.3f)
#define CPU_LEVEL_WEIGHT_PARTICLE (0.4f)
#define CPU_LEVEL_WEIGHT_ACTION (0.3f)
#define CPU_LEVEL_WEIGHT_AUDIO (0.3f)

#define CPU_LEVEL_ELEMENT(node, particle, action, audio) \
        ((node) * CPU_LEVEL_WEIGHT_NODE + (particle) * CPU_LEVEL_WEIGHT_PARTICLE + (action) * CPU_LEVEL_WEIGHT_ACTION + (audio) * CPU_LEVEL_WEIGHT_AUDIO)

const float _cpuLevelArr[] = {
    CPU_LEVEL_ELEMENT(50 ,  100, 20,  2),
    CPU_LEVEL_ELEMENT(100,  200, 40,  6),
    CPU_LEVEL_ELEMENT(300,  300, 60,  12),
    CPU_LEVEL_ELEMENT(700,  400, 70,  18),
    CPU_LEVEL_ELEMENT(1500, 500, 100, 24)
};

// GPU Level
#define GPU_LEVEL_WEIGHT_VERTEX (0.6f)
#define GPU_LEVEL_WEIGHT_BATCH (1.0f - GPU_LEVEL_WEIGHT_VERTEX)

#define GPU_LEVEL_ELEMENT(vertexCount, batchedCount) \
        ((vertexCount) * GPU_LEVEL_WEIGHT_VERTEX + (batchedCount) * GPU_LEVEL_WEIGHT_BATCH)

const float _gpuLevelArr[] = {
    GPU_LEVEL_ELEMENT(1000, 20),
    GPU_LEVEL_ELEMENT(2000, 30),
    GPU_LEVEL_ELEMENT(3000, 40),
    GPU_LEVEL_ELEMENT(4000, 50),
    GPU_LEVEL_ELEMENT(6000, 60),
    GPU_LEVEL_ELEMENT(8000, 70),
    GPU_LEVEL_ELEMENT(10000, 80),
    GPU_LEVEL_ELEMENT(15000, 100),
    GPU_LEVEL_ELEMENT(20000, 120),
    GPU_LEVEL_ELEMENT(25000, 150)
};


const float _particleLevelArr[] = {
    0.0f,
    0.2f,
    0.4f,
    0.6f,
    0.8f,
    1.0f
};

int toCpuLevel(int nodeCount, int particleCount, int actionCount, int audioCount)
{
    int len = CARRAY_SIZE(_cpuLevelArr);
    for (int i = 0; i < len; ++i)
    {
        if (CPU_LEVEL_ELEMENT(nodeCount, particleCount, actionCount, audioCount) < _cpuLevelArr[i])
        {
            return i;
        }
    }

    return len - 1;
}

int toGpuLevel(int vetexCount, int batchedCount)
{
    int len = CARRAY_SIZE(_gpuLevelArr);
    for (int i = 0; i < len; ++i)
    {
        if (GPU_LEVEL_ELEMENT(vetexCount, batchedCount) < _gpuLevelArr[i])
        {
            return i;
        }
    }

    return len - 1;
}

void resetLastTime()
{
    _lastFrameLost100msUpdate = std::chrono::steady_clock::now();
    _lastContinuousFrameLostUpdate = _lastFrameLost100msUpdate;
    _lastLowFpsUpdate = _lastFrameLost100msUpdate;
}

} // namespace {

namespace cocos2d {

int EngineDataManager::getTotalParticleCount()
{
    auto& particleSystems = ParticleSystem::getAllParticleSystems();
    if (particleSystems.empty())
    {
        LOGD("There isn't any ParticleSystem instance!");
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

// static 
void EngineDataManager::onBeforeSetNextScene(EventCustom* event)
{
    log("previous node count: %d", Node::getTotalNodeCount());
    int cpuLevel = 0;
    int gpuLevel = 0;
    getCpuAndGpuLevel(&cpuLevel, &gpuLevel);

    notifyGameStatus(GameStatus::SCENE_CHANGE, cpuLevel, gpuLevel);
}

// static 
void EngineDataManager::onAfterSetNextScene(EventCustom* event)
{
    log("current node count: %d", Node::getTotalNodeCount());
    int cpuLevel = 0;
    int gpuLevel = 0;
    getCpuAndGpuLevel(&cpuLevel, &gpuLevel);

    notifyGameStatus(GameStatus::IN_SCENE, cpuLevel, gpuLevel);
}

// static
void EngineDataManager::onAfterVisitScene(EventCustom* event)
{
    calculateFrameLost();

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

void EngineDataManager::getCpuAndGpuLevel(int* cpuLevel, int* gpuLevel)
{
    if (cpuLevel == nullptr || gpuLevel == nullptr)
        return;

    int totalNodeCount = Node::getTotalNodeCount();
    int totalParticleCount = getTotalParticleCount();
    int totalActionCount = Action::getTotalActionCount();
    int totalPlayingAudioCount = experimental::AudioEngine::getPlayingAudioCount();
    *cpuLevel = toCpuLevel(totalNodeCount, totalParticleCount, totalActionCount, totalPlayingAudioCount);

    auto renderer = Director::getInstance()->getRenderer();
    int vertexCount = renderer->getDrawnVertices();
    int batchedCount = renderer->getDrawnBatches();

    *gpuLevel = toGpuLevel(vertexCount, batchedCount);
}

// static
void EngineDataManager::onEnterForeground(EventCustom* event)
{
    resetLastTime();
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
    dispatcher->addCustomEventListener(Director::EVENT_AFTER_VISIT, std::bind(onAfterVisitScene, std::placeholders::_1));
    dispatcher->addCustomEventListener(Director::EVENT_BEFORE_SET_NEXT_SCENE, std::bind(onBeforeSetNextScene, std::placeholders::_1));
    dispatcher->addCustomEventListener(Director::EVENT_AFTER_SET_NEXT_SCENE, std::bind(onAfterSetNextScene, std::placeholders::_1));
    dispatcher->addCustomEventListener(EVENT_COME_TO_FOREGROUND, std::bind(onEnterForeground, std::placeholders::_1));

    //FIMXE: have to pass cpuLevel 10 and gpuLevel 10 ?
    notifyGameStatus(GameStatus::START, 10, 10);
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

    LOGD("nativeOnChangeExpectedFps, fps: %d", fps);
    auto director = cocos2d::Director::getInstance();
    if (_defaultAnimationInterval < 0)
    {
        _defaultAnimationInterval = director->getAnimationInterval();
    }

    fps = std::min(fps, 60);
    if (fps > 0)
    {
        director->setAnimationInterval(1.0f/fps);
    }
    else if (fps == -1) // -1 means to reset to default FPS
    {
        if (_defaultAnimationInterval > 0)
        {
            director->setAnimationInterval(_defaultAnimationInterval);
        }
        else
        {
            LOGE("_defaultAnimationInterval <= 0");
        }
    }
}

void EngineDataManager::nativeOnChangeSpecialEffectLevel(JNIEnv* env, jobject thiz, jint level)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeSpecialEffectLevel, level: %d", level);

    if (level < 0 || level >= CARRAY_SIZE(_particleLevelArr))
    {
        LOGE("Pass a wrong level value: %d, only 0 ~ 5 is supported!", level);
        return;
    }

    auto& particleSystems = ParticleSystem::getAllParticleSystems();
    if (particleSystems.empty())
    {
        LOGD("There isn't any ParticleSystem instance!");
        return;
    }

    std::vector<unsigned int> validIDs;
    validIDs.reserve(particleSystems.size());

    for (auto&& system : particleSystems)
    {
        if (_psIdCountMap.find(system->_ID) == _psIdCountMap.end())
        {
            _psIdCountMap.insert(std::make_pair(system->_ID, system->getTotalParticles()));
        }
        validIDs.push_back(system->_ID);

        int defaultParticleCount = _psIdCountMap[system->_ID];
        int expectedParticleCount = _particleLevelArr[level] * defaultParticleCount;

        LOGD("Set ParticleSystem(%p)'s total particle count from (%d) to (%d)", system, defaultParticleCount, expectedParticleCount);

        system->setTotalParticles(expectedParticleCount);
    }

    // Remove unused elements in _psIdCountMap
    std::vector<unsigned int> invalidIDs;
    for (const auto& e : _psIdCountMap)
    {
        auto iter = std::find(std::begin(validIDs), std::end(validIDs), e.first);
        if (iter == std::end(validIDs))
        {
            invalidIDs.push_back(e.first);
        }
    }

    for (unsigned int id : invalidIDs)
    {
        LOGD("Remove unused particle system id: %u", id);
        _psIdCountMap.erase(id);
    }
}

void EngineDataManager::nativeOnChangeMuteEnabled(JNIEnv* env, jobject thiz, jboolean enabled)
{
    if (!_isSupported)
        return;

    LOGD("nativeOnChangeMuteEnabled, enabled: %d", enabled);
    if (enabled)
    {
        cocos2d::experimental::AudioEngine::stopAll();
    }
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
