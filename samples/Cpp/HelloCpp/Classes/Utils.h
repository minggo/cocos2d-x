//
//  Utils.hpp
//  cocos2d_tests
//
//  Created by minggo on 10/26/16.
//
//

#ifndef Utils_hpp
#define Utils_hpp

#include <vector>

#include "cocos2d.h"

namespace myutils
{
    typedef struct _ResourceInfo
    {
        _ResourceInfo()
        : spriteNumber(0)
        , drawcallNumber(0)
        , actionNumber(0)
        , particleNumber(0)
        , audioNumber(0)
        {}

        _ResourceInfo(
                      int spriteNumber_,
                      int drawcallNumber_,
                      int actionNumber_,
                      int particleNumber_,
                      int audioNumber_)
        : spriteNumber(spriteNumber_)
        , drawcallNumber(drawcallNumber_)
        , actionNumber(actionNumber_)
        , particleNumber(particleNumber_)
        , audioNumber(audioNumber_)
        {}
        int spriteNumber;
        int drawcallNumber;
        int actionNumber;
        int particleNumber;
        int audioNumber;
    }ResourceInfo;
    
    void addResource(cocos2d::CCNode* parentNode, cocos2d::CCParticleSun *_emitter, const ResourceInfo& resourceInfo, std::vector<int>& audioIDVec);
    unsigned* md5(const char* msg, int len);
}

#endif /* Utils_hpp */
