#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <vector>

#include "ui/UIListView.h"
#include "2d/CCParticleExamples.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
    
    HelloWorld() :_emitter(nullptr),
                  _enableAutoTesting(false),
                  _autoTestingLabel(nullptr),
                  _currentResourceLevel(0){}
    
    virtual bool init() override;
    void gameSettingMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void secondMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void resourceRequirementMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void fpsSelectedMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void autoTestingCallback(cocos2d::Ref* sender);
    void actionCallback();
    void lastActionCallback();
    
    void SDKTestSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void SDKSecondMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void SDKFPSSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void SDKEffectSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void SDKAudioSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    
    using ResourceLevel = struct ResourceLevel
    {
        int nodeNumber;  // for cpu
        int spriteNumber; // for gpu
        int actionNumber;
        int particleNumber;
        int audioNumber;
    };
    
    static void parseJson();
    
    cocos2d::ui::ListView* createListView(const std::vector<std::string>& itemTitles, const cocos2d::Vec2& position);
    void addResources(int level);
    void enableAllListViews();
    void disableAllListViews();
    void enableSDKAudio(bool enabled);
    void enableSDKEffect(bool enabled);
    void enableSDKFPS(bool enabled);
    
    static std::vector<ResourceLevel> _resourceLevelVector;
    static std::vector<int> _durations;
    
    cocos2d::ParticleSun *_emitter;
    bool _enableAutoTesting;
    cocos2d::Label *_autoTestingLabel;
    // used for acton call back
    int _currentResourceLevel;
};

#endif // __HELLOWORLD_SCENE_H__
