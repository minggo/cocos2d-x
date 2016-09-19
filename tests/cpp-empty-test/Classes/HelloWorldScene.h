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
    
    virtual bool init() override;
    void gameSettingMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void secondMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void resourceRequirementMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);
    void fpsSelectedMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type);

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
    
    cocos2d::ui::ListView* createListView(const std::vector<std::string>& itemTitles, const cocos2d::Vec2& position);
    void addResources(int level);
    
    static std::vector<ResourceLevel> _resourceLevelVector;
    
    cocos2d::ParticleSun *_emitter;
};

#endif // __HELLOWORLD_SCENE_H__
