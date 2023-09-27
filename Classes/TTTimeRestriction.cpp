#include "TTTimeRestriction.h"

TTTimeRestriction::TTTimeRestriction():
_limitTime(0),
_currTime(0),
_extraTime(0),
_finishedTime(false),
_pause(false)
{
    _allowFinish = false;
    _typeRestriction = TIME_RESTRICTION;
}

TTTimeRestriction::TTTimeRestriction(int limit, bool allowsFinish):
_limitTime(limit),
_currTime(0),
_extraTime(0),
_finishedTime(false),
_pause(false)
{
    _allowFinish = allowsFinish;
    _typeRestriction = TIME_RESTRICTION;
    
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_PAUSE,CC_CALLBACK_1(TTTimeRestriction::onEvent_pauseGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_CONTINUE,CC_CALLBACK_1(TTTimeRestriction::onEvent_continueGame,this)));
}

TTTimeRestriction::~TTTimeRestriction()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTTimeRestriction::update(float dt)
{
    if(_finishedTime || _pause)
        return;
    
    _currTime += dt;
    if(_currTime >= (_limitTime+_extraTime))
    {
        _currTime = (_limitTime+_extraTime);
        _finishedTime = true;
        
        //Enviamos evento de finalizacion
        sendEvent(EVENT_RESTRICTION_FINISH_TIME);
        
        return;
    }
    
    return;
}

void TTTimeRestriction::reset()
{
    _finishedTime = false;
    _currTime = 0;
    _extraTime = 0;
}

int TTTimeRestriction::getTimeToFinish()
{
    int timeToFinish = (_limitTime+_extraTime)-_currTime;
    return timeToFinish;
}

void TTTimeRestriction::addExtraTime(int time)
{
    _finishedTime = false;
    _extraTime += time;
}

void TTTimeRestriction::onEvent_pauseGame(EventCustom* e)
{
    _pause = true;
}

void TTTimeRestriction::onEvent_continueGame(EventCustom* e)
{
    _pause = false;
}
