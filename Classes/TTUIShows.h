//
//  TTUIShows.h
//  animalcubes
//
//  Created by Marc Serena on 09/10/14.
//
//

#ifndef __animalcubes__TTUIShows__
#define __animalcubes__TTUIShows__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
/*
 * Show decrement
 */
class TTUIShowDecrement: public Ref
{
    
public:
    
    TTUIShowDecrement(cocos2d::ui::TextBMFont* label);
    ~TTUIShowDecrement();

    void reset();
    void addNewDecrement(int finalValue);
    
private:
    
    void decrementUpdate(float _time);
    
public:
    
    int                      _finalValue;
    int                      _actualValue;
    
    cocos2d::ui::TextBMFont* _label;
};

#endif /* defined(__animalcubes__TTUIShows__) */
