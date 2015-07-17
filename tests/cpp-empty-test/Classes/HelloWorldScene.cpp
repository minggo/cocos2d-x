#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "physics/CCComponentPhysics2d.h"
#include "physics/CCPhysicsManager.h"

USING_NS_CC;


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
    

    auto sprite = Sprite::create("bang.png");
    auto physicsComponent = ComponentPhysics2d::create();
    auto physicsBody = PhysicsBody::createBox(sprite->getContentSize());
//    physicsBody->setGravityEnable(false);
    physicsComponent->setPhysicsBody(physicsBody);
    sprite->addComponent(physicsComponent);
    
    sprite->setAnchorPoint(Vec2(0.3, 0.3));
    sprite->runAction(RotateTo::create(10, 180));
    
    sprite->setPosition(200, 200);
    addChild(sprite);
    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
