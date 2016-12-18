#include "HelloWorldScene.h"
#include "json/document.h"

#include "AppMacros.h"
//#include "ui/UIButton.h"
//#include "audio/include/AudioEngine.h"

#include "AnotherScene.h"
#include "MyAction.h"
#include "LoadingScene.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxEngineDataManager.h"
#endif

USING_NS_CC;


ListView* ListView::create(const std::vector<std::string>& itemTitles, const cocos2d::CCPoint& position)
{
    ListView* ret = new ListView();
    ret->init(itemTitles, position);
    ret->autorelease();
    return ret;
}

bool ListView::init(const std::vector<std::string>& itemTitles, const cocos2d::CCPoint& position)
{
    CCNode::init();

    _target = NULL;
    _selector = NULL;
    _isEnabled = true;

    setPosition(ccp(0, 0));
    setAnchorPoint(ccp(0, 0));

    CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCArray* array = CCArray::create();

    for (int i = 0; i < itemTitles.size(); ++i)
    {
        std::string title = itemTitles[i];
        CCMenuItemFont::setFontSize(10);
        CCMenuItemFont* item = CCMenuItemFont::create(title.c_str(), this, menu_selector(ListView::onMenuItemClicked));
        item->setPosition(ccp(position.x + item->getContentSize().width/2, size.height - (position.y + i * (item->getContentSize().height + 10)) + 20));
        item->setTag(i);
        array->addObject(item);
    }

    CCMenu* menu = CCMenu::createWithArray(array);
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(0, 0));
    addChild(menu);

    return true;
}

void ListView::addEventListener(CCObject* target, ListViewSelector selector)
{
    _target = target;
    _selector = selector;
}

void ListView::onMenuItemClicked(cocos2d::CCObject* sender)
{
    CCNode* node = (CCNode*)sender;
    if (_target && _selector && _isEnabled)
    {
        (_target->*_selector)(this, node->getTag());
    }
}


#define GAME_SETTING_MENU_FLAG 10
#define SECOND_MENU_FLAG 11
#define RESOURCE_REQUIREMENT_MENU_FLAG 12
#define FPS_MENU_FLAG 13

#define RESOURCE_PARENT_NODE_FLAG 14

#define SDK_SHOW_COCOS_TEST_BUTTON 0

#define SDK_TEST_MENU_FLAG 15
#define SDK_FPS_MENU_TEST_FLAG 16
#define SDK_EFFECT_MNUE_TEST_FLAG 17
#define SDK_AUDIO_MENU_TEST_FLAG 18
#define SDK_TEST_SECOND_MENU_FLAG 19

//using ResourceLevel = struct ResourceLevel
//{
//    int spriteNumber;
//    int drawcallNumber;
//    int actionNumber;
//    int particleNumber;
//    int audioNumber;
//};

// 游戏资源消耗等级1：ID=3；CPU=0,GPU=0
// 游戏资源消耗等级2：ID=3；CPU=1,GPU=1
// 游戏资源消耗等级3：ID=3；CPU=1,GPU=2
// 游戏资源消耗等级4：ID=3；CPU=2,GPU=3
// 游戏资源消耗等级5：ID=3；CPU=2,GPU=4
// 游戏资源消耗等级6：ID=3；CPU=3,GPU=5
// 游戏资源消耗等级7：ID=3；CPU=3,GPU=6
// 游戏资源消耗等级8：ID=3；CPU=4,GPU=7
// 游戏资源消耗等级9：ID=3；CPU=4,GPU=8
// 游戏资源消耗等级10：ID=3；CPU=5,GPU=9

std::vector<int> HelloWorld::__durations;
std::vector<int> HelloWorld::__runningOrder;
int HelloWorld::__repeatTime = 1;
bool HelloWorld::__randomOrder = false;


