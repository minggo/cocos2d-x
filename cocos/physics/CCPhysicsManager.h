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

#ifndef __CC_PHYSICS_SYSTEM_H__
#define __CC_PHYSICS_SYSTEM_H__

#include <vector>

#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class ComponentPhysics2d;
class PhysicsWorld;

class CC_DLL PhysicsManager
{
public:
    PhysicsManager();
    ~PhysicsManager();
    
    /**
     * will do physics simulation
     */
    void update(float dt);
    
    PhysicsWorld* getPhysicsWorld() const;
    
    // don't invoke these functions, because Node will invoke them automatically
    void addPhysicsComponent(ComponentPhysics2d* componentPhsics2d);
    void removePhysicsComponent(ComponentPhysics2d* componentPhsics2d);
    
private:
    std::vector<ComponentPhysics2d*> _components;
    PhysicsWorld *_physicsWorld;
};

NS_CC_END

#endif /* defined(__CC_PHYSICS_SYSTEM_H__) */
