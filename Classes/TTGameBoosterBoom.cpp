//
//  TTGameBoosterBoom.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTGameBoosterBoom.h"
#include "TTBoardStepsManager.h"
#include "TTParticleManager.h"

#define NUM_COMBOS_COLOR 1

TTGameBoosterBoom::TTGameBoosterBoom():
TTGameBoosters(GAME_BOOSTER_BOOM,
               "booster_ui_bomba.png",
               std::vector<CONTENT_TYPES>{ FIGURE_DYNAMIC_CONTENT,
                                        COMBO_VERTICAL_DYNAMIC_CONTENT,
                                        COMBO_HORIZONTAL_DYNAMIC_CONTENT,
                                        COMBO_AREA_DYNAMIC_CONTENT,
                                        COMBO_COLOR_DYNAMIC_CONTENT,
                                        MUD_STATIC_CONTENT,
                                        WOOD_STATIC_CONTENT,
                                        STONE_STATIC_CONTENT,
                                        NET_STATIC_CONTENT,
                        GLUE_STATIC_CONTENT})
{

}

TTGameBoosterBoom::~TTGameBoosterBoom()
{
    
}

void TTGameBoosterBoom::aplyGameBooster()
{
    TTGameBoosters::aplyGameBooster();
    
    //Sprite de la bomba
    _boomSprite = Sprite::createWithSpriteFrameName("booster_ui_bomba.png");
    float _boomSizeH = TTBoard::getInstance()->_cellSize*1.75f;
    float _boomScale = _boomSizeH/_boomSprite->getBoundingBox().size.height;
    _boomSprite->setScale(_boomScale);
    TTBoard::getInstance()->_parentBoard->addChild(_boomSprite,LAYER_ANIMATIONS);

    Point _initBoomPos = Point(_selectedCell->_posXCell,_selectedCell->_posYCell);
    _boomSprite->setPosition(_initBoomPos);
    _boomSprite->setVisible(true);
    
    auto _scaleUp = ScaleTo::create(0.2f,_boomSprite->getScale()+0.05f);
    auto _scaleDown = ScaleTo::create(0.2f,_boomSprite->getScale());
    auto _finishBoosterApply = CallFunc::create(std::bind([&](TTCell* _selectedCell){
        _boomSprite->removeFromParentAndCleanup(true);

        TTAudioManager::getInstance()->playAudio("BOOM_BOOSTER_EXPLOSION");
        
        // Partículas de explosión
        ValueMap _boomParticlesDic = TTParticleManager::getInstance()->getParticle("booster_bomba");
        ParticleSystemQuad* _boomPartices = ParticleSystemQuad::create(_boomParticlesDic);
        _boomPartices->setAutoRemoveOnFinish(true);
        _boomPartices->setPosition(Point(_selectedCell->_posXCell,_selectedCell->_posYCell));
        TTBoard::getInstance()->_parentBoard->addChild(_boomPartices,LAYER_ANIMATIONS-1);
        _boomPartices->setScale(_boomPartices->getScale()/1.5f);
        _boomPartices->resetSystem();
        
        std::vector<TTCell*> _cellsToExplote = TTBoard::getInstance()->selectCellsFromDiagonal(_selectedCell);
        _cellsToExplote.push_back(_selectedCell);
        std::vector<TTCell*> _completeCellsToExplote = TTBoard::getInstance()->completeCellSelection(_cellsToExplote);
        for(int i = 0; i < _completeCellsToExplote.size(); i++) _cellsToExplote.push_back(_completeCellsToExplote[i]);
        std::map<TTCell *, int> _cellsHitsMap;
        for(int i = 0; i < _cellsToExplote.size(); i++)
        {
            if(_cellsHitsMap.find(_cellsToExplote[i]) != _cellsHitsMap.end())
            {
                _cellsHitsMap.find(_cellsToExplote[i])->second++;
            }
            else
            {
                _cellsHitsMap.insert(std::make_pair(_cellsToExplote[i],1));
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
        TTBoard::getInstance()->_finishInfo.lastSelectedCell = _selectedCell;
        TTBoard::getInstance()->_finishInfo.combinationLength = _cellsHitsMap.size();
        TTBoard::getInstance()->_finishInfo.selectionLength = 1;
        TTBoard::getInstance()->_finishInfo.combosInCombination = numCombosSelectedCells;
        TTBoard::getInstance()->_finishInfo.combosColorInCombination = numCombosColorSelectedCells;
        TTBoard::getInstance()->_finishInfo.combinationTypes = _figureTypes;
        TTBoard::getInstance()->_finishInfo.boosterPlay = getBoosterId();
        TTBoard::getInstance()->_finishInfo.isOvni = false;
        
        // Lanzamos los pasos de tablero
        TTBoard::getInstance()->_stepsManager->stepsToHitCells(_cellsHitsMap);
        TTBoard::getInstance()->showScoreOnBoard();
        sendEvent(EVENT_GAME_CONTINUE);
    },_selectedCell));
    _boomSprite->runAction(Sequence::create(_scaleUp,_scaleDown,_scaleUp,_scaleDown,_scaleUp,_scaleDown,_finishBoosterApply,NULL));
    
    TTAudioManager::getInstance()->playAudio("BOOM_BOOSTER_COUNT");

}
