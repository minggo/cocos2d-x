#include "PhysicsComponentTest.h"

#include <cmath>
#include "ui/CocosGUI.h"
#include "physics/CCComponentPhysics2d.h"
#include "../testResource.h"

USING_NS_CC;

PhysicsComponentTests::PhysicsComponentTests()
{
    ADD_TEST_CASE(PhysicsComponentDemoLogoSmash);
    ADD_TEST_CASE(PhysicsComponentDemoPyramidStack);
    ADD_TEST_CASE(PhysicsComponentDemoClickAdd);
    ADD_TEST_CASE(PhysicsComponentDemoRayCast);
    ADD_TEST_CASE(PhysicsComponentDemoActions);
    ADD_TEST_CASE(PhysicsComponentDemoJoints);
    ADD_TEST_CASE(PhysicsComponentDemoPump);
}

namespace
{
    Color4F STATIC_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
    const int DRAG_BODYS_TAG = 0x80;
}

void PhysicsComponentDemo::toggleDebug()
{
#if CC_USE_PHYSICS
    _debugDraw = !_debugDraw;
    _physicsWorld->setDebugDrawMask(_debugDraw ? PhysicsWorld::DEBUGDRAW_ALL : PhysicsWorld::DEBUGDRAW_NONE);
#endif
}


PhysicsComponentDemo::PhysicsComponentDemo()
: _spriteTexture(nullptr)
, _ball(nullptr)
, _debugDraw(false)
{
}

PhysicsComponentDemo::~PhysicsComponentDemo()
{
}

std::string PhysicsComponentDemo::title() const
{
    return "PhysicsComponentTest";
}

void PhysicsComponentDemo::onEnter()
{
    TestCase::onEnter();

    _spriteTexture = SpriteBatchNode::create("Images/grossini_dance_atlas.png", 100)->getTexture();

    // menu for debug layer
    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Toggle debug", CC_CALLBACK_1(PhysicsComponentDemo::toggleDebugCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(Vec2(VisibleRect::right().x - item->getContentSize().width / 2 - 10, VisibleRect::top().y - item->getContentSize().height / 2 - 10));
}

Sprite* PhysicsComponentDemo::addGrossiniAtPosition(Vec2 p, float scale/* = 1.0*/)
{
    CCLOG("Add sprite %0.2f x %02.f", p.x, p.y);

    int posx, posy;

    posx = CCRANDOM_0_1() * 200.0f;
    posy = CCRANDOM_0_1() * 200.0f;

    posx = (posx % 4) * 85;
    posy = (posy % 3) * 121;

    auto sp = Sprite::createWithTexture(_spriteTexture, Rect(posx, posy, 85, 121));

    sp->setScale(scale);
    sp->setPosition(p);
    addPhysicsComponent(sp, PhysicsBody::createBox(Size(48.0f, 108.0f)));
    //addPhysicsComponent(sp, PhysicsBody::createCircle(sp->getContentSize().height/2));
    this->addChild(sp);

    return sp;
}

void PhysicsComponentDemo::toggleDebugCallback(Ref* sender)
{
    toggleDebug();
}

