//
//  TTBoardStepLaunchKamehamehaComboColor.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepLaunchKamehamehaComboColor__
#define __animalcubes__TTBoardStepLaunchKamehamehaComboColor__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardStepLaunchKamehamehaComboColor: public TTBoardStep
{
    
public:
    
    TTBoardStepLaunchKamehamehaComboColor(TTCell* comboColorCell, std::vector<TTCell*> _cellsAffectComboColor);
    ~TTBoardStepLaunchKamehamehaComboColor();
    
    void starStep();
    void stepFinished();
    
private:
    
    void onEvent_kamehamehaFinish(EventCustom* e);
    
private:
    
    std::vector<Point> _positionsToKill;
    TTCell* _comboColorCell;
    int _typeCells;

    int _kamehamehas;
    
    bool _isActive;
};

#endif /* defined(__animalcubes__TTBoardStepLaunchKamehamehaComboColor__) */
