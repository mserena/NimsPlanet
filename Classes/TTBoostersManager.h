//
//  TTBoostersManager.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoostersManager__
#define __animalcubes__TTBoostersManager__

#include "cocos2d.h"

#include "TTBoardBoosters.h"
#include "TTBoardBoosterMoreMoves.h"
#include "TTBoardBoosterMoreTime.h"
#include "TTBoardBoosterCombos.h"
#include "TTBoardBoosterComboColor.h"

#include "TTGameBoosters.h"
#include "TTGameBoosterComboColor.h"
#include "TTGameBoosterHammer.h"
#include "TTGameBoosterPick.h"
#include "TTGameBoosterBoom.h"
#include "TTGameBoosterRocket.h"

using namespace cocos2d;

class CC_DLL TTBoostersManager: public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTBoostersManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTBoostersManager(){};
    
public:
    
    ~TTBoostersManager();
    
    TTBooster* getBoosterById(std::string boosterId);
    void applyBoosterById(std::string boosterId);
    void activateBoosterById(std::string boosterId);
    
    void loadBoosters();
    void verifyBoostersSelection(Point _pointTouch);
    void releaseBoostersSelection();
    void applyActiveGameBoosters();
    void applyActiveBoardBoosters();
    void resetBoosters();
    
public:
    
    std::map<BOARD_BOOSTERS_TYPE,TTBoardBoosters*>  _boardBoostersInstances;
    std::map<GAME_BOOSTERS_TYPE,TTGameBoosters*>    _gameBoostersInstances;
};


#endif /* defined(__animalcubes__TTBoostersManager__) */
