//
//  TTUserLives.cpp
//  animalplanet
//
//  Created by Marc Serena on 07/11/14.
//
//

#include "TTUserLives.h"
#include "TTUIMap.h"
#include "TTAnimalCubes.h"
#include <time.h>

#define MAX_LIVES               5       //5 lives
#define SECONDS_TO_REGENERATE   1800    //30min

//Singleton
static TTUserLives *_sharedUserLives = nullptr;

TTUserLives* TTUserLives::getInstance()
{
    if (!_sharedUserLives)
    {
        _sharedUserLives = new TTUserLives();
        _sharedUserLives->init();
    }
    
    return _sharedUserLives;
}

void TTUserLives::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedUserLives);
}

void TTUserLives::init()
{
    readLivesInfo();
    
    // Agregamos la clase al update
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTUserLives::updateLivesControl),this,0.5f,false);

}

TTUserLives::~TTUserLives()
{
    
}

void TTUserLives::saveLivesInfo()
{
    std::string _infoLives = "";
    
    for(int i = 0; i < _timeToRegenerate.size(); i++)
    {
        _infoLives += std::to_string(_timeToRegenerate[i]);
        
        if(i != _timeToRegenerate.size()-1)
        {
            _infoLives += ",";
        }
    }
    UserDefault::getInstance()->setStringForKey(USER_LIVES_INFO,_infoLives);
    CCLOG("Almacenamos información de vidas en userdata: %s",_infoLives.c_str());
}

void TTUserLives::readLivesInfo()
{
    _timeToRegenerate.clear();
    
    std::string _infoLives = UserDefault::getInstance()->getStringForKey(USER_LIVES_INFO);
    if(_infoLives == "")
    {
        return;
    }
    
    std::vector<long> _infoLivesVec = tokenizeStringCommaDelimitedToLong(_infoLives);
    
    time_t _actualTime;
    time(&_actualTime);
    
    
    for(int i = 0; i < _infoLivesVec.size(); i++)
    {
        if(_infoLivesVec[i] == 0) continue;
        
        if(_infoLivesVec[i] > _actualTime)
        {
            //guardarlo en el vector de times
            _timeToRegenerate.push_back(_infoLivesVec[i]);
        }
    }
    
     //guardamos la informacion actualizada
    saveLivesInfo();
}

void TTUserLives::updateLivesControl(float dt)
{
    if (!_timeToRegenerate.empty())
    {
        time_t _actualTime;
        time(&_actualTime);

        bool _someElementRemoved=false;;
        //borrar los elementos de time que ya hayan pasado
        _timeToRegenerate.erase(std::remove_if(_timeToRegenerate.begin(), _timeToRegenerate.end(), [_actualTime,&_someElementRemoved](long _value)
        {
            _someElementRemoved =  _someElementRemoved || (_value <= _actualTime);
            return _value <= _actualTime;
        }), _timeToRegenerate.end());

        if (_someElementRemoved)
        {
            saveLivesInfo();
        }
        if(TTAnimalCubes::getInstance()->_gameState!=GAME_STATE_INGAME)
        {
            TTUIMap::getInstance()->updateUIValues();
        }
       
    }
    
}

bool TTUserLives::hasLive()
{
    if (_timeToRegenerate.size()==MAX_LIVES)
    {
        //no hay vidas
        return false;
    }
    return true;
}

bool TTUserLives::playOneLive()
{
    if (_timeToRegenerate.size()==MAX_LIVES)
    {
        //no hay vidas
        return false;
    }
    
    time_t timer;
    time(&timer);

    if (!_timeToRegenerate.empty())
    {
        timer = _timeToRegenerate[_timeToRegenerate.size()-1];
    }
    
    _timeToRegenerate.push_back(timer + SECONDS_TO_REGENERATE);
    
    saveLivesInfo();
    
    return true;
    
 }

livesInfo TTUserLives::getLivesInfo()
{
    int livesCount = MAX_LIVES - (int)_timeToRegenerate.size();
    
    std::string _timeToRegenerateStr = TTLanguageManager::getInstance()->getTag(FULL_TAG);
    
    if (livesCount<MAX_LIVES)
    {
        time_t _actualTime;
        time(&_actualTime);
        long timeDif = _timeToRegenerate[0]-_actualTime;
        if(timeDif < 0) timeDif = 0;
        
        _timeToRegenerateStr = secondsToStringTime((int)timeDif);
        
    }
 
    livesInfo info;
    info._numActiveLives = livesCount;
    info._timeToRegenerate = _timeToRegenerateStr;
    return info;
}

void TTUserLives::restoreAllLives()
{
    _timeToRegenerate.clear();

    saveLivesInfo();
    
    TTUIMap::getInstance()->updateUIValues();
}

void TTUserLives::restoreActualLive()
{
    if (_timeToRegenerate.size()>0)
    {
        //eliminar el ultimo elemento
        _timeToRegenerate.pop_back();
    }
    
    saveLivesInfo();
    
    TTUIMap::getInstance()->updateUIValues();
}

bool TTUserLives::allLivesActive()
{
    return _timeToRegenerate.empty();
}

void TTUserLives::addOneLive()
{
    if(_timeToRegenerate.size() == 0)
        return;
    
    //eliminar el ultimo elemento
    _timeToRegenerate.pop_back();
    
    //guargamos la nueva información de vidas
    saveLivesInfo();
    TTUIMap::getInstance()->updateUIValues();
}