namespace
{
    const int LOGO_WIDTH = 188;
    const int LOGO_HEIGHT = 35;
    const int LOGO_RAW_LENGTH = 24;
    const char LOGO_IMAGE[] =
    {
        15, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -64, 15, 63, -32, -2, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -64, 15, 127, -125, -1, -128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 127, -64, 15, 127, 15, -1, -64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, -64, 15, -2,
        31, -1, -64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, -64, 0, -4, 63, -1, -32, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, -64, 15, -8, 127, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, -1, -64, 0, -8, -15, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -31, -1, -64, 15, -8, -32,
        -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -15, -1, -64, 9, -15, -32, -1, -32, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -15, -1, -64, 0, -15, -32, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 63, -7, -1, -64, 9, -29, -32, 127, -61, -16, 63, 15, -61, -1, -8, 31, -16, 15, -8, 126, 7, -31,
        -8, 31, -65, -7, -1, -64, 9, -29, -32, 0, 7, -8, 127, -97, -25, -1, -2, 63, -8, 31, -4, -1, 15, -13,
        -4, 63, -1, -3, -1, -64, 9, -29, -32, 0, 7, -8, 127, -97, -25, -1, -2, 63, -8, 31, -4, -1, 15, -13,
        -2, 63, -1, -3, -1, -64, 9, -29, -32, 0, 7, -8, 127, -97, -25, -1, -1, 63, -4, 63, -4, -1, 15, -13,
        -2, 63, -33, -1, -1, -32, 9, -25, -32, 0, 7, -8, 127, -97, -25, -1, -1, 63, -4, 63, -4, -1, 15, -13,
        -1, 63, -33, -1, -1, -16, 9, -25, -32, 0, 7, -8, 127, -97, -25, -1, -1, 63, -4, 63, -4, -1, 15, -13,
        -1, 63, -49, -1, -1, -8, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15, -13,
        -1, -65, -49, -1, -1, -4, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15, -13,
        -1, -65, -57, -1, -1, -2, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15, -13,
        -1, -1, -57, -1, -1, -1, 9, -57, -32, 0, 7, -1, -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1,
        -1, -61, -1, -1, -1, -119, -57, -32, 0, 7, -1, -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1,
        -1, -61, -1, -1, -1, -55, -49, -32, 0, 7, -1, -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1,
        -1, -63, -1, -1, -1, -23, -49, -32, 127, -57, -1, -1, -97, -25, -1, -1, 63, -1, -1, -4, -1, 15, -13,
        -1, -1, -63, -1, -1, -1, -16, -49, -32, -1, -25, -1, -1, -97, -25, -1, -1, 63, -33, -5, -4, -1, 15,
        -13, -1, -1, -64, -1, -9, -1, -7, -49, -32, -1, -25, -8, 127, -97, -25, -1, -1, 63, -33, -5, -4, -1,
        15, -13, -1, -1, -64, -1, -13, -1, -32, -49, -32, -1, -25, -8, 127, -97, -25, -1, -2, 63, -49, -13,
        -4, -1, 15, -13, -1, -1, -64, 127, -7, -1, -119, -17, -15, -1, -25, -8, 127, -97, -25, -1, -2, 63,
        -49, -13, -4, -1, 15, -13, -3, -1, -64, 127, -8, -2, 15, -17, -1, -1, -25, -8, 127, -97, -25, -1,
        -8, 63, -49, -13, -4, -1, 15, -13, -3, -1, -64, 63, -4, 120, 0, -17, -1, -1, -25, -8, 127, -97, -25,
        -8, 0, 63, -57, -29, -4, -1, 15, -13, -4, -1, -64, 63, -4, 0, 15, -17, -1, -1, -25, -8, 127, -97,
        -25, -8, 0, 63, -57, -29, -4, -1, -1, -13, -4, -1, -64, 31, -2, 0, 0, 103, -1, -1, -57, -8, 127, -97,
        -25, -8, 0, 63, -57, -29, -4, -1, -1, -13, -4, 127, -64, 31, -2, 0, 15, 103, -1, -1, -57, -8, 127,
        -97, -25, -8, 0, 63, -61, -61, -4, 127, -1, -29, -4, 127, -64, 15, -8, 0, 0, 55, -1, -1, -121, -8,
        127, -97, -25, -8, 0, 63, -61, -61, -4, 127, -1, -29, -4, 63, -64, 15, -32, 0, 0, 23, -1, -2, 3, -16,
        63, 15, -61, -16, 0, 31, -127, -127, -8, 31, -1, -127, -8, 31, -128, 7, -128, 0, 0
    };

    int getPixel(int x, int y)
    {
        return (LOGO_IMAGE[(x >> 3) + y * LOGO_RAW_LENGTH] >> (~x & 0x7)) & 1;
    }

    float frand(void)
    {
        return rand() / RAND_MAX;
    }
}

Sprite* PhysicsComponentDemo::makeBall(Vec2 point, float radius, PhysicsMaterial material)
{
    Sprite* ball = nullptr;
    if (_ball != nullptr)
        ball = Sprite::createWithTexture(_ball->getTexture());
    else
        ball = Sprite::create("Images/ball.png");

    ball->setScale(0.13f * radius);

    addPhysicsComponent(ball, PhysicsBody::createCircle(ball->getContentSize().width / 2, material));
    ball->setPosition(Vec2(point.x, point.y));

    return ball;
}

