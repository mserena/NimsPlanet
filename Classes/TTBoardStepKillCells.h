//
//  TTBoardStepKillCells.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepKillCells__
#define __animalcubes__TTBoardStepKillCells__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardStepKillCells: public TTBoardStep
{
    
public:
    
    TTBoardStepKillCells(std::vector<TTCell*> cellsToKill, bool completeKill = false, std::vector<TTCell*> noSelectableCells = std::vector<TTCell*>(),std::map<int,int> cellInstances = std::map<int,int>());
    ~TTBoardStepKillCells();
    
    void starStep();
    void stepFinished();

private:
    
    void onEvent_cellDeath(EventCustom* e);
    
private:
    
    int _numCellsToKill;
    std::vector<TTCell*> _cellsToKill;
    bool _completeKill;
    std::vector<TTCell*> _noSelectableCells;
    std::map<int,int> _cellInstances;
    
};

#endif /* defined(__animalcubes__TTBoardStepKillCells__) */
