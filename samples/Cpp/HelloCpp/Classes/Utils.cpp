//
//  Utils.cpp
//  cocos2d_tests
//
//  Created by minggo on 10/26/16.
//
//

#include "Utils.h"

//cjh #include "audio/include/AudioEngine.h"

#define DARW_CALL_TAG 100

using namespace cocos2d;

namespace myutils
{
    void addResource(cocos2d::CCNode* parentNode, cocos2d::CCParticleSun *_emitter, const ResourceInfo& resourceInfo, std::vector<int> &audioIDVec)
    {
        // add Sprites and run actions if needed
        CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
        CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
        int spriteNumber = resourceInfo.spriteNumber;
        int actionNumber = resourceInfo.actionNumber;
        int drawcallNumber = resourceInfo.drawcallNumber;
        int drawcall = 0;
        
        if (spriteNumber > 0)
        {
            for (int i = 0; i < spriteNumber; ++i)
            {
                CCSprite *sprite;
                if (drawcall < drawcallNumber)
                {
                    char spritePath[100] = {0};
                    sprintf(spritePath, "sprite%d.png", drawcall % 2);
                    sprite = CCSprite::create(spritePath);
                    sprite->setTag(DARW_CALL_TAG);
                }
                else
                {
                    sprite = CCSprite::create("sprite0.png");
                }
                ++drawcall;
                
                float x = origin.x + visibleSize.width * (std::rand() * 1.0 / RAND_MAX);
                float y = origin.y + visibleSize.height * (std::rand() * 1.0 / RAND_MAX);
                
                float maxRectSize = std::max(sprite->getContentSize().width, sprite->getContentSize().height);
                
                x = std::min(x, origin.x + visibleSize.width - maxRectSize/2);
                x = std::max(x, origin.x + maxRectSize/2);
                
                y = std::min(y, origin.y + visibleSize.height - maxRectSize/2);
                y = std::max(y, origin.y + maxRectSize/2);
                
                sprite->setPosition(ccp(x, y));
                parentNode->addChild(sprite);
                
                if (i < actionNumber)
                {
                    sprite->runAction(CCRepeatForever::create(CCRotateBy::create(3, 360)));
                }
            }
        }
        else
        {
            int removedDrawcall = 0;
            if (drawcallNumber < 0)
                removedDrawcall = -drawcallNumber;
            
            // remove sprite to reduce drawcall
            CCArray* children = parentNode->getChildren();
            int removedSpriteNum = -spriteNumber;
            int spriteRemovedByDrawcall = 0;
            int spriteRemoved = 0;
            if (removedDrawcall > 0)
            {
                CCObject* obj = NULL;
                CCARRAY_FOREACH(children, obj)
                {
                    CCNode* node = (CCNode*)obj;
                    if (node->getTag() == DARW_CALL_TAG)
                    {
                        parentNode->removeChild(node);
                        ++spriteRemovedByDrawcall;
                        ++spriteRemoved;
                        
                        if (spriteRemovedByDrawcall >= removedDrawcall)
                            break;
                    }
                }
            }
            
            children = parentNode->getChildren();
            // remove the rest sprites
            CCObject* obj = NULL;
            CCARRAY_FOREACH(children, obj)
            {
                CCNode* node = (CCNode*)obj;
                if (node->getTag() != DARW_CALL_TAG)
                {
                    parentNode->removeChild(node);
                    ++spriteRemoved;
                    
                    if (spriteRemoved >= removedSpriteNum)
                        break;
                }
            }

        }
        
        
        // add particles
        _emitter->setVisible(true);
//cjh        _emitter->resume();
        _emitter->setTotalParticles(resourceInfo.particleNumber);
        
        // play audioes
        int audioNumber = resourceInfo.audioNumber;
        if (audioNumber > 0)
        {
            for (int i = 0 ; i < audioNumber; ++i)
            {
//cjh                auto audioPath = StringUtils::format("effect%d.mp3", i % 10);
//                int audioID = experimental::AudioEngine::play2d(audioPath.c_str(), true);
//                audioIDVec.push_back(audioID);
            }
        }
        else
        {
            int stoppedAudioNum = -audioNumber;
            for (int i = 0; i < stoppedAudioNum; ++i)
            {
//cjh                experimental::AudioEngine::stop(audioIDVec[0]);
//                audioIDVec.erase(audioIDVec.begin());
            }
        }
    }
    
    /**************************************************************************************************
     md5 algorithm implementation, copy from http://www.ccodechamp.com/c-program-of-md5-encryption-algorithm-c-codechamp/
     **************************************************************************************************/
    
    typedef union uwb {
        unsigned w;
        unsigned char b[4];
    } MD5union;
    
