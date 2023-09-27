//
//  TTMapStopperManager.cpp
//  animalplanet
//
//  Created by Marc Serena on 21/4/15.
//
//

#include "TTMapStopperManager.h"
#include "TTParseManager.h"
#include "TTUIStackManager.h"
#include "TTUIPopup.h"
#include "TTUIPopupSmart.h"
#include "TTUserInfo.h"
#include "TTMapParser.h"

//Singelton
static TTMapStopperManager *_sharedMapStopperManager = nullptr;

TTMapStopperManager* TTMapStopperManager::getInstance()
{
    if (!_sharedMapStopperManager)
    {
        _sharedMapStopperManager = new TTMapStopperManager();
        _sharedMapStopperManager->init();
    }
    
    return _sharedMapStopperManager;
}

void TTMapStopperManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedMapStopperManager);
}

TTMapStopperManager::~TTMapStopperManager()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    
    // Eliminamos el sheduler
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapStopperManager::updateLocalTimeToFinish),this);
}

void TTMapStopperManager::init(void)
{
    // Registramos el evento de tiempo
    _classListeners.push_back(registerListenerEvent(EVENT_TIME_SERVER,CC_CALLBACK_1(TTMapStopperManager::onEvent_getTimeServer,this)));
}

bool TTMapStopperManager::isStopperActive()
{
    _stopperFinishDay = _def->getStringForKey(USER_DATA_STOPPER_FINISH_DATE,"");
    if(_stopperFinishDay == "") return false;
    else return true;
}

void TTMapStopperManager::verifyServerTimeForStopper()
{
    // Obtenemos el tiempo del servidor
    TTParseManager::getInstance()->getServerTime();
}

void TTMapStopperManager::updateLocalTimeToFinish(float _dt)
{
    long long _finishTime = atoll(_stopperFinishDay.c_str());
    _actualTimeClock += (_dt*1000);
    long long _restTime = _finishTime-_actualTimeClock;
    if(_restTime > 0)
    {
        // Conversión a string y actualización del label de la hora
        int seconds = (_restTime / 1000)%60;
        int minutes = ((_restTime / (1000*60))%60);
        int hours   = _restTime / (1000*60*60);
        
        char strHour[10] = {0};
        char strMin[10] = {0};
        char strSec[10] = {0};
        if(hours < 10) sprintf(strHour,"0%d:",hours);
        else sprintf(strHour,"%d:",hours);
        if(minutes < 10) sprintf(strMin,"0%d:",minutes);
        else sprintf(strMin,"%d:",minutes);
        if(seconds < 10) sprintf(strSec,"0%d",seconds);
        else sprintf(strSec,"%d",seconds);
        strcat(strHour,strMin);
        strcat(strHour,strSec);
        
        TTUIPopup::getInstance()->_stopperTime->setString(strHour);
    }
    else
    {
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapStopperManager::updateLocalTimeToFinish),this);
        
        // Volvemos a verificar contra el servidor si el tiempo es correcto y puede
        // librerarse el stopper
        TTParseManager::getInstance()->getServerTime();
    }
}

bool TTMapStopperManager::isStopperFinished()
{
    return _def->getBoolForKey(USER_DATA_STOPPER_FINISHED,false);
}

std::vector<int> TTMapStopperManager::getStoppersUnlokedInfo()
{
    return tokenizeStringCommaDelimitedToInt(_def->getStringForKey(USER_DATA_STOPPERS_UNLOKED));
}

void TTMapStopperManager::resetStopper()
{
    _def->setBoolForKey(USER_DATA_STOPPER_FINISHED,false);
    _def->setStringForKey(USER_DATA_STOPPER_FINISH_DATE,"");
    _def->setIntegerForKey(USER_DATA_STOPPERS_INVITED_FRIENDS,0);
    std::string _stoppersUnlokedInfo = _def->getStringForKey(USER_DATA_STOPPERS_UNLOKED,"");
    if(_stoppersUnlokedInfo != "")
    {
        _stoppersUnlokedInfo += ","+std::to_string(TTUserInfo::getInstance()->getActualUserProgress()._actualLevel);
        _def->setStringForKey(USER_DATA_STOPPERS_UNLOKED,_stoppersUnlokedInfo);
    }
    else
    {
        _def->setStringForKey(USER_DATA_STOPPERS_UNLOKED,std::to_string(TTUserInfo::getInstance()->getActualUserProgress()._actualLevel));
    }
    _def->flush();
    _stopperFinishDay = "";
    _actualTimeClock = 0;
}

