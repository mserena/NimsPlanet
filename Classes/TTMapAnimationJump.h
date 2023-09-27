#ifndef __TTMapAnimationJump_SCENE_H__
#define __TTMapAnimationJump_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTMapAnimation.h"

using namespace cocos2d;

class TTMapAnimationJump: public TTMapAnimation
{
    
public:
    
	TTMapAnimationJump(Node* animationParent,mapAnimation _animMap, float scale);
    ~TTMapAnimationJump();
    
private:
    
    void onResourceLoaded(Texture2D* texture);
    
private:
    
    Sprite* _spriteAnimation;
};

#endif  // __TTMapAnimationJump_SCENE_H__
