//
//  AnotherScene.cpp
//  cocos2d_tests
//
//  Created by minggo on 9/23/16.
//
//

#include "AnotherScene.h"
#include "json/document.h"
//#include "audio/include/AudioEngine.h"

#include "HelloWorldScene.h"

using namespace cocos2d;

cocos2d::CCScene* AnotherScene::create()
{
    CCScene* scene = CCScene::create();
    AnotherScene* layer = new AnotherScene();
    layer->autorelease();
    scene->addChild(layer);
    
    return scene;
}

typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonValue;

AnotherScene::AnotherScene()
: _index(0)
{
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    
    CCLabelTTF* label = CCLabelTTF::create("return to test scene", "", 15);
    CCMenuItemLabel* menuItem = CCMenuItemLabel::create(label, this, menu_selector(AnotherScene::returnSceneCallback));
    menuItem->setPosition(ccp(100, 100));
    CCMenu* menu = CCMenu::create(menuItem, NULL);
    menu->setPosition(ccp(origin.x, origin.y));
    this->addChild(menu);
    
    // preload resources
    CCTextureCache::sharedTextureCache()->addImage("sprite0.png");
    CCTextureCache::sharedTextureCache()->addImage("sprite1.png");

//cjh    experimental::AudioEngine::preload("effec0.mp3");
//    experimental::AudioEngine::preload("effec1.mp3");
//    experimental::AudioEngine::preload("effec2.mp3");
//    experimental::AudioEngine::preload("effec3.mp3");
//    experimental::AudioEngine::preload("effec4.mp3");
//    experimental::AudioEngine::preload("effec5.mp3");
//    experimental::AudioEngine::preload("effec6.mp3");
//    experimental::AudioEngine::preload("effec7.mp3");
//    experimental::AudioEngine::preload("effec8.mp3");
//    experimental::AudioEngine::preload("effec9.mp3");

    _parentNode = CCNode::create();
    this->addChild(_parentNode);
    
    myutils::ResourceInfo resourceLevelInfos[] = {
        // sprite, drawCall, action, particle, audio
        myutils::ResourceInfo(120, 120,  0,   0,   0), // CPU=0,GPU=0
        myutils::ResourceInfo(300, 300,  0, 50, 2), // CPU=1,GPU=1
        myutils::ResourceInfo(350, 350,  0, 200, 1), // CPU=1,GPU=2
        myutils::ResourceInfo(500, 500,  0, 300, 3), // CPU=2,GPU=3
        myutils::ResourceInfo(700, 550,  0, 300, 2), // CPU=2,GPU=4
        myutils::ResourceInfo(900, 550,  0, 500, 3), // CPU=3,GPU=5
        
        myutils::ResourceInfo(1200, 700,  0, 500, 3), // CPU=3,GPU=6
        myutils::ResourceInfo(1500, 700,  0,   400, 5), // CPU=4,GPU=7
        myutils::ResourceInfo(2000, 900,  0,   400, 4), // CPU=4,GPU=8
        myutils::ResourceInfo(5000, 2000,  0, 1000, 5), // CPU=5,GPU=9
    };

    for (int i = 0; i < sizeof(resourceLevelInfos)/sizeof(resourceLevelInfos[0]); ++i)
    {
        _resourceLevelInfos.push_back(resourceLevelInfos[i]);
    }
    
    parseJson();
    
    _emitter = CCParticleSun::create();
    _emitter->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
    _emitter->setTotalParticles(0);
    _emitter->setPosition(ccp(100, 100));
//cjh    _emitter->pause();
    this->addChild(_emitter);
}

void AnotherScene::onEnter()
{
    CCNode::onEnter();

    int level = _runningOrder[0] - 1;
    int duration = _durations[0];

    myutils::addResource(_parentNode, _emitter, _resourceLevelInfos[level], _audioIDVecs);
    schedule(schedule_selector(AnotherScene::scheduleCallback), duration);
    CCLog("AnotherScene::onEnter(), schedule start %d", duration);
}

void AnotherScene::returnSceneCallback(cocos2d::CCObject* sender)
{
    CCScene* scene = HelloWorld::scene();
//cjh    experimental::AudioEngine::stopAll();
    CCDirector::sharedDirector()->replaceScene(scene);
}

void AnotherScene::scheduleCallback(float dt)
{
    unschedule(schedule_selector(AnotherScene::scheduleCallback));
    CCLog("AnotherScene::scheduleCallback, schedule start %f", dt);
    ++_index;
    if (_index >= _runningOrder.size())
    {
        _parentNode->removeAllChildren();
        _emitter->removeFromParent();
//cjh        experimental::AudioEngine::stopAll();
//        
//        experimental::AudioEngine::end();
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
        
        return;
    }
    
    int nextLevel = _runningOrder[_index] - 1;
    int currentLevel = _runningOrder[_index - 1] - 1;
    int duration = _durations[_index];

    myutils::ResourceInfo currentResourceInfo = _resourceLevelInfos[currentLevel];
    myutils::ResourceInfo nextResourceInfo = _resourceLevelInfos[nextLevel];
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
    CCFileUtils* fileUtils = CCFileUtils::sharedFileUtils();
    std::vector<std::string> oldSearchPaths = fileUtils->getSearchPaths();
    std::vector<std::string> newSearchPaths;
    newSearchPaths.push_back(fileUtils->getWritablePath());
    newSearchPaths.push_back("/sdcard/");

    std::vector<std::string>::iterator iter = oldSearchPaths.begin();
    for (; iter != oldSearchPaths.end(); ++iter)
    {
        newSearchPaths.push_back(*iter);
    }
    fileUtils->setSearchPaths(newSearchPaths);

    unsigned long size = 0;
    unsigned char* data = fileUtils->getFileData("gameScene.json", "rb", &size);

    RapidJsonDocument document;
    document.Parse<0>((char*)data, size);
    
    // get duration
    const RapidJsonValue& duration = document["duration"];
    for (RapidJsonValue::ConstValueIterator iter = duration.Begin(); iter != duration.End(); ++iter)
    {
        _durations.push_back(iter->GetInt());
    }
    
    if (document.HasMember("running_order"))
    {
        const RapidJsonValue& runningOrder = document["running_order"];
        for (RapidJsonValue::ConstValueIterator iter = runningOrder.Begin(); iter != runningOrder.End(); ++ iter)
        {
            _runningOrder.push_back(iter->GetInt());
        }
    }
    else
    {
        _runningOrder.push_back(1);
        _runningOrder.push_back(2);
        _runningOrder.push_back(3);
        _runningOrder.push_back(4);
        _runningOrder.push_back(5);
        _runningOrder.push_back(6);
        _runningOrder.push_back(7);
        _runningOrder.push_back(8);
        _runningOrder.push_back(9);
        _runningOrder.push_back(10);
    }

    CC_SAFE_DELETE_ARRAY(data);
}
