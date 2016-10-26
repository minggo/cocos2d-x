//
//  Utils.cpp
//  cocos2d_tests
//
//  Created by minggo on 10/26/16.
//
//

#include "Utils.h"

#include "base/CCDirector.h"
#include "base/ccUTF8.h"
#include "2d/CCSprite.h"
#include "2d/CCActionInterval.h"
#include "audio/include/AudioEngine.h"

using namespace cocos2d;

namespace myutils
{
    void addResource(cocos2d::Node* parentNode, cocos2d::ParticleSun *_emitter, const ResourceInfo& resourceInfo, std::vector<int> &audioIDVec)
    {
        // add Sprites and run actions if needed
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto origin = Director::getInstance()->getVisibleOrigin();
        int spriteNumber = resourceInfo.spriteNumber;
        int actionNumber = resourceInfo.actionNumber;
        int drawcallNumber = resourceInfo.drawcallNumber;
        int drawcall = 0;
        
        if (spriteNumber > 0)
        {
            for (int i = 0; i < spriteNumber; ++i)
            {
                Sprite *sprite;
                if (drawcall < drawcallNumber)
                {
                    auto spritePath = StringUtils::format("sprite%d.png", drawcall % 2);
                    sprite = Sprite::create(spritePath.c_str());
                }
                else
                {
                    sprite = Sprite::create("sprite0.png");
                }
                ++drawcall;
                
                float x = origin.x + visibleSize.width * (std::rand() * 1.0 / RAND_MAX);
                float y = origin.y + visibleSize.height * (std::rand() * 1.0 / RAND_MAX);
                
                float maxRectSize = std::max(sprite->getContentSize().width, sprite->getContentSize().height);
                
                x = std::min(x, origin.x + visibleSize.width - maxRectSize/2);
                x = std::max(x, origin.x + maxRectSize/2);
                
                y = std::min(y, origin.y + visibleSize.height - maxRectSize/2);
                y = std::max(y, origin.y + maxRectSize/2);
                
                sprite->setPosition(Vec2(x, y));
                parentNode->addChild(sprite);
                
                if (i < actionNumber)
                {
                    sprite->runAction(RepeatForever::create(RotateBy::create(3, 360)));
                }
            }
        }
        else
        {
            // remove some sprites
            int removedSpriteNum = -spriteNumber;
            auto children = parentNode->getChildren();
            for (int i = 0; i < removedSpriteNum; ++i)
            {
                parentNode->removeChild(children.at(i));
            }
        }
        
        
        
        // add particles
        _emitter->setVisible(true);
        _emitter->resume();
        if (resourceInfo.particleNumber > 0)
            _emitter->setTotalParticles(resourceInfo.particleNumber);
        else
            _emitter->setTotalParticles(_emitter->getTotalParticles() + resourceInfo.particleNumber);
        
        // play audioes
        int audioNumber = resourceInfo.audioNumber;
        if (audioNumber > 0)
        {
            for (int i = 0 ; i < audioNumber; ++i)
            {
                auto audioPath = StringUtils::format("effect%d.mp3", i % 10);
                int audioID = experimental::AudioEngine::play2d(audioPath.c_str(), true);
                audioIDVec.push_back(audioID);
            }
        }
        else
        {
            int stoppedAudioNum = -audioNumber;
            for (int i = 0; i < stoppedAudioNum; ++i)
            {
                experimental::AudioEngine::stop(audioIDVec[i]);
                audioIDVec.erase(audioIDVec.begin());
            }
        }
    }
}
