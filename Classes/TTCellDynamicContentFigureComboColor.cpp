#include "TTCellDynamicContentFigureComboColor.h"
#include "TTEventsManager.h"
#include "TTParticleManager.h"
#include "TTBoardStepOVNI.h"
#include "TTAudioManager.h"

TTCellDynamicContentFigureComboColor::TTCellDynamicContentFigureComboColor(cocos2d::Node* cellContentParent,int _zLayerPosition, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell):
TTCellDynamicContent(cellContentParent,FIGURE_Z_LAYER,COMBO_COLOR_DYNAMIC_CONTENT,posXCell,posYCell,column,row,totalColumns,totalRows,sizeCell)
{
    _affectAreaCombination = false;
    
    _cellFigureSprite = Sprite::createWithSpriteFrameName("combo_color.png");
    _cellReferenceSize = _cellFigureSprite->getContentSize();
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    _cellFigure = FIGURE_COLOR;
    
    loadGeneralCellGraphs();
    loadAnimations();

}

TTCellDynamicContentFigureComboColor::~TTCellDynamicContentFigureComboColor()
{
    _pathSprite->removeFromParentAndCleanup(true);
}

void TTCellDynamicContentFigureComboColor::loadGeneralCellGraphs()
{
    float imageWidth = _cellFigureSprite->getBoundingBox().size.width;
    float newScale = _sizeFigure/imageWidth;
    _scaleCell = newScale;
    _cellFigureSprite->setScale(newScale,newScale);
    _cellFigureSprite->setVisible(false);
    _parentContent->addChild(_cellFigureSprite,CELL_SPRITE_DYNAMIC_LAYER);
    
    // cargamos los sprites para el path
    _pathSprite = Sprite::createWithSpriteFrameName("combo_color_lineaseleccion.png");
    newScale = (float)_sizeCell/_cellFigureSprite->getContentSize().width;
    _pathSprite->setScale(newScale,newScale);
    _pathSprite->setAnchorPoint(Vec2(0.5,0));
    _pathSprite->setVisible(false);
    _parentContentsCell->addChild(_pathSprite,getZContent()-1);
    
}

void TTCellDynamicContentFigureComboColor::loadAnimations()
{
    _isAnimable = true;
    
    // Scary
    _cellAnimationsScary = AnimationCache::getInstance()->getAnimation("combo_color_anim_cte");
    
    // Rebound
    _cellAnimationsRebound =AnimationCache::getInstance()->getAnimation("combo_color_anim_rebote");
    
    // Stand by
    launchComboStandByAnimation();

    // Jump
    _cellAnimationsJump = AnimationCache::getInstance()->getAnimation("combo_color_anim_salto");
}

void TTCellDynamicContentFigureComboColor::initCell()
{
    TTCellDynamicContent::initCell();
    _cellFigureSprite->setVisible(true);
}

bool TTCellDynamicContentFigureComboColor::isSelectable()
{
    return true;
}

bool TTCellDynamicContentFigureComboColor::isFirstSelectable()
{
    return true;
}

bool TTCellDynamicContentFigureComboColor::isRefillable()
{
    return false;
}

bool TTCellDynamicContentFigureComboColor::isWallContent()
{
    return false;
}

void TTCellDynamicContentFigureComboColor::preSelectCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_SELECTED;
    launchContentAnimation();
}

void TTCellDynamicContentFigureComboColor::unPreSelectCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    stopCellAnimations();
}

void TTCellDynamicContentFigureComboColor::selectedCell()
{
    TTCellDynamicContent::selectedCell();
    if(_isOnAnimation)
        stopCellAnimations();
    launchContentAnimation();
}

void TTCellDynamicContentFigureComboColor::unselectedCell()
{
    TTCellDynamicContent::unselectedCell();
    stopCellAnimations();
    hidePathCell();
}

int TTCellDynamicContentFigureComboColor::hitCell(int _numHits)
{
    _pathSprite->setVisible(false);
    _cellDynamicState = DYNAMIC_CONTENT_STATE_DEATH;
    launchContentAnimation();
    _contentIsDeath = true;
    return _numHits;
}

