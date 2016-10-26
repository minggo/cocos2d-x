//
//  LoadingScene.cpp
//  cocos2d_tests
//
//  Created by minggo on 10/12/16.
//
//

#include "LoadingScene.h"
#include "base/CCDirector.h"
#include "renderer/CCTextureCache.h"
#include "2d/CCLabel.h"

using namespace cocos2d;

LoadingScene* LoadingScene::create(Scene* newScene)
{
    auto scene = new LoadingScene(newScene);
    scene->autorelease();
    return scene;
}

LoadingScene::LoadingScene(Scene* newScene)
: _index(0)
, _newScene(newScene)
{
    _newScene->retain();
    this->scheduleUpdate();
    
    this->schedule(CC_CALLBACK_1(LoadingScene::replaceScene, this), 6, "replaceScene");
    
    auto origin = Director::getInstance()->getVisibleOrigin();
    auto size = Director::getInstance()->getVisibleSize();
    auto label = Label::createWithSystemFont("loading...", "", 15);
    label->setPosition(Vec2(origin.x + size.width/2, origin.y + size.height/2));
    label->setTag(100);
    this->addChild(label);
}

void LoadingScene::replaceScene(float dt)
{
    Director::getInstance()->replaceScene(_newScene);
    _newScene->release();
    _newScene = nullptr;
}

void LoadingScene::update(float dt)
{
//    auto label = static_cast<Label*>(getChildByTag(100));
//    switch(_index % 3)
//    {
//        case 0:
//            label->setString("loading.");
//            break;
//        case 1:
//            label->setString("loading..");
//            break;
//        case 2:
//            label->setString("loading...");
//            break;
//        default:
//            break;
//    }
    
    load(_index % 6);
    _index++;
}


void LoadingScene::load(int level)
{
    for (int i = 0; i < level; ++i)
    {
        Director::getInstance()->getTextureCache()->addImage("Hello.png");
        Director::getInstance()->getTextureCache()->removeUnusedTextures();
    }
}
