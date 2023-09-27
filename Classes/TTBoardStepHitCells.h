//
//  TTBoardStepHitCells.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepHitCells__
#define __animalcubes__TTBoardStepHitCells__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardStepHitCells: public TTBoardStep
{
    
public:
    
    TTBoardStepHitCells(std::map<TTCell*,int> cellsHitsMap);
    ~TTBoardStepHitCells();
    
    void starStep();
    void stepFinished();
    
private:
    
    bool    _isRunningStep;
    std::map<TTCell*,int> _cellsHitsMap;

};

#endif /* defined(__animalcubes__TTBoardStepHitCells__) */
