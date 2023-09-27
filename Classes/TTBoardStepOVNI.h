//
//  TTBoardStepOVNI.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepOVNI__
#define __animalcubes__TTBoardStepOVNI__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

#define WIDTH_OVNI_SPACE             25
#define HEIGHT_OVNI_SPACE            25

#define TIME_ABDUCTION 1.5f
#define ANGLE_ABDUCTION 120.0

using namespace cocos2d;

class TTBoardStepOVNI: public TTBoardStep
{
    
public:
    
    TTBoardStepOVNI(std::vector<TTCell*> abductionCells,std::vector<TTCell*> hitCells);
    ~TTBoardStepOVNI();
    
    void starStep();
    void stepFinished();
    
private:
    
    Node*   _parentOvni;
    Sprite* _ovni;
    Sprite* _ovniPower;
    
    std::vector<TTCell*> _abductionCells;
    std::vector<TTCell*> _hitCells;
    
};

#endif /* defined(__animalcubes__TTBoardStepOVNI__) */
