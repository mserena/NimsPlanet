#include "TTBoard.h"
#include "TTCellAnimations.h"
#include "TTGameUIManager.h"
#include "TTGameBonus.h"
#include "TTAudioManager.h"
#include "TTGame.h"
#include "TTContentCalculator.h"
#include "TTHowToInGameManager.h"

#define BOARD_WIDTH_SPACE                   98
#define BOARD_HEIGHT_SPACE                  70

//Singelton
static TTBoard *_sharedBoard = nullptr;

TTBoard* TTBoard::getInstance()
{
    if (!_sharedBoard)
    {
        _sharedBoard = new TTBoard();
    }
    
    return _sharedBoard;
}

void TTBoard::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedBoard);
    TTContentCalculator::destroyInstance();
}

void TTBoard::initBoardInstance(cocos2d::Layer *parentLayer)
{
    _isUpdatingBoard = false;
    _isInBoardMix = false;
    _isBoardActive = false;
    _cellsToReposition = 0;
    _needToUpdateBoard = false;
    _parentLayer = parentLayer;
    
    // Creamos el nodo padre del tablero
    _parentBoard = Node::create();
    _parentLayer->addChild(_parentBoard,2);
    
    // Creamos el manager de pasos
    _stepsManager = new TTBoardStepsManager();
    
    // Registro de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_CELL_POSITIONED,CC_CALLBACK_1(TTBoard::onEvent_boardCellPositioned,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_ALL_STEPS_FINISHED,CC_CALLBACK_1(TTBoard::onEvent_boardStepsFinished,this)));

}

