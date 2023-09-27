#ifndef __TTMapAnimationAnimator_SCENE_H__
#define __TTMapAnimationAnimator_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTMapAnimation.h"

using namespace cocos2d;

class TTMapAnimationAnimator: public TTMapAnimation
{
    
public:
    
	TTMapAnimationAnimator(Node* animationParent,mapAnimation _animMap, float scale);
    ~TTMapAnimationAnimator();
    
private:
    
    void onResourceLoaded(Texture2D* texture);
    
private:
    
    Sprite* _spriteAnimation;
};

#endif  // __TTMapAnimationAnimator_SCENE_H__
