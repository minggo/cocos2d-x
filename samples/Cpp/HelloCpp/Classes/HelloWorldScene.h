#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <vector>

#include "Utils.h"

class ListView : public cocos2d::CCNode
{
public:
    static ListView* create(const std::vector<std::string>& itemTitles, const cocos2d::CCPoint& position);

    typedef void (cocos2d::CCObject::*ListViewSelector)(ListView* listView, int index);

    void addEventListener(cocos2d::CCObject* target, ListViewSelector selector);

    void onMenuItemClicked(cocos2d::CCObject* sender);

    void setEnabled(bool enabled) { _isEnabled = enabled; }

private:
    bool init(const std::vector<std::string>& itemTitles, const cocos2d::CCPoint& position);

    CCObject* _target;
    ListViewSelector _selector;
    bool _isEnabled;
};

#define listview_selector(selector) ((ListView::ListViewSelector)(&selector))

class HelloWorld : public cocos2d::CCLayer
{
public:
    static cocos2d::CCScene* scene();
    
    HelloWorld() :_emitter(NULL),
                  _enableAutoTesting(true),
                  _autoTestingLabel(NULL),
                  _currentResourceLevel(-1){}
    
    virtual bool init();
    virtual void update(float dt);
    
    void gameSettingMenuSelectedItemEvent(ListView* listView, int index);
    void secondMenuSelectedItemEvent(ListView* listView, int index);
    void resourceRequirementMenuSelectedItemEvent(ListView* listView, int index);
    void fpsSelectedMenuSelectedItemEvent(ListView* listView, int index);


    void autoTestingCallback(cocos2d::CCObject* sender);
    void actionCallback(int index);
    void lastActionCallback();
    
    void SDKTestSelectedItemEvent(ListView* listView, int index);
    void SDKSecondMenuSelectedItemEvent(ListView* listView, int index);
    void SDKFPSSelectedItemEvent(ListView* listView, int index);
    void SDKEffectSelectedItemEvent(ListView* listView, int index);
    void SDKAudioSelectedItemEvent(ListView* listView, int index);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    
    static void parseJson();

    void addResources(int level);
    void enableAllListViews();
    void disableAllListViews();
    void enableSDKAudio(bool enabled);
    void enableSDKEffect(bool enabled);
    void enableSDKFPS(bool enabled);
    
    static int getRandomIndex(std::vector<int>* array);
    static std::vector<myutils::ResourceInfo> _resourceLevelVector;
    static std::vector<int> __durations;
    static std::vector<int> __runningOrder;
    static int __repeatTime;
    static bool __randomOrder;
    
    cocos2d::CCParticleSun *_emitter;
    bool _enableAutoTesting;
    cocos2d::CCLabelTTF *_autoTestingLabel;
    cocos2d::CCLabelTTF *_currentResourceLevelLabel;
    int _currentResourceLevel;
    std::vector<int> _audioIDVec;
    
    bool _isSDKTestExpanded;
    bool _isGameSettingExpanded;
};

#endif // __HELLOWORLD_SCENE_H__