void TTMapStopperManager::forceFinishStopper()
{
    // El stopper ha terminado
    CCLOG("El stopper ha finalizado!");
    
    // Quitamos el update
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapStopperManager::updateLocalTimeToFinish),this);
    
    // Guardamos la variable de stopper terminado
    _def->setBoolForKey(USER_DATA_STOPPER_FINISHED,true);
    _def->flush();
    
    // Se modifica la UI a la de desbloqueo
    TTUIPopup::getInstance()->showPopupStopper(TTUIPopup::getInstance()->_parentLayer);
}

void TTMapStopperManager::userInvitedFriends(int _numFriends)
{
    int _actualInvitedFriends = _def->getIntegerForKey(USER_DATA_STOPPERS_INVITED_FRIENDS,0);
    _actualInvitedFriends += _numFriends;
    if(_actualInvitedFriends >= MIN_FRIENDS_TO_INVITE_STOPPER_UNLOCK)
    {
        // Se han invitado todos los amigos para desbloquear el stopper
        forceFinishStopper();
    }
    else
    {
        _def->setIntegerForKey(USER_DATA_STOPPERS_INVITED_FRIENDS,_actualInvitedFriends);
        
        // Abrimos los candados que toque de la UI
        for(int i = 0; i < _actualInvitedFriends; i++)
        {
            TTUIPopup::getInstance()->_padlockImgParents[i]->getChildByName("lock_off")->setVisible(false);
            TTUIPopup::getInstance()->_padlockImgParents[i]->getChildByName("lock_on")->setVisible(true);
        }
    }
}

int TTMapStopperManager::getUserInvitedFriends()
{
    return _def->getIntegerForKey(USER_DATA_STOPPERS_INVITED_FRIENDS,0);
}

void TTMapStopperManager::onEvent_getTimeServer(EventCustom* e)
{
    TimeServer* _serverTime = static_cast<TimeServer*>(e->getUserData());
    TTUIStackManager::getInstance()->hideUIConnecting();
    if(!_serverTime->_error)
    {
        // Verificamos si hay algún stopper activo
        _def = UserDefault::getInstance();
        _stopperFinishDay = _def->getStringForKey(USER_DATA_STOPPER_FINISH_DATE,"");
        if(_stopperFinishDay != "")
        {
            CCLOG("Hay un stopper activo con fecha de finalización %s",_stopperFinishDay.c_str());
            
            // Verificamos el tiempo trascurrido del stopper
            if(atoll(_stopperFinishDay.c_str()) < _serverTime->_date)
            {
                // El stopper ha terminado
                CCLOG("El stopper ha finalizado!");
                
                _def->setBoolForKey(USER_DATA_STOPPER_FINISHED,true);
                _def->flush();
                
                // Se modifica la UI a la de desbloqueo
                TTUIPopup::getInstance()->showPopupStopper(TTUIPopup::getInstance()->_parentLayer);
            }
            else
            {
                _actualTimeClock = _serverTime->_date;
                Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTMapStopperManager::updateLocalTimeToFinish),this,0.01f,false);
            }
        }
        else
        {
            // Activamos el stopper
            _def->setBoolForKey(USER_DATA_STOPPER_FINISHED,false);
            long long _finishStopperTime = _serverTime->_date+(DAY_IN_MILISECONDS*2);
            _def->setStringForKey(USER_DATA_STOPPER_FINISH_DATE,std::to_string(_finishStopperTime));
            _def->flush();
            
            _stopperFinishDay = std::to_string(_finishStopperTime);
            _actualTimeClock = _serverTime->_date;
            Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTMapStopperManager::updateLocalTimeToFinish),this,0.01f,false);
        }
    }
    else
    {
        // Cerramos la UI y mostramos error de tiempo
        TTUIPopup::getInstance()->onClickExitStopperBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
        TTUIPopupSmart::getInstance()->showPopupState(TTUIPopup::getInstance()->_parentLayer,SMART_POPUP_GENERAL_CONNECTION_ERRROR_STATE,false);
    }
}

void TTMapStopperManager::updateStoppersInfoFromPosition()
{
    std::vector<int> _stoppers = TTMapParser::getInstance()->_mapInfo->_mapStoppers;
    std::vector<int> _stars = TTUserInfo::getInstance()->_userData->_levelStars;
    std::string _stoppersDeleted = "";
    for(int i = 0; i < _stars.size(); i++)
    {
        if(std::find(_stoppers.begin(), _stoppers.end(), i) != _stoppers.end())
        {
            if(_stoppersDeleted == "")
            {
                _stoppersDeleted += std::to_string(i);
            }
            else
            {
                _stoppersDeleted += ","+std::to_string(i);
            }
        }
    }
    _def->setStringForKey(USER_DATA_STOPPERS_UNLOKED,_stoppersDeleted);
    _def->flush();
}