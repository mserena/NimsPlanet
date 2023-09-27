//
//  TTBoardStepWait.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepWait.h"

TTBoardStepWait::TTBoardStepWait(float time):
TTBoardStep::TTBoardStep(),
_waitTime(time)
{
    
}

TTBoardStepWait::~TTBoardStepWait()
{

}

void TTBoardStepWait::starStep()
{
    auto _delyTime = DelayTime::create(_waitTime);
    auto _stepFinish = CallFunc::create(CC_CALLBACK_0(TTBoardStepWait::stepFinished,this));
    Director::getInstance()->getRunningScene()->runAction(Sequence::create(_delyTime,_stepFinish,NULL));
}

void TTBoardStepWait::stepFinished()
{
    TTBoardStep::stepFinished();
}