void TTCellDynamicContentFigureComboColor::launchContentAnimation()
{
    if(!_isAnimable || _blockAnimations)
        return;
    
    switch(_cellDynamicState)
    {
        case DYNAMIC_CONTENT_STATE_STANDBY:
        {

        }
        break;
        case DYNAMIC_CONTENT_STATE_SELECTED:
        {
            if(_isOnAnimation)
                stopCellAnimations();

            Animate* _anim = Animate::create(_cellAnimationsScary);
            Sequence* _animSelectedCombo = Sequence::create(_anim,_anim,NULL);
            _cellFigureSprite->runAction(RepeatForever::create(Sequence::create(_animSelectedCombo,NULL)));
            
            _isOnAnimation = true;
        }
        break;
            
        case DYNAMIC_CONTENT_STATE_FALLING:
        {
            if(_isOnAnimation)
                stopCellAnimations();
            
            Animate* animNode = Animate::create(_cellAnimationsRebound);
            _cellFigureSprite->runAction(Sequence::create(animNode,CallFunc::create(CC_CALLBACK_0(TTCellDynamicContentFigureComboColor::stopCellAnimations,this)),NULL));
            _isOnAnimation = true;
        }
        break;
            
        case DYNAMIC_CONTENT_STATE_HELP:
        {
            if(_isOnAnimation)
                stopCellAnimations();
            
            Animate* animNode = Animate::create(_cellAnimationsJump);
            _cellFigureSprite->runAction(RepeatForever::create(animNode));
            _isOnAnimation = true;
        }
        break;
            
        case DYNAMIC_CONTENT_STATE_DEATH:
        {
            if(_isOnAnimation)
                stopCellAnimations();
            
            // Creamos la animación de muerte
            float _scaleBig = _cellFigureSprite->getScale()+0.1f;
            ScaleTo* _scaleBigFigure = ScaleTo::create(0.1f,_scaleBig);
            CallFunc *_fadeOutFigure = CallFunc::create(std::bind([&](){
                RotateBy* _rotateFigure = RotateBy::create(0.3f,360);
                ScaleTo* _scaleSmallFigure = ScaleTo::create(0.3f,0);
                _cellFigureSprite->runAction(_rotateFigure);
                _cellFigureSprite->runAction(_scaleSmallFigure);
                
                CallFunc *_eraseCell = CallFunc::create(std::bind([&](){
                    TTCellDynamicContent::stopCellAnimations();
                    _cellFigureSprite->setRotation(0);
                    _cellFigureSprite->setScale(_scaleCell);
                    _cellFigureSprite->setVisible(false);
                    sendEvent(EVENT_BOARD_CELL_DEATH);
                }));
                _cellFigureSprite->runAction(Sequence::create(DelayTime::create(0.3f),_eraseCell,NULL));
                
            }));
            _cellFigureSprite->runAction(Sequence::create(_scaleBigFigure,_fadeOutFigure,NULL));
        }
        break;
            
        default:
        break;
    }
}

void TTCellDynamicContentFigureComboColor::launchComboStandByAnimation()
{
    float delay = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2)))+2;
    Animate* _animScary = Animate::create(_cellAnimationsScary);
    _cellFigureSprite->runAction(RepeatForever::create(Sequence::create(DelayTime::create(delay),_animScary,NULL)));
    
    _isOnAnimation = true;
}

void TTCellDynamicContentFigureComboColor::stopCellAnimations()
{
    TTCellDynamicContent::stopCellAnimations();
    
    // Reestore del sprite inicial
    _cellFigureSprite->setSpriteFrame("combo_color.png");
    if(_cellDynamicState == DYNAMIC_CONTENT_STATE_STANDBY)
    {
        launchComboStandByAnimation();
    }
}

void TTCellDynamicContentFigureComboColor::showPathCell(CELL_PATHS pathDirection)
{
    _pathSprite->setRotation(0);
    _pathSprite->cocos2d::Node::setScale(1,1);
    
    // Escalamos la linea
    switch(pathDirection)
    {
        case CELL_PATH_NE:
        case CELL_PATH_SE:
        case CELL_PATH_WS:
        case CELL_PATH_NW:
        {
            // linea diagonal
            float newScaleX = (float)_sizeCell/_cellReferenceSize.width;
            float newScaleY = ((float)_sizeCell+(_sizeCell/3))/_cellReferenceSize.width;
            _pathSprite->setScaleX(newScaleX);
            _pathSprite->setScaleY(newScaleY);
        }
        break;
            
        default:
        {
            // linea recta
            float newScale = (float)_sizeCell/_cellReferenceSize.width;
            _pathSprite->setScale(newScale,newScale);
        }
        break;
    }
    
    _pathSprite->setPosition(_posXCell,_posYCell);
    int rotateAngle = 45*pathDirection;
    _pathSprite->setRotation(rotateAngle);
    _pathSprite->setVisible(true);
}