myutils::ResourceInfo _resourceLevelCArr[] = {
    // sprite, drawCall, action, particle, audio
    myutils::ResourceInfo(120, 120,  0,   0,   0), // CPU=0,GPU=0
    myutils::ResourceInfo(300, 300,  50, 50, 1), // CPU=1,GPU=1
    myutils::ResourceInfo(350, 350,  50, 200, 1), // CPU=1,GPU=2
    myutils::ResourceInfo(500, 500,  100, 300, 2), // CPU=2,GPU=3
    myutils::ResourceInfo(600, 550,  100, 300, 2), // CPU=2,GPU=4
    myutils::ResourceInfo(800, 650, 200, 400, 3), // CPU=3,GPU=5

    myutils::ResourceInfo(1100, 700, 200, 400, 3), // CPU=3,GPU=6
    myutils::ResourceInfo(1500, 700,  0,   400, 4), // CPU=4,GPU=7
    myutils::ResourceInfo(2000, 900,  0,   400, 4), // CPU=4,GPU=8
    myutils::ResourceInfo(3000, 1200,  250, 500, 5), // CPU=5,GPU=9
};

std::vector<myutils::ResourceInfo> HelloWorld::_resourceLevelVector(_resourceLevelCArr, _resourceLevelCArr + sizeof(_resourceLevelCArr)/sizeof(_resourceLevelCArr[0]));

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene* scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();
    HelloWorld::parseJson();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    this->scheduleUpdate();
    
    _isSDKTestExpanded = false;
    _isGameSettingExpanded = false;
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    
    // whether to enable auto testing
    _autoTestingLabel = CCLabelTTF::create("enable auto test", "", 15);
    CCMenuItemLabel* menuItem = CCMenuItemLabel::create(_autoTestingLabel, this, menu_selector(HelloWorld::autoTestingCallback));
    menuItem->setPosition(ccp(origin.x + 60, origin.y + 30));
    CCMenu* menu = CCMenu::create(menuItem, NULL);
    menu->setPosition(ccp(origin.x, origin.y));
    this->addChild(menu, 9999);
    
    _currentResourceLevelLabel = CCLabelTTF::create("未选择资源等级", "", 10);
    _currentResourceLevelLabel->setPosition(ccp(origin.x + visibleSize.width/2, origin.y + visibleSize.height - 50));
    this->addChild(_currentResourceLevelLabel, 9999);
    
    // init _emitter
    _emitter = CCParticleSun::create();
    _emitter->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
    _emitter->setTotalParticles(0);
    _emitter->setPosition(ccp(100, 100));
//cjh    _emitter->pause();
    this->addChild(_emitter);
    
    // add child to contain resources: node, sprite, particle
    CCNode* resourceParentNode = CCNode::create();
    resourceParentNode->setTag(RESOURCE_PARENT_NODE_FLAG);
    this->addChild(resourceParentNode);

    // game setting menu
    std::vector<std::string> titles;
    titles.push_back("游戏设置");
    ListView* listView = ListView::create(titles, ccp(origin.x + visibleSize.width / 2, origin.y + 40));
    listView->setTag(GAME_SETTING_MENU_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::gameSettingMenuSelectedItemEvent));
    this->addChild(listView);

    // second level menu
    titles.clear();
    titles.push_back("选择等级");
    titles.push_back("切换场景");
    titles.push_back("帧率选择");
#if SDK_SHOW_COCOS_TEST_BUTTON
    titles.push_back("pause");
    titles.push_back("resume");
