//
//  TTBoardStepsManager.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepsManager__
#define __animalcubes__TTBoardStepsManager__

#include "cocos2d.h"
#include "TTCell.h"
#include "TTBoardStep.h"
#include "TTBoardStepOVNI.h"
#include "TTBoardStepKillCells.h"
#include "TTBoardStepWait.h"
#include "TTBoardStepConvertCellsToCombo.h"
#include "TTBoardStepLaunchKamehamehaComboColor.h"
#include "TTBoardStepParty.h"
#include "TTBoardStepWaitUpdateCells.h"
#include "TTBoardStepHitCells.h"

using namespace cocos2d;

class TTBoardStepsManager
{
    
public:
    
    TTBoardStepsManager();
    ~TTBoardStepsManager();
    
    void update(float dt);
    void initSteps();
    void allStepsFinished();
    void stepsToHitCells(std::map<TTCell*,int> _cellsHitsMap);
    void stepsToComboColorWithNormalFigure(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor, std::map<TTCell*,int> _cellsHitsMap);
    void stepsToComboColorWithComboFigure(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor,std::map<TTCell*,int> _cellsHitsMap,CONTENT_TYPES comboType);
    void stepsToComboColorWithComboColor(std::vector<TTCell*> _abductionCells,std::vector<TTCell*> _hitCells);
    void stepsToFreeKamehamehaComboColor(TTCell* _comboColorCell, std::vector<TTCell*> _cellsAffectComboColor);
    void stepsToKillCells(std::vector<TTCell*> _cellsToKill, bool _completeKill = false);
    void stepsToFinishParty();
    
    bool isRunningSteps();
    
private:
    
    void onEvent_stepFinish(EventCustom* e);
    
public:
    
    std::vector<std::vector<TTBoardStep*>> _steps;

private:
    
    std::vector<EventListener*> _classListeners;
    
    int                         _subStepsCounter;
    
};

#endif /* defined(__animalcubes__TTBoardStepsManager__) */
