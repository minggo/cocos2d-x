//
//  LoadingScene.hpp
//  cocos2d_tests
//
//  Created by minggo on 10/12/16.
//
//

#ifndef LoadingScene_hpp
#define LoadingScene_hpp

#include "2d/CCScene.h"

class LoadingScene : public cocos2d::Scene
{
public:
    static LoadingScene* create(cocos2d::Scene* newScene);
    
    LoadingScene(cocos2d::Scene* newScene);
    virtual void update(float dt) override;
    
    void replaceScene(float dt);
private:

    void load(int level);
    
    int _index;
    cocos2d::Scene* _newScene;
};

#endif /* LoadingScene_hpp */
