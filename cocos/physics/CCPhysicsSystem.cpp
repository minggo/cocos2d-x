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

#include "CCPhysicsSystem.h"
#include "physics/CCComponentPhysics2d.h"
#include "physics/CCPhysicsWorld.h"

//class PhysicsSystem : public System<PhysicsSystem>
//{
//public:
//    /**
//     * will do physics simulation
//     */
//    virtual void update(float dt) override;
//    
//    void addComponent(ComponentPhysics2d* componentPhsics2d);
//    
//private:
//    std::vector<ComponentPhysics2d*> components;
//};

NS_CC_BEGIN

PhysicsSystem::PhysicsSystem()
{
    _physicsWorld = PhysicsWorld::construct();
}

PhysicsSystem::~PhysicsSystem()
{
    delete _physicsWorld;
}

void PhysicsSystem::update(float dt)
{
    // update physics position
    for (auto component : _components)
        component->beforeSimulation();
    
    // do simulation
    _physicsWorld->update(dt, false);
    
    // update Node's position
    for (auto component : _components)
        component->afterSimulation();
}

void PhysicsSystem::addComponent(ComponentPhysics2d* componentPhsics2d)
{
    _components.push_back(componentPhsics2d);
}

void PhysicsSystem::removeComponent(ComponentPhysics2d* componentPhsics2d)
{
    auto iter = std::find(_components.begin(), _components.end(), componentPhsics2d);
    if (iter != _components.end())
        _components.erase(iter);
}

PhysicsWorld* PhysicsSystem::getPhysicsWorld() const
{
    return _physicsWorld;
}

NS_CC_END
