#ifndef __TTMapAnimationRotation_SCENE_H__
#define __TTMapAnimationRotation_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTMapAnimation.h"

using namespace cocos2d;

class TTMapAnimationRotation: public TTMapAnimation
{
    
public:
    
	TTMapAnimationRotation(Node* animationParent,mapAnimation _animMap, float scale);
    ~TTMapAnimationRotation();
    
private:
    
    void onResourceLoaded(Texture2D* texture);
    
private:
    
    Sprite* _spriteAnimation;
};

#endif  // __TTMapAnimationRotation_SCENE_H__
