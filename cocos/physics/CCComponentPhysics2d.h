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

class ComponentPhysics2d : public Component
{
public:
    static ComponentPhysics2d* create();
    
    virtual ~ComponentPhysics2d();

    void beforeSimulation();
    void afterSimulation();
    
    void setPhysicsBody(PhysicsBody* physicsBody);
    PhysicsBody* getPhysicsBody() const;
    
    virtual void onEnter() override;
    virtual void onExit() override;
    
CC_CONSTRUCTOR_ACCESS:
    ComponentPhysics2d();
    ComponentPhysics2d(PhysicsBody* phsicsBody);
    
private:
    // check if physics body is added to a running Node
    inline bool checkState() const;
    
    inline Vec3 calculateOffset();
    
private:
    PhysicsBody *_physicsBody;
    // whether physics body is added to physics world
    bool _addedToPhysicsWorld;
    Vec2 _physicsPositionBeforeSimulation;
    bool _needCaculateOffset;
    Vec3 _offset;
};

NS_CC_END


#endif /* defined(__CCCOMPONENT_PHYSICS2D_H__) */
