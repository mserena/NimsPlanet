//
//  TTGameBoosterPick.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTGameBoosterPick__
#define __animalcubes__TTGameBoosterPick__

#include "cocos2d.h"
#include "TTGameBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTGameBoosterPick: public TTGameBoosters
{
    
public:
    
    TTGameBoosterPick();
    ~TTGameBoosterPick();
    
    
    void aplyGameBooster();

private:
    
    Sprite* _pickSprite;

};

#endif /* defined(__animalcubes__TTGameBoosterPick__) */