#endif

    listView = ListView::create(titles, ccp(origin.x + visibleSize.width / 6 * 4, origin.y + 40));
    listView->setTag(SECOND_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener(this, listview_selector(HelloWorld::secondMenuSelectedItemEvent));
    this->addChild(listView);

    // resource requirement menu
    titles.clear();

    titles.push_back("资源等级1");
    titles.push_back("资源等级2");
    titles.push_back("资源等级3");
    titles.push_back("资源等级4");
    titles.push_back("资源等级5");
    titles.push_back("资源等级6");
    titles.push_back("资源等级7");
    titles.push_back("资源等级8");
    titles.push_back("资源等级9");
    titles.push_back("资源等级10");

    listView = ListView::create(titles, ccp(origin.x + visibleSize.width / 6 * 5, origin.y + 40));
    listView->setTag(RESOURCE_REQUIREMENT_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener(this, listview_selector(HelloWorld::resourceRequirementMenuSelectedItemEvent));
    this->addChild(listView);

    // fps menu
    titles.clear();
    titles.push_back("25");
    titles.push_back("30");
    titles.push_back("40");
    titles.push_back("60");

    listView = ListView::create(titles, ccp(origin.x + visibleSize.width / 6 * 5, origin.y + 40));
    listView->setTag(FPS_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener(this, listview_selector(HelloWorld::fpsSelectedMenuSelectedItemEvent));
    this->addChild(listView);
    
    // sdk test
    titles.clear();
    titles.push_back("SDK test");
    listView = ListView::create(titles, ccp(origin.x, origin.y + 40));
    listView->setTag(SDK_TEST_MENU_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::SDKTestSelectedItemEvent));
    this->addChild(listView);
    
    // sdk second menu
    titles.clear();
    titles.push_back("fps");
    titles.push_back("effect");
    titles.push_back("audio");
    listView = ListView::create(titles, ccp(origin.x + 80, origin.y + 40));
    listView->setTag(SDK_TEST_SECOND_MENU_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::SDKSecondMenuSelectedItemEvent));
    listView->setVisible(false);
    this->addChild(listView);
    
    // sdk fps
    titles.clear();
    titles.push_back("25");
    titles.push_back("30");
    titles.push_back("40");
    titles.push_back("60");
#if SDK_SHOW_COCOS_TEST_BUTTON
    titles.push_back("默认值");
#endif

    listView = ListView::create(titles, ccp(origin.x + 160, origin.y + 40));
    listView->setTag(SDK_FPS_MENU_TEST_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::SDKFPSSelectedItemEvent));
    listView->setVisible(false);
    this->addChild(listView);
    
    // sdk effect
    titles.clear();
    titles.push_back("0.0");
    titles.push_back("0.2");
    titles.push_back("0.4");
    titles.push_back("0.6");
    titles.push_back("0.8");
    titles.push_back("1.0");

    listView = ListView::create(titles, ccp(origin.x + 160, origin.y + 40));
    listView->setTag(SDK_EFFECT_MNUE_TEST_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::SDKEffectSelectedItemEvent));
    listView->setVisible(false);
    this->addChild(listView);
    
    // skd audio
    titles.clear();
    titles.push_back("on");
    titles.push_back("off");

    listView = ListView::create(titles, ccp(origin.x + 160, origin.y + 40));
    listView->setTag(SDK_AUDIO_MENU_TEST_FLAG);
    listView->addEventListener(this, listview_selector(HelloWorld::SDKAudioSelectedItemEvent));
    listView->setVisible(false);
    this->addChild(listView);

    return true;
}

void HelloWorld::update(float dt)
{
    if (_currentResourceLevel == -1)
        return;
    
    // do some operation to simulate game logic
//    Mat4 mat4;
//    Mat4::createPerspective(60, 4.0 / 3.0, 0.1, 100, &mat4);
//    int cpuLevel[10] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5 };
//    int loopTime = 2000 * cpuLevel[_currentResourceLevel];
//    for (int i = 0; i < loopTime; ++i)
//    {
//        mat4.multiply(mat4);
//        mat4.inverse();
//        mat4.multiply(mat4);
//        mat4.inverse();
//    }
}

int HelloWorld::getRandomIndex(std::vector<int>* array)
{
    int randomIndex = rand() % array->size();
    int ret = (*array)[randomIndex];
    array->erase(array->begin() + randomIndex);
    return ret;
}

