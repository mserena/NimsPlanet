#include "TTDownObjectGoal.h"

TTDownObjectGoal::TTDownObjectGoal(int downObjectsToWin, int initWeight, int maxWeight,int increaseWeight, bool opt):
_initWeight(initWeight),
_actualWeight(initWeight),
_maxWeight(maxWeight),
_increaseWeight(increaseWeight)
{
    _optional = opt;
    _typeGoal = DOWN_OBJECT_GOAL;
    _goalValue = downObjectsToWin;
    _actualValue = 0;
    
    // Registro de recepcion de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_OBJECT_MOVE,CC_CALLBACK_1(TTDownObjectGoal::onEvent_objectMove,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_OBJECT_DOWN,CC_CALLBACK_1(TTDownObjectGoal::onEvent_objectDown,this)));
}

TTDownObjectGoal::~TTDownObjectGoal()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTDownObjectGoal::reset()
{
    _actualValue = 0;
    _actualWeight = _initWeight;
}

void TTDownObjectGoal::onEvent_objectMove(EventCustom* e)
{
    int totalMoves = *(static_cast<int*>(e->getUserData()));
    if(_actualWeight < _maxWeight)
    {
        _actualWeight += (_increaseWeight*totalMoves);
        if(_actualWeight > _maxWeight)
            _actualWeight = _maxWeight;
    }
}

void TTDownObjectGoal::onEvent_objectDown(EventCustom* e)
{
    bool _beforeAchieved = isAchieved();

    _actualValue++;
    
    //enviar evento a la UI
    GenericGoalDataEvent data;
    data.currentValue = _actualValue;
    data.goalValue = _goalValue;
    data.recentAchieved = (_beforeAchieved != isAchieved());

    data.typeGoal = DOWN_OBJECT_GOAL;
    sendEvent(EVENT_OBJECT_DOWN_UI,&data);
    
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
        sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }
}