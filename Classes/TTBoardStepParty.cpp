//
//  TTBoardStepParty.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepParty.h"
#include "TTAudioManager.h"
#include "TTBoard.h"
#include "TTParticleManager.h"

#define STEP_TIME     3.0f
#define MIN_FIREWORKS 5
#define MAX_FIREWORKS 10

TTBoardStepParty::TTBoardStepParty():
TTBoardStep::TTBoardStep()
{
}

TTBoardStepParty::~TTBoardStepParty()
{
}

void TTBoardStepParty::starStep()
{
    CCLOG("Se inicia paso de fiesta de tablero");
    
    // Creamos la tabla de madera
    Size _screenSize = Director::getInstance()->getWinSize();
    _woodInfo = Sprite::createWithSpriteFrameName("barra_madera.png");
    _woodInfo->setPosition(Point(_screenSize.width/2,_screenSize.height+_woodInfo->getBoundingBox().size.height/2));
    _woodInfo->setAnchorPoint(Vec2(0.5,0.5));
    _textWinLevel = Label::createWithBMFont("comicFnt.fnt",TTLanguageManager::getInstance()->getTag("LEVEL_COMPLETE"));
    _textWinLevel->setAnchorPoint(Vec2(0.5,0.5));
    _textWinLevel->setPosition(Vec2(_woodInfo->getBoundingBox().size.width/2,_woodInfo->getBoundingBox().size.height/2));
    _woodInfo->addChild(_textWinLevel);
    float _widthWood = (80*_screenSize.width)/100;
    _woodInfo->setScale(_widthWood/_woodInfo->getBoundingBox().size.width);
    
    //corregimos la escala al 85% del ancho de la tabla
    _textWinLevel->setScale(0.85*(_widthWood/_textWinLevel->getContentSize().width)/_woodInfo->getScale());
    
    TTBoard::getInstance()->_parentBoard->getParent()->addChild(_woodInfo,21);
    
    // Acciones de la tabla
    auto _scaleUp = ScaleTo::create(0.5f,_woodInfo->getScale()*1.10);
    auto _scaleDown = ScaleTo::create(0.5f,_woodInfo->getScale()*0.90);
    _woodInfo->runAction(RepeatForever::create(Sequence::create(_scaleUp,_scaleDown,NULL)));
    
    auto _moveWoodTableUp = EaseExponentialOut::create(MoveTo::create(STEP_TIME/4,Point(_screenSize.width/2,_screenSize.height/2)));
    auto _moveWoodTableDown = EaseExponentialIn::create(MoveTo::create(STEP_TIME/4,Point(_screenSize.width/2,-_woodInfo->getBoundingBox().size.height/2)));
    _woodInfo->runAction(Sequence::create(_moveWoodTableUp,DelayTime::create(STEP_TIME/2),_moveWoodTableDown,NULL));
    
    // Salto de las celdas del tablero
    std::vector<std::vector<TTCell*> > _cells = TTBoard::getInstance()->_Cells;
    for(int i = 0; i < _cells.size(); i++)
    {
        for(int j = 0; j < _cells[i].size(); j++)
        {
            if(!_cells[i][j]->isEmptyCell() && _cells[i][j]->isSelectableCell())
            {
                (static_cast<TTCellDynamicContent*>(_cells[i][j]->_cellContentLayers.back()))->showHelpCell();
            }
        }
    }
    
    // Disparamos los cohetes
    int _numFireWorks = MIN_FIREWORKS+rand()%(MAX_FIREWORKS-MIN_FIREWORKS);
    float _delayFireWorks = STEP_TIME/(float)_numFireWorks;
    int _typeFirework = 0;
    for(int i = 0; i < _numFireWorks;i++)
    {
        launchFireWork(_delayFireWorks*i,_typeFirework);
        _typeFirework++;
        if(_typeFirework == 3)
            _typeFirework = 0;
    }
    
    // Esperamos el tiempo necesareo para terminar el paso de tablero
    auto _finishStep = CallFunc::create(std::bind([&](){
        _woodInfo->stopAllActions();
        _woodInfo->removeFromParentAndCleanup(true);
        stepFinished();
    }));
    TTBoard::getInstance()->_parentBoard->getParent()->runAction(Sequence::create(DelayTime::create(STEP_TIME),_finishStep,NULL));
    
    // Reproducimos sonido de fiesta de tablero
    TTAudioManager::getInstance()->playAudio("LEVEL_PARTY");
}

void TTBoardStepParty::launchFireWork(float timeDelay, int typeFireWork)
{
    auto _launchFireWork = CallFunc::create(std::bind([&](int _type){
        Size _screenSize = Director::getInstance()->getWinSize();
        Point _particlesPosition;
        _particlesPosition.x = (_screenSize.width/4)+rand()%static_cast<int>(((_screenSize.width-(_screenSize.width/4))-(_screenSize.width/4)));
        _particlesPosition.y = (_screenSize.height/4)+rand()%static_cast<int>(((_screenSize.height-(_screenSize.height/4))-(_screenSize.height/4)));
        
        ValueMap _particlesDic;
        ParticleSystemQuad* _particleSystem;
        if(_type == 0)
        {
            _particlesDic = TTParticleManager::getInstance()->getParticle("FireWork_blue");
        }
        else if(_type == 1)
        {
            _particlesDic = TTParticleManager::getInstance()->getParticle("FireWork_red");
        }
        else
        {
            _particlesDic = TTParticleManager::getInstance()->getParticle("FireWork_green");
        }
        
        _particleSystem = ParticleSystemQuad::create(_particlesDic);
        _particleSystem->setAutoRemoveOnFinish(true);
        float _referenceScale1536 = 1.3f;
        _particleSystem->setScale(_particleSystem->getScale()*_referenceScale1536*_screenSize.width/1536);
        _particleSystem->setPosition(_particlesPosition);
        TTBoard::getInstance()->_parentBoard->getParent()->addChild(_particleSystem,20);
        _particleSystem->resetSystem();
    },typeFireWork));

    TTBoard::getInstance()->_parentBoard->getParent()->runAction(Sequence::create(DelayTime::create(timeDelay),_launchFireWork,NULL));
}

void TTBoardStepParty::stepFinished()
{
    TTBoardStep::stepFinished();
    
    
}