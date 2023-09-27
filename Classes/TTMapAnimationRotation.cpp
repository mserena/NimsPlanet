#include "TTMapAnimationRotation.h"

TTMapAnimationRotation::TTMapAnimationRotation(Node* animationParent,mapAnimation animMap, float scale):
TTMapAnimation(animationParent,animMap, scale)
{
    // Cargamos el resource para la animación
    Director::getInstance()->getTextureCache()->addImageAsync(_animMap._animResource,CC_CALLBACK_1(TTMapAnimationRotation::onResourceLoaded, this));
}

TTMapAnimationRotation::~TTMapAnimationRotation()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_animMap._animResource);
}

void TTMapAnimationRotation::onResourceLoaded(Texture2D* texture)
{
    _spriteAnimation = Sprite::createWithTexture(texture);
    _spriteAnimation->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimation->setScale(_scale);
    _spriteAnimation->setPosition(Vec2(_scale*_animMap._animPosition[0],_scale*_animMap._animPosition[1]));
    _animationParent->addChild(_spriteAnimation,INMAP_ANIMATION_LAYER);
    
    //Animación de rotación
    _spriteAnimation->runAction(RepeatForever::create(RotateBy::create(3,360)));
    
    TTMapAnimation::onAnimationCreated();
}