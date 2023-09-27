
#include "TTCellDynamicContentFigure.h"
#include "TTEventsManager.h"
#include "TTParticleManager.h"
#include "TTBoardStepOVNI.h"
#include "TTAudioManager.h"
#include "TTGame.h"
#include "TTContentCalculator.h"
#include "TTGameUIManager.h"

TTCellDynamicContentFigure::TTCellDynamicContentFigure(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell,  int cellInitSubType):
TTCellDynamicContent(cellContentParent,FIGURE_Z_LAYER,cellDynamicType,posXCell,posYCell,column,row,totalColumns,totalRows,sizeCell)
{
    _affectAreaCombination = false;
    
    if(_contentLayerType != FIGURE_DYNAMIC_CONTENT && !isComboContent())
    {
        CCLOG("Se intenta crear una figura no bajable usando el contructor de figura normal");
        abort();
    }
    
    // Decidimos el tipo de figura
    if(cellInitSubType == NUM_CELL_NORMAL_FIGURES)
        _cellFigure = TTContentCalculator::getInstance()->nextFigure();
    else
    {
        _cellFigure = TTContentCalculator::getInstance()->nextFigure(cellInitSubType);
    }
    if(!isComboContent())
    {
        // Creamos una figura normal
        _cellFigureSprite = Sprite::createWithSpriteFrameName(_figureSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
        _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    }
    else
    {
        // Creamos una figura combo
        _cellFigureSprite = Sprite::createWithSpriteFrameName(_figureComboSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
        _cellDynamicState = DYNAMIC_CONTENT_STATE_NOTHING;
    }
    
    loadGeneralCellGraphs();
    loadAnimations();
}

TTCellDynamicContentFigure::~TTCellDynamicContentFigure()
{
    _pathSprite->removeFromParentAndCleanup(true);
    _cellFigureSprite->removeFromParentAndCleanup(true);
}

void TTCellDynamicContentFigure::loadGeneralCellGraphs()
{
    float imageWidth = _cellFigureSprite->getBoundingBox().size.width;
    float newScale = _sizeFigure/imageWidth;
    _scaleCell = newScale;
    _cellFigureSprite->setScale(newScale,newScale);
    _cellFigureSprite->setVisible(false);
    _parentContent->addChild(_cellFigureSprite,CELL_SPRITE_DYNAMIC_LAYER);
    
    // cargamos los sprites para el path
    char strPath[100] = {0};
    sprintf(strPath,"figura%d_lineaseleccion.png",_cellFigure+1);
    _pathSprite = Sprite::createWithSpriteFrameName(strPath);
    _pathSprite->setPosition(Point(_posXCell,_posYCell));
    newScale = (float)_sizeCell/_cellFigureSprite->getContentSize().width;
    _pathSprite->setScale(newScale,newScale);
    _pathSprite->setAnchorPoint(Vec2(0.5,0));
    _pathSprite->setVisible(false);
    int _zPath = getZContent();
    _parentContentsCell->addChild(_pathSprite,_zPath-1);
    
    loadComboArrows();
}

void TTCellDynamicContentFigure::loadComboArrows()
{
    float newScale = (float)_sizeCell/_cellFigureSprite->getContentSize().width;
    
    // cargamos gráficos de combos
    if(isComboContent())
    {
        // Según el tipo de combo cargamos las flechas
        switch (_contentLayerType)
        {
            case COMBO_VERTICAL_DYNAMIC_CONTENT:
            {
                //Up
                Sprite* _arrowUp = Sprite::createWithSpriteFrameName("arrow_combo.png");
                _arrowUp->setAnchorPoint(Vec2(0.5,0.5));
                _arrowUp->setScale(_scaleCell/2);
                Point _initPositionArrowUp;
                Point _finishPositionArrowUp;
                getArrowPositions(ARROW_PATH_N,_sizeCell/2,&_initPositionArrowUp,&_finishPositionArrowUp);
                _arrowUp->setPosition(Vec2(0,0));
                _arrowUp->setRotation(90);
                _arrowUp->setOpacity(0);
                _parentContent->addChild(_arrowUp,CELL_ARROW_DYNAMIC_LAYER);
                _comboArrows.push_back(_arrowUp);
                
                Spawn* _animUp = Spawn::create(FadeTo::create(0.2f,200),ScaleTo::create(0.2f,_scaleCell),EaseExponentialOut::create(MoveTo::create(0.4f,_finishPositionArrowUp)),NULL);
                Spawn* _animLReverseUp = Spawn::create(FadeTo::create(0.2f,0),ScaleTo::create(0.4f,_scaleCell/2),MoveTo::create(0.4f,_initPositionArrowUp),NULL);
                _arrowUp->runAction(RepeatForever::create(Sequence::create(_animUp,_animLReverseUp,DelayTime::create(0.6f),NULL)));
                
                //Right
                Sprite* _arrowDown = Sprite::createWithSpriteFrameName("arrow_combo.png");
                _arrowDown->setAnchorPoint(Vec2(0.5,0.5));
                _arrowDown->setScale(newScale/2);
                Point _initPositionArrowDown;
                Point _finishPositionArrowDown;
                getArrowPositions(ARROW_PATH_S,_sizeCell/2,&_initPositionArrowDown,&_finishPositionArrowDown);
                _arrowDown->setPosition(Vec2(0,-_arrowDown->getBoundingBox().size.width*2));
                _arrowDown->setRotation(270);
                _arrowDown->setOpacity(0);
                _parentContent->addChild(_arrowDown,CELL_ARROW_DYNAMIC_LAYER);
                _comboArrows.push_back(_arrowDown);
                
                Spawn* _animDown = Spawn::create(FadeTo::create(0.2f,200),ScaleTo::create(0.2f,_scaleCell),EaseExponentialOut::create(MoveTo::create(0.4f,_finishPositionArrowDown)),NULL);
                Spawn* _animLReverseDown = Spawn::create(FadeTo::create(0.2f,0),ScaleTo::create(0.4f,_scaleCell/2),MoveTo::create(0.4f,_initPositionArrowDown),NULL);
                _arrowDown->runAction(RepeatForever::create(Sequence::create(_animDown,_animLReverseDown,DelayTime::create(0.6f),NULL)));
            }
            break;
            case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
            {
                
                //Left
                Sprite* _arrowL = Sprite::createWithSpriteFrameName("arrow_combo.png");
                _arrowL->setAnchorPoint(Vec2(0.5,0.5));
                _arrowL->setScale(_scaleCell/2);
                Point _initPositionArrowL;
                Point _finishPositionArrowL;
                getArrowPositions(ARROW_PATH_O,_sizeCell/2,&_initPositionArrowL,&_finishPositionArrowL);
                _arrowL->setPosition(_initPositionArrowL);
                _arrowL->setOpacity(0);
                _parentContent->addChild(_arrowL,CELL_ARROW_DYNAMIC_LAYER);
                _comboArrows.push_back(_arrowL);
                
                Spawn* _animL = Spawn::create(FadeTo::create(0.2f,200),ScaleTo::create(0.2f,_scaleCell),EaseExponentialOut::create(MoveTo::create(0.4f,_finishPositionArrowL)),NULL);
                Spawn* _animLReverseL = Spawn::create(FadeTo::create(0.2f,0),ScaleTo::create(0.4f,_scaleCell/2),MoveTo::create(0.4f,_initPositionArrowL),NULL);
                _arrowL->runAction(RepeatForever::create(Sequence::create(_animL,_animLReverseL,DelayTime::create(0.6f), NULL)));
                
                //Right
                Sprite* _arrowR = Sprite::createWithSpriteFrameName("arrow_combo.png");
                _arrowR->setAnchorPoint(Vec2(0.5,0.5));
                _arrowR->setScale(newScale/2);
                Point _initPositionArrowR;
                Point _finishPositionArrowR;
                getArrowPositions(ARROW_PATH_E,_sizeCell/2,&_initPositionArrowR,&_finishPositionArrowR);
                _arrowR->setPosition(_initPositionArrowR);
                _arrowR->setRotation(180);
                _arrowR->setOpacity(0);
                _parentContent->addChild(_arrowR,CELL_ARROW_DYNAMIC_LAYER);
                _comboArrows.push_back(_arrowR);
                
                Spawn* _animR = Spawn::create(FadeTo::create(0.2f,200),ScaleTo::create(0.2f,_scaleCell),EaseExponentialOut::create(MoveTo::create(0.4f,_finishPositionArrowR)),NULL);
                Spawn* _animLReverseR = Spawn::create(FadeTo::create(0.2f,0),ScaleTo::create(0.4f,_scaleCell/2),MoveTo::create(0.4f,_initPositionArrowR),NULL);
                _arrowR->runAction(RepeatForever::create(Sequence::create(_animR,_animLReverseR,DelayTime::create(0.6f),NULL)));
            }
            break;
            case COMBO_AREA_DYNAMIC_CONTENT:
            {
                for(int i = 0; i < 8; i++)
                {
                    Sprite* _arrow = Sprite::createWithSpriteFrameName("arrow_combo.png");
                    _arrow->setAnchorPoint(Vec2(0.5,0.5));
                    _arrow->setScale(_scaleCell/2);
                    Point _initPositionArrow;
                    Point _finishPositionArrow;
                    getArrowPositions((ARROW_DIRECTIONS)i,_sizeCell/2,&_initPositionArrow,&_finishPositionArrow);
                    _arrow->setPosition(_initPositionArrow);
                    _arrow->setRotation(45*i);
                    _arrow->setOpacity(0);
                    _parentContent->addChild(_arrow,CELL_ARROW_DYNAMIC_LAYER);
                    _comboArrows.push_back(_arrow);
                    
                    Spawn* _anim = Spawn::create(FadeTo::create(0.2f,200),ScaleTo::create(0.2f,_scaleCell),EaseExponentialOut::create(MoveTo::create(0.4f,_finishPositionArrow)),NULL);
                    Spawn* _animLReverse = Spawn::create(FadeTo::create(0.2f,0),ScaleTo::create(0.4f,_scaleCell/2),MoveTo::create(0.4f,_initPositionArrow),NULL);
                    
                    if(i%2 == 0)
                    {
                        _arrow->runAction(RepeatForever::create(Sequence::create(_anim,_animLReverse,DelayTime::create(1.2f),NULL)));
                    }
                    else
                    {
                        _arrow->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.6f),_anim,_animLReverse,DelayTime::create(0.6f),NULL)));
                    }
                }
            }
            break;
            default:
            break;
        }
    }
}

