#include "TTNetworkGoal.h"
#include "TTGameUIManager.h"

TTNetworkGoal::TTNetworkGoal(int breaksToWin, bool opt)
{
    _optional = opt;
    _typeGoal = NETWORK_GOAL;
    _goalValue = breaksToWin;
    _actualValue = 0;
    
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_NETWORK_BREAK,CC_CALLBACK_1(TTNetworkGoal::onEvent_newBreak,this)));
}

TTNetworkGoal::~TTNetworkGoal()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTNetworkGoal::reset()
{
    _actualValue = 0;
}

void TTNetworkGoal::onEvent_newBreak(EventCustom* e)
{
    bool _beforeAchieved = isAchieved();
    
    _actualValue++;
    
    //enviar evento a la UI
    GenericGoalDataEvent data;
    data.currentValue = _actualValue;
    data.goalValue = _goalValue;
    data.recentAchieved = (_beforeAchieved != isAchieved());
    data.typeGoal = NETWORK_GOAL;
    
    sendEvent(EVENT_NETWORK_UI,&data);
    
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
        sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }
    
}