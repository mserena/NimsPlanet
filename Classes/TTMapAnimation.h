#ifndef __TTMapAnimation_SCENE_H__
#define __TTMapAnimation_SCENE_H__

#include "thegame.h"
#include "TTMapDefines.h"
#include "TTEventsManager.h"
#include "TTMapParser.h"

using namespace cocos2d;

class TTMapAnimation
{
    
public:
    
	TTMapAnimation(Node* animationParent, mapAnimation animMap, float scale);
    virtual ~TTMapAnimation();

    virtual void onAnimationCreated();
    virtual void pauseAnimation(){};
    
protected:
    
    Node* _animationParent;
    mapAnimation _animMap;
    float   _scale;
    
};

#endif  // __TTMapAnimation_SCENE_H__
