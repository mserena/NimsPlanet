#ifndef __TTMapAnimationParticles_SCENE_H__
#define __TTMapAnimationParticles_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTMapAnimation.h"

using namespace cocos2d;

class TTMapAnimationParticles: public TTMapAnimation
{
    
public:
    
	TTMapAnimationParticles(Node* animationParent,mapAnimation _animMap, float scale);
    ~TTMapAnimationParticles();
    
private:
    
    ParticleSystemQuad*     _particle;
};

#endif  // __TTMapAnimationParticles_SCENE_H__
