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

#include "cocos2d.h"

#include "Utils.h"

class AnotherScene : public cocos2d::CCLayer {
public:
    static cocos2d::CCScene* create();
    
    AnotherScene();
    virtual void onEnter();
    void returnSceneCallback(cocos2d::CCObject* sender);
    void scheduleCallback(float dt);
    
private:
    
    void parseJson();
    
    std::vector<myutils::ResourceInfo> _resourceLevelInfos;
    std::vector<int> _runningOrder;
    std::vector<int> _durations;
    
    int _index;
    std::vector<int> _audioIDVecs;
    cocos2d::CCParticleSun* _emitter;
    cocos2d::CCNode *_parentNode;
};

#endif /* ANOTHER_SCENE_H */
