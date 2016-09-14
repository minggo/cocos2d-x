#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "ui/UIButton.h"

USING_NS_CC;

#define GAME_SETTING_MENU_FLAG 10
#define SECOND_MENU_FLAG 11
#define RESOURCE_REQUIREMENT_MENU_FLAG 12
#define FPS_MENU_FLAG 13


Scene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

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
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // init menu

    // game setting menu
    std::vector<std::string> titles = { "游戏设置" };
    auto listView = this->createListView(titles, Vec2(origin.x + visibleSize.width / 2, origin.y + 50));
    listView->setTag(GAME_SETTING_MENU_FLAG);
    listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(HelloWorld::gameSettingMenuSelectedItemEvent, this));
    this->addChild(listView);

    // second level menu
    titles = { "选择等级", "切换场景", "帧率选择" };
    listView = this->createListView(titles, Vec2(origin.x + visibleSize.width / 6 * 4, origin.y + 50));
    listView->setTag(SECOND_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(HelloWorld::secondMenuSelectedItemEvent, this));
    this->addChild(listView);
    
    // resource requirement menu
    titles = { "资源需求等级1", "资源需求等级2", "资源需求等级3", "资源需求等级4", "资源需求等级5",
               "资源需求等级6", "资源需求等级7", "资源需求等级8", "资源需求等级9", "资源需求等级10" };
    listView = this->createListView(titles, Vec2(origin.x + visibleSize.width / 6 * 5, origin.y + 50));
    listView->setTag(RESOURCE_REQUIREMENT_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(HelloWorld::resourceRequirementMenuSelectedItemEvent, this));
    this->addChild(listView);
    
    // fps menu
    titles = { "25", "30", "40", "60" };
    listView = this->createListView(titles, Vec2(origin.x + visibleSize.width / 6 * 5, origin.y + 50));
    listView->setTag(FPS_MENU_FLAG);
    listView->setVisible(false);
    listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(HelloWorld::fpsSelectedMenuSelectedItemEvent, this));
    this->addChild(listView);
    
    return true;
}

void HelloWorld::gameSettingMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type)
{
    if (type == cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END)
    {
        // show second menu
        this->getChildByTag(SECOND_MENU_FLAG)->setVisible(true);
    }
}

void HelloWorld::secondMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type)
{
    if (type == cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END)
    {
        auto listView = static_cast<ui::ListView*>(sender);
        switch (listView->getCurSelectedIndex()) {
            case 0:
                // 选择等级
                this->getChildByTag(FPS_MENU_FLAG)->setVisible(false);
                this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG)->setVisible(true);
                break;
                
            case 1:
                // 切换场景
            {
                auto scene = HelloWorld::scene();
                Director::getInstance()->replaceScene(scene);
            }
                break;
            case 2:
                // 帧率选择
                this->getChildByTag(RESOURCE_REQUIREMENT_MENU_FLAG)->setVisible(false);
                this->getChildByTag(FPS_MENU_FLAG)->setVisible(true);
                break;
                
            default:
                break;
        }
    }
}

void HelloWorld::resourceRequirementMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type)
{
    if (type == ui::ListView::EventType::ON_SELECTED_ITEM_END)
    {
        auto listView = static_cast<ui::ListView*>(sender);
        auto resourceRequirementLevel = listView->getCurSelectedIndex;
        switch (resourceRequirementLevel) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
                
            default:
                break;
        }
    }
}

void HelloWorld::fpsSelectedMenuSelectedItemEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type)
{
    if (type == ui::ListView::EventType::ON_SELECTED_ITEM_END)
    {
        auto listView = static_cast<ui::ListView*>(sender);
        switch (listView->getCurSelectedIndex()) {
            case 0:
                // 25
                Director::getInstance()->setAnimationInterval(1 / 25.0);
                break;
            case 1:
                // 30
                Director::getInstance()->setAnimationInterval(1 / 30.0);
                break;
            case 2:
                // 40
                Director::getInstance()->setAnimationInterval(1 / 40.0);
                break;
            case 3:
                // 60
                Director::getInstance()->setAnimationInterval(1 / 60.0);
                break;
                
            default:
                break;
        }
    }
}

cocos2d::ui::ListView* HelloWorld::createListView(const std::vector<std::string>& itemTitles, const cocos2d::Vec2& position)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto listView = ui::ListView::create();
    listView->setDirection(ui::ScrollView::Direction::VERTICAL);
    listView->setPosition(position);
    listView->setContentSize(Size(visibleSize.width / 7, visibleSize.height / 4 * 3));
    
    for (const auto& item: itemTitles)
    {
        auto button = ui::Button::create("button.png", "buttonHighlighted.png");
        button->setTitleText(item);
        button->setScale9Enabled(true);
        button->setContentSize(button->getTitleRenderer()->getContentSize());
        listView->pushBackCustomItem(button);
    }
    
    return listView;
}