void TTCellDynamicContentFigure::getArrowPositions(ARROW_DIRECTIONS _arrowDirection, float _offset, Point* _initPoint, Point* _finishPoint)
{
    (*_initPoint) = Point(0,0);
    switch(_arrowDirection)
    {
        case ARROW_PATH_O:
            (*_finishPoint) = Point(-_offset,0);
        break;
        case ARROW_PATH_NO:
            (*_finishPoint) = Point(-_offset,_offset);
        break;
        case ARROW_PATH_N:
            (*_finishPoint) = Point(0,_offset);
        break;
        case ARROW_PATH_NE:
            (*_finishPoint) = Point(_offset,_offset);
        break;
        case ARROW_PATH_E:
            (*_finishPoint) = Point(_offset,0);
        break;
        case ARROW_PATH_SE:
            (*_finishPoint) = Point(_offset,-_offset);
        break;
        case ARROW_PATH_S:
            (*_finishPoint) = Point(0,-_offset);
        break;
        case ARROW_PATH_SO:
            (*_finishPoint) = Point(-_offset,-_offset);
        break;
    }
}

void TTCellDynamicContentFigure::loadAnimations()
{
    _isAnimable = true;
    
    // Scary
    char strAnimScary[100] = {0};
    if(!isComboContent())
    {
        sprintf(strAnimScary,"figura%d_%s",_cellFigure+1,ANIM_SELECTION_SCARY);
        _cellAnimationsScary = AnimationCache::getInstance()->getAnimation(strAnimScary);
    }
    else
    {
        sprintf(strAnimScary,"figura%d_bomba_%s",_cellFigure+1,ANIM_SELECTION_TURN);
        _cellAnimationsScary = AnimationCache::getInstance()->getAnimation(strAnimScary);
    }
    
    // Rebound
    char strAnimRebound[100] = {0};
    if(!isComboContent())
    {
        sprintf(strAnimRebound,"figura%d_%s",_cellFigure+1,ANIM_SELECTION_REBOUND);
    }
    else
    {
        sprintf(strAnimRebound,"figura%d_bomba_%s",_cellFigure+1,ANIM_SELECTION_REBOUND);
    }
    _cellAnimationsRebound =AnimationCache::getInstance()->getAnimation(strAnimRebound);
    
    // Stand by
    if(!isComboContent())
    {
        for(int i = 0; i < NUM_CELL_ANIMATIONS_STANDBY; i++)
        {
            std::string animName = _cellAnimationsInfo.find(CELL_ANIMATIONS_STANDBY_ID(i))->second;
            char strAnim[100] = {0};
            sprintf(strAnim,"figura%d_%s",_cellFigure+1,animName.c_str());
            _cellAnimationsStandBy.push_back(AnimationCache::getInstance()->getAnimation(strAnim));
        }
    }
    else
    {
        launchComboStandByAnimation();
    }

    // Jump
    char strAnimJump[100] = {0};
    if(!isComboContent())
    {
        sprintf(strAnimJump,"figura%d_%s",_cellFigure+1,ANIM_SELECTION_JUMP);
        _cellAnimationsJump = AnimationCache::getInstance()->getAnimation(strAnimJump);
    }
    else
    {
        sprintf(strAnimJump,"figura%d_bomba_%s",_cellFigure+1,ANIM_SELECTION_JUMP);
        _cellAnimationsJump = AnimationCache::getInstance()->getAnimation(strAnimJump);
    }
}

