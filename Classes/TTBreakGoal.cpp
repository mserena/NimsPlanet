#include "TTBreakGoal.h"
#include "TTGameUIManager.h"

TTBreakGoal::TTBreakGoal(int breaksToWin, bool opt)
{
    _optional = opt;
    _typeGoal = BREAKS_GOAL;
    _goalValue = breaksToWin;
    _actualValue = 0;
    
    _classListeners.push_back(registerListenerEvent(EVENT_GOAL_NEW_BREAK,CC_CALLBACK_1(TTBreakGoal::onEvent_newBreak,this)));
}

TTBreakGoal::~TTBreakGoal()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTBreakGoal::reset()
{
    _actualValue = 0;
}

void TTBreakGoal::onEvent_newBreak(EventCustom* e)
{
    bool _beforeAchieved = isAchieved();

    _actualValue++;
    
    //enviar evento a la UI
    GenericGoalDataEvent data;
    data.currentValue = _actualValue;
    data.goalValue = _goalValue;
    data.recentAchieved = (_beforeAchieved != isAchieved());
    data.typeGoal = BREAKS_GOAL;

    sendEvent(EVENT_BREAKS_UI,&data);
    
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
        sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }

}