void HelloWorld::autoTestingCallback(cocos2d::CCObject* sender)
{
    if (_enableAutoTesting)
    {
        _autoTestingLabel->setString("disable auto test");

        this->disableAllListViews();
        
        CCArray* actions = CCArray::create();
        size_t loopSize = HelloWorld::__runningOrder.size();
        for (int i = 0; i < HelloWorld::__repeatTime; ++i)
        {
            std::vector<int> orders = HelloWorld::__runningOrder;
            for (size_t j = 0; j < loopSize; ++j)
            {
                int index = HelloWorld::__randomOrder ? HelloWorld::getRandomIndex(&orders) : orders[j];
                index -= 1;
                actions->addObject(MyCallFunc::create(this, (MyCallFunc::CallFunc)&HelloWorld::actionCallback, index));
                actions->addObject(CCDelayTime::create(HelloWorld::__durations[index]));
            }
        }
        actions->addObject(CCCallFunc::create(this, callfunc_selector(HelloWorld::lastActionCallback)));
        CCSequence* sequence = CCSequence::create(actions);
        this->runAction(sequence);
    }
    else
    {
        // stop sequence
        this->stopAllActions();
        
        _autoTestingLabel->setString("enable auto test");

        this->enableAllListViews();
    }
    
    this->_enableAutoTesting = !this->_enableAutoTesting;
}

void HelloWorld::actionCallback(int index)
{
    this->addResources(index);
}

void HelloWorld::lastActionCallback()
{
    CCNode* resourceParentNode = this->getChildByTag(RESOURCE_PARENT_NODE_FLAG);
    resourceParentNode->removeAllChildren();
    
//cjh    experimental::AudioEngine::stopAll();
    _emitter->setVisible(false);
    this->enableAllListViews();
    
    _autoTestingLabel->setString("enable auto test");
    _enableAutoTesting = true;
    _currentResourceLevelLabel->setString("未选择资源等级");
    _currentResourceLevel = -1;
}

void HelloWorld::gameSettingMenuSelectedItemEvent(ListView* listView, int index)
{
    {
        if (_isGameSettingExpanded)
        {
            // hide second & third menu
            this->getChildByTag(SECOND_MENU_FLAG)->setVisible(false);
            this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG)->setVisible(false);
            this->getChildByTag(FPS_MENU_FLAG)->setVisible(false);
        }
        else
        {
            // show second menu
            this->getChildByTag(SECOND_MENU_FLAG)->setVisible(true);
        }
        _isGameSettingExpanded = !_isGameSettingExpanded;
    }
}

void HelloWorld::secondMenuSelectedItemEvent(ListView* listView, int index)
{
    {
        switch (index) {
            case 0:
                // 选择等级
                this->getChildByTag(FPS_MENU_FLAG)->setVisible(false);
                static_cast<ListView*>(this->getChildByTag(FPS_MENU_FLAG))->setEnabled(false);
                
                this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG)->setVisible(true);
                static_cast<ListView*>(this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG))->setEnabled(true);
                break;
                
            case 1:
                // 切换场景
            {
                CCScene* scene = LoadingScene::create(AnotherScene::create());
//cjh                experimental::AudioEngine::stopAll();
                CCDirector::sharedDirector()->replaceScene(scene);
            }
                break;
            case 2:
                // 帧率选择
                this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG)->setVisible(false);
                static_cast<ListView*>(this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG))->setEnabled(false);
                
                this->getChildByTag(FPS_MENU_FLAG)->setVisible(true);
                static_cast<ListView*>(this->getChildByTag(FPS_MENU_FLAG))->setEnabled(true);
                break;

            case 3:
                // Director::pause
                getChildByTag(SECOND_MENU_FLAG)->setVisible(false);
                CCDirector::sharedDirector()->pause();
                break;

            case 4:
                // Director::resume
                getChildByTag(SECOND_MENU_FLAG)->setVisible(false);
                CCDirector::sharedDirector()->resume();
                break;
            default:
                break;
        }
    }
}

void HelloWorld::resourceRequirementMenuSelectedItemEvent(ListView* listView, int index)
{
    {
        this->addResources(index);
        listView->setVisible(false);
        getChildByTag(SECOND_MENU_FLAG)->setVisible(false);
        _isGameSettingExpanded = false;
    }
}

void HelloWorld::fpsSelectedMenuSelectedItemEvent(ListView* listView, int index)
{
    {
        switch (index) {
            case 0:
                // 25
                CCDirector::sharedDirector()->setAnimationInterval(1 / 25.0);
                break;
            case 1:
                // 30
                CCDirector::sharedDirector()->setAnimationInterval(1 / 30.0);
                break;
            case 2:
                // 40
                CCDirector::sharedDirector()->setAnimationInterval(1 / 40.0);
                break;
            case 3:
                // 60
                CCDirector::sharedDirector()->setAnimationInterval(1 / 60.0);
                break;
                
            default:
                break;
        }
        listView->setVisible(false);
        getChildByTag(SECOND_MENU_FLAG)->setVisible(false);
        _isGameSettingExpanded = false;
    }
}