void TTCellDynamicContentFigure::initCell()
{
    TTCellDynamicContent::initCell();
    _cellFigureSprite->setVisible(true);
}

bool TTCellDynamicContentFigure::isSelectable()
{
    return true;
}

bool TTCellDynamicContentFigure::isFirstSelectable()
{
    return true;
}

bool TTCellDynamicContentFigure::isRefillable()
{
    return false;
}

bool TTCellDynamicContentFigure::isWallContent()
{
    return false;
}

void TTCellDynamicContentFigure::preSelectCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_SELECTED;
    launchContentAnimation();
}

void TTCellDynamicContentFigure::unPreSelectCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    stopCellAnimations();
}

void TTCellDynamicContentFigure::selectedCell()
{
    TTCellDynamicContent::selectedCell();
    if(_isOnAnimation)
        stopCellAnimations();
    launchContentAnimation();
}

void TTCellDynamicContentFigure::unselectedCell()
{
    TTCellDynamicContent::unselectedCell();
    stopCellAnimations();
    hidePathCell();
}

int TTCellDynamicContentFigure::hitCell(int _numHits)
{
    if(!isEmptyCell())
    {
        hidePathCell();
        
        if(isComboContent())
        {
            for(int i = 0; i < _comboArrows.size(); i++)
            {
                _comboArrows[i]->stopAllActions();
                _comboArrows[i]->removeFromParentAndCleanup(true);
            }
        }
        
        _isUpdatingContent = true;
        _cellDynamicState = DYNAMIC_CONTENT_STATE_DEATH;
        launchContentAnimation();
    }
    
    _contentIsDeath = true;
    return _numHits;
}
     
