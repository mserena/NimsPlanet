//
//  TTBoardStepsManager.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepsManager.h"
#include "TTBoard.h"

TTBoardStepsManager::TTBoardStepsManager()
{
    _steps.clear();
    
     _classListeners.push_back(registerListenerEvent(EVENT_BOARD_STEP_FINISHED,CC_CALLBACK_1(TTBoardStepsManager::onEvent_stepFinish,this)));
}

TTBoardStepsManager::~TTBoardStepsManager()
{
    for(int i = 0; i < _steps.size(); i++)
    {
        for(int j = 0; j < _steps[i].size(); j++)
        {
            delete(_steps[i][j]);
        }
    }
    _steps.clear();
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTBoardStepsManager::initSteps()
{
    if(_steps.size() == 0)
        return;
    
    _subStepsCounter = _steps[0].size();
    for(int i = 0; i < _steps[0].size(); i++)
    {
        _steps[0][i]->starStep();
    }
}

void TTBoardStepsManager::update(float dt)
{
    for(int i = 0; i < _steps.size(); i++)
    {
        for(int j = 0; j < _steps[i].size(); j++)
        {
            _steps[i][j]->updateStep(dt);
        }
    }
}

bool TTBoardStepsManager::isRunningSteps()
{
    if(_steps.size() > 0)
        return true;
    return false;
}

void TTBoardStepsManager::allStepsFinished()
{
    CCLOG("Todos los steps terminados! ");
    _steps.clear();
    sendEvent(EVENT_BOARD_ALL_STEPS_FINISHED);
}

void TTBoardStepsManager::stepsToComboColorWithNormalFigure(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor, std::map<TTCell*,int> _cellsHitsMap)
{
    // Kamehameha
    TTBoardStepLaunchKamehamehaComboColor* _kamehamehaComboColor = new TTBoardStepLaunchKamehamehaComboColor(_comboColorCell,_cellsAffectComboColor);
    std::vector<TTBoardStep*> _actualStepKamehameha;
    _actualStepKamehameha.push_back(_kamehamehaComboColor);
    _steps.push_back(_actualStepKamehameha);

    stepsToHitCells(_cellsHitsMap);
}

void TTBoardStepsManager::stepsToComboColorWithComboFigure(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor,std::map<TTCell*,int> _cellsHitsMap,CONTENT_TYPES comboType)
{
    
    //Paso 1. Debería mirar si hay algún kame y pasarlo a despues de la creación de combos
    std::vector<std::vector<TTBoardStep*>> _stepsBackup = _steps;
    _steps.clear();
    
    //Paso 2. Printado de los hits a dar
    /*std::map<TTCell*,int>::iterator _cellsHitsMapItr;
    for(_cellsHitsMapItr = _cellsHitsMap.begin(); _cellsHitsMapItr != _cellsHitsMap.end(); ++_cellsHitsMapItr)
    {
        CCLOG("Celda c,r = %d,%d hits = %d", (*_cellsHitsMapItr).first->_column,(*_cellsHitsMapItr).first->_row,(*_cellsHitsMapItr).second);
    }*/
    
    // Kamehameha
    TTBoardStepLaunchKamehamehaComboColor* _kamehamehaComboColor = new TTBoardStepLaunchKamehamehaComboColor(_comboColorCell,_cellsAffectComboColor);
    std::vector<TTBoardStep*> _actualStepKamehameha;
    _actualStepKamehameha.push_back(_kamehamehaComboColor);
    _steps.push_back(_actualStepKamehameha);
    
    // Convert Cells to Combo
    TTBoardStepConvertCellsToCombo* _convertCellsToCombo = new TTBoardStepConvertCellsToCombo(_cellsAffectComboColor, comboType);
    std::vector<TTBoardStep*> _actualStepConvert;
    _actualStepConvert.push_back(_convertCellsToCombo);
    _steps.push_back(_actualStepConvert);
    
    // Delay de tiempo
    TTBoardStepWait* _wait = new TTBoardStepWait(1.0f);
    std::vector<TTBoardStep*> _actualStepTime;
    _actualStepTime.push_back(_wait);
    _steps.push_back(_actualStepTime);
    
    //Recuperamos el backup
    for(int i = 0; i < _stepsBackup.size(); i++)
    {
        _steps.push_back(_stepsBackup[i]);
    }
    
    // Ejecutamos paso de hit de las celdas
    TTBoardStepHitCells* _hitCells = new TTBoardStepHitCells(_cellsHitsMap);
    std::vector<TTBoardStep*> _actualStepHit;
    _actualStepHit.push_back(_hitCells);
    _steps.push_back(_actualStepHit);
    
    TTBoardStepWaitUpdateCells* _waitCells = new TTBoardStepWaitUpdateCells();
    std::vector<TTBoardStep*> _actualStepWait;
    _actualStepWait.push_back(_waitCells);
    _steps.push_back(_actualStepWait);
    
    // Temporal, lanzar desde TTBoard
    initSteps();
    
}

void TTBoardStepsManager::stepsToHitCells(std::map<TTCell*,int> _cellsHitsMap)
{
    // Ejecutamos paso de hit de las celdas
    TTBoardStepHitCells* _hitCells = new TTBoardStepHitCells(_cellsHitsMap);
    std::vector<TTBoardStep*> _actualStepHit;
    _actualStepHit.push_back(_hitCells);
    _steps.push_back(_actualStepHit);
    
    TTBoardStepWaitUpdateCells* _waitCells = new TTBoardStepWaitUpdateCells();
    std::vector<TTBoardStep*> _actualStepWait;
    _actualStepWait.push_back(_waitCells);
    _steps.push_back(_actualStepWait);
    
    // Temporal, lanzar desde TTBoard
    initSteps();
}

void TTBoardStepsManager::stepsToFreeKamehamehaComboColor(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor)
{
    TTBoardStepLaunchKamehamehaComboColor* _kamehamehaComboColor = new TTBoardStepLaunchKamehamehaComboColor(_comboColorCell,_cellsAffectComboColor);
    std::vector<TTBoardStep*> _actualStepKamehameha;
    _actualStepKamehameha.push_back(_kamehamehaComboColor);
    _steps.push_back(_actualStepKamehameha);
}

void TTBoardStepsManager::stepsToComboColorWithComboColor(std::vector<TTCell*> _abductionCells,std::vector<TTCell*> _hitCells)
{
    //OVNI Show
    TTBoardStepOVNI* _ovniShow = new TTBoardStepOVNI(_abductionCells,_hitCells);
    std::vector<TTBoardStep*> _actualStepOVNI;
    _actualStepOVNI.push_back(_ovniShow);
    _steps.push_back(_actualStepOVNI);
    
    // Temporal, lanzar desde TTBoard
    initSteps();
}

void TTBoardStepsManager::stepsToKillCells(std::vector<TTCell*> _cellsToKill, bool _completeKill)
{
    // Death cells
    TTBoardStepKillCells* _killCells = new TTBoardStepKillCells(_cellsToKill,_completeKill);
    std::vector<TTBoardStep*> _actualStepKill;
    _actualStepKill.push_back(_killCells);
    _steps.push_back(_actualStepKill);
    
    // Temporal, lanzar desde TTBoard
    initSteps();
}

void TTBoardStepsManager::stepsToFinishParty()
{
    TTBoardStepParty* _party = new TTBoardStepParty();
    std::vector<TTBoardStep*> _actualStepParty;
    _actualStepParty.push_back(_party);
    _steps.push_back(_actualStepParty);
    
    // Delay de tiempo
    TTBoardStepWait* _wait = new TTBoardStepWait(2.0f);
    std::vector<TTBoardStep*> _actualStepTime;
    _actualStepTime.push_back(_wait);
    _steps.push_back(_actualStepTime);
    
    initSteps();
}

void TTBoardStepsManager::onEvent_stepFinish(EventCustom* e)
{
    CCLOG("Step finalizado");
    
    _subStepsCounter--;
    if(_subStepsCounter == 0)
    {
        // Eliminamos el step terminado
        for(int j = 0; j < _steps[0].size(); j++)
        {
            delete(_steps[0][j]);
        }
        _steps[0].clear();
        _steps.erase(_steps.begin());
    }
    else
    {
        return;
    }
    
    if(_steps.size() == 0)
    {
        allStepsFinished();
    }
    else
    {
        initSteps();
    }
}