TTBoard::~TTBoard()
{
    CCLOG("Se elimina Tablero");
    
    delete(_stepsManager);
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTBoard::setBackgroundBoard(Texture2D* texture)
{
    _boardBg = Sprite::createWithTexture(texture);
    Size _screenSize = Director::getInstance()->getWinSize();
    _boardBg->setScale(_screenSize.height/_boardBg->getContentSize().height);
    _boardBg->setPosition(Point(_screenSize.width*0.5, _screenSize.height*0.5));
    _boardBg->setAnchorPoint(Point(0.5f,0.5f));
    _parentLayer->addChild(_boardBg,0);
    _boardBg->setVisible(false);
}

void TTBoard::clearBoard()
{
    _needToUpdateBoard = false;
    _cellsToReposition = 0;
    _isUpdatingBoard = false;
    _isInBoardMix = false;
    _selectedCells.clear();
    _parentBoard->removeAllChildrenWithCleanup(true);
    _parentBoard->setVisible(false);
    _Cells.clear();
    _boardGoals.clear();
    _helpCombination.clear();
}

void TTBoard::finishBoardControl(int _restrictionBonus)
{
    _isBoardActive = false;
    //desactivar juego
    TTGameUIManager::getInstance()->pauseUI();
    
    // Paso de tablero para la fiesta de juego ganado
    _stepsManager->stepsToFinishParty();
    
    // TODO: Temporal, mover a despues de que termine el paso de fiesta
    auto _delayFinishBoard = DelayTime::create(3.5f);
    auto _verifyBonusBoard = CallFunc::create(std::bind([&](int _restrictionBonus){
        if(_restrictionBonus != 0)
        {
            // Se inicia Bonus
            CCLOG("Iniciamos Bonus con %d movimientos.",_restrictionBonus);
            TTGameBonus::getInstance()->initGameBonus(_parentLayer,_restrictionBonus);
        }
        else
        {
            finishBoard();
        }
    },_restrictionBonus));

    _parentBoard->runAction(Sequence::create(_delayFinishBoard,_verifyBonusBoard,NULL));
}

void TTBoard::finishBoard()
{
    // Deshabilita el tablero
    _isBoardActive = false;
    clearBoard();
    sendEvent(EVENT_GAME_FINISHED_BOARD);
}

bool TTBoard::initBoard(initBoardInfo boardInfo, std::vector<TTGoal*> goals)
{
    //Borramos toda la informacion anterior
    clearBoard();
    
    std::vector< std::vector<std::string> > boardMap = boardInfo.boardMap;
    _boardGoals = goals;
    
    //control de calculo de figuras
    TTContentCalculator::getInstance()->init(boardInfo.figuresWeight);
    
    // Definimos los tamaño celda
    Size _screenSize = Director::getInstance()->getWinSize();
    Size _spaceForBoard = Size((BOARD_WIDTH_SPACE*_screenSize.width)/100,(BOARD_HEIGHT_SPACE*_screenSize.height)/100);
    Size _cellSizeWH = Size(_spaceForBoard.width/boardInfo.maxCellsBoard,_spaceForBoard.height/boardInfo.maxCellsBoard);
    if(_cellSizeWH.width < _cellSizeWH.height)
        _cellSize = _cellSizeWH.width;
    else
        _cellSize = _cellSizeWH.height;
    
    // Definimos posición del tablero
    float _uiUpSpace = (HEIGHT_UI_UP_SPACE*_screenSize.height)/100;
    float _posXBoard = (_spaceForBoard.width-(_cellSize*boardMap.size()))/2;
    float _posYBoard = (_spaceForBoard.height-(_cellSize*boardMap[0].size()))/2;
    float _marginX = (_screenSize.width-_spaceForBoard.width)/2;
    _parentBoard->setPosition(_posXBoard+_marginX,_screenSize.height-(_posYBoard+_uiUpSpace));
    
    // Creacion de las celdas del mapa
    float _posYCell = 0;
    float _posXCell = 0;
    for(int i = 0; i < boardMap.size(); i++)
    {
        // Posición de las celdas
        _posXCell = (i*_cellSize)+_cellSize/2;
        _posYCell = -_cellSize/2;
        std::vector<TTCell*> _columnCells;
        for( int j = 0; j < boardMap[i].size(); j++)
        {
            TTCell *cell = new TTCell(_parentLayer,_parentBoard,boardMap[i][j],_posXCell,_posYCell,_cellSize,i,j,boardMap.size(),boardMap[i].size());
            _columnCells.push_back(cell);
            _posYCell -= _cellSize;
        }
        _Cells.push_back(_columnCells);
    }
    
    // Creamos la selección de booster
    _parentHelpBooster = Node::create();
    _helpFrameBooster = Sprite::createWithSpriteFrameName("booster_help.png");
    _parentHelpBooster->addChild(_helpFrameBooster);
    _arrowBooster = Sprite::createWithSpriteFrameName("booster_flecha.png");

    float _sizeHelp = _helpFrameBooster->getContentSize().width;
    _arrowBooster->setAnchorPoint(Point(0.2,0));
    _arrowBooster->setPosition(Point(0,_sizeHelp/2));

    //hacemos la flecha un poco más grande
    _arrowBooster->setScale(1.4);
    
    _parentHelpBooster->addChild(_arrowBooster);
    _parentHelpBooster->setVisible(false);

    float _newSize = _cellSize;
    float _newScale = _newSize/_helpFrameBooster->getBoundingBox().size.width;
    _parentHelpBooster->setScale(_newScale);

    auto _jumpArrow = JumpBy::create(0.8f,Point(0,0),_arrowBooster->getBoundingBox().size.height/4,1);
    _arrowBooster->runAction(RepeatForever::create(_jumpArrow));
    
    _parentBoard->addChild(_parentHelpBooster,LAYER_ANIMATIONS);
    
    // Activamos el tablero
    _isBoardActive = true;
    updateBoardState();
    
    return true;
}

void TTBoard::update(float dt)
{
    _stepsManager->update(dt);
}

void TTBoard::applyBooster()
{
    _parentHelpBooster->setVisible(false);

    // volvemos a lanzar la ayuda de combinación
    stopShowHelpCombination();
    _helpCombination.clear();
    launchShowHelpCombination();
}

bool TTBoard::selectBoardCells(float x, float y)
{
    if(_isUpdatingBoard || _isInBoardMix)
        return false;
       
    for(int i = 0; i < _Cells.size(); i++)
    {
        for( int j = 0; j < _Cells[i].size(); j++)
        {
            if(_Cells[i][j]->isCellTouched(x,y))
            {
                //borramos la ayuda de la pantalla
                stopShowHelpCombination();
                
                //verifica si la celda puede ser iniciadora de una selección
                if(_selectedCells.size() == 0 && !_Cells[i][j]->isFirstSelectableCell()) return false;
                
                //verifica que la celda no este seleccionada
                bool _cellIsSelected = false;
                for(int w = 0; w < _selectedCells.size(); w++)
                {
                    if( _selectedCells[w]->getCellId() == _Cells[i][j]->getCellId())
                    {
                        _cellIsSelected = true;
                        
                        // verifica si hay que deshacer el camino
                        if(w != _selectedCells.size()-1)
                        {
                            //deshacer todo el camino hasta w
                            std::vector<TTCell*> _auxSelectedCells;
                            for(int y = 0; y < _selectedCells.size(); y++)
                            {
                                if(y > w)
                                    _selectedCells[y]->unselectCell();
                                else
                                    _auxSelectedCells.push_back(_selectedCells[y]);
                            }
                            
                            _selectedCells.clear();
                            _selectedCells = _auxSelectedCells;
                            
                            // borramos el path de la ultima figura
                            _selectedCells[w]->hidePathCell();
                            
                            // Muestra la ayuda de combinaciones en caso necesareo
                            showResultHelp();

                            // Audio de deshacer el camino
                            TTAudioManager::getInstance()->playAudio("GENERIC_UNSELECT");
                            
                            return false;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                
                //verifica que la celda sea del mismo tipo o combos
                bool _cellIsSameType = false;
                if(_selectedCells.size() == 0)
                    _cellIsSameType = true;
                
                if(!_cellIsSameType)
                {
                    // Combinación normal
                    _cellIsSameType = true;
                    int _newCellFigure = _Cells[i][j]->getCellFigure();
                    for(int m = 0; m < _selectedCells.size(); m++)
                    {
                        if(_newCellFigure != _selectedCells[m]->getCellFigure())
                        {
                            _cellIsSameType = false;
                            break;
                        }
                    }
                }

                if(!_cellIsSameType)
                {
                    //Combinación de combos
                    _cellIsSameType = _Cells[i][j]->isComboCell();
                    if(_cellIsSameType)
                    {
                        for(int m = 0; m < _selectedCells.size(); m++)
                        {
                            if(!_selectedCells[m]->isComboCell())
                            {
                                _cellIsSameType = false;
                                break;
                            }
                        }
                    }
                }
                
                //verifica que la celda sea continua a la anteriormente seleccionada
                CELL_PATHS moveDirection;
                bool _cellIsContinuous = false;
                if(_selectedCells.size() == 0)
                    _cellIsContinuous = true;
                else
                {
                    cocos2d::Vec2 _lastCellId = _selectedCells[_selectedCells.size()-1]->getCellId();
                    cocos2d::Vec2 _currCellId = _Cells[i][j]->getCellId();
                    
                    _cellIsContinuous =
                    (_currCellId.x >=_lastCellId.x-1 && _currCellId.x <=_lastCellId.x+1) &&
                    (_currCellId.y >=_lastCellId.y-1 && _currCellId.y <=_lastCellId.y+1);

                    if(_cellIsContinuous)
                    {
                        moveDirection = calcPathDirection(_currCellId, _lastCellId);
                    }
                }

                // Verifica combinaciones con combo de color
                bool _isComoColorCombination = false;
                if(_selectedCells.size() == 0)
                {
                    _isComoColorCombination = _Cells[i][j]->isComboColorCell();
                }
                else
                {
                    _isComoColorCombination = _selectedCells[0]->isComboColorCell() || _Cells[i][j]->isComboColorCell();
                }
                
                // Agrega la celda según las comprobaciones
                if(_isComoColorCombination)
                {
                    if(_selectedCells.size() == 0)
                    {
                        _Cells[i][j]->selectCell();
                        _selectedCells.push_back(_Cells[i][j]);
                        
                        //Reproduce sonido de selección
                        _Cells[i][j]->playSelectionSound(_selectedCells.size()-1);
                        return true;
                    }
                    else
                    {
                        cocos2d::Vec2 _lastCellId = _selectedCells[0]->getCellId();
                        cocos2d::Vec2 _currCellId = _Cells[i][j]->getCellId();
                        _cellIsContinuous =
                        (_currCellId.x >=_lastCellId.x-1 && _currCellId.x <=_lastCellId.x+1) &&
                        (_currCellId.y >=_lastCellId.y-1 && _currCellId.y <=_lastCellId.y+1);
                        
                        if(_selectedCells.size() < 2 && _cellIsContinuous)
                        {
                            _Cells[i][j]->selectCell();
                            _selectedCells[0]->showPathCell(moveDirection);
                            _selectedCells.push_back(_Cells[i][j]);
                            
                            //Reproduce sonido de selección
                            _Cells[i][j]->playSelectionSound(_selectedCells.size()-1);
                            
                            //muestra la ayuda de combinaciones para el combo de color
                            showResultHelp();
                            
                            return true;
                        }
                        else if(!_cellIsSelected && _cellIsContinuous)
                        {
                            // Reemplazamos la selección del combo de color
                            _selectedCells[1]->unPreSelectCell();
                            _selectedCells[1]->unselectCell();
                            _selectedCells.erase(_selectedCells.begin()+1);
                            _selectedCells.push_back(_Cells[i][j]);
                            
                            _Cells[i][j]->selectCell();
                            moveDirection = calcPathDirection(_selectedCells[1]->getCellId(), _selectedCells[0]->getCellId());
                            _selectedCells[0]->showPathCell(moveDirection);
                            
                            //Reproduce sonido de selección
                            _Cells[i][j]->playSelectionSound(_selectedCells.size()-1);
                            
                            //muestra la ayuda de combinaciones para el combo de color
                            showResultHelp();
                        }
                    }
                }
                else
                {
                    if(!_cellIsSelected && _cellIsSameType && _cellIsContinuous)
                    {
                        _Cells[i][j]->selectCell();
                        if(_selectedCells.size() != 0) 
                            _selectedCells[_selectedCells.size()-1]->showPathCell(moveDirection);
                        _selectedCells.push_back(_Cells[i][j]);
                        
                        //Reproduce sonido de selección
                        _Cells[i][j]->playSelectionSound(_selectedCells.size()-1);
                        
                        // Muestra la ayuda de combinaciones, si hay algun combo
                        showResultHelp();
                        
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

CELL_PATHS TTBoard::calcPathDirection(Vec2 _currCellId, Vec2 _lastCellId)
{
    // Calculamos la dirección del movimiento
    CELL_PATHS moveDirection;
    if(_currCellId.x > _lastCellId.x)
    {
        if(_currCellId.y > _lastCellId.y)
            moveDirection = CELL_PATH_SE;
        else if(_currCellId.y == _lastCellId.y)
            moveDirection = CELL_PATH_E;
        else
            moveDirection = CELL_PATH_NE;
    }
    else if(_currCellId.x == _lastCellId.x)
    {
        if(_currCellId.y > _lastCellId.y)
            moveDirection = CELL_PATH_S;
        else
            moveDirection = CELL_PATH_N;
    }
    else
    {
        if(_currCellId.y > _lastCellId.y)
            moveDirection = CELL_PATH_WS;
        else if(_currCellId.y == _lastCellId.y)
            moveDirection = CELL_PATH_W;
        else
            moveDirection = CELL_PATH_NW;
    }
    return moveDirection;
}

bool TTBoard::isComboSelection()
{
    if(_selectedCells.size() >= MIN_SELECT_CELLS_COMBO)
    {
        if(_selectedCells[0]->isComboColorCell() || _selectedCells[1]->isComboColorCell())
            return true;
        
        bool _allCellsCombo = true;
        for(int i = 0; i < _selectedCells.size(); i++)
        {
            if(!_selectedCells[i]->isComboCell())
                _allCellsCombo = false;
        }
        return _allCellsCombo;
    }
    
    return false;
}

void TTBoard::clearResultHelp()
{
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            // Resetea los helpers de combos
            if(!_Cells[i][j]->isEmptyCell())
            {
                _Cells[i][j]->unPreSelectCell();
            }
            
            // Resetea los helpers de la red
            if(_Cells[i][j]->_cellContentLayers.size() > 0 && typeid(*_Cells[i][j]->_cellContentLayers.back()) == typeid(TTCellStaticContentNetwork))
            {
                TTCellStaticContentNetwork* _networkContent = static_cast<TTCellStaticContentNetwork*>(_Cells[i][j]->_cellContentLayers.back());
                _networkContent->setHitsLabel(std::to_string(_networkContent->_cellHits));
            }
        }
    }
}

void TTBoard::showResultHelp()
{
    clearResultHelp();
    
    //Condición de marcado de la ayuda para redes
    if(_selectedCells.size() > 0)
    {
        std::vector<TTCellContent*> _cellsNetworkContent;
        for(int i = 0; i < _selectedCells.size(); i++)
        {
            if(_selectedCells[i]->_cellContentLayers.size() > 0 && typeid(*_selectedCells[i]->_cellContentLayers.back()) == typeid(TTCellStaticContentNetwork))
            {
                _cellsNetworkContent.push_back(_selectedCells[i]->_cellContentLayers.back());
            }
        }
        
        for(int i = 0; i < _cellsNetworkContent.size(); i++)
        {
            TTCellStaticContentNetwork* _networkContent = static_cast<TTCellStaticContentNetwork*>(_cellsNetworkContent[i]);
            int _newHitsToNetwork = _networkContent->_cellHits-_selectedCells.size();
            if(_newHitsToNetwork < 0) _newHitsToNetwork = 0;
            _networkContent->setHitsLabel(std::to_string(_newHitsToNetwork));
        }
    }
    
    // Condición para el marcado de la ayuda para combos
    if(_selectedCells.size() < MIN_SELECT_CELLS)
    {
        if(!isComboSelection())
        {
            return;
        }
    }
    
    // Verifica si hay que marcar helpers
    bool _combos = _selectedCells[0]->isComboColorCell();
    for(int o = 0; o < _selectedCells.size(); o++)
    {
        if(_selectedCells[o]->isComboCell() || _selectedCells[o]->isComboColorCell())
            _combos = true;
    }
    if(!_combos)
    {
        return;
    }
    
    // Calcula nuevos helpers
    std::vector<TTCell*> _cellsToMarkHelper = _selectedCells;
    std::vector<TTCell*> _combosAnalized;
    for(int i = 0; i < _cellsToMarkHelper.size(); i++)
    {
        if(!_cellsToMarkHelper[i]->isComboCell() && !_cellsToMarkHelper[i]->isComboColorCell())
            continue;
        
        std::vector<TTCell*> _newCellsSelected;
        
        // Verificamos que el como no se haya analizado ya
        bool _analizedCombo = false;
        for(int j = 0; j < _combosAnalized.size(); j++)
        {
            if(_cellsToMarkHelper[i] == _combosAnalized[j])
            {
                _analizedCombo = true;
                break;
            }
        }
        if(_analizedCombo) continue;
        
        _combosAnalized.push_back(_cellsToMarkHelper[i]);
        
        switch(_cellsToMarkHelper[i]->getCellContentTypes().back())
        {
            case COMBO_VERTICAL_DYNAMIC_CONTENT:
            {
                _newCellsSelected = selectCellsFromColumn(_cellsToMarkHelper[i]);
                for(int j = 0; j < _newCellsSelected.size(); j++) _cellsToMarkHelper.push_back(_newCellsSelected[j]);
            }
            break;
            case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
            {
                _newCellsSelected = selectCellsFromRow(_cellsToMarkHelper[i]);
                for(int j = 0; j < _newCellsSelected.size(); j++) _cellsToMarkHelper.push_back(_newCellsSelected[j]);
            }
            break;
            case COMBO_AREA_DYNAMIC_CONTENT:
            {
                _newCellsSelected = selectCellsFromDiagonal(_cellsToMarkHelper[i]);
                for(int j = 0; j < _newCellsSelected.size(); j++) _cellsToMarkHelper.push_back(_newCellsSelected[j]);
            }
            break;
            case COMBO_COLOR_DYNAMIC_CONTENT:
            {
                //si un combo de color viene seleccionado, cogemos la figura que originó la selección
                TTCell *_cellSelected = nullptr;
                for(int w = 0; w < _selectedCells.size(); w++)
                {
                    if(_selectedCells[w]->getCellFigure() <= 5)
                    {
                        _cellSelected = _selectedCells[w];
                        break;
                    }
                }
                
                if(_cellSelected != nullptr)
                {
                    _newCellsSelected = selectCellsSameType(_cellSelected);
                    for(int j = 0; j < _newCellsSelected.size(); j++) _cellsToMarkHelper.push_back(_newCellsSelected[j]);
                }
            }
            break;
            default:
            break;
        }
    }

    // printa los nuevos helpers
    std::map<TTCellStaticContentNetwork*,int> _mapNetworksAffectByCombo;
    for(int i = 0;  i < _cellsToMarkHelper.size(); i++)
    {
        _cellsToMarkHelper[i]->preSelectCell();
        
        if(_cellsToMarkHelper[i]->_cellContentLayers.size() > 0 && typeid(*_cellsToMarkHelper[i]->_cellContentLayers.back()) == typeid(TTCellStaticContentNetwork))
        {
            TTCellStaticContentNetwork* _networkContent = static_cast<TTCellStaticContentNetwork*>(_cellsToMarkHelper[i]->_cellContentLayers.back());
            
            if(_mapNetworksAffectByCombo.find(_networkContent) != _mapNetworksAffectByCombo.end())
            {
                _mapNetworksAffectByCombo.find(_networkContent)->second++;
            }
            else
            {
                _mapNetworksAffectByCombo.insert(std::make_pair(_networkContent,1));
            }
        }
    }
    
    std::map<TTCellStaticContentNetwork*,int>::iterator _mapNetworksAffectByComboItr;
    for(_mapNetworksAffectByComboItr = _mapNetworksAffectByCombo.begin(); _mapNetworksAffectByComboItr != _mapNetworksAffectByCombo.end(); ++_mapNetworksAffectByComboItr)
    {
        int _newHitsToNetwork = (*_mapNetworksAffectByComboItr).first->_cellHits-(*_mapNetworksAffectByComboItr).second;
        if(_newHitsToNetwork < 0) _newHitsToNetwork = 0;
        (*_mapNetworksAffectByComboItr).first->setHitsLabel(std::to_string(_newHitsToNetwork));
    }
    
    TTAudioManager::getInstance()->playAudio("HELP_BOARD");
}

void TTBoard::finishSelectBoardCells()
{
    
    if(_selectedCells.size() == 0)
        return;
    
    // Resetea los helpers de los combos
    clearResultHelp();
    
    // Resetea los agujeros del tablero
    _cellHoles.clear();
    _cellExcludeHoles.clear();
    
    // Copiamos datos de la selección del usuario
    TTCell* lastSelectedCell = _selectedCells[_selectedCells.size()-1];
    std::map<TTCell*,int> _cellsHitsMap;
    
    // Verifica, en caso de tutorial, que se hayan seleccionado todas las casillas en su correcto orden
    if(TTHowToInGameManager::getInstance()->_isTutorialActive)
    {
        if(TTHowToInGameManager::getInstance()->getTotalCellsToSelectActualStep() != _selectedCells.size())
        {
            for(int i = 0; i < _selectedCells.size(); i++)
                _selectedCells[i]->unselectCell();
            
            if(_selectedCells.size() > 0)
                _selectedCells[0]->playUnSelectionSound(0);
            
            resetPlayCounters();
            
            return;
        }
        else
        {
            // Verificamos el orden de las celdas seleccionadas
            std::vector<TTCell*> _combinationCells = TTHowToInGameManager::getInstance()->getActualAllowedCells();
            for(int i = 0; i < _combinationCells.size(); i++)
            {
                if(_combinationCells[i] != _selectedCells[i])
                {
                    for(int i = 0; i < _selectedCells.size(); i++)
                        _selectedCells[i]->unselectCell();
                    
                    if(_selectedCells.size() > 0)
                        _selectedCells[0]->playUnSelectionSound(0);
                    
                    resetPlayCounters();
                    
                    return;
                }
            }
        }
    }
    
    // verifica seleccion
    if(_selectedCells.size() >= MIN_SELECT_CELLS || isComboSelection())
    {
        _isUpdatingBoard = true;
        
        // rellenamos el array de hits con las selecciones de usuario
        for(int i = 0; i < _selectedCells.size(); i++)
        {
            //Verificamos si hay alguna red
            if(_selectedCells[i]->_cellContentLayers.size() > 0 && typeid(*_selectedCells[i]->_cellContentLayers.back()) == typeid(TTCellStaticContentNetwork))
            {
                _cellsHitsMap.insert(std::make_pair(_selectedCells[i],_selectedCells.size()));
            }
            else
            {
                _cellsHitsMap.insert(std::make_pair(_selectedCells[i],1));
            }
        }
        
        // rellenamos el array de hits con las celdas afectadas por combinaciones colindantes
        std::vector<TTCell*> _areaAffectedCells = getAreaCombinationAffectCells();
        for(int i = 0; i < _areaAffectedCells.size(); i++)
        {
            if(_cellsHitsMap.find(_areaAffectedCells[i]) != _cellsHitsMap.end())
            {
                _cellsHitsMap.find(_areaAffectedCells[i])->second++;
            }
            else
            {
                _cellsHitsMap.insert(std::make_pair(_areaAffectedCells[i],1));
            }
        }
        
        // rellenamos el array de hits con las celdas afectadas por los combos
        std::vector<TTCell*> _cellAffectedFromCombo = completeCellSelection(_selectedCells);
        for(int i = 0; i < _cellAffectedFromCombo.size(); i++)
        {
            if(_cellsHitsMap.find(_cellAffectedFromCombo[i]) != _cellsHitsMap.end())
            {
                _cellsHitsMap.find(_cellAffectedFromCombo[i])->second++;
            }
            else
            {
                _cellsHitsMap.insert(std::make_pair(_cellAffectedFromCombo[i],1));
            }
        }
        
        // Verificamos si la selección es de combo de color o normal
        bool _isComboColorCombination = _selectedCells[0]->isComboColorCell() || _selectedCells[1]->isComboColorCell();
        if(_isComboColorCombination)
        {
            int _posComboColorOtherCell = _selectedCells[0]->isComboColorCell() ? 1 : 0;
            int _posCombocolor = _selectedCells[0]->isComboColorCell() ? 0 : 1;
            
            if(_selectedCells[_posComboColorOtherCell]->isComboCell())
            {
                // Combo color - combo normal
                std::vector<TTCell*> _cellsAffectComboColor = selectCellsSameType(_selectedCells[_posComboColorOtherCell]);
                
                // Añadimos las celdas afectadas por el combo de color
                for(int i = 0; i < _cellsAffectComboColor.size(); i++)
                {
                    if(_cellsAffectComboColor[i] == _selectedCells[_posComboColorOtherCell]) continue;
                    
                    if(_cellsHitsMap.find(_cellsAffectComboColor[i]) != _cellsHitsMap.end())
                    {
                        _cellsHitsMap.find(_cellsAffectComboColor[i])->second++;
                    }
                    else
                    {
                        _cellsHitsMap.insert(std::make_pair(_cellsAffectComboColor[i],1));
                    }
                }
                
                std::vector<TTCell*> _cellsFromCombosCreatedByComboColor;
                for(int o = 0; o < _cellsAffectComboColor.size(); o++)
                {
                    if(_cellsAffectComboColor[o]->isComboCell()) continue;
                    if(_cellsAffectComboColor[o]->isWallCell()) continue;
                    
                    switch(_selectedCells[_posComboColorOtherCell]->getCellContentTypes().back())
                    {
                        case COMBO_VERTICAL_DYNAMIC_CONTENT:
                        {
                            std::vector<TTCell*> _cells = selectCellsFromColumn(_cellsAffectComboColor[o]);
                            for(int p = 0; p < _cells.size(); p++) _cellsFromCombosCreatedByComboColor.push_back(_cells[p]);
                        }
                        break;
                        case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
                        {
                            std::vector<TTCell*> _cells = selectCellsFromRow(_cellsAffectComboColor[o]);
                            for(int p = 0; p < _cells.size(); p++) _cellsFromCombosCreatedByComboColor.push_back(_cells[p]);
                        }
                        break;
                        case COMBO_AREA_DYNAMIC_CONTENT:
                        {
                            std::vector<TTCell*> _cells = selectCellsFromDiagonal(_cellsAffectComboColor[o]);
                            for(int p = 0; p < _cells.size(); p++) _cellsFromCombosCreatedByComboColor.push_back(_cells[p]);
                        }
                        break;
                        default:
                        break;
                    }
                }
                
                // Añadimos las celdas de los combos creados por el combo de color
                for(int i = 0; i < _cellsFromCombosCreatedByComboColor.size(); i++)
                {
                    if(_cellsHitsMap.find(_cellsFromCombosCreatedByComboColor[i]) != _cellsHitsMap.end())
                    {
                        _cellsHitsMap.find(_cellsFromCombosCreatedByComboColor[i])->second++;
                    }
                    else
                    {
                        _cellsHitsMap.insert(std::make_pair(_cellsFromCombosCreatedByComboColor[i],1));
                    }
                }

                // rellenamos el array de hits con las celdas afectadas por los combos
                std::vector<TTCell*> _cellAffectedFromCombo = completeCellSelection(_cellsFromCombosCreatedByComboColor);
                for(int i = 0; i < _cellAffectedFromCombo.size(); i++)
                {
                    if(_cellsHitsMap.find(_cellAffectedFromCombo[i]) != _cellsHitsMap.end())
                    {
                        _cellsHitsMap.find(_cellAffectedFromCombo[i])->second++;
                    }
                    else
                    {
                        _cellsHitsMap.insert(std::make_pair(_cellAffectedFromCombo[i],1));
                    }
                }

                _stepsManager->stepsToComboColorWithComboFigure(_selectedCells[_posCombocolor],_cellsAffectComboColor,_cellsHitsMap,_selectedCells[_posComboColorOtherCell]->getCellContentTypes().back());
                
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
                _finishInfo.lastSelectedCell = lastSelectedCell;
                _finishInfo.combinationLength = _cellsHitsMap.size();
                _finishInfo.selectionLength = _selectedCells.size();
                _finishInfo.combosInCombination = numCombosSelectedCells;
                _finishInfo.combosColorInCombination = numCombosColorSelectedCells;
                _finishInfo.combinationTypes = _figureTypes;
                _finishInfo.boosterPlay = -1;
                _finishInfo.isOvni = false;

            }
            else if(_selectedCells[_posComboColorOtherCell]->isComboColorCell())
            {
                std::vector<TTCell*> _hitCells;
                std::vector<TTCell*> _abductionCells;

                // Combo color - Combo color
                for(int v = 0; v < _Cells.size(); v++)
                {
                    for(int w = 0; w < _Cells[v].size(); w++)
                    {
                        if(_Cells[v][w]->isEmptyCell()) continue;
                        
                        if(_Cells[v][w]->isSelectableCell() && _Cells[v][w]->isFirstSelectableCell())
                        {
                            _abductionCells.push_back(_Cells[v][w]);
                        }
                        else
                        {
                            _hitCells.push_back(_Cells[v][w]);
                        }
                    }
                }
        
                _stepsManager->stepsToComboColorWithComboColor(_abductionCells,_hitCells);
                
                // Recolectamos información de la jugada actual
                int numCombosSelectedCells = 0;
                int numCombosColorSelectedCells = 0;
                int combinationLength = 0;
                std::map<int,int> _figureTypes;
                for(int v = 0; v < _Cells.size(); v++)
                {
                    for(int w = 0; w < _Cells[v].size(); w++)
                    {
                        if(!_Cells[v][w]->isEmptyCell())
                        {
                            combinationLength++;
                            if(_Cells[v][w]->isComboCell()) numCombosSelectedCells++;
                            if(_Cells[v][w]->isComboColorCell()) numCombosColorSelectedCells++;
                            if(_Cells[v][w]->getCellFigure() != -1) _figureTypes[_Cells[v][w]->getCellFigure()]++;
                        }
                    }
                }
                _finishInfo.lastSelectedCell = lastSelectedCell;
                _finishInfo.combinationLength = combinationLength;
                _finishInfo.selectionLength = _selectedCells.size();
                _finishInfo.combosInCombination = numCombosSelectedCells;
                _finishInfo.combosColorInCombination = numCombosColorSelectedCells;
                _finishInfo.combinationTypes = _figureTypes;
                _finishInfo.boosterPlay = -1;
                _finishInfo.isOvni = true;
            }
            else
            {
                // Combo color - figura normal
                std::map<TTCell*,int> _cellsHitsComboColor;
                std::vector<TTCell*> _cellsAffectComboColor = selectCellsSameType(_selectedCells[_posComboColorOtherCell]);
                
                // rellenamos el array de hits con las celdas afectadas por los combos
                std::vector<TTCell*> _cellAffectedFromCombo = completeCellSelection(_cellsAffectComboColor);
                for(int i = 0; i < _cellAffectedFromCombo.size(); i++)
                {
                    if(_cellsHitsMap.find(_cellAffectedFromCombo[i]) != _cellsHitsMap.end())
                    {
                        _cellsHitsMap.find(_cellAffectedFromCombo[i])->second++;
                    }
                    else
                    {
                        _cellsHitsMap.insert(std::make_pair(_cellAffectedFromCombo[i],1));
                    }
                }
                
                // Rellenamos el mapa de celdas afectadas en la jugada con el combo de color
                for(int i = 0; i < _cellsAffectComboColor.size(); i++)
                {
                    if(_cellsHitsMap.find(_cellsAffectComboColor[i]) != _cellsHitsMap.end())
                    {
                        _cellsHitsMap.find(_cellsAffectComboColor[i])->second++;
                    }
                    else
                    {
                        _cellsHitsMap.insert(std::make_pair(_cellsAffectComboColor[i],1));
                    }
                }
                
                _stepsManager->stepsToComboColorWithNormalFigure(_selectedCells[_posCombocolor],_cellsAffectComboColor,_cellsHitsMap);
                
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
                _finishInfo.lastSelectedCell = lastSelectedCell;
                _finishInfo.combinationLength = _cellsHitsMap.size();
                _finishInfo.selectionLength = _selectedCells.size();
                _finishInfo.combosInCombination = numCombosSelectedCells;
                _finishInfo.combosColorInCombination = numCombosColorSelectedCells;
                _finishInfo.combinationTypes = _figureTypes;
                _finishInfo.boosterPlay = -1;
                _finishInfo.isOvni = false;
            }
        }
        else
        {
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
            _finishInfo.lastSelectedCell = lastSelectedCell;
            _finishInfo.combinationLength = _cellsHitsMap.size();
            _finishInfo.selectionLength = _selectedCells.size();
            _finishInfo.combosInCombination = numCombosSelectedCells;
            _finishInfo.combosColorInCombination = numCombosColorSelectedCells;
            _finishInfo.combinationTypes = _figureTypes;
            _finishInfo.boosterPlay = -1;
            _finishInfo.isOvni = false;

            // Verificamos la creación de nuevos combos
            bool allCellsSameType = true;
            int cellSampleType = _selectedCells[0]->getCellFigure();
            for(int i = 0; i < _selectedCells.size(); i++)
            {
                if(_selectedCells[i]->getCellFigure() != cellSampleType)
                    allCellsSameType = false;
            }
            
            if(_selectedCells.size() >= MIN_SELECT_CELLS_TO_CREATE_COMBO && allCellsSameType)
            {
                // Hay que crear un nuevo combo
                auto lastCellSelected = _selectedCells.back();
                
                if(!lastCellSelected->isWallCell())
                {
                    CONTENT_TYPES newCellComboType = getNewComboType(_selectedCells);
                    
                    if(_selectedCells.size() >= MIN_SELECT_CELLS_TO_CREATE_COMBO && _selectedCells.size() < MIN_SELECT_CELLS_TO_CREATE_COMBO_COLOR)
                    {
                        // Creamos un combo normal
                        if(lastCellSelected->isComboCell())
                        {
                            //Lanzamos los FX de la celda en caso de que ya fuera un combo
                            (static_cast<TTCellDynamicContentFigure*>(lastCellSelected->_cellContentLayers.back()))->launchComboFX();
                        }
                        
                        lastCellSelected->convertCellToCombo(newCellComboType);
                    }
                    else
                    {
                        // Creamos un combo de color
                        lastCellSelected->convertCellToComboColor();
                    }
                    
                    if(lastCellSelected->_cellContentLayers.size() > 0)
                    {
                        lastCellSelected->_cellContentLayers.back()->_contentIsBlocked = true;
                    }
                }
            }
            
            // Guardamos los tipos de las celdas hiteadas
            std::map<TTCell*,int>::iterator _itrCellsHitMap;
            for(_itrCellsHitMap = _cellsHitsMap.begin(); _itrCellsHitMap != _cellsHitsMap.end(); _itrCellsHitMap++)
            {
                if(_itrCellsHitMap->first->_cellContentLayers.size() > 0)
                {
                    _lastCellsTypeHited.push_back(_itrCellsHitMap->first->_cellContentLayers.back()->_contentLayerType);
                }
            }
            
            
            // Ejecutamos los hit de las celdas
            _stepsManager->stepsToHitCells(_cellsHitsMap);
        }
        
        // mostramos el score en el tablero
        showScoreOnBoard();
        
        // ejectua los efectos de sonido de finalización de jugada
        playSoundFX();
        
        // Ocultamos el tutorial en caso de que haya
        if(TTHowToInGameManager::getInstance()->_isTutorialActive)
        {
            TTHowToInGameManager::getInstance()->hideStepTutorial();
        }
    }
    else
    {
        for(int i = 0; i < _selectedCells.size(); i++)
            _selectedCells[i]->unselectCell();
        
        if(_selectedCells.size() > 0)
            _selectedCells[0]->playUnSelectionSound(0);
        
        launchShowHelpCombination();
    }
    
    // resetea las variables de selección
    resetPlayCounters();
}

void TTBoard::playSoundFX()
{
    //Verificamos información de jugada para sonidos
    if(_finishInfo.combosInCombination == 1)
    {
        TTAudioManager::getInstance()->playAudio("COMBO_SOUND_SIMPLY");
    }
    else if(_finishInfo.combosInCombination > 1)
    {
        TTAudioManager::getInstance()->playAudio("COMBO_SOUND_COMPLEX");
    }
}

void TTBoard::showScoreOnBoard()
{
    int scoreWined = TTGame::getInstance()->calcScoreWined(_finishInfo);
    
    // Creamos el label para el score de tablero
    auto _scoreBoardLabel  = Label::createWithBMFont("markerfeltScore.fnt","0");
    _parentBoard->addChild(_scoreBoardLabel,LAYER_ANIMATIONS);
    
    Size _screensize = Director::getInstance()->getWinSize();
    float _scale = (5*_screensize.width/100)/_scoreBoardLabel->getContentSize().width;
    _scoreBoardLabel->setScale(_scale);
    
    Point _posScore = Point(_finishInfo.lastSelectedCell->_posXCell,_finishInfo.lastSelectedCell->_posYCell);
    _scoreBoardLabel->setPosition(_posScore);
    _scoreBoardLabel->setString(std::to_string(scoreWined));
    _scoreBoardLabel->setVisible(true);
    
    // Seteamos la acción de movimiento
    float _time = 0.6f;
    auto moveLabel = MoveTo::create(_time,Point(_scoreBoardLabel->getPosition().x,_scoreBoardLabel->getPosition().y+(_cellSize*1.5f)));
    auto fadeOut = FadeTo::create(_time,0);
    auto scaleOut = ScaleBy::create(_time, 0.8f);
    auto _cleanLabel = RemoveSelf::create();
    _scoreBoardLabel->runAction(Sequence::create(Spawn::create(moveLabel,fadeOut, scaleOut, NULL), _cleanLabel,NULL));
}

std::vector<TTCell*> TTBoard::completeCellSelection(std::vector<TTCell*> _cellsToComplete)
{
    std::vector<TTCell*> _comboAffectCells; // Todas las celdas afectadas por los combos
    std::vector<TTCell*> _combosAnalized;   // Combos ya analizados
    for(int i = 0; i < _cellsToComplete.size(); i++)
    {
        if(!_cellsToComplete[i]->isComboCell() && !_cellsToComplete[i]->isComboColorCell())
            continue;
        
        // Verificamos que el como no se haya analizado ya
        bool _analizedCombo = false;
        for(int j = 0; j < _combosAnalized.size(); j++)
        {
            if(_cellsToComplete[i] == _combosAnalized[j])
            {
                _analizedCombo = true;
                break;
            }
        }
        if(_analizedCombo) continue;
        
        _combosAnalized.push_back(_cellsToComplete[i]);
        
        // Analizamos el nuevo combo
        std::vector<CONTENT_TYPES> _contentsCell = _cellsToComplete[i]->getCellContentTypes();
        switch(_contentsCell[_contentsCell.size()-1])
        {
            case COMBO_VERTICAL_DYNAMIC_CONTENT:
            {
                std::vector<TTCell*> _cells = selectCellsFromColumn(_cellsToComplete[i]);
                for(int j = 0; j < _cells.size(); j++)
                {
                    _comboAffectCells.push_back(_cells[j]);
                    _cellsToComplete.push_back(_cells[j]);
                }
            }
            break;
            case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
            {
                std::vector<TTCell*> _cells = selectCellsFromRow(_cellsToComplete[i]);
                for(int j = 0; j < _cells.size(); j++)
                {
                    _comboAffectCells.push_back(_cells[j]);
                    _cellsToComplete.push_back(_cells[j]);
                }
            }
            break;
            case COMBO_AREA_DYNAMIC_CONTENT:
            {
                std::vector<TTCell*> _cells = selectCellsFromDiagonal(_cellsToComplete[i]);
                for(int j = 0; j < _cells.size(); j++)
                {
                    _comboAffectCells.push_back(_cells[j]);
                    _cellsToComplete.push_back(_cells[j]);
                }
            }
            break;
            case COMBO_COLOR_DYNAMIC_CONTENT:
            {
                if(_selectedCells.size() != 0)
                {
                    // Caso normal
                    int _posCombocolor = _selectedCells[0]->isComboColorCell() ? 0 : 1;
                    TTCell* _nativeComboColor = _selectedCells[_posCombocolor];
                    if(_cellsToComplete.size()>1 && _cellsToComplete != _selectedCells && _cellsToComplete[i] != _nativeComboColor)
                    {
                        //Decide random figure type
                        TTCell *_cellSelected = nullptr;
                        for(int w = 0; w < _cellsToComplete.size(); w++)
                        {
                            if(_cellsToComplete[w]->getCellFigure() <= 5)
                            {
                                _cellSelected = _cellsToComplete[w];
                                break;
                            }
                        }
                        
                        if(_cellSelected != nullptr)
                        {
                            std::vector<TTCell*> _cellsSelectedComboColor = selectCellsSameType(_cellSelected);
                            for(int u = 0; u < _cellsSelectedComboColor.size(); u++)
                            {
                                _comboAffectCells.push_back(_cellsSelectedComboColor[u]);
                                _cellsToComplete.push_back(_cellsSelectedComboColor[u]);
                            }
                            
                            _stepsManager->stepsToFreeKamehamehaComboColor(_cellsToComplete[i],_cellsSelectedComboColor);
                        }
                    }

                }
                else
                {
                    // Caso especial para Boosters
                    TTCell *_cellSelected = nullptr;
                    for(int w = 0; w < _cellsToComplete.size(); w++)
                    {
                        if(_cellsToComplete[w]->getCellFigure() <= 5)
                        {
                            _cellSelected = _cellsToComplete[w];
                            break;
                        }
                    }
                    
                    if(_cellSelected != nullptr)
                    {
                        std::vector<TTCell*> _cellsSelectedComboColor = selectCellsSameType(_cellSelected);
                        for(int u = 0; u < _cellsSelectedComboColor.size(); u++)
                        {
                            _comboAffectCells.push_back(_cellsSelectedComboColor[u]);
                            _cellsToComplete.push_back(_cellsSelectedComboColor[u]);
                        }
                        
                        _stepsManager->stepsToFreeKamehamehaComboColor(_cellsToComplete[i],_cellsSelectedComboColor);
                    }
                }
            }
            break;
            default:
            break;
        }

    }
    
    return _comboAffectCells;
}

std::vector<TTCell*> TTBoard::getAreaCombinationAffectCells()
{
    std::vector<TTCell*> _areaSelectionCells;
    for(int a = 0; a < _selectedCells.size(); a++)
    {
        if(_selectedCells[a]->isFirstSelectableCell())
        {
            getActionAreaCells(_selectedCells[a],&_areaSelectionCells);
        }
    }
    
    //Eliminamos las celdas propias de la combinación
    for(int c = 0; c < _selectedCells.size(); c++)
    {
        TTCell* _actualCell = _selectedCells[c];
        for(int b = 0; b < _areaSelectionCells.size(); b++)
        {
            if(_actualCell == _areaSelectionCells[b])
            {
                _areaSelectionCells.erase(_areaSelectionCells.begin()+b);
                b--;
            }
        }
    }
    
    // Eliminamos las celdas que no pueden son afectadas por las combinaciones colindantes
    for(int h = 0; h < _areaSelectionCells.size(); h++)
    {
        if(!_areaSelectionCells[h]->isAffectedFromAreaCombination())
        {
            _areaSelectionCells.erase(_areaSelectionCells.begin()+h);
            h--;
        }
    }
    
    return _areaSelectionCells;
}

void TTBoard::resetPlayCounters()
{
    _selectedCells.clear();
}

std::vector<TTCell*> TTBoard::getAllNormalCells()
{
    std::vector<TTCell*> _posibleCells;
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[0].size(); j++)
        {
            if(_Cells[i][j]->isSelectableCell() && !_Cells[i][j]->isWallCell())
            {
                _posibleCells.push_back(_Cells[i][j]);
            }
        }
    }
    return _posibleCells;
}

CONTENT_TYPES TTBoard::getNewComboType(std::vector<TTCell*> _cells)
{
    if(_selectedCells[_selectedCells.size()-2]->_column == _selectedCells[_selectedCells.size()-1]->_column)
        return COMBO_VERTICAL_DYNAMIC_CONTENT;
    else if(_selectedCells[_selectedCells.size()-2]->_row == _selectedCells[_selectedCells.size()-1]->_row)
        return COMBO_HORIZONTAL_DYNAMIC_CONTENT;
    else
        return COMBO_AREA_DYNAMIC_CONTENT;
}

std::vector<TTCell*> TTBoard::selectCellsFromColumn(TTCell* Cell)
{
    int column = Cell->_column;
    std::vector<TTCell*> _collectVector;
    
    for(int i = 0; i < _Cells[column].size(); i++)
    {
        if(_Cells[column][i]->isEmptyCell() || _Cells[column][i] == Cell) continue;
        
        //Control de StrongWalls por arriba
        if(i < Cell->_row && _Cells[column][i]->isStrongWallCell())
        {
            _collectVector.clear();
        }
        
        _collectVector.push_back(_Cells[column][i]);
        
        //Control de StrongWalls por debajo
        if(i > Cell->_row && _Cells[column][i]->isStrongWallCell())
        {
            break;
        }
    }
    
    return _collectVector;
}

std::vector<TTCell*> TTBoard::selectCellsFromRow(TTCell* Cell)
{
    int row = Cell->_row;
    std::vector<TTCell*> _collectVector;
    
    for(int i = 0; i < _Cells.size(); i++)
    {
        if(_Cells[i][row]->isEmptyCell() || _Cells[i][row] == Cell) continue;
        
        //Control de StrongWalls por izquierda
        if(i < Cell->_column && _Cells[i][row]->isStrongWallCell())
        {
            _collectVector.clear();
        }
        
        _collectVector.push_back(_Cells[i][row]);
        
        //Control de StrongWalls por derecha
        if(i > Cell->_column && _Cells[i][row]->isStrongWallCell())
        {
            break;
        }
    }
    
    return _collectVector;
}

void TTBoard::getActionAreaCells(TTCell* Cell, std::vector<TTCell*> *collectVector)
{
    // No seleccionamos las celdas diagonales
    int column = Cell->_column;
    int row = Cell->_row;
    
    // Cuadrado de seleccion
    std::vector<TTCell*> _squareCells;
    
    int minColumn = 0;
    if(column == 0) minColumn = column;
    else minColumn = column-1;
    
    int maxColumn = 0;
    if(column == _Cells.size()-1) maxColumn = column;
    else maxColumn = column+1;
    
    int minRow = 0;
    if(row == 0) minRow = row;
    else minRow = row-1;
    
    int maxRow = 0;
    if(row == _Cells[0].size()-1) maxRow = row;
    else maxRow = row+1;
    
    for(int t = minColumn; t <= maxColumn; t++)
    {
        for(int y = minRow; y <= maxRow; y++)
        {
            if(t != column && y != row)
                continue;
            
            _squareCells.push_back(_Cells[t][y]);
        }
    }
    
    // seleccioamos celdas
    for(int i = 0; i < _squareCells.size(); i++)
    {
        if(_squareCells[i]->isEmptyCell())
            continue;
        
        bool isSelected = false;
        for(int j = 0; j < collectVector->size() && !isSelected; j++)
        {
            if(_squareCells[i]->_row == (*collectVector)[j]->_row && _squareCells[i]->_column == (*collectVector)[j]->_column)
            {
                isSelected = true;
            }
        }
        
        if(!isSelected)
            collectVector->push_back(_squareCells[i]);
    }
    
}

std::vector<TTCell*> TTBoard::selectCellsFromDiagonal(TTCell* Cell)
{
    int column = Cell->_column;
    int row = Cell->_row;
    
    std::vector<TTCell*> _collectVector;
    
    // Cuadrado de seleccion
    std::vector<TTCell*> _squareCells;
    
    int minColumn = 0;
    if(column == 0) minColumn = column;
    else minColumn = column-1;
    
    int maxColumn = 0;
    if(column == _Cells.size()-1) maxColumn = column;
    else maxColumn = column+1;
    
    int minRow = 0;
    if(row == 0) minRow = row;
    else minRow = row-1;
    
    int maxRow = 0;
    if(row == _Cells[0].size()-1) maxRow = row;
    else maxRow = row+1;
    
    for(int t = minColumn; t <= maxColumn; t++)
    {
        for(int y = minRow; y <= maxRow; y++)
        {
            _squareCells.push_back(_Cells[t][y]);
        }
    }

    // seleccioamos celdas
    for(int i = 0; i < _squareCells.size(); i++)
    {
        if(_squareCells[i]->isEmptyCell() || _squareCells[i] == Cell) continue;
        _collectVector.push_back(_squareCells[i]);
    }
    
    return _collectVector;
}

std::vector<TTCell*> TTBoard::selectCellsSameType(TTCell* cell)
{
    std::vector<TTCell*> _collectVector;
    
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[0].size(); j++)
        {
            if(!_Cells[i][j]->isSelectableCell())
                continue;
            
            if(cell->getCellFigure() == _Cells[i][j]->getCellFigure())
            {
                _collectVector.push_back(_Cells[i][j]);
            }
        }
    }
    
    return _collectVector;
}

int TTBoard::getLastCellOnBoardColumn(int column)
{
    for(int i = _Cells[column].size()-1; i >= 0; i--)
    {
        if(_Cells[column][i]->isEmptyCell())
            continue;
        return i;
    }
    return _Cells[column].size()-1;
}

void TTBoard::updateBoardState()
{
    _needToUpdateBoard = false;
    _cellsToReposition = 0;
    
    // Caida normal de de celdas
    for(int i = 0; i < _Cells.size(); i++)
    {
        int lastRowInColumn = getLastCellOnBoardColumn(i);
        
        for(int j = _Cells[i].size()-1; j >= 0; j--)
        {
            if(!_Cells[i][j]->isEmptyCell() && _Cells[i][j]->isRefillableCell())
            {
                for(int u = j-1; u >= 0; u--)
                {
                    // Si hemos llegado a una piedra la caida de celdas no puede continuar
                    if(_Cells[i][u]->isWallCell())
                        break;
                    
                    if(!_Cells[i][u]->isEmptyCell() && _Cells[i][u]->hasMovableContent())
                    {
                        TTCell* cellToMove = _Cells[i][u];
                        TTCell* cellDestination = _Cells[i][j];
                        
                        if(j == lastRowInColumn && cellToMove->isDownObjectCell())
                        {
                            // Movimiento de un objecto bajable cuando debe desaparecer
                            cellToMove->moveDownObjectOutOfBoard(cellDestination);
                        }
                        else
                        {
                            // Movimiento normal de celdas
                            cellToMove->moveCell(cellDestination);
                            _cellsToReposition++;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Relleno inteligente de nuevas celdas
    generateNewCells();
    
    // Detección de celdas vacías
    _cellHoles.clear();
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            if(!_Cells[i][j]->isEmptyCell() && _Cells[i][j]->isRefillableCell())
            {
                bool _isHoleNoRefillable = false;
                for(int u = 0; u < _cellExcludeHoles.size() && !_isHoleNoRefillable; u++)
                {
                    if(_cellExcludeHoles[u].x == i && _cellExcludeHoles[u].y == j)
                        _isHoleNoRefillable = true;
                }
                
                if(!_isHoleNoRefillable)
                {
                    CCLOG("Push Board Hole:%d,%d",i,j);
                    _cellHoles.push_back(Vec2(i,j));
                }
            }
        }
    }
    
    // Si no hay celdas para reposicionar, desbloqueamos el tablero
    if(_cellsToReposition == 0)
    {
        unlockBoard();
    }
}

void TTBoard::generateNewCells()
{
    // Verificamos los tipos de celdas a crear
    std::vector<CONTENT_TYPES> _newCellTypes;
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            // Si llegamos a una piedra, debemos dejar de generar celdas en la columna
            if(_Cells[i][j]->isWallCell())
                break;
            
            if(!_Cells[i][j]->isEmptyCell() && _Cells[i][j]->isRefillableCell())
            {
                // Si no hay contenido dinámico hay que generar nuevo
                _newCellTypes.push_back(FIGURE_DYNAMIC_CONTENT);
            }
        }
    }
    
    for(int i = 0; i < _boardGoals.size(); i++)
    {
        switch(_boardGoals[i]->_typeGoal)
        {
            case DOWN_OBJECT_GOAL:
            {
                int totalObjectsToDown = _boardGoals[i]->_goalValue;
                int objectsDown = _boardGoals[i]->_actualValue;
                std::vector<TTCell*> _inBoardDownObjects;
                for(int i = 0; i < _Cells.size(); i++)
                {
                    for(int j = 0; j < _Cells[i].size(); j++)
                    {
                        if(_Cells[i][j]->isDownObjectCell())
                            _inBoardDownObjects.push_back(_Cells[i][j]);
                    }
                }
                
                int objectsToGenerate = totalObjectsToDown - (_inBoardDownObjects.size()+objectsDown);
                int actualDownObjectsWeight = static_cast<TTDownObjectGoal*>(_boardGoals[i])->_actualWeight;
                int maxDownObjectsWeight = static_cast<TTDownObjectGoal*>(_boardGoals[i])->_maxWeight;
                
                if(actualDownObjectsWeight < static_cast<TTDownObjectGoal*>(_boardGoals[i])->_initWeight && objectsToGenerate > 0)
                {
                    CCLOG("ALPACAS -> Reinicio de pesos de las alpacas");
                    
                    //reinicio de pesos
                    actualDownObjectsWeight = static_cast<TTDownObjectGoal*>(_boardGoals[i])->_initWeight;
                    static_cast<TTDownObjectGoal*>(_boardGoals[i])->_actualWeight = actualDownObjectsWeight;
                }
                
                if(objectsToGenerate > 0 && _inBoardDownObjects.size() == 0)
                {
                    if(_newCellTypes.size() == 0)
                        break;
                    
                    CCLOG("ALPACAS -> Se fuerza caida de alpaca");
                    // buscamos una posición random en el vector de tipos de celda a crear
                    _newCellTypes[rand()%_newCellTypes.size()] = DOWN_OBJECT_DYNAMIC_CONTENT;
                }
                else
                {
                    for(int r = 0; r < objectsToGenerate; r++)
                    {
                        int randWeight = rand()%maxDownObjectsWeight;
                        CCLOG("ALPACAS -> Peso random = %d, Peso actual sistema = %d",randWeight,actualDownObjectsWeight);
                        if(randWeight < actualDownObjectsWeight)
                        {
                            if(_newCellTypes.size() == 0)
                                break;
                            
                            // buscamos una posición random en el vector de tipos de celda a crear
                            _newCellTypes[rand()%_newCellTypes.size()] = DOWN_OBJECT_DYNAMIC_CONTENT;
                            static_cast<TTDownObjectGoal*>(_boardGoals[i])->_actualWeight = static_cast<TTDownObjectGoal*>(_boardGoals[i])->_initWeight;
                        }
                    }
                }
            }
            break;
                
            default:
            break;
        }
    }
    
    // Generamos las celdas según el mapa de celdas
    int cellsTypeIdx = 0;
    for(int i = 0; i < _Cells.size(); i++)
    {
        std::vector<int> _newCellsIds;
        std::vector<int> _emptyCells;
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            // Si encontramos una barrera dejamos de buscar
            if(_Cells[i][j]->isWallCell())
                break;
            
            if(_Cells[i][j]->isEmptyCell())
            {
                _emptyCells.push_back(1);
            }
            else
            {
                _emptyCells.push_back(0);
                if(_Cells[i][j]->isRefillableCell())
                    _newCellsIds.push_back(j);
            }
        }
     
        for(int w = 0; w < _newCellsIds.size(); w++)
        {
            _Cells[i][_newCellsIds[w]]->generateNewCell(_newCellTypes[cellsTypeIdx],_newCellsIds.size(),(_newCellsIds.size()-1)-w,_emptyCells);
            _cellsToReposition++;
            cellsTypeIdx++;
        }
    }
}

void TTBoard::unlockBoard()
{
    CCLOG("Verifica si puede desbloquear el tablero");
    
    if(!_isBoardActive)
        return;
    
    // si han quedado agujeros en el tablero los intentamos rellenar
    if(_cellHoles.size() > 0)
    {
        CCLOG("Hay agujeros para rellenar en el tablero");
        stepFillHolesInBoard();
        return;
    }
    
    CCLOG("Integridad del tablero verificada");
    
    // Verificamos la propagación del tablero
    verifyCellsPropagation();
    
    _helpCombination.clear();
    bool combinationFound = launchShowHelpCombination();
        
    if(!combinationFound)
    {
        CCLOG("No se ha encontrado combinación");
            
        // Esperamos al tiempo de resituar las celdas en el tablero
        DelayTime *delayRemixBoard = DelayTime::create(MOVE_MIX_TIME*2+SHOW_LABEL_MIX_TIME);
        CallFunc *unlock = CallFunc::create(std::bind([&](){
            finishActionsBoard();
        }));
        _parentLayer->runAction(Sequence::create(delayRemixBoard,unlock,NULL));
    }
    else
    {
        CCLOG("Se ha encontrado combinación de tablero");
        finishActionsBoard();
    }
}

void TTBoard::verifyCellsPropagation()
{
    // Lanzamos el siguiente paso de tutorial
    if(TTHowToInGameManager::getInstance()->_isTutorialActive)
    {
        TTHowToInGameManager::getInstance()->nextStepTutorial();
    }
    
    if(_lastCellsTypeHited.size() > 0)
    {
        bool _glueCellHited = false;
        for(int i = 0; i < _lastCellsTypeHited.size(); i++)
        {
            if(_lastCellsTypeHited[i] == GLUE_STATIC_CONTENT)
            {
                _glueCellHited = true;
                break;
            }
        }
        _lastCellsTypeHited.clear();
        
        if(!_glueCellHited)
        {
            // Verificamos si hay que crear nuevos pegotes
            std::vector<TTCell*> _glueCells;
            for(int i = 0; i < _Cells.size(); i++)
            {
                for(int j = 0; j < _Cells[i].size(); j++)
                {
                    if(_Cells[i][j]->_cellContentLayers.size() > 0 && typeid(*_Cells[i][j]->_cellContentLayers.back()) == typeid(TTCellStaticContentGlue))
                    {
                        _glueCells.push_back(_Cells[i][j]);
                    }
                }
            }
            if(_glueCells.size() > 0)
            {
                //Verificamos si algun pegote se puede propagar
                TTCell* _cellToPropagate = nullptr;
                std::random_shuffle(_glueCells.begin(),_glueCells.end());
                for(int i = 0; i < _glueCells.size(); i++)
                {
                    std::vector<TTCell*> _cellsToPropagation = selectCellsFromDiagonal(_glueCells[i]);
                    for(int j = 0; j < _cellsToPropagation.size(); j++)
                    {
                        if(_cellsToPropagation[j]->isSelectableCell())
                        {
                            _cellToPropagate = _cellsToPropagation[j];
                            break;
                        }
                    }
                    if(_cellToPropagate != nullptr) break;
                }
                
                if(_cellToPropagate != nullptr)
                {
                    // Propagamos el pegote
                    TTCellContent* _layer = new TTCellStaticContentGlue(_parentBoard,_cellToPropagate->_cellContentLayers.back()->getZContent(),GLUE_STATIC_CONTENT,_cellToPropagate->_posXCell,_cellToPropagate->_posYCell,_cellToPropagate->_sizeCell,_cellToPropagate->getCellId(),true);
                    _cellToPropagate->_cellContentLayers.push_back(_layer);
                }
            }
        }
    }
}

void TTBoard::finishActionsBoard()
{
    _isUpdatingBoard = false;
    sendEvent(EVENT_GAME_READY_TO_PLAY);
}

void TTBoard::stepFillHolesInBoard()
{
    //Buscamos el primer agujero rellenable en el tablero
    std::map<int,bool> _cellsRepositioned;  //control de celdas que se han movido
    std::map<int,bool> _cellsMoved;         //control de celdas que se han movido
    std::map<int,bool> _cellsExcluded;      //control de celdas excluidas del llenado
    for(int i = 0; i < _cellHoles.size(); i++)
    {
        int column = _cellHoles[i].x;
        int row = _cellHoles[i].y;
        if(!_Cells[column][row]->isEmptyCell() && _Cells[column][row]->isRefillableCell())
        {
            CCLOG("Tratando agujero en posición %d, %d",column,row);
            
            bool probarDiagonales = true;
            //comprobar si es una celda que se pueda intentar rellenar por las diagonales
            //evitar el caso de varias celdas vacias en vertical
            if (row!=0 && (_cellsMoved[column*10+(row-1)]==true || _cellsExcluded[column*10+(row-1)]==true) )
            {
                probarDiagonales = false;
            }
            if (probarDiagonales)
            {
                // Miramos si la celda puede ser llenada desde la fila inmediatamente superior, desde izquierda o derecha
                if(column != 0 && !_Cells[column-1][row-1]->isEmptyCell() && _Cells[column-1][row-1]->hasMovableContent() && _Cells[column-1][row-1]->isSelectableCell() && _cellsMoved[(column-1)*10+(row-1)]==false)
                {
                    //Puede ser rellenada desde la izquierda
                    CCLOG("La celda puede ser rellenada desde la izquierda");
                    _cellsToReposition++;
                    _Cells[column-1][row-1]->moveCell(_Cells[column][row]);
                    _cellsMoved[column*10+row]=true;
                    _cellsRepositioned[((column-1)*10)+(row-1)]=true;
                }
                else if(column != _Cells.size()-1 && !_Cells[column+1][row-1]->isEmptyCell() && _Cells[column+1][row-1]->hasMovableContent() && _Cells[column+1][row-1]->isSelectableCell() && _cellsMoved[(column+1)*10+(row-1)]==false)
                {
                    //Puede ser rellenada desde la derecha
                    CCLOG("La celda puede ser rellenada desde la derecha");
                    _cellsToReposition++;
                    _Cells[column+1][row-1]->moveCell(_Cells[column][row]);
                    _cellsMoved[column*10+row]=true;
                    _cellsRepositioned[((column+1)*10)+(row-1)]=true;
                }
                else
                {
                    // Verificamos si la celda podrá ser rellenada en un futuro
                    if(_cellsRepositioned[((column-1)*10)+(row-1)] || _cellsRepositioned[((column+1)*10)+(row-1)])
                    {
                        //Podrá ser rellenada en un futuro
                        CCLOG("La celda podrá ser rellenada en un futuro");
                        
                        // Sonido de caída por pasos
                        TTAudioManager::getInstance()->playAudio("DOWN_LINE_HOLES");
                    }
                    else
                    {
                        //No puede ser rellenada
                        CCLOG("La celda no puede ser rellenada");
                        _cellExcludeHoles.push_back(Vec2(column,row));
                        _cellsExcluded[column*10+row]=true;
                    }
                }
            } else {
                CCLOG("La celda no puede ser rellenada en esta iteración");
                _cellsExcluded[column*10+row]=true;
            }
        }
    }
    
    // Verifica los agujeros no llenables de nuevo para actualizar la lista
    for(int i = 0; i < _cellExcludeHoles.size(); i ++)
    {
        int column = _cellExcludeHoles[i].x;
        int row = _cellExcludeHoles[i].y;
        
        if((column != 0 && !_Cells[column-1][row-1]->isEmptyCell() && _Cells[column-1][row-1]->hasMovableContent() && _Cells[column-1][row-1]->isSelectableCell()) ||
           (column != _Cells.size()-1 && !_Cells[column+1][row-1]->isEmptyCell() && _Cells[column+1][row-1]->hasMovableContent() && _Cells[column+1][row-1]->isSelectableCell()))
        {
            // El agujero ahora es llenable
            CCLOG("La celda %d,%d ahora puede ser llenada",column,row);
            _cellExcludeHoles.erase(_cellExcludeHoles.begin()+i);
        }
    }
    
    // Verifica si hay que updatear el tablero
    if(_cellsToReposition == 0)
    {
        updateBoardState();
    }
    else
    {
        _needToUpdateBoard = true;
    }
}

void TTBoard::launchAnimStandBy()
{
    if(!_isBoardActive)
        return;
    
    // Lanza la animacion standBy, como máximo un tercio de las figuras
    int _numFiguresToAnimate = ANIM_STANDBY_MIN_FIGURES+rand()%(AMIM_STANDBY_MAX_FIGURES-ANIM_STANDBY_MIN_FIGURES);
    for(int i = 0; i < _numFiguresToAnimate; i++)
    {
        int _randColumn = rand()%_Cells.size();
        int _randRow = rand()%_Cells[0].size();
        _Cells[_randColumn][_randRow]->launchCellAnimationStandBy();
    }
    
    // Rellamada a la funcion para lanzar animacion standBy
    int _timeToPlayStandByAnimation = ANIM_STANDBY_MIN_TIME+rand()%(AMIM_STANDBY_MAX_TIME-ANIM_STANDBY_MIN_TIME);
    DelayTime *delayLaunchStandByAnimation = DelayTime::create(_timeToPlayStandByAnimation);
    CallFunc *launchStandByAnimation = CallFunc::create(CC_CALLBACK_0(TTBoard::launchAnimStandBy,this));
    _parentBoard->runAction(Sequence::create(delayLaunchStandByAnimation,launchStandByAnimation,NULL));
}

bool TTBoard::searchForCombination(bool onlySearchCombos)
{
    // Si ya existia combinacion...
    if(_helpCombination.size() > 0)
        return true;
    
    // Buscamos cualquier combinacion en el tablero
    bool combinationFound = false;
    int minCombSize = MIN_SELECT_CELLS;
    if(onlySearchCombos)
        minCombSize = MIN_SELECT_CELLS_COMBO;
    
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            TTCell *actualCell = _Cells[i][j];
            if(actualCell->isEmptyCell() || !actualCell->isSelectableCell() || actualCell->isWallCell())
                continue;
            
            if(onlySearchCombos && !actualCell->isComboCell())
                continue;
            
            std::vector<TTCell*> combination;
            combination.push_back(actualCell);
            
            bool match = true;
            while(match && combination.size() < minCombSize)
                match = sameCellsInSquare(combination[combination.size()-1],&combination,onlySearchCombos);
            
            if(combination.size() >= minCombSize)
            {
                int _numNoFirstSelectableCells = 0;
                for(int b = 0; b < combination.size(); b++)
                {
                    if(!combination[b]->isFirstSelectableCell()) _numNoFirstSelectableCells++;
                }
                
                // Verificamos que no haya dos redes en los extremos de la combinación
                if(_numNoFirstSelectableCells >= 2)
                {
                    combination.clear();
                    continue;
                }
                else
                {
                    combinationFound = true;
                    for(int g = 0; g < combination.size(); g++)
                    {
                        _helpCombination.push_back(combination[g]);
                    }
                    break;
                }
            }
        }
        if(combinationFound)
            break;
    }
 
    if (combinationFound==false && !onlySearchCombos)
    {
        //buscar solo combos
        combinationFound = searchForCombination(true);
        if (!combinationFound)
        {
            //buscar combos de color
            combinationFound = searchForcombinationComboColor();
        }
    }
    return combinationFound;
}


bool TTBoard::searchForcombinationComboColor()
{
    // Si ya existia combinacion...
    if(_helpCombination.size() > 0)
        return true;
    bool combinationFound = false;

    for(int i = 0; i < _Cells.size() && !combinationFound; i++)
    {
        for(int j = 0; j < _Cells[i].size() && !combinationFound; j++)
        {
            TTCell *actualCell = _Cells[i][j];
            if(actualCell->isEmptyCell() || !actualCell->isSelectableCell() || actualCell->isWallCell())
                continue;
            //buscar un combo de color
            if(!actualCell->isComboColorCell())
                continue;
            //buscar en el cuadrado de la celda
            for(int ii = i-1 ; ii <= i+1 && !combinationFound; ii++)
            {
                if (ii<0 || ii>=_Cells.size())
                    continue;
                for(int jj = j-1 ; jj <= j+1 && !combinationFound; jj++)
                {
                    if (jj<0 || jj>=_Cells[ii].size())
                        continue;
                    
                    if (ii==i && jj==j)  //no tener en cuenta el propio combo de color
                        continue;
                    
                    if(_Cells[ii][jj]->isEmptyCell() || !_Cells[ii][jj]->isSelectableCell() || _Cells[ii][jj]->isWallCell())
                        continue;

                    //si llegamos aquí, se trata de una celda seleccionable, con lo que se puede
                    //realizar una combinación con el combo color
                    _helpCombination.push_back(_Cells[i][j]);
                    _helpCombination.push_back(_Cells[ii][jj]);

                    combinationFound = true;
                }
                
            }
        }
    }
    return combinationFound;
   
}

bool TTBoard::sameCellsInSquare(TTCell* initCell, std::vector<TTCell*> *combination,bool onlySearchCombos)
{
    bool match = false;
    int minCombSize = MIN_SELECT_CELLS;
    if(onlySearchCombos)
        minCombSize = MIN_SELECT_CELLS_COMBO;
    
    // Cuadrado de seleccion
    int column = initCell->_column;
    int row = initCell->_row;
    std::vector<TTCell*> _squareCells;
    
    int minColumn = 0;
    if(column == 0) minColumn = column;
    else minColumn = column-1;
    
    int maxColumn = 0;
    if(column == _Cells.size()-1) maxColumn = column;
    else maxColumn = column+1;
    
    int minRow = 0;
    if(row == 0) minRow = row;
    else minRow = row-1;
    
    int maxRow = 0;
    if(row == _Cells[0].size()-1) maxRow = row;
    else maxRow = row+1;
    
    for(int t = minColumn; t <= maxColumn; t++)
    {
        for(int y = minRow; y <= maxRow; y++)
        {
            if((initCell->getCellId() != _Cells[t][y]->getCellId()) && !_Cells[t][y]->isEmptyCell() && _Cells[t][y]->isSelectableCell())
                _squareCells.push_back(_Cells[t][y]);
        }
    }
    
    // buscamos una celda del mismo tipo en el cuadrado de seleccion
    for(int z = 0; z < _squareCells.size(); z++)
    {
        bool includeCell = false;
        if(onlySearchCombos)
        {
            if(_squareCells[z]->isComboCell())
                includeCell = true;
        }
        else
        {
            if(initCell->getCellFigure() == _squareCells[z]->getCellFigure())
                includeCell = true;
        }
        
        if(!includeCell)
            continue;
        
        // verificamos que la celda no este ya en el vector
        bool included = false;
        for(int t = 0; t < combination->size(); t++)
        {
            if((*combination)[t]->getCellId() == _squareCells[z]->getCellId())
                included = true;
        }
        
        if(!included)
        {
            combination->push_back(_squareCells[z]);
            match = true;
            
            if(combination->size() >= minCombSize)
                return true;
        }
    }

    return match;
}

void TTBoard::showHelpCombination(bool show)
{
    if(!(_helpCombination.size() > 0) || _selectedCells.size() > 0)
        return;
    
    for(int i = 0; i < _helpCombination.size(); i++)
    {
        if(show)
        {
            if(_helpCombination[i]->isSelectableCell() && _helpCombination[i]->isFirstSelectableCell())
            {
                (static_cast<TTCellDynamicContentFigure*>(_helpCombination[i]->_cellContentLayers.back()))->showHelpCell();
            }
            else if(_helpCombination[i]->isSelectableCell() && !_helpCombination[i]->isFirstSelectableCell())
            {
                (static_cast<TTCellDynamicContentFigure*>(_helpCombination[i]->_cellContentLayers[_helpCombination[i]->_cellContentLayers.size()-2]))->showHelpCell();
            }
        }
        else
        {
            if(_helpCombination[i]->isSelectableCell() && _helpCombination[i]->isFirstSelectableCell())
            {
                (static_cast<TTCellDynamicContentFigure*>(_helpCombination[i]->_cellContentLayers.back()))->stopHelpCell();
            }
            else if(_helpCombination[i]->isSelectableCell() && !_helpCombination[i]->isFirstSelectableCell())
            {
                (static_cast<TTCellDynamicContentFigure*>(_helpCombination[i]->_cellContentLayers[_helpCombination[i]->_cellContentLayers.size()-2]))->stopHelpCell();
            }
        }
    }
}

bool TTBoard::launchShowHelpCombination()
{
    // Buscar combinacion
    bool combinationFound = searchForCombination();
    
    // verifica integridad del tablero - busca combinaciones normales
    if(!combinationFound)
    {
        // Remover las celdas
        CCLOG("No hay combinaciones en el tablero! Hay que remezclar");
        
        _isInBoardMix = true;
        
        // Creamos mensaje de remezclar tablero
        Size _screenSize = Director::getInstance()->getWinSize();
        Sprite*  _woodInfo;
        Label*  _textWinLevel;
        _woodInfo = Sprite::createWithSpriteFrameName("barra_madera.png");
        _woodInfo->setPosition(Point(_screenSize.width/2,_screenSize.height+_woodInfo->getBoundingBox().size.height/2));
        _woodInfo->setAnchorPoint(Vec2(0.5,0.5));
        _textWinLevel = Label::createWithBMFont("comicFnt.fnt",TTLanguageManager::getInstance()->getTag("WITHOUT_COMBINATIONS"));
        _textWinLevel->setAnchorPoint(Vec2(0.5,0.5));
        _textWinLevel->setPosition(Vec2(_woodInfo->getBoundingBox().size.width/2,_woodInfo->getBoundingBox().size.height/2));
        _woodInfo->addChild(_textWinLevel);
        
        float _widthWood = (80*_screenSize.width)/100;
        _woodInfo->setScale(_widthWood/_woodInfo->getBoundingBox().size.width);
        
        //corregimos la escala al 85% del ancho de la tabla
        _textWinLevel->setScale(0.85*(_widthWood/_textWinLevel->getContentSize().width)/_woodInfo->getScale());

        TTBoard::getInstance()->_parentBoard->getParent()->addChild(_woodInfo,21);
        _woodInfo->setPosition(_screenSize.width/2, _screenSize.height);
        
        auto _moveWoodTableUp = EaseExponentialOut::create(MoveTo::create(SHOW_LABEL_MIX_TIME/4,Point(_screenSize.width/2,_screenSize.height/2)));
        auto _moveWoodTableDown = EaseExponentialIn::create(MoveTo::create(SHOW_LABEL_MIX_TIME/4,Point(_screenSize.width/2,-_screenSize.height/2)));
        auto _remove = RemoveSelf::create(true);

        _woodInfo->runAction(Sequence::create(_moveWoodTableUp, DelayTime::create(SHOW_LABEL_MIX_TIME), _moveWoodTableDown,CallFunc::create(std::bind(&TTBoard::launchMixBoard, this)),DelayTime::create(MOVE_MIX_TIME),CallFunc::create(std::bind(&TTBoard::finishMixBoard, this)),_remove, NULL));
        
        TTAudioManager::getInstance()->playAudio("MIX_BOARD_ENTER");
        
        return false;
    }
    else
    {
        if(TTHowToInGameManager::getInstance()->_isTutorialActive)
        {
            return true;
        }
        
        // Evento para mostrar ayuda de tablero
        DelayTime *delayLaunchHelpAnimation = DelayTime::create(ANIM_HELP_TIME);
        CallFunc *launchHelpAnimation = CallFunc::create(std::bind(&TTBoard::showHelpCombination, this,true));
        Sequence *helpAction = Sequence::create(delayLaunchHelpAnimation,launchHelpAnimation,NULL);
        helpAction->setTag(HELP_ACTION_COMBINATION_TAG);
        _parentBoard->runAction(helpAction);
    }
    return true;
}

void TTBoard::finishMixBoard()
{
    _isInBoardMix = false;
}

void TTBoard::launchMixBoard()
{
    TTAudioManager::getInstance()->playAudio("MIX_BOARD_EXIT");
    if(mixBoardCellsForCombination())
    {
        // volvemos a lanzar la ayuda de combinación
        stopShowHelpCombination();
        _helpCombination.clear();
        launchShowHelpCombination();
    }
}

void TTBoard::stopShowHelpCombination()
{
    _parentBoard->stopActionByTag(HELP_ACTION_COMBINATION_TAG);
    showHelpCombination(false);
}

bool TTBoard::mixBoardCellsForCombination(bool forceCombosCombination)
{
    int minCombinationSearch = MIN_SELECT_CELLS;
    
    // 0. Obtenemos mapa de posicionamiento del tablero
    std::vector<std::vector<int> > mapPositions;
    std::vector<std::vector<int> > posiblePositions;
    for(int i = 0; i < _Cells.size(); i++)
    {
        std::vector<int> positions;
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            if(_Cells[i][j]->isSelectableCell() && !_Cells[i][j]->isWallCell())
            {
                positions.push_back(1);
                
                // guardamos la posicion
                std::vector<int> position;
                position.push_back(i);
                position.push_back(j);
                posiblePositions.push_back(position);
            }
            else
            {
                positions.push_back(0);
            }
        }
        mapPositions.push_back(positions);
    }
    // reordenamos al azar el vector de posibles posiciones
    std::srand((unsigned int)time(NULL));
    std::random_shuffle(posiblePositions.begin(),posiblePositions.end(),RNG());
    
    // 1. Buscamos casillas del mismo tipo
    std::vector<std::vector<TTCell*> > _newCellsCombination;
    for(int i = 0; i < NUM_CELL_NORMAL_FIGURES; i++)
    {
        std::vector<TTCell *> _cells;
        _newCellsCombination.push_back(_cells);
    }
    
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
        {
            if(_Cells[i][j]->isSelectableCell() && !_Cells[i][j]->isComboColorCell() && !_Cells[i][j]->isWallCell())
                _newCellsCombination[_Cells[i][j]->getCellFigure()].push_back(_Cells[i][j]);
        }
    }
    
    bool combinationFound = false;
    std::vector<int> possibleCellTypesCombination;
    for(int i = 0; i < _newCellsCombination.size(); i++)
    {
        if(_newCellsCombination[i].size() >= minCombinationSearch)
        {
            combinationFound = true;
            possibleCellTypesCombination.push_back(i);
        }
    }
    
    if(!combinationFound || forceCombosCombination)
    {
        CCLOG("No hay casillas para hacer una combinación del mismo tipo. Buscamos en los combos");
        minCombinationSearch = MIN_SELECT_CELLS_COMBO;
        _newCellsCombination.clear();
        possibleCellTypesCombination.clear();
        
        //Buscamos todos los combos
        std::vector<TTCell*> comboCells;
        for(int i = 0; i < _Cells.size(); i++)
        {
            for(int j = 0; j < _Cells[i].size(); j++)
            {
                if(_Cells[i][j]->isComboCell())
                    comboCells.push_back(_Cells[i][j]);
            }
        }
        
        if(comboCells.size() < minCombinationSearch)
        {
            CCLOG("No hay casillas para hacer una combinación de combos. Generamos un nuevo tablero con combinacion");

            int randTypeCombination = rand()%NUM_CELL_NORMAL_FIGURES;
            minCombinationSearch = MIN_SELECT_CELLS;
            bool combinationPositioned = false;
            for(int i = 0; i < posiblePositions.size(); i++)
            {
                std::vector<std::vector<int> > positionNewCombination;
                positionNewCombination.push_back(posiblePositions[i]);
                
                bool match = true;
                while(match && positionNewCombination.size() < minCombinationSearch)
                    match = samePositionsInSquare(positionNewCombination[positionNewCombination.size()-1], mapPositions, &positionNewCombination, minCombinationSearch);
                
                if(positionNewCombination.size() >= minCombinationSearch)
                {
                    // generamos fichas para crear la combinación en el tablero
                    for(int w = 0; w < positionNewCombination.size(); w++)
                    {
                        TTCell* _cellToGenerate = _Cells[positionNewCombination[w][0]][positionNewCombination[w][1]];
                        (static_cast<TTCellDynamicContentFigure*>(_cellToGenerate->_cellContentLayers.back()))->convertCellToNormalFigure(randTypeCombination);
                    }
                    combinationPositioned = true;
                    break;
                }
            }
            
            if(!combinationPositioned)
            {
                CCLOG("Imposible generar nuevo tablero con combinación, se finaliza partida");
                stopShowHelpCombination();
                sendEvent(EVENT_GAME_FINISH);
                return false;
            }
            
            TTAudioManager::getInstance()->playAudio("MIX_BOARD");
            return true;
        }
        
        _newCellsCombination.push_back(comboCells);
        possibleCellTypesCombination.push_back(0);
    }
    
    // 2. Montamos una combinación random
    bool combinationPositioned = false;
    std::vector<std::vector<int> > censoredPositions;
    for(int i = 0; i < posiblePositions.size(); i++)
    {
        std::vector<std::vector<int> > positionNewCombination;
        positionNewCombination.push_back(posiblePositions[i]);
        
        bool match = true;
        while(match && positionNewCombination.size() < minCombinationSearch)
            match = samePositionsInSquare(positionNewCombination[positionNewCombination.size()-1], mapPositions, &positionNewCombination, minCombinationSearch);
        
        if(positionNewCombination.size() >= minCombinationSearch)
        {
            // movemos fichas para crear la combinación en el tablero
            int randFigureIdx = rand()%possibleCellTypesCombination.size();
            std::vector<TTCell*> cellsToPlace = _newCellsCombination[possibleCellTypesCombination[randFigureIdx]];
            
            // verificamos si ya hay alguna posicion con una figura correcta
            std::vector<std::vector<int> > positionNewCombinationAux;
            for(int w = 0; w < positionNewCombination.size(); w++)
            {
                TTCell* _cellInNewPos = _Cells[positionNewCombination[w][0]][positionNewCombination[w][1]];
                bool isInPosition = false;
                for(int v = 0; v < cellsToPlace.size(); v++)
                {
                    if(cellsToPlace[v]->getCellId() == _cellInNewPos->getCellId())
                    {
                        isInPosition = true;
                        std::vector<int> posCell = {cellsToPlace[v]->_column,cellsToPlace[v]->_row};
                        censoredPositions.push_back(posCell);
                        cellsToPlace.erase(cellsToPlace.begin()+v);
                        break;
                    }
                }
                
                if(!isInPosition)
                    positionNewCombinationAux.push_back(positionNewCombination[w]);
            }
            positionNewCombination.clear();
            positionNewCombination = positionNewCombinationAux;
            
            for(int w = 0; w < positionNewCombination.size(); w++)
            {
                TTCell* _cellDestinationSwitch = _Cells[positionNewCombination[w][0]][positionNewCombination[w][1]];
                if(cellsToPlace[w]->getCellId() != _cellDestinationSwitch->getCellId())
                    cellsToPlace[w]->switchCell(_cellDestinationSwitch);
                
                //censuramos ambas casilla
                std::vector<int> posCellToPlace = {cellsToPlace[w]->_column,cellsToPlace[w]->_row};
                std::vector<int> posCellDestination = {_cellDestinationSwitch->_column,_cellDestinationSwitch->_row};
                censoredPositions.push_back(posCellToPlace);
                censoredPositions.push_back(posCellDestination);
            }
            combinationPositioned = true;
            break;
        }
    }
    
    // No hay espacio para colocar la combinación, probamos con combos
    if(!combinationPositioned)
    {
        CCLOG("No hay espacio en el tablero para colocar la combinación. Buscamos en los combos");
        mixBoardCellsForCombination(true);
        return false;
    }
    
    // 3. reordenamos al azar las otras figuras no censuradas para mezclar el tablero y mostramos tabla de madera
    mixBoardCellsRandom(posiblePositions, censoredPositions);
    TTAudioManager::getInstance()->playAudio("MIX_BOARD");
    return true;
}


void TTBoard::mixBoardCellsRandom(std::vector<std::vector<int> > posiblePositions,std::vector<std::vector<int> > censoredPositions)
{
    for(int h = 0; h < posiblePositions.size(); h++)
    {
        bool isCensoredPosition = false;
        for(int c = 0; c < censoredPositions.size(); c++)
        {
            if(posiblePositions[h][0] == censoredPositions[c][0] && posiblePositions[h][1] == censoredPositions[c][1])
                isCensoredPosition = true;
        }
        
        if(isCensoredPosition)
            continue;
        
        // buscamos una nueva posición para la casilla
        for(int b = h+1; b < posiblePositions.size(); b++)
        {
            bool isCensoredPosition = false;
            for(int c = 0; c < censoredPositions.size(); c++)
            {
                if(posiblePositions[b][0] == censoredPositions[c][0] && posiblePositions[b][1] == censoredPositions[c][1])
                    isCensoredPosition = true;
            }
            
            if(isCensoredPosition)
                continue;
            
            // switch de casillas
            TTCell* originCell = _Cells[posiblePositions[h][0]][posiblePositions[h][1]];
            TTCell* destinationCell = _Cells[posiblePositions[b][0]][posiblePositions[b][1]];
            originCell->switchCell(destinationCell);
            
            // censuramos ambas casillas
            censoredPositions.push_back(posiblePositions[h]);
            censoredPositions.push_back(posiblePositions[b]);
            break;
        }
    }
}

bool TTBoard::samePositionsInSquare(std::vector<int> cell, std::vector<std::vector<int> > mapPositions, std::vector<std::vector<int> > *positions, int minCombinationSearch)
{
    bool match = false;
    int column = cell[0];
    int row = cell[1];
    
    std::vector<std::vector<int> > _squarePositions;
    
    int minColumn = 0;
    if(column == 0) minColumn = column;
    else minColumn = column-1;
    
    int maxColumn = 0;
    if(column == _Cells.size()-1) maxColumn = column;
    else maxColumn = column+1;
    
    int minRow = 0;
    if(row == 0) minRow = row;
    else minRow = row-1;
    
    int maxRow = 0;
    if(row == _Cells[0].size()-1) maxRow = row;
    else maxRow = row+1;
    
    for(int t = minColumn; t <= maxColumn; t++)
    {
        for(int y = minRow; y <= maxRow; y++)
        {
            std::vector<int> position;
            position.push_back(t);
            position.push_back(y);
            _squarePositions.push_back(position);
        }
    }

    for(int i = 0; i < _squarePositions.size(); i++)
    {
        if(mapPositions[_squarePositions[i][0]][_squarePositions[i][1]] == 1)
        {
            //verificamos que la casilla no este ya incluida
            bool included = false;
            for(int j = 0; j < (*positions).size(); j++)
            {
                if((*positions)[j][0] == _squarePositions[i][0] && (*positions)[j][1] == _squarePositions[i][1])
                    included = true;
            }
            
            if(!included)
            {
                (*positions).push_back(_squarePositions[i]);
                match = true;
                
                if((*positions).size() >= minCombinationSearch)
                    return match;
            }
        }
    }
    
    return match;
}

void TTBoard::showBoard()
{
    // Mostramos el fondo
    _boardBg->setVisible(true);
    
    // Inicializacion de las celdas
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j < _Cells[i].size(); j++)
            _Cells[i][j]->initCell();
    }
    _parentBoard->setVisible(true);
    
    // Launch de las animaciones standBy
    launchAnimStandBy();
}