void TTCellDynamicContentFigure::launchContentAnimation()
{
    if(!_isAnimable || _blockAnimations)
        return;
    
    switch(_cellDynamicState)
    {
        case DYNAMIC_CONTENT_STATE_STANDBY:
        {
            if(!_isOnAnimation && !_blockAnimations && !isComboContent())
            {
                int animId = rand()%NUM_CELL_ANIMATIONS_STANDBY;
                Animate* animNode = Animate::create(_cellAnimationsStandBy[animId]);
                float delay = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.7));
                _cellFigureSprite->runAction(Sequence::create(DelayTime::create(delay),animNode,CallFunc::create(CC_CALLBACK_0(TTCellDynamicContentFigure::stopCellAnimations,this)),NULL));
                _isOnAnimation = true;
            }
        }
        break;
        case DYNAMIC_CONTENT_STATE_SELECTED:
        {
            if(_isOnAnimation)
                stopCellAnimations();

            if(!isComboContent())
            {
                _cellFigureSprite->runAction(RepeatForever::create(Animate::create(_cellAnimationsScary)));
            
            }
            else
            {
                Animate* _anim = Animate::create(_cellAnimationsScary);
                Sequence* _animSelectedCombo = Sequence::create(_anim,_anim,NULL);
                _cellFigureSprite->runAction(RepeatForever::create(Sequence::create(_animSelectedCombo,DelayTime::create(0.2f),NULL)));
            }
            
            _isOnAnimation = true;
        }
        break;
            
        case DYNAMIC_CONTENT_STATE_FALLING:
        {
            if(_isOnAnimation)
                stopCellAnimations();
            
            Animate* animNode = Animate::create(_cellAnimationsRebound);
            _cellFigureSprite->runAction(Sequence::create(animNode,CallFunc::create(CC_CALLBACK_0(TTCellDynamicContentFigure::stopCellAnimations,this)),NULL));
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
            CallFunc *_figureActions = CallFunc::create(std::bind([&](){
                int _uiPosition = 0;
                bool isGoalFigure = false;
                for(int i = 0; i < TTGame::getInstance()->_gameConfig->goals.size(); i++)
                {
                    if(TTGame::getInstance()->_gameConfig->goals[i]->_typeGoal == COLLECT_FIGURES_GOAL)
                    {
                        if(static_cast<TTCollectFiguresGoal*>(TTGame::getInstance()->_gameConfig->goals[i])->_figureToCollect == _cellFigure)
                        {
                            _uiPosition = i;
                            isGoalFigure = true;
                            break;
                        }
                    }
                }

                Sequence* _actionsSequence;
                if(isGoalFigure)
                {
                    //Muerte al marcador
                    CCLOG("Figura goal");
                    Sprite* _icon = static_cast<Sprite*>(TTGameUIManager::getInstance()->_goals[_uiPosition]._obj_icon->getChildByName("iconGoal"));
                    Point _position = _icon->getPosition();
                    _position.x += _icon->getContentSize().width/2;
                    _position.y += _icon->getContentSize().height/2;
                    Point _destination = _icon->convertToWorldSpace(_position);
                    Point _destinationUI = _parentContent->convertToNodeSpace(_destination);
                    auto _move = EaseExponentialOut::create(MoveTo::create(0.8f,_destinationUI));
                    _actionsSequence = Sequence::create(_move,NULL);
                }
                else
                {
                    //Muerte normal
                    RotateBy* _rotateFigure = RotateBy::create(0.3f,360);
                    ScaleTo* _scaleSmallFigure = ScaleTo::create(0.3f,0);
                    float _scaleBig = _cellFigureSprite->getScale()+0.1f;
                    ScaleTo* _scaleBigFigure = ScaleTo::create(0.1f,_scaleBig);
                    _actionsSequence = Sequence::create(Spawn::create(_scaleBigFigure,_rotateFigure,_scaleSmallFigure,NULL),NULL);
                }
                
                CallFunc *_eraseCell = CallFunc::create(std::bind([&](){
                    TTCellDynamicContent::stopCellAnimations();
                    _cellFigureSprite->setRotation(0);
                    _cellFigureSprite->setScale(_scaleCell);
                    _cellFigureSprite->setVisible(false);
                    _isUpdatingContent = false;
                }));
                _cellFigureSprite->runAction(Sequence::create(_actionsSequence,_eraseCell,NULL));
                
            }));
            _cellFigureSprite->runAction(_figureActions);
            launchComboFX();
        }
        break;
            
        default:
        break;
    }
}

