#include "TTCellDynamicContent.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"
#include "TTConfigParser.h"
#include "TTUserInfo.h"

TTCellDynamicContent::TTCellDynamicContent(cocos2d::Node* parentContentsCell, int _zLayerPosition, CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell):
TTCellContent(cellDynamicType),
_posXCell(posXCell),
_posYCell(posYCell),
_sizeCell(sizeCell),
_cellDynamicState(DYNAMIC_CONTENT_STATE_NOTHING),
_blockAnimations(false),
_isOnAnimation(false),
_isAnimable(false),
_column(column),
_row(row),
_totalColumns(totalColumns),
_totalRows(totalRows)
{
    _parentContentsCell = parentContentsCell;
    _parentContent = Node::create();
    _parentContent->setVisible(true);
    _parentContent->setPosition(Point(_posXCell,_posYCell));
    _parentContentsCell->addChild(_parentContent,_zLayerPosition);
    
    int _selectedWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
    int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[_selectedWorld]._charactersPack;
    _sizeFigure = (TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._figuresSizePercentage*_sizeCell)/100;
}

TTCellDynamicContent::~TTCellDynamicContent()
{
    _parentContent->removeFromParentAndCleanup(true);
}

void TTCellDynamicContent::initCell()
{
    if(!isComboContent()) _isOnAnimation = false;
    _blockAnimations= false;
}

bool TTCellDynamicContent::isEmptyCell()
{
    /*if(_cellDynamicType == EMPTY_DYNAMIC_CONTENT)
        return true;*/
    return false;
}

bool TTCellDynamicContent::isMovableContent()
{
    return true;
}

bool TTCellDynamicContent::isStrongWallContent()
{
    return false;
}

int TTCellDynamicContent::getZContent()
{
    return _parentContent->getZOrder();
}

void TTCellDynamicContent::selectedCell()
{
    // launch animation selecction
    _cellDynamicState = DYNAMIC_CONTENT_STATE_SELECTED;
}

void TTCellDynamicContent::unselectedCell()
{
    _cellDynamicState = DYNAMIC_CONTENT_STATE_STANDBY;
}

void TTCellDynamicContent::stopCellAnimations()
{
    if(!_isAnimable || _blockAnimations)
        return;
    
    // Paramos las animaciones del sprite
    _isOnAnimation = false;
    _cellFigureSprite->stopAllActions();
}

void TTCellDynamicContent::finishFallCell()
{
    // Envia evento de reposicionado de celda
    sendEvent(EVENT_BOARD_CELL_POSITIONED);
}

