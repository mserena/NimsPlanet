//
//  TTGameBoosterRocket.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTGameBoosterRocket__
#define __animalcubes__TTGameBoosterRocket__

#include "cocos2d.h"
#include "TTGameBoosters.h"
#include "TTCell.h"

using namespace cocos2d;

class TTGameBoosterRocket: public TTGameBoosters
{
    
public:
    
    TTGameBoosterRocket();
    ~TTGameBoosterRocket();
    
    
    void aplyGameBooster();

private:
    
    Sprite* _bgSpritePixel;
    Sprite* _bgSprite;
    Sprite* _rocketSprite;

};

#endif /* defined(__animalcubes__TTGameBoosterRocket__) */
