//
//  MyAction.cpp
//  cocos2d_tests
//
//  Created by minggo on 9/26/16.
//
//

#include "MyAction.h"

MyCallFunc * MyCallFunc::create(const std::function<void(int)> &func, int param)
{
    MyCallFunc *ret = new (std::nothrow) MyCallFunc();
    
    if (ret && ret->initWithFunction(func, param))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MyCallFunc::initWithFunction(const std::function<void(int)> &func, int param)
{
    _function = func;
    _data = param;
    return true;
}

void MyCallFunc::execute()
{
    _function(_data);
}
