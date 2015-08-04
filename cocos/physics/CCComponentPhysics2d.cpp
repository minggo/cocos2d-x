/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "physics/CCComponentPhysics2d.h"
#include "physics/CCPhysicsBody.h"
#include "physics/CCPhysicsManager.h"
#include "2d/CCNode.h"
#include "base/CCDirector.h"

NS_CC_BEGIN

ComponentPhysics2d* ComponentPhysics2d::create()
{
    auto ret = new (std::nothrow) ComponentPhysics2d();
    if (ret) ret->autorelease();
    
    return ret;
}

ComponentPhysics2d* ComponentPhysics2d::create(PhysicsBody *physicsBody)
{
    auto ret = new (std::nothrow) ComponentPhysics2d(physicsBody);
    if (ret) ret->autorelease();
    
    return ret;
}

ComponentPhysics2d::ComponentPhysics2d()
: _physicsBody(nullptr)
, _physicsRotation(0.0f)
, _ownerOriginRotation(0.0f)
{}

ComponentPhysics2d::ComponentPhysics2d(PhysicsBody* physicsBody)
: _physicsBody(nullptr) // should set to null first to invoke setPhysicsBody()
, _physicsRotation(0.0f)
, _ownerOriginRotation(0.0f)
{
    setPhysicsBody(physicsBody);
}

ComponentPhysics2d::~ComponentPhysics2d()
{
    removePhysicsBody();
}

void ComponentPhysics2d::beforeSimulation()
{
    CC_ASSERT(_physicsBody != nullptr);
    
    _nodeToWorldTransform = _owner->getNodeToWorldTransform();
    
    // set scale
    Vec3 scale;
    _nodeToWorldTransform.getScale(&scale);
    _physicsBody->setScale(scale.x, scale.y);
    
    // set rotation
    if (_owner->getParent())
    {
        _physicsRotation = getPhysicsRotation(_owner->getParent()) + _owner->getRotation();
    }
    _physicsBody->setRotation(_physicsRotation - _ownerOriginRotation);
    
    // set position
    auto worldPosition = _ownerCenterOffset;
    _nodeToWorldTransform.transformPoint(&worldPosition);
    _physicsBody->setPosition(Vec2(worldPosition.x, worldPosition.y));
    
    auto positionInParent = worldPosition;
    getParentToWorldTransform().getInversed().transformPoint(&positionInParent);
    _offset.x = positionInParent.x - _owner->getPosition().x;
    _offset.y = positionInParent.y - _owner->getPosition().y;
}

void ComponentPhysics2d::afterSimulation()
{
    // set Node position
    auto worldPosition = _physicsBody->getPosition();
    Vec3 positionInParent(worldPosition.x, worldPosition.y, 0);
    getParentToWorldTransform().getInversed().transformPoint(&positionInParent);
    _owner->setPosition(positionInParent.x - _offset.x, positionInParent.y - _offset.y);
    
    // set Node rotation
    _physicsRotation = _physicsBody->getRotation();
    _owner->setRotation(_physicsRotation - getPhysicsRotation(_owner->getParent()) + _ownerOriginRotation);
}

void ComponentPhysics2d::setPhysicsBody(PhysicsBody *physicsBody)
{
    CC_ASSERT(physicsBody != nullptr);
    
    if (physicsBody != _physicsBody)
    {
        removeFromPhysicsManager();
        
        _physicsBody = physicsBody;
        _physicsBody->retain();
        _physicsBody->_componentBelongsTo = this;
        
        addToPhysicsManager();
    }
}

PhysicsBody* ComponentPhysics2d::getPhysicsBody() const
{
    return _physicsBody;
}

void ComponentPhysics2d::onEnter()
{
    CC_ASSERT(_physicsBody != nullptr);
    
    addToPhysicsManager();
}

void ComponentPhysics2d::onExit()
{
    removeFromPhysicsManager();
}

void ComponentPhysics2d::onAdd()
{
    auto contentSize = _owner->getContentSize();
    _ownerCenterOffset.x = 0.5 * contentSize.width;
    _ownerCenterOffset.y = 0.5 * contentSize.height;
    
    _ownerOriginRotation = _owner->getRotation();
}

void ComponentPhysics2d::onRemove()
{
    removePhysicsBody();
}

void ComponentPhysics2d::removePhysicsBody()
{
    removeFromPhysicsManager();
    
    if (_physicsBody)
    {
        _physicsBody->_componentBelongsTo = nullptr;
        _physicsBody->release();
        _physicsBody = nullptr;
    }
}

void ComponentPhysics2d::addToPhysicsManager()
{
    if (_owner && _owner->getScene())
        _owner->getScene()->getPhysicsManager()->addPhysicsComponent(this);
}

void ComponentPhysics2d::removeFromPhysicsManager()
{
    if (_owner && _owner->getScene())
        _owner->getScene()->getPhysicsManager()->removePhysicsComponent(this);
}

Mat4 ComponentPhysics2d::getParentToWorldTransform() const
{
    if (_owner->getParent())
        return _owner->getParent()->getNodeToWorldTransform();
    else
        return _owner->getNodeToWorldTransform();
}

float ComponentPhysics2d::getPhysicsRotation(Node *node) const
{
    if (!node)
        return 0.0f;
    
    auto physicsComponent = node->getComponent<ComponentPhysics2d>();
    if (physicsComponent)
    {
        return physicsComponent->_physicsRotation;
    }
    else
    {
        auto parent = node->getParent();
        if (parent)
            return getPhysicsRotation(parent) + node->getRotation();
        else
            return 0.0f;
    }
}

NS_CC_END
