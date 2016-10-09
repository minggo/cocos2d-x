//
//  MyAction.hpp
//  cocos2d_tests
//
//  Created by minggo on 9/26/16.
//
//

#ifndef MY_ACTION_H
#define MY_ACTION_H

#include <functional>

#include "2d/CCActionInstant.h"

class CC_DLL  MyCallFunc : public cocos2d::CallFunc
{
public:
    static MyCallFunc * create(const std::function<void(int)>& func, int param);

    virtual void execute() override;
    
    /** initializes the action with the callback and the data to pass as an argument */
    bool initWithFunction(const std::function<void(int)>& func, int param);
    
protected:
    int _data;
    std::function<void(int)> _function;
};


#endif /* MY_ACTION_H */
