//
//  TTGameBoosterBoom.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTGameBoosterBoom__
#define __animalcubes__TTGameBoosterBoom__

#include "cocos2d.h"
#include "TTGameBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTGameBoosterBoom: public TTGameBoosters
{
    
public:
    
    TTGameBoosterBoom();
    ~TTGameBoosterBoom();
    
    
    void aplyGameBooster();

private:
    
    Sprite* _boomSprite;

};

#endif /* defined(__animalcubes__TTGameBoosterBoom__) */
