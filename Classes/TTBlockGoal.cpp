#include "TTBlockGoal.h"
#include "TTGameUIManager.h"

TTBlockGoal::TTBlockGoal(int breaksToWin, bool opt)
{
    _optional = opt;
    _typeGoal = BLOCK_GOAL;
    _goalValue = breaksToWin;
    _actualValue = 0;
    
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_WOOD_BREAK,CC_CALLBACK_1(TTBlockGoal::onEvent_newBreak,this)));
}

TTBlockGoal::~TTBlockGoal()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTBlockGoal::reset()
{
    _actualValue = 0;
}

void TTBlockGoal::onEvent_newBreak(EventCustom* e)
{
    bool _beforeAchieved = isAchieved();

    _actualValue++;
    
    GenericGoalDataEvent dataEvent;
    dataEvent.currentValue = _actualValue;
    dataEvent.goalValue = _goalValue;
    dataEvent.typeGoal = BLOCK_GOAL;
    dataEvent.recentAchieved = (_beforeAchieved != isAchieved());
    sendEvent(EVENT_BLOCK_UI,&dataEvent);
    
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
        sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }

}