// SDK related test: event call back

void HelloWorld::SDKTestSelectedItemEvent(ListView* listView, int index)
{
    {
        if (_isSDKTestExpanded)
        {
            this->getChildByTag(SDK_TEST_SECOND_MENU_FLAG)->setVisible(false);
            this->getChildByTag(SDK_FPS_MENU_TEST_FLAG)->setVisible(false);
            this->getChildByTag(SDK_AUDIO_MENU_TEST_FLAG)->setVisible(false);
            this->getChildByTag(SDK_EFFECT_MNUE_TEST_FLAG)->setVisible(false);
        }
        else
        {
            this->getChildByTag(SDK_TEST_SECOND_MENU_FLAG)->setVisible(true);
        }
        
        _isSDKTestExpanded = !_isSDKTestExpanded;
    }
}

void HelloWorld::SDKSecondMenuSelectedItemEvent(ListView* listView, int index)
{
    {
        switch (index) {
            case 0:
                // fps
                this->enableSDKEffect(false);
                this->enableSDKAudio(false);
                this->enableSDKFPS(true);
                
                break;
            case 1:
                // effect
                this->enableSDKAudio(false);
                this->enableSDKFPS(false);
                this->enableSDKEffect(true);
                
                break;
            case 2:
                // audio
                this->enableSDKFPS(false);
                this->enableSDKEffect(false);
                this->enableSDKAudio(true);
                break;
                
            default:
                break;
        }
    }
}

void HelloWorld::SDKFPSSelectedItemEvent(ListView* listView, int index)
{
    {
        int fps = 0;

        switch (index) {
            case 0:
                fps = 25;
                break;
            case 1:
                fps = 30;
                break;
            case 2:
                fps = 40;
                break;
            case 3:
                fps = 60;
                break;
            case 4:
                fps = -1; // Pass -1 to set the default value
            default:
                break;
        }
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        EngineDataManager::notifyGameStatus(EngineDataManager::TEST_CHANGE_FPS_RATE, fps, 0);
#endif
        listView->setVisible(false);
        getChildByTag(SDK_TEST_SECOND_MENU_FLAG)->setVisible(false);
        _isSDKTestExpanded = false;
    }
}

void HelloWorld::SDKEffectSelectedItemEvent(ListView* listView, int index)
{
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        EngineDataManager::notifyGameStatus(EngineDataManager::TEST_CHANGE_SPECIAL_EFFECTS, index, 0);
#endif
        listView->setVisible(false);
        getChildByTag(SDK_TEST_SECOND_MENU_FLAG)->setVisible(false);
        _isSDKTestExpanded = false;
    }
}

void HelloWorld::SDKAudioSelectedItemEvent(ListView* listView, int index)
{
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        EngineDataManager::notifyGameStatus(EngineDataManager::TEST_MUTE_ENABLED, index, 0);
#endif
        listView->setVisible(false);
        getChildByTag(SDK_TEST_SECOND_MENU_FLAG)->setVisible(false);
        _isSDKTestExpanded = false;
    }
}


void HelloWorld::addResources(int level)
{
    assert(level < 10);
    
    CCLOG("add resources level: %d", level);
    
    _currentResourceLevel = level;
    
    // remove previous resources
    CCNode* resourceParentNode = this->getChildByTag(RESOURCE_PARENT_NODE_FLAG);
    resourceParentNode->removeAllChildren();
    
    // stop all audios
//cjh    experimental::AudioEngine::stopAll();

    myutils::ResourceInfo resourceLevel = HelloWorld::_resourceLevelVector[level];
    myutils::addResource(resourceParentNode, _emitter, resourceLevel, _audioIDVec);

    char levelString[100] = {0};
    sprintf(levelString, "当前资源等级:%d", level + 1);
    _currentResourceLevelLabel->setString(levelString);
}

