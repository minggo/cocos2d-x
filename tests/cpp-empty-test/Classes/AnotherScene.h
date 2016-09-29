//
//  AnotherScene.hpp
//  cocos2d_tests
//
//  Created by minggo on 9/23/16.
//
//

#ifndef ANOTHER_SCENE_H
#define ANOTHER_SCENE_H

#include "2d/CCScene.h"
#include "2d/CCLayer.h"

class AnotherScene : public cocos2d::Layer {
public:
    static cocos2d::Scene* create();
    
    AnotherScene();
    void returnSceneCallback(cocos2d::Ref* sender);
};

#endif /* ANOTHER_SCENE_H */
