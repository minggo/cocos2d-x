#pragma once

#include <map>

#include "../BaseTest.h"


DEFINE_TEST_SUITE(PhysicsComponentTests);

class PhysicsComponentDemo : public TestCase
{
public:
    PhysicsComponentDemo();
    virtual ~PhysicsComponentDemo();
    
    virtual void onEnter() override;
    virtual std::string title() const override;
    
    void toggleDebugCallback(cocos2d::Ref* sender);
    
    cocos2d::Sprite* addGrossiniAtPosition(cocos2d::Vec2 p, float scale = 1.0);
    cocos2d::Sprite* makeBall(cocos2d::Vec2 point, float radius, cocos2d::PhysicsMaterial material = cocos2d::PHYSICSBODY_MATERIAL_DEFAULT);
    cocos2d::Sprite* makeBox(cocos2d::Vec2 point, cocos2d::Size size, int color = 0, cocos2d::PhysicsMaterial material = cocos2d::PHYSICSBODY_MATERIAL_DEFAULT);
    cocos2d::Sprite* makeTriangle(cocos2d::Vec2 point, cocos2d::Size size, int color = 0, cocos2d::PhysicsMaterial material = cocos2d::PHYSICSBODY_MATERIAL_DEFAULT);
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void toggleDebug();
    
protected:
    void addPhysicsComponent(cocos2d::Node *node, cocos2d::PhysicsBody *physicsBody);
    
    cocos2d::Texture2D* _spriteTexture;
    cocos2d::SpriteBatchNode* _ball;
    std::unordered_map<int, cocos2d::Node*> _mouses;
    bool _debugDraw;
};

class PhysicsComponentDemoLogoSmash : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoLogoSmash);
    
    void onEnter() override;
    virtual std::string title() const override;
};

class PhysicsComponentDemoClickAdd : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoClickAdd);
    
    virtual ~PhysicsComponentDemoClickAdd();
    void onEnter() override;
    virtual std::string subtitle() const override;
    
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* event);
};