void TTCellDynamicContentFigureComboColor::hidePathCell()
{
    _pathSprite->setVisible(false);
}

void TTCellDynamicContentFigureComboColor::finishFallCell()
{
    TTCellDynamicContent::finishFallCell();
    _blockAnimations = false;
    if(_cellDynamicState == DYNAMIC_CONTENT_STATE_FALLING)
    {
        launchContentAnimation();
        _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    }
}

void TTCellDynamicContentFigureComboColor::showHelpCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_HELP;
    launchContentAnimation();
    _blockAnimations = true;
}

void TTCellDynamicContentFigureComboColor::stopHelpCell()
{
    if(_cellDynamicState != DYNAMIC_CONTENT_STATE_HELP)
        return;
    
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    _blockAnimations = false;
    stopCellAnimations();
}

void TTCellDynamicContentFigureComboColor::launchComboFx(std::vector<Point> _positionsToKill, int _cellFigure)
{
    for(int i = 0; i < _positionsToKill.size(); i++)
        launchComboKamehameha(_cellFigure, _positionsToKill[i],0.5f,200);
    TTAudioManager::getInstance()->playAudio("COMBO_COLOUR_SOUND");
}

void TTCellDynamicContentFigureComboColor::launchComboKamehameha(int _cellFigure, Point _destination, float _time, float _speed)
{    
    // Cargamos el sistema de partículas
    Node* _parentParticles = Node::create();
    _parentParticles->setPosition(Point(_posXCell,_posYCell));

    char strParticles[100] = {0};
    sprintf(strParticles,"Figure%d_combo_fire",_cellFigure+1);
    ValueMap _fireParticle = TTParticleManager::getInstance()->getParticle(strParticles);
    ParticleSystemQuad* _fire = ParticleSystemQuad::create(_fireParticle);
    _fire->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    
    ValueMap _fireWiteParticle = TTParticleManager::getInstance()->getParticle("Wite_combo_fire");
    ParticleSystemQuad* _fireWite = ParticleSystemQuad::create(_fireWiteParticle);
    _fireWite->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    
    _fire->setAutoRemoveOnFinish(true);
    _fireWite->setAutoRemoveOnFinish(true);
    
    if(_speed != 0)
    {
        _fire->setSpeed(_speed);
        _fireWite->setSpeed(_speed);
    }
    _parentParticles->addChild(_fire,0);
    _parentParticles->addChild(_fireWite,1);
    
    
    // Calculamos la curva de bezier
    Point _startPoint = Vec2(_posXCell,_posYCell);
    ccBezierConfig bezierConfg;
    
    if(_destination.y < _startPoint.y && _destination.x <= _startPoint.x)
    {
        //CCLOG("Destino de la partícula hacia abajo y izquierda, posicion origen: %f,%f posicion destino: %f,%f",_startPoint.x,_startPoint.y,_destination.x,_destination.y);
        bezierConfg.controlPoint_1 = Point(_startPoint.x-(_sizeCell*2),_startPoint.y+_sizeCell);
        bezierConfg.controlPoint_2 = Point(_destination.x-(_sizeCell*2),_destination.y+_sizeCell);
    }
    else if(_destination.y < _startPoint.y && _destination.x > _startPoint.x)
    {
        //CCLOG("Destino de la partícula hacia abajo y derecha, posicion origen: %f,%f posicion destino: %f,%f",_startPoint.x,_startPoint.y,_destination.x,_destination.y);
        bezierConfg.controlPoint_1 = Point(_startPoint.x-(_sizeCell*2),_startPoint.y+_sizeCell);
        bezierConfg.controlPoint_2 = Point(_destination.x-(_sizeCell*2),_destination.y-_sizeCell);
    }
    else if(_destination.y >= _startPoint.y && _destination.x > _startPoint.x)
    {
        //CCLOG("Destino de la partícula hacia arriba y derecha, posicion origen: %f,%f posicion destino: %f,%f",_startPoint.x,_startPoint.y,_destination.x,_destination.y);
        bezierConfg.controlPoint_1 = Point(_startPoint.x-(_sizeCell*2),_startPoint.y-(_sizeCell*2));
        bezierConfg.controlPoint_2 = Point(_destination.x+(_sizeCell*2),_destination.y-(_sizeCell*2));
    }
    else if(_destination.y >= _startPoint.y && _destination.x <= _startPoint.x)
    {
        //CCLOG("Destino de la partícula hacia arriba y izquierda, posicion origen: %f,%f posicion destino: %f,%f",_startPoint.x,_startPoint.y,_destination.x,_destination.y);
        bezierConfg.controlPoint_1 = Point(_startPoint.x+(_sizeCell*2),_startPoint.y-(_sizeCell*2));
        bezierConfg.controlPoint_2 = Point(_destination.x+(_sizeCell*2),_destination.y+(_sizeCell*2));
    }
    
    bezierConfg.endPosition = _destination;
    auto _bezierMove = TTBezierRotationTo::create(_time, bezierConfg);
    auto _cleanParticles = RemoveSelf::create(true);
    auto _sendEventKamehameha = TTSendEvent::create(EVENT_BOARD_KAMEHAMEHA_FINISHED);
    _parentParticles->runAction(Sequence::create(_bezierMove,_cleanParticles,_sendEventKamehameha,NULL));
    
    _parentContentsCell->addChild(_parentParticles,LAYER_ANIMATIONS);
    
    //Escalamos el tamaño de las particulas en función del tamaño de la celda
    float _scaleFire = (float)0.75*_sizeCell/100;
    _fire->setScale(_scaleFire);
    _fireWite->setScale(_scaleFire);

    
    _fire->resetSystem();
    _fireWite->resetSystem();
}

