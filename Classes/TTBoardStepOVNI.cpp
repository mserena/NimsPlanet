//
//  TTBoardStepOVNI.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepOVNI.h"
#include "TTAudioManager.h"

TTBoardStepOVNI::TTBoardStepOVNI(std::vector<TTCell*> abductionCells,std::vector<TTCell*> hitCells):
TTBoardStep::TTBoardStep(),
_hitCells(hitCells),
_abductionCells(abductionCells)
{
    // OVNI
    _ovni = Sprite::createWithSpriteFrameName("ufo.png");
    _ovni->setScale(scale_sprite(_ovni,WIDTH_OVNI_SPACE,HEIGHT_OVNI_SPACE));
    _ovni->setAnchorPoint(Vec2(0.5,0.5));
    _ovni->setVisible(false);
    
    // OVNI estela abducción
    _ovniPower = Sprite::createWithSpriteFrameName("ufo_power.png");
    _ovniPower->setScale(scale_sprite(_ovniPower,WIDTH_OVNI_SPACE,HEIGHT_OVNI_SPACE));
    _ovniPower->setAnchorPoint(Vec2(0.5,1));
    _ovniPower->setRotation(60);
    _ovniPower->setVisible(false);
    
    //Padre del OVNI
    _parentOvni = Node::create();
    float _posXOVNI = -_ovni->getBoundingBox().size.width;
    float _posYOVNI = Director::getInstance()->getWinSize().height-_ovni->getBoundingBox().size.height;
    _parentOvni->setPosition(Point(_posXOVNI,_posYOVNI));
    
    Director::getInstance()->getRunningScene()->addChild(_parentOvni,20);
    _parentOvni->addChild(_ovni,2);
    _parentOvni->addChild(_ovniPower,1);
}

TTBoardStepOVNI::~TTBoardStepOVNI()
{
    _parentOvni->removeFromParentAndCleanup(true);
}

void TTBoardStepOVNI::starStep()
{
    TTAudioManager::getInstance()->playAudio("UFO_ABDUCTION");
    _ovni->setVisible(true);
    
    // Sequencia rotación del ovni
    auto _rotateOvni = RotateBy::create(0.8f,_ovni->getRotation()+10);
    _ovni->runAction(RepeatForever::create(Sequence::create(_rotateOvni,_rotateOvni->reverse(), NULL)));
    
    // Movimiento hacia el centro de la pantalla
    Point _finishPosOvni = Point(Director::getInstance()->getWinSize().width/2,_parentOvni->getPosition().y);
    auto _moveOvni = EaseExponentialOut::create(MoveTo::create(0.5f,_finishPosOvni));
    
    // Movimiento levitación
    auto _levitateOvni = CallFunc::create(std::bind([&](){
        auto _moveUp = MoveTo::create(1.0f,Point(_parentOvni->getPosition().x,_parentOvni->getPosition().y+5));
        auto _moveDown = MoveTo::create(1.0f,Point(_parentOvni->getPosition().x,_parentOvni->getPosition().y-5));
        auto _levitate = RepeatForever::create(Sequence::create(_moveUp,_moveDown,NULL));
        _parentOvni->runAction(_levitate);
    }));
    
    // Abduction y hits
    auto _abduction = CallFunc::create(std::bind([&](){
        //Avisamos a todas las celdas que van a ser abducidas
        for(int i = 0; i < _abductionCells.size(); i++)
        {
            _abductionCells[i]->unselectCell();
            _abductionCells[i]->prepareToAbduction(_parentOvni->getPosition());
        }
        
        bool _hitWood = false;
        bool _hitMud = false;
        bool _hitStone = false;
        
        // Aplicamos los hits a las celdas
        for(int i = 0; i < _hitCells.size(); i++)
        {
            _hitCells[i]->hitCell();
            
            if(_hitCells[i]->_cellContentLayers.size() > 0 && typeid(*_hitCells[i]->_cellContentLayers.back()) == typeid(TTCellStaticContentStone))
            {
                _hitStone = true;
            }
            else if(_hitCells[i]->_cellContentLayers.size() > 1 && typeid(*_hitCells[i]->_cellContentLayers[_hitCells[i]->_cellContentLayers.size()-2]) == typeid(TTCellStaticContentMud))
            {
                _hitMud = true;
            }
            else if(_hitCells[i]->_cellContentLayers.size() > 0 && typeid(*_hitCells[i]->_cellContentLayers.back()) == typeid(TTCellStaticContentWood))
            {
                _hitWood = true;
            }
        }
        
        //Play de sonidos
        if(_hitStone) TTAudioManager::getInstance()->playAudio("STONE_BREAK");
        if(_hitMud) TTAudioManager::getInstance()->playAudio("MUD_BREAK");
        if(_hitWood) TTAudioManager::getInstance()->playAudio("WOOD_BREAK");
        
        _ovniPower->setVisible(true);
        auto _scalePower = ScaleTo::create(0.2f,_ovniPower->getScale()*4.5f);
        auto _rotatePower = RotateBy::create(TIME_ABDUCTION,-ANGLE_ABDUCTION);
        auto _finishStep = CallFunc::create(CC_CALLBACK_0(TTBoardStepOVNI::stepFinished,this));
        _ovniPower->runAction(Sequence::create(Spawn::create(_scalePower,_rotatePower,NULL),_finishStep,NULL));
    }));
    
    _parentOvni->runAction(Sequence::create(_moveOvni,_levitateOvni,_abduction,NULL));
}

void TTBoardStepOVNI::stepFinished()
{
    _ovniPower->setVisible(false);
    _parentOvni->stopAllActions();
    auto _delayWait = DelayTime::create(1.0f);
    auto _moveOut = MoveTo::create(0.2f,Point(_parentOvni->getPosition().x,Director::getInstance()->getWinSize().height+(_ovni->getBoundingBox().size.height/2)));
    
    auto _finishStep = CallFunc::create(std::bind([&](){
        TTBoardStep::stepFinished();
    }));
    
    _parentOvni->runAction(Sequence::create(_delayWait,_moveOut,_finishStep,NULL));
}