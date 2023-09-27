//
//  TTMapStopperManager.h
//  animalplanet
//
//  Created by Marc Serena on 21/4/15.
//
//

#ifndef __animalplanet__TTMapStopperManager__
#define __animalplanet__TTMapStopperManager__

#include "cocos2d.h"
#include "TTEventsManager.h"

using namespace cocos2d;
using namespace CocosDenshion;

//Un día en milisegundos
#define DAY_IN_MILISECONDS      86400000
#define MIN_IN_MILISECONDS      60000

//Valores del stopper
#define MIN_FRIENDS_TO_INVITE_STOPPER_UNLOCK    3

//Default data para los stopper
#define USER_DATA_STOPPER_FINISH_DATE         "_UserStopperFinishDate"
#define USER_DATA_STOPPER_FINISHED            "_UserStopperFinished"
#define USER_DATA_STOPPERS_UNLOKED            "_UserStoppersUnloked"
#define USER_DATA_STOPPERS_INVITED_FRIENDS    "_UserStoppersInvitedFriends"

class CC_DLL TTMapStopperManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTMapStopperManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTMapStopperManager(){}
    
public:
    
    ~TTMapStopperManager();
    void init(void);
    
    bool isStopperActive();
    void verifyServerTimeForStopper();
    void updateLocalTimeToFinish(float _dt);
    bool isStopperFinished();
    void resetStopper();
    std::vector<int> getStoppersUnlokedInfo();
    void forceFinishStopper();
    void userInvitedFriends(int _numFriends);
    int getUserInvitedFriends();
    
    void updateStoppersInfoFromPosition();

private:
    
    //Eventos
    void onEvent_getTimeServer(EventCustom* e);
    
public:
    
    UserDefault*                    _def;
    std::string                     _stopperFinishDay;
    long double                     _actualTimeClock;
    
    std::vector<EventListener*>     _classListeners;
    
};

#endif /* defined(__animalplanet__TTMapStopperManager__) */