void TTCellDynamicContentFigure::launchComboStandByAnimation()
{
    float delay = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2)))+2;
    Animate* _animScary = Animate::create(_cellAnimationsScary);
    _cellFigureSprite->runAction(RepeatForever::create(Sequence::create(DelayTime::create(delay),_animScary,DelayTime::create(0.5f),_animScary,NULL)));
    
    _isOnAnimation = true;
}

void TTCellDynamicContentFigure::stopCellAnimations()
{
    TTCellDynamicContent::stopCellAnimations();
    
    // Reestore del sprite inicial
    if(_contentLayerType == FIGURE_DYNAMIC_CONTENT)
        _cellFigureSprite->setSpriteFrame(_figureSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
    else if(isComboContent())
    {
        _cellFigureSprite->setSpriteFrame(_figureComboSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
        if(_cellDynamicState == DYNAMIC_CONTENT_STATE_STANDBY)
        {
            launchComboStandByAnimation();
        }
    }
}

void TTCellDynamicContentFigure::showPathCell(CELL_PATHS pathDirection)
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
            float newScaleX = (float)_sizeCell/_cellFigureSprite->getContentSize().width;
            float newScaleY = ((float)_sizeCell+(_sizeCell/3))/_cellFigureSprite->getContentSize().width;
            _pathSprite->setScaleX(newScaleX);
            _pathSprite->setScaleY(newScaleY);
        }
        break;
            
        default:
        {
            // linea recta
            float newScale = (float)_sizeCell/_cellFigureSprite->getContentSize().width;
            _pathSprite->setScale(newScale,newScale);
        }
        break;
    }
    
    // Posicionamos la linea segun si la figura es grande o pequeña
    if(_figureSpritesMap.find(CELL_FIGURES(_cellFigure))->second._isSmallFigure)
    {
        
        _pathSprite->setPosition(_posXCell,_posYCell-10);
    }
    else
    {
        _pathSprite->setPosition(_posXCell,_posYCell);
    }
    
    int rotateAngle = 45*pathDirection;
    _pathSprite->setRotation(rotateAngle);
    _pathSprite->setVisible(true);
}

