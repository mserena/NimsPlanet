//
//  TTGameBoosterRocket.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTGameBoosterRocket.h"
#include "TTBoardStepsManager.h"
#include "TTParticleManager.h"

#define NUM_COMBOS_COLOR 1

TTGameBoosterRocket::TTGameBoosterRocket():
TTGameBoosters(GAME_BOOSTER_ROCKET,
               "booster_ui_cohete.png",
               std::vector<CONTENT_TYPES>{ FIGURE_DYNAMIC_CONTENT,
                                       COMBO_VERTICAL_DYNAMIC_CONTENT,
                                       COMBO_HORIZONTAL_DYNAMIC_CONTENT,
                                       COMBO_AREA_DYNAMIC_CONTENT,
                                       COMBO_COLOR_DYNAMIC_CONTENT,
                                       WOOD_STATIC_CONTENT,
                                       MUD_STATIC_CONTENT,
                                       STONE_STATIC_CONTENT,
                                       NET_STATIC_CONTENT,
                                       GLUE_STATIC_CONTENT})
{
    // Cargamos el pixel para la veladura
    _bgSpritePixel = Sprite::create("pixelBgBooster.png");
    _bgSpritePixel->retain();
}

TTGameBoosterRocket::~TTGameBoosterRocket()
{
    
}

void TTGameBoosterRocket::aplyGameBooster()
{
    TTGameBoosters::aplyGameBooster();
    int _actualRow = _selectedCell->getCellId().y;
    std::vector<std::vector<TTCell*>> _CellsBoard = TTBoard::getInstance()->_Cells;
    
    // Colocamos la veladura
    _bgSprite = Sprite::createWithTexture(_bgSpritePixel->getTexture());
    _bgSprite->setAnchorPoint(Point(0,0.5));
    float _bgSizeH = TTBoard::getInstance()->_cellSize;
    float _bgScaleH = _bgSizeH/_bgSprite->getBoundingBox().size.height;
    _bgSprite->setScaleY(_bgScaleH);
    
    int _initCellsLimit = 0;
    for(int i = 0; i < _CellsBoard.size(); i++)
    {
        if(!_CellsBoard[i][_actualRow]->isEmptyCell())
        {
            _initCellsLimit = i;
            break;
        }
    }
    
    int _finishCellsLimit = 0;
    for(int i = _CellsBoard.size()-1; i >= 0; i--)
    {
        if(!_CellsBoard[i][_actualRow]->isEmptyCell())
        {
            _finishCellsLimit = i;
            break;
        }
    }
    
    float _bgSizeW = TTBoard::getInstance()->_cellSize*((_finishCellsLimit-_initCellsLimit)+1);
    float _bgScaleW = _bgSizeW/_bgSprite->getBoundingBox().size.width;
    _bgSprite->setScaleX(_bgScaleW);
    Point _initCellPos = Point(_CellsBoard[_initCellsLimit][_actualRow]->_posXCell-(TTBoard::getInstance()->_cellSize/2),_CellsBoard[_initCellsLimit][_actualRow]->_posYCell);
    _bgSprite->setPosition(_initCellPos);
    TTBoard::getInstance()->_parentBoard->addChild(_bgSprite,LAYER_ANIMATIONS);
    
    // Cargamos el cohete
    _rocketSprite = Sprite::createWithSpriteFrameName("booster_pajaro_cohete.png");
    float _rocketSizeH = TTBoard::getInstance()->_cellSize*2.0f;
    float _rocketScale = _rocketSizeH/_rocketSprite->getBoundingBox().size.height;
    _rocketSprite->setScale(_rocketScale);
    
    Size winSize = Director::getInstance()->getWinSize();
    float _posYRocket = (TTBoard::getInstance()->_parentBoard->convertToWorldSpace(Point(0,_CellsBoard[_initCellsLimit][_actualRow]->_posYCell))).y;
    Point _posInitRocket = Point(-_rocketSprite->getBoundingBox().size.width/2,_posYRocket);
    Point _posFinishRocket = Point(winSize.width+(_rocketSprite->getBoundingBox().size.width/2),_posYRocket);
    _rocketSprite->setPosition(_posInitRocket);
    _rocketSprite->setAnchorPoint(Point(0.5,0.5));
    TTBoard::getInstance()->_parentBoard->getParent()->addChild(_rocketSprite,LAYER_ANIMATIONS+1);
    
    // Cargamos las partículas
    ValueMap _rocketParticlesDic = TTParticleManager::getInstance()->getParticle("booster_rocket");
    ParticleSystemQuad* _rocketPartices = ParticleSystemQuad::create(_rocketParticlesDic);
    
    float _sizeParticles = TTBoard::getInstance()->_cellSize/100;
    _sizeParticles = 50*_sizeParticles/100;
    _rocketPartices->setScale(_sizeParticles);
    _rocketPartices->setPosition(Point(_rocketSprite->getContentSize().width/2,_rocketSprite->getContentSize().height/2));

    _rocketPartices->resetSystem();
    _rocketSprite->addChild(_rocketPartices,1);
    
    // Acciones
    auto _moveRocket = MoveTo::create(1.5f,_posFinishRocket);
    auto _finishBoosterApply = CallFunc::create(std::bind([&](TTCell* _selectedCell){
        _rocketSprite->removeFromParentAndCleanup(true);
        _bgSprite->removeFromParentAndCleanup(true);
        
        std::vector<TTCell*> _cellsToExplote;// = TTBoard::getInstance()->selectCellsFromRow(_selectedCell);
        std::vector<std::vector<TTCell*> > _Cells = TTBoard::getInstance()->_Cells;
        int row = _selectedCell->_row;
        for(int i = 0; i < _Cells.size(); i++)
        {
            if(_Cells[i][row]->isEmptyCell()) continue;
            _cellsToExplote.push_back(_Cells[i][row]);
        }
        
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

    _rocketSprite->runAction(Sequence::create(_moveRocket,_finishBoosterApply,NULL));
    
    TTAudioManager::getInstance()->playAudio("ROCKET_BOOSTER");

}
