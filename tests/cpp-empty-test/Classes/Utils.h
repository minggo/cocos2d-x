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

#include "2d/CCNode.h"
#include "2d/CCParticleExamples.h"

namespace myutils
{
    using ResourceInfo = struct ResourceInfo
    {
        int spriteNumber;
        int drawcallNumber;
        int actionNumber;
        int particleNumber;
        int audioNumber;
    };
    
    void addResource(cocos2d::Node* parentNode, cocos2d::ParticleSun *_emitter, const ResourceInfo& resourceInfo, std::vector<int>& audioIDVec);
    unsigned* md5(const char* msg, int len);
}

#endif /* Utils_hpp */
