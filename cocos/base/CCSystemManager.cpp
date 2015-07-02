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

#include "CCSystemManager.h"
#include "base/CCScheduler.h"

NS_CC_BEGIN

// BaseSystem implementation

size_t BaseSystem::_typeCount = 0;

BaseSystem::BaseSystem() : _enabled(true)
{}

BaseSystem::~BaseSystem()
{}

void BaseSystem::setEnabled(bool value)
{
    _enabled = value;
}

bool BaseSystem::getEnabled() const
{
    return _enabled;
}

// SchedulerSystem implementation

ScheduleSystem::ScheduleSystem()
{
    _scheduler = new (std::nothrow) Scheduler();
}

ScheduleSystem::~ScheduleSystem()
{
    delete _scheduler;
}

void ScheduleSystem::update(float dt)
{
    _scheduler->update(dt);
}


// SystemManager implementation

SystemManager::SystemManager()
{
    // TBD
    // create renderer system
    // Should create PhysicsSystem here too, but don't want to SystemManager depend on PhysicsSystem, how to fix it?
    
    // add ScheduleSystem at begin, then it will be invoked first
    addSystem<ScheduleSystem>(new ScheduleSystem);
}

SystemManager::~SystemManager()
{
    for (auto system : _systems)
        system->release();
    
    // TBD delete renderer system
}

void SystemManager::removeSystem(BaseSystem* system)
{
    CC_ASSERT(system != nullptr);
    
    for (auto s : _typeSystemMap)
    {
        if (s.second == system)
        {
            s.second->release();
            _typeSystemMap.erase(s.first);
            break;
        }
    }
    
    auto pos = std::find(_systems.begin(), _systems.end(), system);
    _systems.erase(pos);
}

void SystemManager::update(float dt)
{
    for (auto system : _systems)
    {
        if (system->getEnabled())
            system->update(dt);
    }
    
    // TBD: invoke renderer system
}

NS_CC_END
