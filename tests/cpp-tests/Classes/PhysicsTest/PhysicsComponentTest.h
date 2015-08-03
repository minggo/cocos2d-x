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

class PhysicsComponentDemoOneWayPlatform : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoOneWayPlatform);
    
    void onEnter() override;
    virtual std::string title() const override;
    
    bool onContactBegin(cocos2d::PhysicsContact& contact);
};

class PhysicsComponentDemoSlice : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoSlice);
    
    void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    bool slice(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data);
    void clipPoly(cocos2d::PhysicsShapePolygon* shape, cocos2d::Vec2 normal, float distance);
    
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:
    int _sliceTag;
};

class PhysicsComponentDemoBug3988 : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoBug3988);
    
    void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
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

class PhysicsComponentPositionRotationTest : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentPositionRotationTest);
    
    void onEnter() override;
    virtual std::string title() const override;
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

class PhysicsComponentDemoBug5482 : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentDemoBug5482);
    
    void onEnter() override;
    void onExit() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void changeBodyCallback(cocos2d::Ref* sender);
private:
    cocos2d::Sprite* _nodeA;
    cocos2d::Sprite* _nodeB;
    cocos2d::PhysicsBody* _body;
    cocos2d::MenuItemFont* _button;
    bool _bodyInA;
};

class PhysicsComponentFixedUpdate : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentFixedUpdate);
    void onEnter() override;
    void updateStart(float delta);
    void addBall();
    virtual void update(float delta) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class PhysicsComponentTransformTest : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentTransformTest);
    
    void onEnter() override;
    virtual std::string title() const override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:
    cocos2d::Sprite* _parentSprite;
    cocos2d::Layer* _rootLayer;
};

class PhysicsComponentIssue9959 : public PhysicsComponentDemo
{
public:
    CREATE_FUNC(PhysicsComponentIssue9959);
    
    void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};