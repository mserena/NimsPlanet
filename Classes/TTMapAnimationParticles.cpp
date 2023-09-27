#include "TTMapAnimationParticles.h"

TTMapAnimationParticles::TTMapAnimationParticles(Node* animationParent,mapAnimation animMap, float scale):
TTMapAnimation(animationParent,animMap, scale)
{
    // Cargamos el resource para las partículas
    _particle = ParticleSystemQuad::create(_animMap._animResource);
    _particle->setScale(scale);
    _particle->setPosition(Vec2(_scale*_animMap._animPosition[0],_scale*_animMap._animPosition[1]));
    _particle->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    _animationParent->addChild(_particle,INMAP_ANIMATION_LAYER);
    
    CallFunc *_launchParticles = CallFunc::create(std::bind([&](){
        _particle->stopSystem();
        _particle->resetSystem();
    }));
    
    if(_animMap._timeRepeat == 0)
        _particle->runAction(_launchParticles);
    else
        _particle->runAction(RepeatForever::create(Sequence::create(DelayTime::create(_animMap._timeRepeat),_launchParticles, NULL)));
    
    TTMapAnimation::onAnimationCreated();
}

TTMapAnimationParticles::~TTMapAnimationParticles()
{
    _particle->stopAllActions();
    _particle->stopSystem();
    _particle->unscheduleUpdate();
    _particle->removeFromParentAndCleanup(true);
}