    typedef unsigned DigestArray[4];
    
    unsigned func0( unsigned abcd[] ){
        return ( abcd[1] & abcd[2]) | (~abcd[1] & abcd[3]);}
    
    unsigned func1( unsigned abcd[] ){
        return ( abcd[3] & abcd[1]) | (~abcd[3] & abcd[2]);}
    
    unsigned func2( unsigned abcd[] ){
        return  abcd[1] ^ abcd[2] ^ abcd[3];}
    
    unsigned func3( unsigned abcd[] ){
        return abcd[2] ^ (abcd[1] |~ abcd[3]);}
    
    typedef unsigned (*DgstFctn)(unsigned a[]);
    
    /*Use binary integer part of the sines of integers (Radians) as constants*/
    /*or*/
    /* Hardcode the below table but i want generic code that why coded it
     using calcuate table function
     k[ 0.. 3] := { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee }
     k[ 4.. 7] := { 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 }
     k[ 8..11] := { 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be }
     k[12..15] := { 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 }
     k[16..19] := { 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa }
     k[20..23] := { 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 }
     k[24..27] := { 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed }
     k[28..31] := { 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a }
     k[32..35] := { 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c }
     k[36..39] := { 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 }
     k[40..43] := { 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 }
     k[44..47] := { 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 }
     k[48..51] := { 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 }
     k[52..55] := { 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 }
     k[56..59] := { 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 }
     k[60..63] := { 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 }*/
    
    unsigned *calctable( unsigned *k)
    {
        double s, pwr;
        int i;
        
        pwr = pow( 2, 32);
        for (i=0; i<64; i++) {
            s = fabs(sin(1+i));
            k[i] = (unsigned)( s * pwr );
        }
        return k;
    }
    
    /*Rotate Left r by N bits
     or
     We can directly hardcode below table but as i explained above we are opting
     generic code so shifting the bit manually.
     r[ 0..15] := {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22}
     r[16..31] := {5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20}
     r[32..47] := {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23}
     r[48..63] := {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21}
     */
    unsigned rol( unsigned r, short N )
    {
        unsigned  mask1 = (1<<N) -1;
        return ((r>>(32-N)) & mask1) | ((r<<N) & ~mask1);
    }

    
    unsigned* md5(const char* msg, int mlen)
    {
        /*Initialize Digest Array as A , B, C, D */
        static DigestArray h0 = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
        static DgstFctn ff[] = { &func0, &func1, &func2, &func3 };
        static short M[] = { 1, 5, 3, 7 };
        static short O[] = { 0, 1, 5, 0 };
        static short rot0[] = { 7,12,17,22};
        static short rot1[] = { 5, 9,14,20};
        static short rot2[] = { 4,11,16,23};
        static short rot3[] = { 6,10,15,21};
        static short *rots[] = {rot0, rot1, rot2, rot3 };
        static unsigned kspace[64];
        static unsigned *k;
        
        static DigestArray h;
        DigestArray abcd;
        DgstFctn fctn;
        short m, o, g;
        unsigned f;
        short *rotn;
        union {
            unsigned w[16];
            char     b[64];
        }mm;
        int os = 0;
        int grp, grps, q, p;
        unsigned char *msg2;
        
        if (k==NULL) k= calctable(kspace);
        
        for (q=0; q<4; q++) h[q] = h0[q];   // initialize
        
        {
            grps  = 1 + (mlen+8)/64;
            msg2 = (unsigned char *)malloc( 64*grps);
            memcpy( msg2, msg, mlen);
            msg2[mlen] = (unsigned char)0x80;
            q = mlen + 1;
            while (q < 64*grps){ msg2[q] = 0; q++ ; }
            {
                MD5union u;
                u.w = 8*mlen;
                q -= 8;
                memcpy(msg2+q, &u.w, 4 );
            }
        }
        
        for (grp=0; grp<grps; grp++)
        {
            memcpy( mm.b, msg2+os, 64);
            for(q=0;q<4;q++) abcd[q] = h[q];
            for (p = 0; p<4; p++) {
                fctn = ff[p];
                rotn = rots[p];
                m = M[p]; o= O[p];
                for (q=0; q<16; q++) {
                    g = (m*q + o) % 16;
                    f = abcd[1] + rol( abcd[0]+ fctn(abcd) + k[q+16*p] + mm.w[g], rotn[q%4]);
                    
                    abcd[0] = abcd[3];
                    abcd[3] = abcd[2];
                    abcd[2] = abcd[1];
                    abcd[1] = f;
                }
            }
            for (p=0; p<4; p++)
                h[p] += abcd[p];
            os += 64;
        }
        return h;
    }
}
