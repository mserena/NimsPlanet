//
//  TTBoardBoosterMoreMoves.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoardBoosterMoreMoves__
#define __animalcubes__TTBoardBoosterMoreMoves__

#include "cocos2d.h"
#include "TTBoardBoosters.h"
#include "TTGameInfo.h"

using namespace cocos2d;

class TTBoardBoosterMoreMoves: public TTBoardBoosters
{
    
public:
    
    TTBoardBoosterMoreMoves();
    ~TTBoardBoosterMoreMoves();
    
    void aplyBoardBooster(gameInfo* _gameConfig);
};

#endif /* defined(__animalcubes__TTBoardBoosterMoreMoves__) */