Sprite* PhysicsComponentDemo::makeBox(Vec2 point, Size size, int color, PhysicsMaterial material)
{
    bool yellow = false;
    if (color == 0)
    {
        yellow = CCRANDOM_0_1() > 0.5f;
    }
    else
    {
        yellow = color == 1;
    }

    auto box = yellow ? Sprite::create("Images/YellowSquare.png") : Sprite::create("Images/CyanSquare.png");
    box->setScaleX(size.width / 100.0f);
    box->setScaleY(size.height / 100.0f);

    addPhysicsComponent(box, PhysicsBody::createBox(box->getContentSize(), material));

    box->setPosition(Vec2(point.x, point.y));

    return box;
}

Sprite* PhysicsComponentDemo::makeTriangle(Vec2 point, Size size, int color, PhysicsMaterial material)
{
    bool yellow = false;
    if (color == 0)
    {
        yellow = CCRANDOM_0_1() > 0.5f;
    }
    else
    {
        yellow = color == 1;
    }

    auto triangle = yellow ? Sprite::create("Images/YellowTriangle.png") : Sprite::create("Images/CyanTriangle.png");

    if (size.height == 0)
    {
        triangle->setScale(size.width / 100.0f);
    }
    else
    {
        triangle->setScaleX(size.width / 50.0f);
        triangle->setScaleY(size.height / 43.5f);
    }

    Vec2 vers[] = { Vec2(0, triangle->getContentSize().height / 2), Vec2(triangle->getContentSize().width / 2, -triangle->getContentSize().height / 2), Vec2(-triangle->getContentSize().width / 2, -triangle->getContentSize().height / 2) };

    addPhysicsComponent(triangle, PhysicsBody::createPolygon(vers, 3, material));
    triangle->setPosition(Vec2(point.x, point.y));

    return triangle;
}

bool PhysicsComponentDemo::onTouchBegan(Touch* touch, Event* event)
{
    auto location = touch->getLocation();
    auto arr = _physicsWorld->getShapes(location);
    
    PhysicsBody* body = nullptr;
    for (auto& obj : arr)
    {
        if ((obj->getBody()->getTag() & DRAG_BODYS_TAG) != 0)
        {
            body = obj->getBody();
            break;
        }
    }

    if (body != nullptr)
    {
        Node* mouse = Node::create();
        auto physicsBody = PhysicsBody::create(PHYSICS_INFINITY, PHYSICS_INFINITY);
        physicsBody->setDynamic(false);
        addPhysicsComponent(mouse, physicsBody);
        mouse->setPosition(location);
        this->addChild(mouse);
        PhysicsJointPin* joint = PhysicsJointPin::construct(physicsBody, body, location);
        joint->setMaxForce(5000.0f * body->getMass());
        _physicsWorld->addJoint(joint);
        _mouses.insert(std::make_pair(touch->getID(), mouse));

        return true;
    }

    return false;
}

void PhysicsComponentDemo::onTouchMoved(Touch* touch, Event* event)
{
    auto it = _mouses.find(touch->getID());

    if (it != _mouses.end())
    {
        it->second->setPosition(touch->getLocation());
    }
}

void PhysicsComponentDemo::onTouchEnded(Touch* touch, Event* event)
{
    auto it = _mouses.find(touch->getID());

    if (it != _mouses.end())
    {
        this->removeChild(it->second);
        _mouses.erase(it);
    }
}

void PhysicsComponentDemo::addPhysicsComponent(Node *node, PhysicsBody *physicsBody)
{
    auto physicsComponent = ComponentPhysics2d::create();
    physicsComponent->setPhysicsBody(physicsBody);
    node->addComponent(physicsComponent);
}

// Implementation of PhysicsComponentDemoLogoSmash

void PhysicsComponentDemoLogoSmash::onEnter()
{
    PhysicsComponentDemo::onEnter();
    
    _physicsWorld->setGravity(Vec2(0, 0));
    _physicsWorld->setUpdateRate(5.0f);
    
    _ball = SpriteBatchNode::create("Images/ball.png", sizeof(LOGO_IMAGE)/sizeof(LOGO_IMAGE[0]));
    addChild(_ball);
    for (int y = 0; y < LOGO_HEIGHT; ++y)
    {
        for (int x = 0; x < LOGO_WIDTH; ++x)
        {
            if (getPixel(x, y))
            {
                float xJitter = 0.05 * frand();
                float yJitter = 0.05 * frand();
                
                Node* ball = makeBall(Vec2(2*(x - LOGO_WIDTH/2 + xJitter) + VisibleRect::getVisibleRect().size.width/2,
                                           2*(LOGO_HEIGHT-y + yJitter) + VisibleRect::getVisibleRect().size.height/2 - LOGO_HEIGHT/2),
                                      0.95f, PhysicsMaterial(0.01f, 0.0f, 0.0f));
                
                auto physicsBody = ball->getComponent<ComponentPhysics2d>()->getPhysicsBody();
                physicsBody->setMass(1.0);
                physicsBody->setMoment(PHYSICS_INFINITY);
                
                _ball->addChild(ball);
            }
        }
    }
    
    
    auto bullet = makeBall(Vec2(400, 0), 10, PhysicsMaterial(PHYSICS_INFINITY, 0, 0));
    bullet->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setVelocity(Vec2(200, 0));
    
    bullet->setPosition(Vec2(-500, VisibleRect::getVisibleRect().size.height/2));
    
    _ball->addChild(bullet);
}

