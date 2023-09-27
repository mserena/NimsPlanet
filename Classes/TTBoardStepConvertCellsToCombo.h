//
//  TTBoardStepConvertCellsToCombo.h
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#ifndef __animalcubes__TTBoardStepConvertCellsToCombo__
#define __animalcubes__TTBoardStepConvertCellsToCombo__

#include "cocos2d.h"
#include "TTBoardStep.h"
#include "TTCell.h"

using namespace cocos2d;

class TTBoardStepConvertCellsToCombo: public TTBoardStep
{
    
public:
    
    TTBoardStepConvertCellsToCombo(std::vector<TTCell*> cellsToConvert, CONTENT_TYPES comboType);
    ~TTBoardStepConvertCellsToCombo();
    
    void starStep();
    void stepFinished();
    
private:
    
    std::vector<TTCell*> _cellsToConvert;
    CONTENT_TYPES _comboType;
};

#endif /* defined(__animalcubes__TTBoardStepConvertCellsToCombo__) */
