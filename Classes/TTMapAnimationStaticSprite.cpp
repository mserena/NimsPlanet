#include "TTMapAnimationStaticSprite.h"

TTMapAnimationStaticSprite::TTMapAnimationStaticSprite(Node* animationParent,mapAnimation animMap, float scale):
TTMapAnimation(animationParent,animMap, scale)
{
    // Cargamos el resource para la animación
    Director::getInstance()->getTextureCache()->addImageAsync(_animMap._animResource,CC_CALLBACK_1(TTMapAnimationStaticSprite::onResourceLoaded, this));
}

TTMapAnimationStaticSprite::~TTMapAnimationStaticSprite()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_animMap._animResource);
}

void TTMapAnimationStaticSprite::onResourceLoaded(Texture2D* texture)
{
    _spriteAnimation = Sprite::createWithTexture(texture);
    _spriteAnimation->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimation->setScale(_scale);
    _spriteAnimation->setPosition(Vec2(_scale*_animMap._animPosition[0],_scale*_animMap._animPosition[1]));
    _animationParent->addChild(_spriteAnimation,INMAP_ANIMATION_LAYER);
    
    TTMapAnimation::onAnimationCreated();
}