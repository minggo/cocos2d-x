//
//  MyAction.hpp
//  cocos2d_tests
//
//  Created by minggo on 9/26/16.
//
//

#ifndef MY_ACTION_H
#define MY_ACTION_H

#include "cocos2d.h"

class CC_DLL MyCallFunc : public cocos2d::CCCallFunc
{
public:
    typedef void (cocos2d::CCObject::*CallFunc)(int param);

    static MyCallFunc * create(cocos2d::CCObject* target, CallFunc func, int param);

    virtual void execute();
    
    /** initializes the action with the callback and the data to pass as an argument */
    bool initWithFunction(cocos2d::CCObject* target, CallFunc func, int param);
    
protected:
    int _data;

    CCObject* _target;
    CallFunc _selector;
};


#endif /* MY_ACTION_H */
