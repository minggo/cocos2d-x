//
//  MyAction.cpp
//  cocos2d_tests
//
//  Created by minggo on 9/26/16.
//
//

#include "MyAction.h"

MyCallFunc * MyCallFunc::create(cocos2d::CCObject* target, CallFunc func, int param)
{
    MyCallFunc *ret = new (std::nothrow) MyCallFunc();
    
    if (ret && ret->initWithFunction(target, func, param))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

bool MyCallFunc::initWithFunction(cocos2d::CCObject* target, CallFunc func, int param)
{
    _target = target;
    _selector = func;
    _data = param;
    return true;
}

void MyCallFunc::execute()
{
    if (_target && _selector)
    {
        (_target->*_selector)(_data);
    }
}
