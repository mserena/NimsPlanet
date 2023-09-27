#include "TTMapAnimation.h"
#include "TTMapDefines.h"

TTMapAnimation::TTMapAnimation(Node* animationParent, mapAnimation animMap, float scale):
_animationParent(animationParent),
_animMap(animMap),
_scale(scale)
{
    
}

TTMapAnimation::~TTMapAnimation()
{

}

void TTMapAnimation::onAnimationCreated()
{
    //La animación ha sido cargada
    sendEvent(EVENT_MAP_ANIM_LOADED);
}
