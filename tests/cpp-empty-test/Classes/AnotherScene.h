//
//  AnotherScene.hpp
//  cocos2d_tests
//
//  Created by minggo on 9/23/16.
//
//

#ifndef ANOTHER_SCENE_H
#define ANOTHER_SCENE_H

#include <vector>
#include <string>

#include "2d/CCScene.h"
#include "2d/CCLayer.h"

#include "Utils.h"

class AnotherScene : public cocos2d::Layer {
public:
    static cocos2d::Scene* create();
    
    AnotherScene();
    virtual void onEnter() override;
    void returnSceneCallback(cocos2d::Ref* sender);
    void scheduleCallback(float dt);
    
private:
    
    void parseJson();
    
    std::vector<myutils::ResourceInfo> _resourceLevelInfos;
    std::vector<int> _runningOrder;
    std::vector<int> _durations;
    static std::vector<std::string> __keys;
    
    int _index;
    std::vector<int> _audioIDVecs;
    cocos2d::ParticleSun* _emitter;
    cocos2d::Node *_parentNode;
};

#endif /* ANOTHER_SCENE_H */
