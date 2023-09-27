//
//  TTBoardStep.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStep.h"

TTBoardStep::TTBoardStep()
{
    
}

TTBoardStep::~TTBoardStep()
{

}

void TTBoardStep::stepFinished()
{
    sendEvent(EVENT_BOARD_STEP_FINISHED);
}