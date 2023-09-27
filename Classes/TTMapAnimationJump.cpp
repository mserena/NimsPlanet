#include "TTMapAnimationJump.h"

TTMapAnimationJump::TTMapAnimationJump(Node* animationParent,mapAnimation animMap, float scale):
TTMapAnimation(animationParent,animMap, scale)
{
    // Cargamos el resource para la animación
    Director::getInstance()->getTextureCache()->addImageAsync(_animMap._animResource,CC_CALLBACK_1(TTMapAnimationJump::onResourceLoaded, this));
}

TTMapAnimationJump::~TTMapAnimationJump()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_animMap._animResource);
}

void TTMapAnimationJump::onResourceLoaded(Texture2D* texture)
{
    _spriteAnimation = Sprite::createWithTexture(texture);
    _spriteAnimation->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimation->setScale(_scale);
    _spriteAnimation->setPosition(Vec2(_scale*_animMap._animPosition[0],_scale*_animMap._animPosition[1]));
    _animationParent->addChild(_spriteAnimation,INMAP_ANIMATION_LAYER);
    
    //Animación de salto
    Point _positionDown = _spriteAnimation->getPosition();
    Point _positionUp = Point(_spriteAnimation->getPosition().x,_spriteAnimation->getPosition().y+_spriteAnimation->getContentSize().height/11);
    auto _moveUp = MoveTo::create(1.0f,_positionUp);
    auto _moveDown = MoveTo::create(1.0f,_positionDown);
    _spriteAnimation->runAction(RepeatForever::create(Sequence::create(_moveUp,_moveDown, NULL)));
    
    TTMapAnimation::onAnimationCreated();
}