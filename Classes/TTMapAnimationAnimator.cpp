#include "TTMapAnimationAnimator.h"

TTMapAnimationAnimator::TTMapAnimationAnimator(Node* animationParent,mapAnimation animMap, float scale):
TTMapAnimation(animationParent,animMap, scale)
{
    // Cargamos el resource para la animación
    char strTexture[100] = {0};
    sprintf(strTexture,"%s.png",_animMap._animResource.c_str());
    Director::getInstance()->getTextureCache()->addImageAsync(strTexture,CC_CALLBACK_1(TTMapAnimationAnimator::onResourceLoaded, this));
}

TTMapAnimationAnimator::~TTMapAnimationAnimator()
{
    char strTexture[100] = {0};
    sprintf(strTexture,"%s.png",_animMap._animResource.c_str());
    Director::getInstance()->getTextureCache()->removeTextureForKey(strTexture);
    
    char strPlist[100] = {0};
    sprintf(strPlist,"%s.plist",_animMap._animResource.c_str());
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(strPlist);

    AnimationCache::getInstance()->removeAnimation(_animMap._animResource);
}

void TTMapAnimationAnimator::onResourceLoaded(Texture2D* texture)
{
    // Cargamos la animación
    char strTexture[100] = {0};
    sprintf(strTexture,"%s.png",_animMap._animResource.c_str());
    char strPlist[100] = {0};
    sprintf(strPlist,"%s.plist",_animMap._animResource.c_str());
    char strPlistAnim[100] = {0};
    sprintf(strPlistAnim,"%s_animation.plist",_animMap._animResource.c_str());
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(strPlist,strTexture);
    AnimationCache::getInstance()->addAnimationsWithFile(strPlistAnim);
    
    // Creamos el sprite contenedor
    _spriteAnimation = Sprite::create();
    _spriteAnimation->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimation->setScale(_scale);
    _spriteAnimation->setPosition(Vec2(_scale*_animMap._animPosition[0],_scale*_animMap._animPosition[1]));
    _animationParent->addChild(_spriteAnimation,INMAP_ANIMATION_LAYER);
    
    //Animator
    Animate* animNode = Animate::create(AnimationCache::getInstance()->getAnimation(_animMap._animResource));
    Sequence* _animSequence = Sequence::create(animNode,DelayTime::create(1),NULL);
    _spriteAnimation->runAction(RepeatForever::create(_animSequence));
    
    TTMapAnimation::onAnimationCreated();
}