// TODO


#include "audio/include/SimpleAudioEngine.h"

using namespace CocosDenshion;

SimpleAudioEngine * g_SimpleAudioEnginePepper = nullptr; 


SimpleAudioEngine* SimpleAudioEngine::getInstance()
{
  if (!g_SimpleAudioEnginePepper)
      g_SimpleAudioEnginePepper = new SimpleAudioEngine();

  return g_SimpleAudioEnginePepper; 
}

void SimpleAudioEngine::end()
{
  if (g_SimpleAudioEnginePepper)
      delete g_SimpleAudioEnginePepper;

  g_SimpleAudioEnginePepper = nullptr; 
}
  
  
SimpleAudioEngine::SimpleAudioEngine() {}
SimpleAudioEngine::~SimpleAudioEngine() {}

void SimpleAudioEngine::preloadBackgroundMusic(const char* filePath) {}
void SimpleAudioEngine::playBackgroundMusic(const char* filePath, bool loop) {}
void SimpleAudioEngine::stopBackgroundMusic(bool releaseData) {}
void SimpleAudioEngine::pauseBackgroundMusic() {}
void SimpleAudioEngine::resumeBackgroundMusic() {}
void SimpleAudioEngine::rewindBackgroundMusic() {}
bool SimpleAudioEngine::willPlayBackgroundMusic() { return false; }
bool SimpleAudioEngine::isBackgroundMusicPlaying() { return false; }

    // 
    // properties
    //


float SimpleAudioEngine::getBackgroundMusicVolume() { return 0.0f; }
void SimpleAudioEngine::setBackgroundMusicVolume(float volume) {}
float SimpleAudioEngine::getEffectsVolume(){ return 0.0f; }
void SimpleAudioEngine::setEffectsVolume(float volume) {}
unsigned int SimpleAudioEngine::playEffect(const char* filePath, bool loop, float pitch, float pan, float gain) { return 0; }
void SimpleAudioEngine::pauseEffect(unsigned int soundId){}
void SimpleAudioEngine::pauseAllEffects() {}
void SimpleAudioEngine::resumeEffect(unsigned int soundId){}
void SimpleAudioEngine::resumeAllEffects() {}
void SimpleAudioEngine::stopEffect(unsigned int soundId){}
void SimpleAudioEngine::stopAllEffects() {}
void SimpleAudioEngine::preloadEffect(const char* filePath) {}
void SimpleAudioEngine::unloadEffect(const char* filePath) {}