std::string PhysicsComponentDemoLogoSmash::title() const
{
    return "Logo Smash";
}

// Implementation of PhysicsComponentDemoClickAdd

PhysicsComponentDemoClickAdd::~PhysicsComponentDemoClickAdd()
{
    Device::setAccelerometerEnabled(false);
}

void PhysicsComponentDemoClickAdd::onEnter()
{
    PhysicsComponentDemo::onEnter();
    
    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = CC_CALLBACK_2(PhysicsComponentDemoClickAdd::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    Device::setAccelerometerEnabled(true);
    auto accListener = EventListenerAcceleration::create(CC_CALLBACK_2(PhysicsComponentDemoClickAdd::onAcceleration, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(accListener, this);
    
    auto node = Node::create();
    addPhysicsComponent(node, PhysicsBody::createEdgeBox(VisibleRect::getVisibleRect().size));
    node->setPosition(VisibleRect::center());
    this->addChild(node);
    
    addGrossiniAtPosition(VisibleRect::center());
}

std::string PhysicsComponentDemoClickAdd::subtitle() const
{
    return "multi touch to add grossini";
}

void PhysicsComponentDemoClickAdd::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    //Add a new body/atlas sprite at the touched location
    
    for( auto touch: touches)
    {
        auto location = touch->getLocation();
        
        addGrossiniAtPosition( location );
    }
}

void PhysicsComponentDemoClickAdd::onAcceleration(Acceleration* acc, Event* event)
{
    static float prevX=0, prevY=0;
    
#define FILTER_FACTOR 0.05f
    
    float accelX = (float) acc->x * FILTER_FACTOR + (1- FILTER_FACTOR)*prevX;
    float accelY = (float) acc->y * FILTER_FACTOR + (1- FILTER_FACTOR)*prevY;
    
    prevX = accelX;
    prevY = accelY;
    
    auto v = Vec2( accelX, accelY);
    v = v * 200;
    
    _physicsWorld->setGravity(v);
}

void PhysicsComponentDemoPyramidStack::onEnter()
{
	PhysicsComponentDemo::onEnter();

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PhysicsComponentDemoPyramidStack::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PhysicsComponentDemoPyramidStack::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(PhysicsComponentDemoPyramidStack::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    auto node = Node::create();
    addPhysicsComponent(node, PhysicsBody::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0, 50), VisibleRect::rightBottom() + Vec2(0, 50)));
    this->addChild(node);

    auto ball = Sprite::create("Images/ball.png");
    ball->setScale(1);
    ball->setTag(100);
    addPhysicsComponent(ball, PhysicsBody::createCircle(10));
    ball->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
    ball->setPosition(VisibleRect::bottom() + Vec2(0, 60));
    this->addChild(ball);

    scheduleOnce(CC_SCHEDULE_SELECTOR(PhysicsComponentDemoPyramidStack::updateOnce), 3.0);

    for (int i = 0; i < 14; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            auto sp = addGrossiniAtPosition(VisibleRect::bottom() + Vec2((i / 2 - j) * 11, (14 - i) * 23 + 100), 0.2f);
            sp->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
        }
    }
}

void PhysicsComponentDemoPyramidStack::updateOnce(float delta)
{
    auto ball = getChildByTag(100);
    if (ball)
        ball->setScale(ball->getScale() * 3);
}

std::string PhysicsComponentDemoPyramidStack::title() const
{
    return "Pyramid Stack";
}

PhysicsComponentDemoRayCast::PhysicsComponentDemoRayCast()
: _angle(0.0f)
, _node(nullptr)
, _mode(0)
{}

