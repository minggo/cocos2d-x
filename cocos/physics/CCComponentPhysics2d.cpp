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

ComponentPhysics2d::ComponentPhysics2d()
: _physicsBody(nullptr)
{}

ComponentPhysics2d::ComponentPhysics2d(PhysicsBody* physicsBody)
{
    CC_ASSERT(physicsBody != nullptr);
    
    _physicsBody = physicsBody;
    _physicsBody->retain();
}

ComponentPhysics2d::~ComponentPhysics2d()
{
    CC_SAFE_RELEASE(_physicsBody);
}

void ComponentPhysics2d::beforeSimulation()
{
    if (!checkState())
        return;
    
    _nodeToWorldTransform = _owner->getNodeToWorldTransform();
    Vec3 scale;
    _nodeToWorldTransform.decompose(&scale, &_ownerRotation, nullptr);
    
    // set scale
    // because there is floating point precision problem when caculation scale value in Mat4::decompose()
    if (fabs(scale.x - _ownerScale.x) > MATH_EPSILON
        || fabs(scale.y - _ownerScale.y) > MATH_EPSILON)
    {
        _ownerScale = scale;
        _physicsBody->setScale(_ownerScale.x, _ownerScale.y);
    }
    
    // set rotation
    Vec3 zAxis(0, 0, 1);
    _physicsBody->setRotation(CC_RADIANS_TO_DEGREES(_ownerRotation.toAxisAngle(&zAxis)));
    
    // set position
    Vec3 offset = _offset;
    _nodeToWorldTransform.transformPoint(&offset);
    _physicsPositionBeforeSimulation.x = offset.x;
    _physicsPositionBeforeSimulation.y = offset.y;
    _physicsBody->setPosition(_physicsPositionBeforeSimulation);
}

void ComponentPhysics2d::afterSimulation()
{
    if (!checkState())
        return;
    
    // update Node's position
    
    // set position
    auto pos = _physicsBody->getPosition();
    Vec2 offset(pos.x - _physicsPositionBeforeSimulation.x, pos.y - _physicsPositionBeforeSimulation.y);
    auto ownerPosition = _owner->getPosition();
    _owner->setPosition(ownerPosition.x + offset.x, ownerPosition.y + offset.y);
    
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
}

void ComponentPhysics2d::setPhysicsBody(PhysicsBody *physicsBody)
{
    // can not change physics body
    CC_ASSERT(_physicsBody == nullptr);
    CC_ASSERT(physicsBody != nullptr);
    
    _physicsBody = physicsBody;
    _physicsBody->retain();
}

PhysicsBody* ComponentPhysics2d::getPhysicsBody() const
{
    return _physicsBody;
}

void ComponentPhysics2d::onEnter()
{
    // should set physics body
    CC_ASSERT(_physicsBody != nullptr);
    
    _owner->getScene()->getPhysicsManager()->addPhysicsComponent(this);
}

void ComponentPhysics2d::onExit()
{
    _owner->getScene()->getPhysicsManager()->removePhysicsComponent(this);
}

void ComponentPhysics2d::onAdd()
{
    auto contentSize = _owner->getContentSize();
    _offset.x = 0.5 * contentSize.width;
    _offset.y = 0.5 * contentSize.height;
}

bool ComponentPhysics2d::checkState() const
{
    if (_owner->getScene() != Director::getInstance()->getRunningScene())
        return false;
    
    // don't update position if physics body is disabled
    if (!_physicsBody->isEnabled())
        return false;
    
    return true;
}

NS_CC_END
