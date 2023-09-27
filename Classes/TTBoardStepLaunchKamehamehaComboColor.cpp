//
//  TTBoardStepLaunchKamehamehaComboColor.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepLaunchKamehamehaComboColor.h"

TTBoardStepLaunchKamehamehaComboColor::TTBoardStepLaunchKamehamehaComboColor(TTCell* comboColorCell, std::vector<TTCell*> _cellsAffectComboColor):
TTBoardStep::TTBoardStep(),
_comboColorCell(comboColorCell),
_kamehamehas(_cellsAffectComboColor.size()),
_isActive(false)
{
    if (_cellsAffectComboColor.size()>0)  //control de tamaño
    {
        _typeCells = _cellsAffectComboColor[0]->getCellFigure();
    }
    
    for(int u = 0; u < _cellsAffectComboColor.size(); u++)
        _positionsToKill.push_back(Point(_cellsAffectComboColor[u]->_posXCell,_cellsAffectComboColor[u]->_posYCell));
    
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_KAMEHAMEHA_FINISHED,CC_CALLBACK_1(TTBoardStepLaunchKamehamehaComboColor::onEvent_kamehamehaFinish,this)));
}

TTBoardStepLaunchKamehamehaComboColor::~TTBoardStepLaunchKamehamehaComboColor()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTBoardStepLaunchKamehamehaComboColor::starStep()
{
    if (_positionsToKill.size()>0)
    {
        _isActive = true;
        (static_cast<TTCellDynamicContentFigureComboColor*>(_comboColorCell->_cellContentLayers.back()))->launchComboFx(_positionsToKill,_typeCells);
    }
    else
    {
        //no habia ningun elemento, el step ha acabado
        stepFinished();
    }
}

void TTBoardStepLaunchKamehamehaComboColor::stepFinished()
{
    TTBoardStep::stepFinished();
}

void TTBoardStepLaunchKamehamehaComboColor::onEvent_kamehamehaFinish(EventCustom* e)
{
    if(!_isActive) return;

    _kamehamehas--;
    if(_kamehamehas == 0)
        stepFinished();
}