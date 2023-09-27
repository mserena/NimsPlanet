//
//  TTBoardStepWait.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepWait__
#define __animalcubes__TTBoardStepWait__

#include "cocos2d.h"
#include "TTBoardStep.h"

using namespace cocos2d;

class TTBoardStepWait: public TTBoardStep
{
    
public:
    
    TTBoardStepWait(float time);
    ~TTBoardStepWait();
    
    void starStep();
    void stepFinished();

private:
    
    float _waitTime;

};

#endif /* defined(__animalcubes__TTBoardStepWait__) */
