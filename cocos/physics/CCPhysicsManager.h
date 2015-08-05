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
class Scene;
class Node;

/// @cond DO_NOT_SHOW

/**
 * Now `PhysicsManager` belongs to a `Scene`. `Scene` will create `PhysicsManager` automatically.
 * The reason to design like this are:
 * - PhysicsManager keeps physics world, it is reasonale that a scene has its own physics world.
 *   It is useful that when a scene is active again, you don't have to create physics world and 
 *   do all related things again.
 * - Keep compatibility
 */
class CC_DLL PhysicsManager
{
public:
    /**
     * Create a PhysicsManager with `Scene`. `Scene` will create it automatically,
     * so don't create it yourself.
     *
     * @scene The scene this `PhysicsManager` belongs to.
     */
    PhysicsManager(Scene *scene);
    ~PhysicsManager();
    
    /**
     * Do physics simulation. It is invoked by `Scene`, don't invoke it yourself.
     *
     */
    void update(float dt);
    
    PhysicsWorld* getPhysicsWorld() const;
    
    void addPhysicsComponent(ComponentPhysics2d* componentPhsics2d);
    void removePhysicsComponent(ComponentPhysics2d* componentPhsics2d);
    
private:
    void beforeSimulation(Node *node);
    void afterSimulation(Node* node);
private:
    std::vector<ComponentPhysics2d*> _components;
    PhysicsWorld *_physicsWorld;
    Scene *_scene;
};

/// @endcond DO_NOT_SHOW

NS_CC_END

#endif /* defined(__CC_PHYSICS_SYSTEM_H__) */
