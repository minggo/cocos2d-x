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

#ifndef __CCCOMPONENT_PHYSICS2D_H__
#define __CCCOMPONENT_PHYSICS2D_H__

#include "platform/CCPlatformMacros.h"
#include "2d/CCComponent.h"

NS_CC_BEGIN

class PhysicsBody;

class CC_DLL ComponentPhysics2d : public Component
{
public:
    static ComponentPhysics2d* create();
    static ComponentPhysics2d* create(PhysicsBody *physicsBody);
    
    virtual ~ComponentPhysics2d();

    void beforeSimulation();
    void afterSimulation();
    
    void setPhysicsBody(PhysicsBody *physicsBody);
    PhysicsBody* getPhysicsBody() const;
    
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void onAdd() override;
    virtual void onRemove() override;
    
CC_CONSTRUCTOR_ACCESS:
    ComponentPhysics2d();
    ComponentPhysics2d(PhysicsBody *phsicsBody);
    
private:
    void removePhysicsBody();
    void addToPhysicsManager();
    void removeFromPhysicsManager();
    
private:
    PhysicsBody *_physicsBody;
    Vec2 _physicsPositionBeforeSimulation;
    // offset between owner's center point and down left point
    Vec3 _offset;
    Mat4 _nodeToWorldTransform;
    Vec3 _ownerScale;
    Quaternion _ownerRotation;
};

NS_CC_END


#endif /* defined(__CCCOMPONENT_PHYSICS2D_H__) */
