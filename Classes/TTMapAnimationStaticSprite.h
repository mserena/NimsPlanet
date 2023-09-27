#ifndef __TTMapAnimationStaticSprite_SCENE_H__
#define __TTMapAnimationStaticSprite_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTMapAnimation.h"

using namespace cocos2d;

class TTMapAnimationStaticSprite: public TTMapAnimation
{
    
public:
    
	TTMapAnimationStaticSprite(Node* animationParent,mapAnimation _animMap, float scale);
    ~TTMapAnimationStaticSprite();
    
private:
    
    void onResourceLoaded(Texture2D* texture);
    
private:
    
    Sprite* _spriteAnimation;
};

#endif  // __TTMapAnimationStaticSprite_SCENE_H__
