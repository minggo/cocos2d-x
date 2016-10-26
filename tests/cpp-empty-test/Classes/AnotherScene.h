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
    void returnSceneCallback(cocos2d::Ref* sender);
    void scheduleCallback(float dt);
    
private:
    
    void parseJson();
    
    std::vector<myutils::ResourceInfo> _resourceLevelInfos;
    static std::vector<int> __runningOrder;
    static std::vector<int> __durations;
    static std::vector<std::string> __keys;
    
    
    int _currentLevel;
    std::vector<int> _audioIDVecs;
    cocos2d::ParticleSun* _emitter;
    cocos2d::Node *_parentNode;
};

#endif /* ANOTHER_SCENE_H */
