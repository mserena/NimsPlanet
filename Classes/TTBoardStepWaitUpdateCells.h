//
//  TTBoardStepWaitUpdateCells.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepWaitUpdateCells__
#define __animalcubes__TTBoardStepWaitUpdateCells__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardStepWaitUpdateCells: public TTBoardStep
{
    
public:
    
    TTBoardStepWaitUpdateCells();
    ~TTBoardStepWaitUpdateCells();
    
    void updateStep(float dt);
    void starStep();
    void stepFinished();
    
private:
    
    bool    _isRunningStep;

};

#endif /* defined(__animalcubes__TTBoardStepWaitUpdateCells__) */
