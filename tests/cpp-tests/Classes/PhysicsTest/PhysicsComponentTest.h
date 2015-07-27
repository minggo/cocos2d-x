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

class PhysicsComponentDemoPyramidStack : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoPyramidStack);

    void onEnter() override;
    void updateOnce(float delta);
    virtual std::string title() const override;
};

class PhysicsComponentDemoRayCast : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoRayCast);

    PhysicsComponentDemoRayCast();

    void onEnter() override;
    virtual std::string title() const override;
    void update(float delta) override;
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

    void changeModeCallback(cocos2d::Ref* sender);

    bool anyRay(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data);

private:
    float _angle;
    cocos2d::DrawNode* _node;
    int _mode;
};

class PhysicsComponentDemoActions : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoActions);

    void onEnter() override;
    virtual std::string title() const override;
};

class PhysicsComponentDemoJoints : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoJoints);
    
    void onEnter() override;
    virtual std::string title() const override;
};

class PhysicsComponentDemoPump : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoPump);

    void onEnter() override;
    void update(float delta) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    float _distance;
    float _rotationV;
};

class PhysicsComponentContactTest : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentContactTest);

    void onEnter() override;
    void resetTest();
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onDecrease(cocos2d::Ref* sender);
    void onIncrease(cocos2d::Ref* sender);

private:
    int _yellowBoxNum;
    int _blueBoxNum;
    int _yellowTriangleNum;
    int _blueTriangleNum;
};

class PhysicsComponentSetGravityEnableTest : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentSetGravityEnableTest);
    
    void onEnter() override;
    void onScheduleOnce(float delta);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};