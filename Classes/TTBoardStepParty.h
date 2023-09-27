//
//  TTBoardStepParty.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepParty__
#define __animalcubes__TTBoardStepParty__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"
#include "TTLanguageManager.h"

using namespace cocos2d;

class TTBoardStepParty: public TTBoardStep
{
    
public:
    
    TTBoardStepParty();
    ~TTBoardStepParty();
    
    void starStep();
    void stepFinished();
    
    void launchFireWork(float timeDelay, int typeFireWork);
    
private:
    
    Sprite*  _woodInfo;
    Label*  _textWinLevel;
    
};

#endif /* defined(__animalcubes__TTBoardStepParty__) */