void PhysicsComponentDemoRayCast::onEnter()
{
    PhysicsComponentDemo::onEnter();

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(PhysicsComponentDemoRayCast::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    _physicsWorld->setGravity(Point::ZERO);

    auto node = DrawNode::create();
    addPhysicsComponent(node, PhysicsBody::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0, 50), VisibleRect::rightBottom() + Vec2(0, 50)));
    node->drawSegment(VisibleRect::leftBottom() + Vec2(0, 50), VisibleRect::rightBottom() + Vec2(0, 50), 1, STATIC_COLOR);
    this->addChild(node);

    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Change Mode(any)", CC_CALLBACK_1(PhysicsComponentDemoRayCast::changeModeCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(Vec2(VisibleRect::left().x + 100, VisibleRect::top().y - 10));

    scheduleUpdate();
}

void PhysicsComponentDemoRayCast::changeModeCallback(Ref* sender)
{
    _mode = (_mode + 1) % 3;

    switch (_mode)
    {
    case 0:
        ((MenuItemFont*)sender)->setString("Change Mode(any)");
        break;
    case 1:
        ((MenuItemFont*)sender)->setString("Change Mode(nearest)");
        break;
    case 2:
        ((MenuItemFont*)sender)->setString("Change Mode(multiple)");
        break;

    default:
        break;
    }
}

bool PhysicsComponentDemoRayCast::anyRay(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)
{
    *((Vec2*)data) = info.contact;
    return false;
}

