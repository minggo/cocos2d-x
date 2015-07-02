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

#ifndef __CC_SYSTEMMANAGER__
#define __CC_SYSTEMMANAGER__

#include <vector>
#include <unordered_map>
#include "platform/CCPlatformMacros.h"
#include "base/CCRef.h"

NS_CC_BEGIN

class BaseSystem : public Ref
{
public:
    virtual ~BaseSystem();
    BaseSystem();
    
    virtual void update(float dt) = 0;
    
    void setEnabled(bool value);
    bool getEnabled() const;
    
protected:
    static size_t _typeCount;
    bool _enabled;
};

template<typename T>
class System : public BaseSystem
{
public:
    
    static size_t getType()
    {
        static size_t type = _typeCount++;
        return type;
    }
};

class Scheduler;

class ScheduleSystem : public System<ScheduleSystem>
{
    // make Director to access '_scheudler' to keep compatibility
    friend class Director;
    // make SystemManager to delete SchedulerSystem
    friend class SystemManager;
public:
    virtual void update(float dt) override;
    
private:
    ScheduleSystem();
    ~ScheduleSystem();
    
private:
    Scheduler *_scheduler;
};

/** SystemManager is to manage all systems, it may include
  * - physics system
  * - schedule system
  * - render system (TBD)
  * ...
  *
  * Physics system is added internally, and it will be invoked after schedule system.
  * You can added your own system, the invoked sequence is 'schedule system', 'developer added system', 'physics system', 'render system'.
  */
class SystemManager
{
    friend class Director;
public:
    
    template<typename T>
    void addSystem(BaseSystem* system)
    {
        CC_ASSERT(system != nullptr);
        
        system->retain();
        _systems.push_back(system);
        _typeSystemMap.insert(std::make_pair(T::getType(), system));
    }
    
    template<typename T>
    T* getSystem() const
    {
        auto iter = _typeSystemMap.find(T::getType());
        return static_cast<T*>(iter->second);
    }
    
    void removeSystem(BaseSystem* system);
    
    void update(float dt);
    
private:
    SystemManager();
    ~SystemManager();
    
private:
    std::vector<BaseSystem*> _systems;
    std::unordered_map<size_t, BaseSystem*> _typeSystemMap;
    
};


NS_CC_END

#endif /* defined(__CC_SYSTEMMANAGER__) */
