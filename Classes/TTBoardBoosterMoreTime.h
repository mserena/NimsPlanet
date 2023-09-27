//
//  TTBoardBoosterMoreTime.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoardBoosterMoreTime__
#define __animalcubes__TTBoardBoosterMoreTime__

#include "cocos2d.h"
#include "TTBoardBoosters.h"
#include "TTGameInfo.h"

using namespace cocos2d;

class TTBoardBoosterMoreTime: public TTBoardBoosters
{
    
public:
    
    TTBoardBoosterMoreTime();
    ~TTBoardBoosterMoreTime();
    
    void aplyBoardBooster(gameInfo* _gameConfig);
};

#endif /* defined(__animalcubes__TTBoardBoosterMoreTime__) */
