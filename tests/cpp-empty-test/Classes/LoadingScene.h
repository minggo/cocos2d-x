//
//  LoadingScene.hpp
//  cocos2d_tests
//
//  Created by minggo on 10/12/16.
//
//

#ifndef LoadingScene_hpp
#define LoadingScene_hpp

#include <vector>
#include <string>

#include "2d/CCScene.h"

class LoadingScene : public cocos2d::Scene
{
public:
    static LoadingScene* create(cocos2d::Scene* newScene);
    
    LoadingScene(cocos2d::Scene* newScene);
    virtual void update(float dt) override;
    
    void replaceScene(float dt);
private:

    void load();
    void computeMD5(const std::string& fileName);
    
    int _index;
    cocos2d::Scene* _newScene;
    bool _endOfMd5;
    std::vector<std::string> _bitfiles;
    std::vector<std::string> _resfiles;
};

#endif /* LoadingScene_hpp */
