//
//  TTUIDefines.h
//  animalcubes
//
//  Created by Marc Serena on 13/10/14.
//
//

#ifndef animalcubes_TTUIDefines_h
#define animalcubes_TTUIDefines_h

#include "TTUIShows.h"

typedef struct goalImageTextUI
{
    cocos2d::ui::Widget*           _obj_off;
    cocos2d::ui::Widget*           _obj_on;
    cocos2d::ui::Widget*           _obj_ok;
    cocos2d::ui::Widget*           _tick_ok;
    Sprite*                        _obj_icon;
    cocos2d::ui::TextBMFont*       _label_obj;
    TTUIShowDecrement*             _showDecrement;
}goalUI;

typedef struct boosterUI
{
    cocos2d::ui::Widget*           _boosterOff;
    cocos2d::ui::Widget*           _boosterOnParent;
    cocos2d::ui::Widget*           _boosterMore;
    cocos2d::ui::Widget*           _boosterInfo;
    cocos2d::ui::Widget*           _boosterApplyed;
    cocos2d::ui::TextBMFont*       _labelNumBoosters;
}boosterUI;

#endif
