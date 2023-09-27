//
//  TTGameBoosterPick.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTGameBoosterPick.h"
#include "TTBoardStepsManager.h"
#include "TTParticleManager.h"

#define NUM_COMBOS_COLOR 1

TTGameBoosterPick::TTGameBoosterPick():
TTGameBoosters(GAME_BOOSTER_PICK,
               "booster_ui_pico.png",
               std::vector<CONTENT_TYPES>{ FIGURE_DYNAMIC_CONTENT,
                                        COMBO_HORIZONTAL_DYNAMIC_CONTENT,
                                        COMBO_VERTICAL_DYNAMIC_CONTENT,
                                        COMBO_AREA_DYNAMIC_CONTENT,
                                        COMBO_COLOR_DYNAMIC_CONTENT,
                                        WOOD_STATIC_CONTENT,
                                        MUD_STATIC_CONTENT,
                                        STONE_STATIC_CONTENT,
                                        NET_STATIC_CONTENT,
                                        GLUE_STATIC_CONTENT})
{

}

TTGameBoosterPick::~TTGameBoosterPick()
{
    
}

void TTGameBoosterPick::aplyGameBooster()
{
    TTGameBoosters::aplyGameBooster();
    
    _pickSprite = Sprite::createWithSpriteFrameName("booster_ui_pico.png");
    float _pickSizeH = TTBoard::getInstance()->_cellSize*3;
    float _pickScale = _pickSizeH/_pickSprite->getBoundingBox().size.height;
    _pickSprite->setScale(_pickScale);
    _pickSprite->setAnchorPoint(Point(0,0));
    TTBoard::getInstance()->_parentBoard->addChild(_pickSprite,LAYER_ANIMATIONS);
    
    Point _initHammerPos = Point(_selectedCell->_posXCell-(TTBoard::getInstance()->_cellSize*2.5f),_selectedCell->_posYCell);
    _pickSprite->setPosition(_initHammerPos);
    _pickSprite->setVisible(true);
    
    auto _rotateB = RotateBy::create(0.5f,-10);
    auto _rotateF = RotateBy::create(0.2f,30);
    auto _boosterEffect = CallFunc::create(std::bind([&]()
    {
        TTAudioManager::getInstance()->playAudio("PICK_BOOSTER");
        
        ValueMap _hitParticlesDic = TTParticleManager::getInstance()->getParticle("star_win");
        ParticleSystemQuad* _hitPartices = ParticleSystemQuad::create(_hitParticlesDic);
        _hitPartices->setAutoRemoveOnFinish(true);
        _hitPartices->setPosition(Point(_selectedCell->_posXCell,_selectedCell->_posYCell));
        TTBoard::getInstance()->_parentBoard->addChild(_hitPartices,20);
        _hitPartices->setScale(_hitPartices->getScale()/2);
        _hitPartices->resetSystem();
    }));

    auto _finishBoosterApply = CallFunc::create(std::bind([&](TTCell* _cellToErase)
    {
        _pickSprite->removeFromParentAndCleanup(true);

        // Si es un combo de color, seleccionamos una celda del tablero aleatória
        std::vector<TTCell*> _cellsSelection;
        if(_cellToErase->isComboColorCell())
        {
            for(int i = 0; i < TTBoard::getInstance()->_Cells.size(); i++)
            {
                for(int j = 0; j < TTBoard::getInstance()->_Cells[i].size(); j++)
                {
                    if(TTBoard::getInstance()->_Cells[i][j]->isSelectableCell())
                    {
                        _cellsSelection.push_back(TTBoard::getInstance()->_Cells[i][j]);
                        break;
                    }
                }
                
                if(_cellsSelection.size() > 0) break;
            }
        }
        _cellsSelection.push_back(_cellToErase);
        std::vector<TTCell*> _cells = TTBoard::getInstance()->completeCellSelection(_cellsSelection);
        
        // Añadimos las celdas afectadas
        std::map<TTCell *, int> _cellsHitsMap;
        
        // Eliminamos la celda golpeada por el pico, que debe morir entera
        for(int i = 0; i < _cellsSelection.size(); i++)
        {
            if(_cellsSelection[i] == _cellToErase)
            {
                _cellsSelection.erase(_cellsSelection.begin()+i);
                i--;
            }
        }
        
        for(int i = 0; i < _cells.size(); i++)
        {
            if(_cellsHitsMap.find(_cells[i]) != _cellsHitsMap.end())
            {
                _cellsHitsMap.find(_cells[i])->second++;
            }
            else
            {
                _cellsHitsMap.insert(std::make_pair(_cells[i],1));
            }
        }
        
        // Recolectamos información de la jugada actual
        int numCombosSelectedCells = 0;
        int numCombosColorSelectedCells = 0;
        std::map<int,int> _figureTypes;
        std::map<TTCell*, int>::const_iterator itrHitCells;
        for(itrHitCells = _cellsHitsMap.begin(); itrHitCells != _cellsHitsMap.end(); ++itrHitCells)
        {
            if((*itrHitCells).first->isEmptyCell())
                continue;
            
            if((*itrHitCells).first->getCellFigure() != -1) _figureTypes[(*itrHitCells).first->getCellFigure()]++;
            
            if((*itrHitCells).first->isComboCell())
            {
                numCombosSelectedCells++;
            }
            
            if((*itrHitCells).first->isComboColorCell())
            {
                numCombosColorSelectedCells++;
            }
        }
        
        // Reculectamos info de la celda eliminada
        if(!_cellToErase->isEmptyCell())
        {
            if(_cellToErase->getCellFigure() != -1) _figureTypes[_cellToErase->getCellFigure()]++;
            
            if(_cellToErase->isComboCell())
            {
                numCombosSelectedCells++;
            }
            
            if(_cellToErase->isComboColorCell())
            {
                numCombosColorSelectedCells++;
            }
            
        }
        
        TTBoard::getInstance()->_finishInfo.lastSelectedCell = _cellToErase;
        TTBoard::getInstance()->_finishInfo.combinationLength = _cellsHitsMap.size();
        TTBoard::getInstance()->_finishInfo.selectionLength = 1;
        TTBoard::getInstance()->_finishInfo.combosInCombination = numCombosSelectedCells;
        TTBoard::getInstance()->_finishInfo.combosColorInCombination = numCombosColorSelectedCells;
        TTBoard::getInstance()->_finishInfo.combinationTypes = _figureTypes;
        TTBoard::getInstance()->_finishInfo.boosterPlay = getBoosterId();
        TTBoard::getInstance()->_finishInfo.isOvni = false;
        
        // Lanzamos los pasos de tablero
        _cellToErase->killCell();
        TTBoard::getInstance()->_stepsManager->stepsToHitCells(_cellsHitsMap);
        sendEvent(EVENT_GAME_CONTINUE);
    },_selectedCell));

    
    _pickSprite->runAction(Sequence::create(_rotateB,_rotateF,_boosterEffect,DelayTime::create(0.1f),_finishBoosterApply,NULL));
}