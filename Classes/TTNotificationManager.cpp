//
//  TTNotificationManager.cpp
//  animalplanet
//
//  Created by Luis Prieto on 06/02/15.
//
//

#include "TTNotificationManager.h"
#include "TTUserLives.h"
#include "TTLanguageManager.h"
#include "TTLocalNotification.h"

//Singleton
static TTNotificationManager *_sharedNotificationManager = nullptr;

TTNotificationManager* TTNotificationManager::getInstance()
{
    if (!_sharedNotificationManager)
        _sharedNotificationManager = new TTNotificationManager();
    
    return _sharedNotificationManager;
}

void TTNotificationManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedNotificationManager);
}

void TTNotificationManager::show()
{
    TTNotificationManager::cancel();
    
    //Lanzamos notificaciones locales.
    livesInfo _livesInfo = TTUserLives::getInstance()->getLivesInfo();
    if(_livesInfo._numActiveLives < MAX_GOALS)
    {
        time_t _actualTime;
        time(&_actualTime);
        
        long _timeToRegenerate = TTUserLives::getInstance()->_timeToRegenerate[TTUserLives::getInstance()->_timeToRegenerate.size()-1];
        long _timeDif = _timeToRegenerate-_actualTime;  //segundos que nos quedan para sumar la ultima vida
        
        std::string message_full_lives = TTLanguageManager::getInstance()->getTag("NOTIFICATION_FULL_LIVES");
        
        TTLocalNotification::show(message_full_lives, (int)_timeDif, ID_NOTIFICATION_FULLLIVES); //notificación local para cuando se recuperan todas las vidas.
    }

    std::string message_miss_you = TTLanguageManager::getInstance()->getTag("NOTIFICATION_MISS_YOU");
    TTLocalNotification::show(message_miss_you, 86400*DAYS_NOTIFICATION_MISSYOU, ID_NOTIFICATION_MISSYOU); //86400 = segundos por día.
}

void TTNotificationManager::cancel()
{
    TTLocalNotification::cancel(ID_NOTIFICATION_FULLLIVES);
    TTLocalNotification::cancel(ID_NOTIFICATION_MISSYOU);
}
