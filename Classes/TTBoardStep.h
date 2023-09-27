//
//  TTBoardStep.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStep__
#define __animalcubes__TTBoardStep__

#include "cocos2d.h"
#include "TTEventsManager.h"
#include "thegame.h"

using namespace cocos2d;

class TTBoardStep
{
    
public:
    
    TTBoardStep();
    virtual ~TTBoardStep();
    
    virtual void updateStep(float dt){};
    virtual void starStep(){};
    virtual void stepFinished();
    virtual void processStepEvent(EventCustom* e){};
    
protected:
    
    std::vector<EventListener*> _classListeners;
};

#endif /* defined(__animalcubes__TTBoardStep__) */
