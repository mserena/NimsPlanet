//
//  TTGameBoosterComboColor.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTGameBoosterComboColor.h"
#include "TTParticleManager.h"

#define NUM_COMBOS_COLOR 1

TTGameBoosterComboColor::TTGameBoosterComboColor():
TTGameBoosters(GAME_BOOSTER_COMBO_COLOR,
               "booster_ui_combo_color.png",
               std::vector<CONTENT_TYPES>{ FIGURE_DYNAMIC_CONTENT,
                                        COMBO_HORIZONTAL_DYNAMIC_CONTENT,
                                        COMBO_VERTICAL_DYNAMIC_CONTENT,
                                        COMBO_AREA_DYNAMIC_CONTENT})
{
    _onlySelectableCells = true;
}

TTGameBoosterComboColor::~TTGameBoosterComboColor()
{
    
}

void TTGameBoosterComboColor::aplyGameBooster()
{
    TTGameBoosters::aplyGameBooster();
    
    TTAudioManager::getInstance()->playAudio("COMBOCOLOR_BOOSTER");
    
    ValueMap _ccParticlesDic = TTParticleManager::getInstance()->getParticle("booster_combo_color");
    ParticleSystemQuad* _ccPartices = ParticleSystemQuad::create(_ccParticlesDic);
    _ccPartices->setAutoRemoveOnFinish(true);
    _ccPartices->setPosition(Point(_selectedCell->_posXCell,_selectedCell->_posYCell));
    TTBoard::getInstance()->_parentBoard->addChild(_ccPartices,LAYER_ANIMATIONS);
    _ccPartices->resetSystem();
    
    _selectedCell->convertCellToComboColor();
    
    sendEvent(EVENT_GAME_CONTINUE);
}