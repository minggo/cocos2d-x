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
#include "json/document.h"
//#include "audio/include/AudioEngine.h"

#include "HelloWorldScene.h"

using namespace cocos2d;

std::vector<std::string> AnotherScene::__keys = {"key1", "key2", "key3", "key4", "key5", "key6", "key7", "key8", "key9", "key10"};

cocos2d::Scene* AnotherScene::create()
{
    auto scene = Scene::create();
    auto layer = new AnotherScene();
    layer->autorelease();
    scene->addChild(layer);
    
    return scene;
}

typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonValue;

AnotherScene::AnotherScene()
: _index(0)
{
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto label = Label::createWithSystemFont("return to test scene", "", 15);
    auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(AnotherScene::returnSceneCallback, this));
    menuItem->setPosition(Vec2(100, 100));
    auto menu = Menu::create(menuItem, nullptr);
    menu->setPosition(Vec2(origin.x, origin.y));
    this->addChild(menu);
    
    // preload resources
    Director::getInstance()->getTextureCache()->addImage("sprite0.png");
    Director::getInstance()->getTextureCache()->addImage("sprite1.png");
//    experimental::AudioEngine::preload("effec0.mp3");
//    experimental::AudioEngine::preload("effec1.mp3");
//    experimental::AudioEngine::preload("effec2.mp3");
//    experimental::AudioEngine::preload("effec3.mp3");
//    experimental::AudioEngine::preload("effec4.mp3");
//    experimental::AudioEngine::preload("effec5.mp3");
//    experimental::AudioEngine::preload("effec6.mp3");
//    experimental::AudioEngine::preload("effec7.mp3");
//    experimental::AudioEngine::preload("effec8.mp3");
//    experimental::AudioEngine::preload("effec9.mp3");

    _parentNode = Node::create();
    this->addChild(_parentNode);
    
    _resourceLevelInfos = {
        // sprite, drawCall, action, particle, audio
        {120, 120,  0,   0,   0}, // CPU=0,GPU=0
        {300, 300,  0, 50, 2}, // CPU=1,GPU=1
        {350, 350,  0, 200, 1}, // CPU=1,GPU=2
        {500, 500,  0, 300, 3}, // CPU=2,GPU=3
        {700, 550,  0, 300, 2}, // CPU=2,GPU=4
        {900, 550,  0, 500, 3}, // CPU=3,GPU=5
        
        {1200, 700,  0, 500, 3}, // CPU=3,GPU=6
        {1500, 700,  0,   400, 5}, // CPU=4,GPU=7
        {2000, 900,  0,   400, 4}, // CPU=4,GPU=8
        {5000, 2000,  0, 1000, 5}, // CPU=5,GPU=9
    };
    
    parseJson();
    
    _emitter = ParticleSun::create();
    _emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
    _emitter->setTotalParticles(0);
    _emitter->setPosition(Vec2(100, 100));
    _emitter->pause();
    this->addChild(_emitter);
}

void AnotherScene::onEnter()
{
    Node::onEnter();
    
    int level = _runningOrder[0] - 1;
    std::string key = __keys[level];
    int duration = _durations[0];
    
    myutils::addResource(_parentNode, _emitter, _resourceLevelInfos[level], _audioIDVecs);
    schedule(schedule_selector(AnotherScene::scheduleCallback), duration);
    log("schedule start %d", duration);
}

void AnotherScene::returnSceneCallback(cocos2d::Ref* sender)
{
    auto scene = HelloWorld::scene();
//    experimental::AudioEngine::stopAll();
    Director::getInstance()->replaceScene(scene);
}

void AnotherScene::scheduleCallback(float dt)
{
    unschedule(schedule_selector(AnotherScene::scheduleCallback));
    log("schedule start %f", dt);
    ++_index;
    if (_index >= _runningOrder.size())
    {
        _parentNode->removeAllChildren();
        _emitter->removeFromParent();
//        experimental::AudioEngine::stopAll();
//        
//        experimental::AudioEngine::end();
        Director::getInstance()->getTextureCache()->removeUnusedTextures();
        
        return;
    }
    
    int nextLevel = _runningOrder[_index] - 1;
    int currentLevel = _runningOrder[_index - 1] - 1;
    std::string key = __keys[nextLevel];
    int duration = _durations[_index];
    
    auto currentResourceInfo = _resourceLevelInfos[currentLevel];
    auto nextResourceInfo = _resourceLevelInfos[nextLevel];
    myutils::ResourceInfo subResourceInfo;
    subResourceInfo.spriteNumber = nextResourceInfo.spriteNumber - currentResourceInfo.spriteNumber;
    subResourceInfo.actionNumber = nextResourceInfo.actionNumber - currentResourceInfo.actionNumber;
    subResourceInfo.drawcallNumber = nextResourceInfo.drawcallNumber - currentResourceInfo.drawcallNumber;
    subResourceInfo.particleNumber = nextResourceInfo.particleNumber;
    subResourceInfo.audioNumber = nextResourceInfo.audioNumber - currentResourceInfo.audioNumber;
    myutils::addResource(_parentNode, _emitter, subResourceInfo, _audioIDVecs);
    
    schedule(schedule_selector(AnotherScene::scheduleCallback), duration);
}

void AnotherScene::parseJson()
{

    auto fileUtils = FileUtils::getInstance();
    fileUtils->addSearchPath("/sdcard", true);
    fileUtils->addSearchPath(fileUtils->getWritablePath(), true);
    
    auto fileContent = fileUtils->getStringFromFile("gameScene.json");
    
    RapidJsonDocument document;
    document.Parse<0>(fileContent.c_str());
    
    // get duration
    const RapidJsonValue& duration = document["duration"];
    for (auto iter = duration.onBegin(); iter != duration.onEnd(); ++iter)
    {
        _durations.push_back(iter->GetInt());
    }
    
    if (document.HasMember("running_order"))
    {
        const RapidJsonValue& runningOrder = document["running_order"];
        for (auto iter = runningOrder.onBegin(); iter != runningOrder.onEnd(); ++ iter)
        {
            _runningOrder.push_back(iter->GetInt());
        }
    }
    else
    {
        _runningOrder = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    }
}
