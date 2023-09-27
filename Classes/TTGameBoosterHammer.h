//
//  TTGameBoosterHammer.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTGameBoosterHammer__
#define __animalcubes__TTGameBoosterHammer__

#include "cocos2d.h"
#include "TTGameBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTGameBoosterHammer: public TTGameBoosters
{
    
public:
    
    TTGameBoosterHammer();
    ~TTGameBoosterHammer();
    
    
    void aplyGameBooster();

private:
    
    Sprite* _hammerSprite;

};

#endif /* defined(__animalcubes__TTGameBoosterHammer__) */
