//
//  TTUserLives.h
//  animalplanet
//
//  Created by Marc Serena on 07/11/14.
//
//

#ifndef __animalplanet__TTUserLives__
#define __animalplanet__TTUserLives__

#include "thegame.h"

using namespace cocos2d;

#define USER_LIVES_INFO     "_UserLivesInfo"


typedef struct livesInfo
{
    int _numActiveLives;
    std::string _timeToRegenerate;
}livesInfo;

// Clase singleton
class CC_DLL TTUserLives: public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUserLives* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTUserLives(){}
    
public:
    
    void init();
    ~TTUserLives();
    
    void saveLivesInfo();
    void readLivesInfo();
    
    bool allLivesActive();
    void updateLivesControl(float dt);
    livesInfo getLivesInfo();
    bool hasLive();
    bool playOneLive();
    void restoreAllLives();
    void restoreActualLive();
    void addOneLive();
    
public:
    
    std::vector<long> _timeToRegenerate;
};

#endif /* defined(__animalplanet__TTUserLives__) */