void TTCellDynamicContentFigure::hidePathCell()
{
    _pathSprite->setVisible(false);
}

void TTCellDynamicContentFigure::finishFallCell()
{
    TTCellDynamicContent::finishFallCell();
    _blockAnimations = false;
    if(_cellDynamicState == DYNAMIC_CONTENT_STATE_FALLING)
    {
        launchContentAnimation();
        _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    }
}

void TTCellDynamicContentFigure::convertCellToCombo(CONTENT_TYPES comboType)
{
    stopCellAnimations();
    
    _cellDynamicState = DYNAMIC_CONTENT_STATE_NOTHING;
    _contentLayerType = comboType;
    
    //Modificamos el sprite
    _cellFigureSprite->removeFromParentAndCleanup(true);
    _cellFigureSprite = Sprite::createWithSpriteFrameName(_figureComboSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
    float imageWidth = _cellFigureSprite->getBoundingBox().size.width;
    float newScale = _sizeFigure/imageWidth;
    _scaleCell = newScale;
    _cellFigureSprite->setScale(newScale,newScale);
    _parentContent->addChild(_cellFigureSprite,CELL_SPRITE_DYNAMIC_LAYER);
    
    //Borrar el anterior indicador y creamos el idicador
    for(int i = 0; i < _comboArrows.size(); i++)
    {
        _comboArrows[i]->removeFromParentAndCleanup(true);
    }
    _comboArrows.clear();
    loadComboArrows();
    
    // Cargamos las animaciones de combos
    _cellAnimationsStandBy.clear();
    _cellAnimationsScary = nullptr;
    _cellAnimationsJump = nullptr;
    _cellAnimationsRebound = nullptr;
    loadAnimations();
    
    // Agregamos animación para convertir figura a combo
    float _scaleBig = _cellFigureSprite->getScale()+0.1f;
    float _scaleNormal = _cellFigureSprite->getScale();
    ScaleTo* _scaleBigFigure = ScaleTo::create(0.1f,_scaleBig);
    ScaleTo* _scaleNormalFigure = ScaleTo::create(0.1f,_scaleNormal);
    _cellFigureSprite->runAction(Sequence::create(_scaleBigFigure,_scaleNormalFigure,NULL));
}

void TTCellDynamicContentFigure::convertCellToNormalFigure(int cellFigure)
{
    stopCellAnimations();
    
    if(isComboContent())
    {
        for(int i = 0; i < _comboArrows.size(); i++)
        {
            _comboArrows[i]->stopAllActions();
            _comboArrows[i]->removeFromParentAndCleanup(true);
        }
    }
    
    _cellFigure = cellFigure;
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    _contentLayerType = FIGURE_DYNAMIC_CONTENT;
    
    // Modificamos el sprite
    _cellFigureSprite->setSpriteFrame(_figureSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
    
    // cargamos los sprites para el path
    char strPath[100] = {0};
    sprintf(strPath,"figura%d_lineaseleccion.png",_cellFigure+1);
    _pathSprite->setSpriteFrame(strPath);
    
    // Cargamos las animaciones de combos
    _cellAnimationsStandBy.clear();
    _cellAnimationsScary = nullptr;
    _cellAnimationsJump = nullptr;
    _cellAnimationsRebound = nullptr;
    loadAnimations();

}

void TTCellDynamicContentFigure::showHelpCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_HELP;
    launchContentAnimation();
    _blockAnimations = true;
}

void TTCellDynamicContentFigure::stopHelpCell()
{
    if(_cellDynamicState != DYNAMIC_CONTENT_STATE_HELP)
        return;
    
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
    _blockAnimations = false;
    stopCellAnimations();
}


void TTCellDynamicContentFigure::launchComboFX()
{
    if(!isComboContent())
        return;
    
    Size _screenSize = Director::getInstance()->getWinSize();
    switch (_contentLayerType)
    {
        case COMBO_VERTICAL_DYNAMIC_CONTENT:
        {
            //Miramos si hay contenido no atravesable por el combo
            std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
            int _supLimit = _screenSize.height;
            int _infLimit = _screenSize.height;
            float _timeSupLimit = 0.5f;
            float _timeInfLimit = 0.5f;
            for(int i = 0; i < _cellsBoard[_column].size(); i++)
            {
                if(i < _row && _cellsBoard[_column][i]->isStrongWallCell())
                {
                    // Limite superior
                    _supLimit = _sizeCell*(_row-i);
                    _timeSupLimit = (_supLimit*_timeSupLimit)/_screenSize.height;
                }

                if(i > _row && _cellsBoard[_column][i]->isStrongWallCell())
                {
                    // Limite inferior
                    _infLimit = _sizeCell*(i-_row);
                    _timeInfLimit = (_infLimit*_timeInfLimit)/_screenSize.height;
                    break;
                }
            }
        
            // Lanzamos comboa arriba y abajo
            launchComboKamehameha(ARROW_PATH_N,_supLimit,_timeSupLimit);
            launchComboKamehameha(ARROW_PATH_S,_infLimit,_timeInfLimit);
        }
        break;
        case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
        {
            //Miramos si hay contenido no atravesable por el combo
            std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
            int _leftLimit = _screenSize.width;
            int _rightLimit = _screenSize.width;
            float _timeLeftLimit = 0.5f;
            float _timeRightLimit = 0.5f;
            for(int i = 0; i < _cellsBoard.size(); i++)
            {
                if(i < _column && _cellsBoard[i][_row]->isStrongWallCell())
                {
                    // Limite izquierda
                    _leftLimit = _sizeCell*(_column-i);
                    _timeLeftLimit = (_leftLimit*_timeLeftLimit)/_screenSize.width;
                }

                if(i > _column && _cellsBoard[i][_row]->isStrongWallCell())
                {
                    // Limite derecha
                    _rightLimit = _sizeCell*(i-_column);
                    _timeRightLimit = (_rightLimit*_timeRightLimit)/_screenSize.width;
                    break;
                }
            }
            
            // Lanzamos comboa derecha izquierda
            launchComboKamehameha(ARROW_PATH_O,_leftLimit,_timeLeftLimit);
            launchComboKamehameha(ARROW_PATH_E,_rightLimit,_timeRightLimit);
        }
        break;
        case COMBO_AREA_DYNAMIC_CONTENT:
        {
            launchComboKamehameha(ARROW_PATH_O,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_NO,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_N,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_NE,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_E,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_SE,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_S,_sizeCell,0.25f,125);
            launchComboKamehameha(ARROW_PATH_SO,_sizeCell,0.25f,125);
        }
        break;
        default:
        break;
    }
}

void TTCellDynamicContentFigure::launchComboKamehameha(ARROW_DIRECTIONS _kamehamehaDirection, float _offset, float _time, float _speed)
{
    CCLOG("Lanzamos Kamehameha! ");
    
    // Cargamos el sistema de partículas
    Node* _parentParticles = Node::create();
    
    char strParticles[100] = {0};
    sprintf(strParticles,"Figure%d_combo_fire",_cellFigure+1);
    ValueMap _fireParticle = TTParticleManager::getInstance()->getParticle(strParticles);
    ParticleSystemQuad* _fire = ParticleSystemQuad::create(_fireParticle);
    _fire->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    ValueMap _fireWiteParticle = TTParticleManager::getInstance()->getParticle("Wite_combo_fire");
    ParticleSystemQuad* _fireWite = ParticleSystemQuad::create(_fireWiteParticle);
    _fireWite->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    _fire->setRotation(_kamehamehaDirection*45);
    _fireWite->setRotation(_kamehamehaDirection*45);
    
    _fire->setAutoRemoveOnFinish(true);
    _fireWite->setAutoRemoveOnFinish(true);

    if(_speed != 0)
    {
        _fire->setSpeed(_speed);
        _fireWite->setSpeed(_speed);
    }
    _parentParticles->addChild(_fire,0);
    _parentParticles->addChild(_fireWite,1);

    Point _finalPosition = Point(0,0);
    switch(_kamehamehaDirection)
    {
        case ARROW_PATH_O:
            _finalPosition = Point(-_offset,0);
        break;
        case ARROW_PATH_NO:
            _finalPosition = Point(-_offset,_offset);
        break;
        case ARROW_PATH_N:
            _finalPosition = Point(0,_offset);
        break;
        case ARROW_PATH_NE:
            _finalPosition = Point(_offset,_offset);
        break;
        case ARROW_PATH_E:
            _finalPosition = Point(_offset,0);
        break;
        case ARROW_PATH_SE:
            _finalPosition = Point(_offset,_offset);
        break;
        case ARROW_PATH_S:
            _finalPosition = Point(0,-_offset);
        break;
        case ARROW_PATH_SO:
            _finalPosition = Point(-_offset,-_offset);
        break;
    }
    
    MoveTo* _moveParticlesUp = MoveTo::create(_time,_finalPosition);
    auto _cleanParticles = RemoveSelf::create(true);
    _parentParticles->runAction(Sequence::create(_moveParticlesUp, _cleanParticles,NULL));
    
    _parentContent->addChild(_parentParticles,10);
    
    //Escalamos el tamaño de las particulas en función del tamaño de la celda
    Size _screenSize = Director::getInstance()->getWinSize();
    float _scaleFire = 1.5*_sizeCell/100;
    _fire->setScale(_scaleFire);
    _fireWite->setScale(_scaleFire);
    
    _fire->resetSystem();
    _fireWite->resetSystem();
    
}

void TTCellDynamicContentFigure::prepareToAbduction(Point _abductionPoint)
{
    _contentIsDeath = true;
    _pathSprite->setVisible(false);
    _cellFigureSprite->setVisible(false);
    TTCellDynamicContent::stopCellAnimations();
    
    if(isComboContent())
    {
        for(int i = 0; i < _comboArrows.size(); i++)
        {
            _comboArrows[i]->stopAllActions();
            _comboArrows[i]->removeFromParentAndCleanup(true);
        }
    }
    
    Sprite* _spriteAbduction;
    if(!isComboContent())
    {
        _spriteAbduction = Sprite::createWithSpriteFrameName(_figureSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
    }
    else
    {
        _spriteAbduction = Sprite::createWithSpriteFrameName(_figureComboSpritesMap.find(CELL_FIGURES(_cellFigure))->second._figureNameFile);
    }
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
    
    auto _move = MoveTo::create(0.20f,_abductionPosition);
    auto _scale = ScaleTo::create(0.20f,0.1f);
    auto _removeSprite = RemoveSelf::create(true);
    _spriteAbduction->runAction(Sequence::create(_delayToAbduction, Spawn::create(_move,_scale, NULL),_removeSprite,NULL));
}

void TTCellDynamicContentFigure::playSelectSound(int _numSelection)
{
    if(_numSelection == 0)
    {
        std::string _audio = "FIGURE";
        _audio += std::to_string(_cellFigure);
        _audio += "_SELECTION";
        TTAudioManager::getInstance()->playAudio(_audio);
    }
    else
    {
        std::string _audio = "NOTE";
        if(_numSelection > 11)
            _numSelection = 11;
        _audio += std::to_string(_numSelection);
        TTAudioManager::getInstance()->playAudio(_audio);
    }
}

void TTCellDynamicContentFigure::playUnSelectSound(int _numSelection)
{
    if(_numSelection == 0)
    {
        std::string _audio = "FIGURE";
        _audio += std::to_string(_cellFigure);
        _audio += "_UNSELECTION";
        TTAudioManager::getInstance()->playAudio(_audio);
    }
}