void TTBoard::consolidateBoard()
{
    // Se limpian las celdas excluidas
    _cellHoles.clear();
    _cellExcludeHoles.clear();
    
    // Consolidamos el tablero, eliminando los contenidos muertos de las celdas
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j <_Cells[i].size(); j++)
        {
            _Cells[i][j]->consolidateCell();
        }
    }
    
    // Printamos todos las celdas rellenables que han quedao
    for(int i = 0; i < _Cells.size(); i++)
    {
        for(int j = 0; j <_Cells[i].size(); j++)
        {
            if(_Cells[i][j]->isRefillableCell())
            {
                CCLOG("La celda %d %d esta libre para ser rellenada.",i,j);
            }
        }
    }
}

void TTBoard::onEvent_boardCellPositioned(EventCustom* e)
{
    _cellsToReposition--;
    if(_cellsToReposition == 0)
    {
        CCLOG("Todas las celdas reposicionadas, intentamos desbloquear tablero");
        if(_needToUpdateBoard)
        {
            updateBoardState();
        }
        else
        {
            unlockBoard();
            // Sonido de caída por pasos
            TTAudioManager::getInstance()->playAudio("DOWN_LINE_HOLES");

        }
    }
}

void TTBoard::onEvent_boardStepsFinished(EventCustom* e)
{
    if(_isBoardActive)
    {
        // Consolidamos el tablero
        consolidateBoard();
        
        // actualiza el tablero
        updateBoardState();
    
        // envia evento con la información de la jugada
        sendEvent(EVENT_GAME_FINISH_PLAY,&_finishInfo);
    }
}