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
                  _enableAutoTesting(true),
                  _autoTestingLabel(nullptr){}
    
    virtual bool init() override;
    void gameSettingMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void secondMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void resourceRequirementMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void fpsSelectedMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void autoTestingCallback(cocos2d::Ref* sender);
    void actionCallback(int index);
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
        int spriteNumber;
        int drawcallNumber;
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
    
    static int getRandomIndex(std::vector<int>* array);
    static std::vector<ResourceLevel> _resourceLevelVector;
    static std::vector<int> __durations;
    static std::vector<int> __indexes;
    static int __repeatTime;
    static bool __randomOrder;
    
    cocos2d::ParticleSun *_emitter;
    bool _enableAutoTesting;
    cocos2d::Label *_autoTestingLabel;
    cocos2d::Label *_currentResourceLevelLabel;
    
    bool _isSDKTestExpanded;
    bool _isGameSettingExpanded;
};

#endif // __HELLOWORLD_SCENE_H__
