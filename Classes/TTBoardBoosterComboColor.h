//
//  TTBoardBoosterComboColor.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoardBoosterComboColor__
#define __animalcubes__TTBoardBoosterComboColor__

#include "cocos2d.h"
#include "TTBoardBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardBoosterComboColor: public TTBoardBoosters
{
    
public:
    
    TTBoardBoosterComboColor();
    ~TTBoardBoosterComboColor();
    
    void aplyBoardBooster(std::vector<TTCell*> _cellsBoard);
};

#endif /* defined(__animalcubes__TTBoardBoosterComboColor__) */
