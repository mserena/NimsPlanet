//
//  TTBoardBoosterCombos.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoardBoosterCombos__
#define __animalcubes__TTBoardBoosterCombos__

#include "cocos2d.h"
#include "TTBoardBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardBoosterCombos: public TTBoardBoosters
{
    
public:
    
    TTBoardBoosterCombos();
    ~TTBoardBoosterCombos();
    
    void aplyBoardBooster(std::vector<TTCell*> _cellsBoard);
};

#endif /* defined(__animalcubes__TTBoardBoosterCombos__) */