void PhysicsComponentDemoRayCast::update(float delta)
{
    float L = 150.0f;
    Vec2 point1 = VisibleRect::center();
    Vec2 d(L * cosf(_angle), L * sinf(_angle));
    Vec2 point2 = point1 + d;

    removeChild(_node);
    _node = DrawNode::create();
    switch (_mode)
    {
    case 0:
    {
        Vec2 point3 = point2;
        auto func = CC_CALLBACK_3(PhysicsComponentDemoRayCast::anyRay, this);

        _physicsWorld->rayCast(func, point1, point2, &point3);
        _node->drawSegment(point1, point3, 1, STATIC_COLOR);

        if (point2 != point3)
        {
            _node->drawDot(point3, 2, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
        }
        addChild(_node);

        break;
    }
    case 1:
    {
        Vec2 point3 = point2;
        float friction = 1.0f;
        PhysicsRayCastCallbackFunc func = [&point3, &friction](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)->bool
        {
            if (friction > info.fraction)
            {
                point3 = info.contact;
                friction = info.fraction;
            }

            return true;
        };

        _physicsWorld->rayCast(func, point1, point2, nullptr);
        _node->drawSegment(point1, point3, 1, STATIC_COLOR);

        if (point2 != point3)
        {
            _node->drawDot(point3, 2, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
        }
        addChild(_node);

        break;
    }
    case 2:
    {
#define MAX_MULTI_RAYCAST_NUM 5
        Vec2 points[MAX_MULTI_RAYCAST_NUM];
        int num = 0;

        PhysicsRayCastCallbackFunc func = [&points, &num](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)->bool
        {
            if (num < MAX_MULTI_RAYCAST_NUM)
            {
                points[num++] = info.contact;
            }

            return true;
        };

        _physicsWorld->rayCast(func, point1, point2, nullptr);

        _node->drawSegment(point1, point2, 1, STATIC_COLOR);

        for (int i = 0; i < num; ++i)
        {
            _node->drawDot(points[i], 2, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
        }

        addChild(_node);

        break;
    }

    default:
        break;
    }

    _angle += 0.25f * (float)M_PI / 180.0f;
}

void PhysicsComponentDemoRayCast::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    //Add a new body/atlas sprite at the touched location

    for (auto &touch : touches)
    {
        auto location = touch->getLocation();

        float r = CCRANDOM_0_1();

        if (r < 1.0f / 3.0f)
        {
            addChild(makeBall(location, 5 + CCRANDOM_0_1() * 10));
        }
        else if (r < 2.0f / 3.0f)
        {
            addChild(makeBox(location, Size(10 + CCRANDOM_0_1() * 15, 10 + CCRANDOM_0_1() * 15)));
        }
        else
        {
            addChild(makeTriangle(location, Size(10 + CCRANDOM_0_1() * 20, 10 + CCRANDOM_0_1() * 20)));
        }
    }
}

std::string PhysicsComponentDemoRayCast::title() const
{
    return "Ray Cast";
}

void PhysicsComponentDemoActions::onEnter()
{
    PhysicsComponentDemo::onEnter();
    _physicsWorld->setGravity(Vect::ZERO);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PhysicsComponentDemoActions::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PhysicsComponentDemoActions::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(PhysicsComponentDemoActions::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    auto node = Node::create();
    addPhysicsComponent(node, PhysicsBody::createEdgeBox(VisibleRect::getVisibleRect().size));
    node->setPosition(VisibleRect::center());
    this->addChild(node);

    Sprite* sp1 = addGrossiniAtPosition(VisibleRect::center());
    Sprite* sp2 = addGrossiniAtPosition(VisibleRect::left() + Vec2(50, 0));
    Sprite* sp3 = addGrossiniAtPosition(VisibleRect::right() - Vec2(20, 0));
    Sprite* sp4 = addGrossiniAtPosition(VisibleRect::leftTop() + Vec2(50, -50));
    sp4->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setGravityEnable(false);

    sp1->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
    sp2->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
    sp3->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
    sp4->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);

    auto actionTo = JumpTo::create(2, Vec2(100, 100), 50, 4);
    auto actionBy = JumpBy::create(2, Vec2(300, 0), 50, 4);
    auto actionUp = JumpBy::create(2, Vec2(0, 50), 80, 4);
    auto actionByBack = actionBy->reverse();
    auto rotateBy = RotateBy::create(2, 180);
    auto rotateByBack = RotateBy::create(2, -180);

    sp1->runAction(RepeatForever::create(actionUp));
    sp2->runAction(RepeatForever::create(Sequence::create(actionBy, actionByBack, nullptr)));
    sp3->runAction(actionTo);
    sp4->runAction(RepeatForever::create(Sequence::create(rotateBy, rotateByBack, nullptr)));
}

std::string PhysicsComponentDemoActions::title() const
{
    return "Actions";
}

// implementation of PhysicsComponentDemoJoints


void PhysicsComponentDemoJoints::onEnter()
{
    PhysicsComponentDemo::onEnter();
    toggleDebug();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(PhysicsComponentDemo::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PhysicsComponentDemo::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PhysicsComponentDemo::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    float width = (VisibleRect::getVisibleRect().size.width - 10) / 4;
    float height = (VisibleRect::getVisibleRect().size.height - 50) / 4;
    
    Node* node = Node::create();
    PhysicsBody* box = PhysicsBody::create();
    addPhysicsComponent(node, box);
    
    box->setDynamic(false);
    node->setPosition(Point::ZERO);
    this->addChild(node);
    
    PhysicsBody *sp1PhysicsBody = nullptr;
    PhysicsBody *sp2PhysicsBody = nullptr;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Vec2 offset(VisibleRect::leftBottom().x + 5 + j * width + width/2, VisibleRect::leftBottom().y + 50 + i * height + height/2);
            box->addShape(PhysicsShapeEdgeBox::create(Size(width, height), PHYSICSSHAPE_MATERIAL_DEFAULT, 1, offset));
            
            auto sp1 = makeBall(offset - Vec2(30, 0), 10);
            sp1PhysicsBody = sp1->getComponent<ComponentPhysics2d>()->getPhysicsBody();
            sp1PhysicsBody->setTag(DRAG_BODYS_TAG);
            auto sp2 = makeBall(offset + Vec2(30, 0), 10);
            sp2PhysicsBody = sp2->getComponent<ComponentPhysics2d>()->getPhysicsBody();
            sp2PhysicsBody->setTag(DRAG_BODYS_TAG);
            
            this->addChild(sp1);
            this->addChild(sp2);
            switch (i*4 + j)
            {
                case 0:
                case 1:
                {
                    PhysicsJointPin* joint = PhysicsJointPin::construct(sp1PhysicsBody, sp2PhysicsBody, offset);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 2:
                {
                    PhysicsJointDistance* joint = PhysicsJointDistance::construct(sp1PhysicsBody, sp2PhysicsBody, Point::ZERO, Point::ZERO);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 3:
                {
                    PhysicsJointLimit* joint = PhysicsJointLimit::construct(sp1PhysicsBody, sp2PhysicsBody, Point::ZERO, Point::ZERO, 30.0f, 60.0f);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 4:
                {
                    PhysicsJointSpring* joint = PhysicsJointSpring::construct(sp1PhysicsBody, sp2PhysicsBody, Point::ZERO, Point::ZERO, 500.0f, 0.3f);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 5:
                {
                    PhysicsJointGroove* joint = PhysicsJointGroove::construct(sp1PhysicsBody, sp2PhysicsBody, Vec2(30, 15), Vec2(30, -15), Vec2(-30, 0));
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 6:
                {
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp1PhysicsBody, box, sp1->getPosition()));
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp2PhysicsBody, box, sp2->getPosition()));
                    PhysicsJointRotarySpring* joint = PhysicsJointRotarySpring::construct(sp1PhysicsBody, sp2PhysicsBody, 3000.0f, 60.0f);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 7:
                case 8:
                case 10:
                {
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp1PhysicsBody, box, sp1->getPosition()));
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp2PhysicsBody, box, sp2->getPosition()));
                    PhysicsJointRotaryLimit* joint = PhysicsJointRotaryLimit::construct(sp1PhysicsBody, sp2PhysicsBody, 0.0f,(float) M_PI_2);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                case 9:
                {
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp1PhysicsBody, box, sp1->getPosition()));
                    getPhysicsWorld()->addJoint(PhysicsJointPin::construct(sp2PhysicsBody, box, sp2->getPosition()));
                    PhysicsJointGear* joint = PhysicsJointGear::construct(sp1PhysicsBody, sp2PhysicsBody, 0.0f, 2.0f);
                    getPhysicsWorld()->addJoint(joint);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

std::string PhysicsComponentDemoJoints::title() const
{
    return "Joints";
}

void PhysicsComponentDemoPump::onEnter()
{
    PhysicsComponentDemo::onEnter();
    toggleDebug();

    _distance = 0.0f;
    _rotationV = 0.0f;
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PhysicsComponentDemoPump::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PhysicsComponentDemoPump::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(PhysicsComponentDemoPump::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    scheduleUpdate();

    auto node = Node::create();
    addPhysicsComponent(node, PhysicsBody::create());
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setDynamic(false);

    PhysicsMaterial staticMaterial(PHYSICS_INFINITY, 0, 0.5f);
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(50, 0), VisibleRect::leftTop() + Vec2(50, -130), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(190, 0), VisibleRect::leftTop() + Vec2(100, -50), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(100, -50), VisibleRect::leftTop() + Vec2(100, -90), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(50, -130), VisibleRect::leftTop() + Vec2(100, -145), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(100, -145), VisibleRect::leftBottom() + Vec2(100, 80), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(150, -80), VisibleRect::leftBottom() + Vec2(150, 80), staticMaterial, 2.0f));
    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->addShape(PhysicsShapeEdgeSegment::create(VisibleRect::leftTop() + Vec2(150, -80), VisibleRect::rightTop() + Vec2(-100, -150), staticMaterial, 2.0f));

    node->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCategoryBitmask(0x01);
    node->setPosition(Vec2::ZERO);
    this->addChild(node);

    // balls
    for (int i = 0; i < 6; ++i)
    {
        auto ball = makeBall(VisibleRect::leftTop() + Vec2(75 + CCRANDOM_0_1() * 90, 0), 22, PhysicsMaterial(0.05f, 0.0f, 0.1f));
        ball->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(DRAG_BODYS_TAG);
        addChild(ball);
    }
    
    Vec2 vec[4] =
    {
        VisibleRect::leftTop() + Vec2(102, -148),
        VisibleRect::leftTop() + Vec2(148, -161),
        VisibleRect::leftBottom() + Vec2(148, 20),
        VisibleRect::leftBottom() + Vec2(102, 20)
    };

    // pump
    auto pump = Node::create();
    auto center = PhysicsShape::getPolyonCenter(vec, 4);
    pump->setPosition(center);
    addPhysicsComponent(pump, PhysicsBody::createPolygon(vec, 4, PHYSICSBODY_MATERIAL_DEFAULT, -center));
    this->addChild(pump);
    pump->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCategoryBitmask(0x02);
    pump->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setGravityEnable(false);

    // small gear
    auto sgear = Node::create();
    addPhysicsComponent(sgear, PhysicsBody::createCircle(44));
    sgear->setPosition(VisibleRect::leftBottom() + Vec2(125, 0));
    this->addChild(sgear);
    sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCategoryBitmask(0x04);
    sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCollisionBitmask(0x04);
    sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setTag(1);

    _physicsWorld->addJoint(PhysicsJointPin::construct(node->getComponent<ComponentPhysics2d>()->getPhysicsBody(), sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), sgear->getPosition()));
    _physicsWorld->addJoint(PhysicsJointDistance::construct(pump->getComponent<ComponentPhysics2d>()->getPhysicsBody(), sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), Vec2(0, 0), Vec2(0, -44)));

    // big gear
    auto bgear = Node::create();
    addPhysicsComponent(bgear, PhysicsBody::createCircle(100));
    bgear->setPosition(VisibleRect::leftBottom() + Vec2(275, 0));
    this->addChild(bgear);
    bgear->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCategoryBitmask(0x04);

    _physicsWorld->addJoint(PhysicsJointPin::construct(bgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), node->getComponent<ComponentPhysics2d>()->getPhysicsBody(), bgear->getPosition()));
    _physicsWorld->addJoint(PhysicsJointGear::construct(sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), bgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), (float)-M_PI_2, -2.0f));

    // plugger
    Vec2 seg[] = { VisibleRect::leftTop() + Vec2(75, -120), VisibleRect::leftBottom() + Vec2(75, -100) };
    Vec2 segCenter = (seg[1] + seg[0]) / 2;
    seg[1] -= segCenter;
    seg[0] -= segCenter;
    auto plugger = Node::create();
    addPhysicsComponent(plugger, PhysicsBody::createEdgeSegment(seg[0], seg[1], PhysicsMaterial(0.01f, 0.0f, 0.5f), 20));
    plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setDynamic(true);
    plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setMass(30);
    plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setMoment(100000);
    plugger->setPosition(segCenter);
    this->addChild(plugger);
    plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCategoryBitmask(0x02);
    sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody()->setCollisionBitmask(0x04 | 0x01);
    _physicsWorld->addJoint(PhysicsJointPin::construct(node->getComponent<ComponentPhysics2d>()->getPhysicsBody(), plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody(), VisibleRect::leftBottom() + Vec2(75, -90)));
    _physicsWorld->addJoint(PhysicsJointDistance::construct(plugger->getComponent<ComponentPhysics2d>()->getPhysicsBody(), sgear->getComponent<ComponentPhysics2d>()->getPhysicsBody(), Vec2::ZERO, Vec2(44, 0)));
    
}

