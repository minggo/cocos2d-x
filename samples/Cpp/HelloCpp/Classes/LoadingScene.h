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

#include "cocos2d.h"

class LoadingScene : public cocos2d::CCScene
{
public:
    static LoadingScene* create(cocos2d::CCScene* newScene);
    
    LoadingScene(cocos2d::CCScene* newScene);
    virtual void update(float dt);
    
    void replaceScene(float dt);
private:

    void load();
    void computeMD5(const std::string& fileName);
    
    int _index;
    cocos2d::CCScene* _newScene;
    bool _endOfMd5;
    std::vector<std::string> _bitfiles;
    std::vector<std::string> _resfiles;
};

#endif /* LoadingScene_hpp */
