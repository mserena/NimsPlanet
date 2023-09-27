#include "TTStoneGoal.h"
#include "TTGameUIManager.h"

TTStoneGoal::TTStoneGoal(int breaksToWin, bool opt)
{
    _optional = opt;
    _typeGoal = STONE_GOAL;
    _goalValue = breaksToWin;
    _actualValue = 0;
    
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_STONE_BREAK,CC_CALLBACK_1(TTStoneGoal::onEvent_newBreak,this)));
}

TTStoneGoal::~TTStoneGoal()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTStoneGoal::reset()
{
    _actualValue = 0;
}

void TTStoneGoal::onEvent_newBreak(EventCustom* e)
{
    bool _beforeAchieved = isAchieved();
    
    _actualValue++;
    
    //enviar evento a la UI
    GenericGoalDataEvent data;
    data.currentValue = _actualValue;
    data.goalValue = _goalValue;
    data.recentAchieved = (_beforeAchieved != isAchieved());
    data.typeGoal = STONE_GOAL;
    
    sendEvent(EVENT_STONE_UI,&data);
    
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
        sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }
    
}