void PhysicsComponentDemoPump::update(float delta)
{
    for (const auto& body : _physicsWorld->getAllBodies())
    {
        if (body->getTag() == DRAG_BODYS_TAG && body->getPosition().y < 0.0f)
        {
            if (body->getNode()!=nullptr)
            {
                CCLOG("body->getNode() is not nullptr");
                body->getNode()->setPosition(VisibleRect::leftTop() + Vec2(75 + CCRANDOM_0_1() * 90, 0)); 
            }
            else
            {
                CCLOG("body->getNode() is nullptr");
            }
            
            body->setVelocity(Vec2(0, 0));
        }
    }

    PhysicsBody* gear = _physicsWorld->getBody(1);

    if (gear != nullptr)
    {
        if (_distance != 0.0f)
        {
            CCLOG("CALLED");
            _rotationV += _distance / 2500.0f;

            if (_rotationV > 30) _rotationV = 30.0f;
            if (_rotationV < -30) _rotationV = -30.0f;
        }

        gear->setAngularVelocity(_rotationV);
        _rotationV *= 0.995f;
    }
}

bool PhysicsComponentDemoPump::onTouchBegan(Touch* touch, Event* event)
{
    PhysicsComponentDemo::onTouchBegan(touch, event);

    _distance = touch->getLocation().x - VisibleRect::center().x;

    return true;
}

void PhysicsComponentDemoPump::onTouchMoved(Touch* touch, Event* event)
{
    PhysicsComponentDemo::onTouchMoved(touch, event);

    _distance = touch->getLocation().x - VisibleRect::center().x;
}

void PhysicsComponentDemoPump::onTouchEnded(Touch* touch, Event* event)
{
    PhysicsComponentDemo::onTouchEnded(touch, event);

    _distance = 0;
}

std::string PhysicsComponentDemoPump::title() const
{
    return "Pump";
}

std::string PhysicsComponentDemoPump::subtitle() const
{
    return "touch screen on left or right";
}