void HelloWorld::enableAllListViews()
{
    static_cast<ListView*>(this->getChildByTag(GAME_SETTING_MENU_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(SECOND_MENU_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(FPS_MENU_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(SDK_TEST_MENU_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(SDK_EFFECT_MNUE_TEST_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(SDK_FPS_MENU_TEST_FLAG))->setEnabled(true);
    static_cast<ListView*>(this->getChildByTag(SDK_AUDIO_MENU_TEST_FLAG))->setEnabled(true);
}

void HelloWorld::disableAllListViews()
{
    static_cast<ListView*>(this->getChildByTag(GAME_SETTING_MENU_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(SECOND_MENU_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(FPS_MENU_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(SDK_TEST_MENU_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(SDK_EFFECT_MNUE_TEST_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(SDK_FPS_MENU_TEST_FLAG))->setEnabled(false);
    static_cast<ListView*>(this->getChildByTag(SDK_AUDIO_MENU_TEST_FLAG))->setEnabled(false);
}

void HelloWorld::enableSDKAudio(bool enabled)
{
    this->getChildByTag(SDK_AUDIO_MENU_TEST_FLAG)->setVisible(enabled);
    static_cast<ListView*>(this->getChildByTag(SDK_AUDIO_MENU_TEST_FLAG))->setEnabled(enabled);
}

void HelloWorld::enableSDKEffect(bool enabled)
{
    this->getChildByTag(SDK_EFFECT_MNUE_TEST_FLAG)->setVisible(enabled);
    static_cast<ListView*>(this->getChildByTag(SDK_EFFECT_MNUE_TEST_FLAG))->setEnabled(enabled);
}

void HelloWorld::enableSDKFPS(bool enabled)
{
    this->getChildByTag(SDK_FPS_MENU_TEST_FLAG)->setVisible(enabled);
    static_cast<ListView*>(this->getChildByTag(SDK_FPS_MENU_TEST_FLAG))->setEnabled(enabled);
}

typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> RapidJsonValue;

void HelloWorld::parseJson()
{
    static bool parsed = false;
    if (!parsed)
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

        CCLog("writable path is %s", fileUtils->getWritablePath().c_str());

        unsigned long size = 0;
        unsigned char* data = fileUtils->getFileData("configure.json", "rb", &size);

        RapidJsonDocument document;
        document.Parse<0>((char*)data, size);


        assert(document.HasMember("duration"));
        assert(document.HasMember("repeat_time"));
        assert(document.HasMember("random_order"));
        assert(document.HasMember("show_status"));
        
        // whether to show status(fps, drawcall and so on)
        bool showStatus = document["show_status"].GetBool();
        CCDirector::sharedDirector()->setDisplayStats(showStatus);
        
        // get duration
        const RapidJsonValue& duration = document["duration"];
        for (RapidJsonValue::ConstValueIterator iter = duration.Begin(); iter != duration.End(); ++iter)
        {
            HelloWorld::__durations.push_back(iter->GetInt());
        }
        
        if (document.HasMember("running_order"))
        {
            // ignore repeat and random
            HelloWorld::__randomOrder = false;
            HelloWorld::__repeatTime = 1;
            
            const RapidJsonValue& runningOrder = document["running_order"];
            for (RapidJsonValue::ConstValueIterator iter = runningOrder.Begin(); iter != runningOrder.End(); ++ iter)
            {
                HelloWorld::__runningOrder.push_back(iter->GetInt());
            }
        }
        else
        {
            __runningOrder.push_back(1);
            __runningOrder.push_back(2);
            __runningOrder.push_back(3);
            __runningOrder.push_back(4);
            __runningOrder.push_back(5);
            __runningOrder.push_back(6);
            __runningOrder.push_back(7);
            __runningOrder.push_back(8);
            __runningOrder.push_back(9);
            __runningOrder.push_back(10);

            HelloWorld::__repeatTime = document["repeat_time"].GetInt();
            HelloWorld::__randomOrder = document["random_order"].GetBool();
        }
        
        parsed = true;

        CC_SAFE_DELETE_ARRAY(data);
    }
}
