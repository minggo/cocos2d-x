//
//  AnotherScene.cpp
//  cocos2d_tests
//
//  Created by minggo on 9/23/16.
//
//

#include "AnotherScene.h"
#include "2d/CCLabel.h"
#include "base/CCDirector.h"
#include "2d/CCMenuItem.h"

#include "HelloWorldScene.h"

using namespace cocos2d;

cocos2d::Scene* AnotherScene::create()
{
    auto scene = Scene::create();
    auto layer = new AnotherScene();
    layer->autorelease();
    scene->addChild(layer);
    
    return scene;
}

AnotherScene::AnotherScene()
{
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto label = Label::createWithSystemFont("return to test scene", "", 15);
    auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(AnotherScene::returnSceneCallback, this));
    menuItem->setPosition(Vec2(100, 100));
    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Vec2(origin.x, origin.y));
    this->addChild(menu);
}

void AnotherScene::returnSceneCallback(cocos2d::Ref* sender)
{
    auto scene = HelloWorld::scene();
    Director::getInstance()->replaceScene(scene);
}
