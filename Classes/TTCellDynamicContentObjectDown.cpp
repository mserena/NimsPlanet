#include "TTCellDynamicContentObjectDown.h"
#include "TTEventsManager.h"
#include "TTGameUIManager.h"
#include "TTGame.h"
#include "TTUserInfo.h"

TTCellDynamicContentObjectDown::TTCellDynamicContentObjectDown(cocos2d::Node* cellContentParent,int _zLayerPosition, CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell):
TTCellDynamicContent(cellContentParent,FIGURE_Z_LAYER,cellDynamicType,posXCell,posYCell,column,row,totalColumns,totalRows,sizeCell)
{
    
    _affectAreaCombination = false;
    
    if(_contentLayerType != DOWN_OBJECT_DYNAMIC_CONTENT)
    {
        CCLOG("Se intenta crear una figura no bajable usando el contructor de figura bajable");
        abort();
    }
   
    _cellDynamicState = DYNAMIC_CONTENT_STATE_NOTHING;
    
    // Creamos una figura normal
    int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
    char objectStr[100] = {0};
    sprintf(objectStr,"bajable_world%d.png",_actualWorld+1);
    _cellFigureSprite = Sprite::createWithSpriteFrameName(objectStr);
    float imageWidth = _cellFigureSprite->getBoundingBox().size.width;
    float newScale = _sizeFigure/imageWidth;
    _cellFigureSprite->setScale(newScale,newScale);
    _cellFigureSprite->setVisible(false);
    _parentContent->addChild(_cellFigureSprite,0);
    
}

TTCellDynamicContentObjectDown::~TTCellDynamicContentObjectDown()
{

}

void TTCellDynamicContentObjectDown::initCell()
{
    TTCellDynamicContent::initCell();
    _cellFigureSprite->setVisible(true);
}

bool TTCellDynamicContentObjectDown::isSelectable()
{
    return false;
}

bool TTCellDynamicContentObjectDown::isFirstSelectable()
{
    return false;
}

bool TTCellDynamicContentObjectDown::isRefillable()
{
    return false;
}

bool TTCellDynamicContentObjectDown::isWallContent()
{
    return false;
}

int TTCellDynamicContentObjectDown::hitCell(int _numHits)
{
    // No sucede nada al ser golpeado
    return _numHits;
}

float TTCellDynamicContentObjectDown::moveDownObjectOutOfBoard(int _posYDestination,int _rowDestination,int _rowInit, int _totalRows)
{
    // Situa sprite temporal de bajada de objetos
    int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
    char objectStr[100] = {0};
    sprintf(objectStr,"bajable_world%d.png",_actualWorld+1);
    Sprite* _tmpDownObject = Sprite::createWithSpriteFrameName(objectStr);
    float newScale = _sizeFigure/_tmpDownObject->getBoundingBox().size.width;
    _tmpDownObject->setScale(newScale,newScale);
    _tmpDownObject->setPosition(_posXCell,_posYCell);
    _tmpDownObject->setVisible(true);
    _parentContentsCell->addChild(_tmpDownObject,LAYER_ANIMATIONS);
     
    // Movimiento del sprite fuera del tablero
    int posYOutCell = _posYDestination-_sizeCell;
    int _totalCellsMove = (_rowDestination-_rowInit)+1;
    float _fallTime = FALL_TIME/_totalRows;
    _fallTime *= _totalCellsMove;
    MoveTo* moveCell = MoveTo::create(_fallTime,Point(_posXCell,posYOutCell));
    
    //Muerte al marcador
    int _uiPosition = 0;
    for(int i = 0; i < TTGame::getInstance()->_gameConfig->goals.size(); i++)
    {
        if(TTGame::getInstance()->_gameConfig->goals[i]->_typeGoal == DOWN_OBJECT_GOAL)
        {
            _uiPosition = i;
            break;
        }
    }

    Sprite* _icon = static_cast<Sprite*>(TTGameUIManager::getInstance()->_goals[_uiPosition]._obj_icon->getChildByName("iconGoal"));
    Point _position = _icon->getPosition();
    _position.x += _icon->getContentSize().width/2;
    _position.y += _icon->getContentSize().height/2;
    Point _destination = _icon->convertToWorldSpace(_position);
    Point _destinationUI = _parentContentsCell->convertToNodeSpace(_destination);
    auto _moveToUI = EaseExponentialOut::create(MoveTo::create(1.5f,_destinationUI));
    
    CallFunc* finshMove = CallFunc::create(std::bind(&TTCellDynamicContentObjectDown::finishMoveDownObjectOutOfBoard, this, _tmpDownObject));
    _tmpDownObject->runAction(Sequence::create(moveCell,_moveToUI,finshMove,NULL));
     
    //Envia evento de finalización de objecto bajado
    sendEvent(EVENT_OBJECT_DOWN);
     
    return _fallTime+1.0f;
}

void TTCellDynamicContentObjectDown::finishMoveDownObjectOutOfBoard(Sprite* _tmpDownObject)
{
    _tmpDownObject->setVisible(false);
}

void TTCellDynamicContentObjectDown::moveCellAlert(int moves)
{
    //Envia evento de movimiento de objeto bajable
    sendEvent(EVENT_OBJECT_MOVE,&moves);
}
