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
#include "2d/CCNode.h"

NS_CC_BEGIN

ComponentPhysics2d* ComponentPhysics2d::create()
{
    auto ret = new (std::nothrow) ComponentPhysics2d();
    if (ret) ret->autorelease();
    
    return ret;
}

ComponentPhysics2d::ComponentPhysics2d() : _physicsBody(nullptr)
{}

ComponentPhysics2d::ComponentPhysics2d(PhysicsBody* physicsBody)
: _addedToPhysicsWorld(false)
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
    
    // update physics body's position
    
    auto&& nodeToWorldTransform = _owner->getNodeToWorldTransform();
    Vec3 scale;
    Quaternion rotation;
    nodeToWorldTransform.decompose(&scale, &rotation, nullptr);
    
    // set scale
    _physicsBody->setScale(scale.x, scale.y);
    
    // set rotation
    Vec3 zAxis(0, 0, 1);
    _physicsBody->setRotation(CC_RADIANS_TO_DEGREES(rotation.toAxisAngle(&zAxis)));
    
    // set position
    Vec3 offset = calculateOffset();
    nodeToWorldTransform.transformPoint(&offset);
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
    Mat4 parentToWorldTransfrom;
    if (_owner->getParent())
        parentToWorldTransfrom = _owner->getParent()->getNodeToParentTransform();
    else
        parentToWorldTransfrom = _owner->getNodeToParentTransform();
    
    float rotation = _physicsBody->getRotation();
    Vec3 rotationVec(rotation, 0, 0);
    parentToWorldTransfrom.getInversed().transformVector(&rotationVec);
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

bool ComponentPhysics2d::checkState()
{
    if (_owner->getScene() == nullptr)
        return false;
    
    // don't update position if physics body is disabled
    if (_physicsBody == nullptr || (!_physicsBody->isEnabled()))
        return false;
    
    return true;
}

namespace
{
    bool runOneTime = true;
    Vec3 offset(0, 0, 0);
}

Vec3 ComponentPhysics2d::calculateOffset() const
{
    if (runOneTime)
    {
        auto contentSize = _owner->getContentSize();
        offset.x = 0.5 * contentSize.width;
        offset.y = 0.5 * contentSize.height;
        runOneTime = false;
    }
    
    return offset;
}

NS_CC_END