void TTCellDynamicContentFigureComboColor::prepareToAbduction(Point _abductionPoint)
{
    _contentIsDeath = true;
    _cellFigureSprite->setVisible(false);
    _pathSprite->setVisible(false);
    TTCellDynamicContent::stopCellAnimations();
    
    Sprite* _spriteAbduction = Sprite::createWithSpriteFrameName("combo_color.png");
    _spriteAbduction->setScale(_scaleCell);
    _parentContent->addChild(_spriteAbduction);
    
    
    //calculo del momento a empezar el movimiento
    auto _abductionPosition = _parentContent->convertToNodeSpace(_abductionPoint);
    auto _spritePosition = _spriteAbduction->getPosition();
    
    float _cellAngleToPoint = atan2(_abductionPosition.y - _spritePosition.y, _abductionPosition.x - _spritePosition.x) * 180 / M_PI;
    
    float _runAngle = _cellAngleToPoint - (180-ANGLE_ABDUCTION)/2;
    
    const float _velocityAngularDeg = ANGLE_ABDUCTION/TIME_ABDUCTION;
    auto _timeToRunAngle = _runAngle/_velocityAngularDeg;
    
    //CCLOG("CELL(%d,%d) cellAngleToPoint(%f) time(%f)",_column,_row, _cellAngleToPoint, _timeToRunAngle);
    
    auto _delayToAbduction = DelayTime::create(_timeToRunAngle);
   
    auto _move = MoveTo::create(0.20f,_parentContent->convertToNodeSpace(_abductionPoint));
    auto _scale = ScaleTo::create(0.20f,0.1f);
    auto _removeSprite = RemoveSelf::create(true);
    _spriteAbduction->runAction(Sequence::create(_delayToAbduction,Spawn::create(_move,_scale,NULL),_removeSprite,NULL));
}

void TTCellDynamicContentFigureComboColor::playSelectSound(int _numSelection)
{
    TTAudioManager::getInstance()->playAudio("COMBOCOLOR_SELECT");
}

void TTCellDynamicContentFigureComboColor::playUnSelectSound(int _numSelection)
{
    TTAudioManager::getInstance()->playAudio("COMBOCOLOR_UNSELECT");
}

