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
, _ownerScale(1, 1, 1)
{}

ComponentPhysics2d::ComponentPhysics2d(PhysicsBody* physicsBody)
: _ownerScale(1, 1, 1)
, _physicsBody(nullptr) // should set to null first to invoke setPhysicsBody()
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
    
    bool transfromDirty = _owner->getNodeToWorldTransfromDirty();
    _nodeToWorldTransform = _owner->getNodeToWorldTransform();
    
    if (transfromDirty)
    {
        _nodeToWorldTransform.decompose(&_ownerScale, &_ownerRotation, nullptr);
        
        // set scale
        _physicsBody->setScale(_ownerScale.x, _ownerScale.y);
        
        // set rotation
        Vec3 zAxis;
        float angle = CC_RADIANS_TO_DEGREES(_ownerRotation.toAxisAngle(&zAxis));
        if (fabs(zAxis.z - 1.0f) < FLT_EPSILON)
            angle = 360 - angle;
        _physicsBody->setRotation(angle);
    }
    
    // set position
    _physicsPositionBeforeSimulation = _offset;
    _nodeToWorldTransform.transformPoint(&_physicsPositionBeforeSimulation);
    if (transfromDirty)
        _physicsBody->setPosition(Vec2(_physicsPositionBeforeSimulation.x, _physicsPositionBeforeSimulation.y));
}

void ComponentPhysics2d::afterSimulation()
{
    // update Node's position
    
    // set position
    auto pos = _physicsBody->getPosition();
    Vec2 offset(pos.x - _physicsPositionBeforeSimulation.x, pos.y - _physicsPositionBeforeSimulation.y);
    auto ownerPosition = _owner->getPosition();
    _owner->setPosition(ownerPosition.x + offset.x, ownerPosition.y + offset.y);
    
//    _owner->setPosition(_physicsBody->getPosition());
    
    // set rotation
    Mat4 parentToWorldTransform;
    if (_owner->getParent())
        parentToWorldTransform = _owner->getParent()->getNodeToParentTransform();
    else
        parentToWorldTransform = _owner->getNodeToParentTransform();
    
    float rotation = _physicsBody->getRotation();
    Vec3 rotationVec(rotation, 0, 0);
    parentToWorldTransform.getInversed().transformVector(&rotationVec);
    _owner->setRotation(rotationVec.x);
    
//    _owner->setRotation(_physicsBody->getRotation());
//    CCLOG("physics rotation: %f", _physicsBody->getRotation());
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
    _offset.x = 0.5 * contentSize.width;
    _offset.y = 0.5 * contentSize.height;
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

NS_CC_END
