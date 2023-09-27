//
//  TTGameBoosters.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTGameBoosters.h"
#include "TTUserInfo.h"
#include "TTStoreManager.h"

TTGameBoosters::TTGameBoosters(GAME_BOOSTERS_TYPE subtypeBooster, std::string boosterIcon, std::vector<CONTENT_TYPES> cellsAllowed):
TTBooster(GAME_BOOSTERS),
_boosterSubType(subtypeBooster),
_boosterIcon(boosterIcon),
_boosterActivated(false),
_activeSelection(false),
_onlySelectableCells(false),
_cellsAllowed(cellsAllowed)
{

}

TTGameBoosters::~TTGameBoosters()
{
    
}

//identificador interno de booster
int TTGameBoosters::getBoosterId()
{
    return _boosterType*100+_boosterSubType;
}

bool TTGameBoosters::canBeApplied(gameInfo* _gameinfo)
{
    //solo se puede aplicar si está dentro de la definicion del gameinfo
    return _gameinfo->isBoosterDefined(_boosterType, _boosterSubType);
}

bool TTGameBoosters::isCellAllowed(TTCell* _cell)
{
    if(_cellsAllowed.size() == 0)
    {
        return _cell->isSelectableCell();
    }
    bool _allowedType = false;
    for(int i = 0; i < _cellsAllowed.size() && !_allowedType; i++)
    {
        _allowedType = ( _cell->getCellContentTypes().size() > 0 &&_cell->getCellContentTypes().back() == _cellsAllowed[i]);
        if(_onlySelectableCells && !_cell->isSelectableCell())
        {
            _allowedType = false;
        }
    }
    return _allowedType;
}

void TTGameBoosters::showApplyPosition()
{
    Node* _parentSelection = TTBoard::getInstance()->_parentHelpBooster;
    std::vector<std::vector<TTCell*>> _cellsInBoard = TTBoard::getInstance()->_Cells;
    //buscar una celda en medio del tablero
    int minDistance = 10000;
    int _col=-1;
    int _fil=-1;
    int _centerCol = (int)_cellsInBoard.size()/2;
    int _centerFil = (int)_cellsInBoard[0].size()/2;
    
    for(int i = 0; i < _cellsInBoard.size(); i++)
    {
        for(int j = 0; j < _cellsInBoard[0].size(); j++)
        {
            int dist = (i-_centerCol)*(i-_centerCol)+(j-_centerFil)*(j-_centerFil);
            if(isCellAllowed(_cellsInBoard[i][j]) && dist<minDistance) {
                minDistance = dist;
                _col = i;
                _fil = j;
            }
         }
    }
    
    _parentSelection->setVisible(true);
    _parentSelection->setPosition(_cellsInBoard[_col][_fil]->_posXCell,_cellsInBoard[_col][_fil]->_posYCell);
    _selectedCell = _cellsInBoard[_col][_fil];
    
    sendEvent(EVENT_GAME_PAUSE);
    _activeSelection = true;

}

void TTGameBoosters::moveApplyPosition(Point _touchPoint)
{
    if (!_activeSelection)
        return;

    std::vector<std::vector<TTCell*>> _cellsInBoard = TTBoard::getInstance()->_Cells;
    for(int i = 0; i < _cellsInBoard.size(); i++)
    {
        for(int j = 0; j < _cellsInBoard[0].size(); j++)
        {
            if(_cellsInBoard[i][j]->isCellTouched(_touchPoint.x,_touchPoint.y,_cellsAllowed))
            {
                Node* _parentSelection = TTBoard::getInstance()->_parentHelpBooster;
                _parentSelection->setPosition(_cellsInBoard[i][j]->_posXCell,_cellsInBoard[i][j]->_posYCell);
                _selectedCell = _cellsInBoard[i][j];
                return;
            }
        }
    }
}

void TTGameBoosters::aplyGameBooster()
{
    _activeSelection = false;
    _boosterActivated = false;
    
    // Restamos el booster usado
    std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"1_"+std::to_string(_boosterSubType));
    TTStoreManager::getInstance()->takeUserVirtualItem(_boosterItemId);
    
    //identificamos el booster en la jugada
    TTBoard::getInstance()->_finishInfo.boosterPlay = getBoosterId();
}

void TTGameBoosters::resetBooster()
{
    _activeSelection = false;
    _boosterActivated = false;
}
