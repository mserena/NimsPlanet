#include "TTCell.h"
#include "TTEventsManager.h"
#include "thegame.h"
#include "TTHowToInGameManager.h"
#include "TTAudioManager.h"

TTCell::TTCell(cocos2d::Layer* parentLayer, cocos2d::Node* parentBoard,std::string cellType, int posXCell, int posYCell,int sizeCell, int column, int row, int totalColumns, int totalRows):
_parentLayer(parentLayer),
_posXCell(posXCell),
_posYCell(posYCell),
_sizeCell(sizeCell),
_column(column),
_row(row),
_totalRows(totalRows),
_totalColumns(totalColumns),
_parentBoard(parentBoard)
{
    // Obtenemos el tipo de celda y todos sus layers
    unsigned first = cellType.find("[");
    unsigned last = cellType.find("]");
    
    std::string _cellTypeIDString = cellType.substr(0,first);
    _cellTypeID = static_cast<CELL_TYPE_ID>(atoi(cellType.substr(1,cellType.size()).c_str()));
    
    switch(_cellTypeID)
    {
        case CELL_EMPTY:
        {
            // No hay que cargar nada
        }
        break;
            
        case CELL_WITH_NO_CONTENT:
        {
            _cellContentLayers.clear();
            loadCellGraph();
            _cellTypeID = CELL_WITH_CONTENT;
        }
        break;
        
        case CELL_WITH_CONTENT:
        {
            std::string _cellLayersIDString = cellType.substr(first+1,last-first-1);
            std::vector<std::string> _cellLayersID = tokenizeString(_cellLayersIDString,"|");
            for(int i = 0; i < _cellLayersID.size(); i++)
            {
                std::string _layerDefinition = _cellLayersID[i];
                int zLayer = CELL_CONTENT_LAYER;
                if( i > 0) zLayer = _cellContentLayers.back()->getZContent();
                
                if(_layerDefinition.find("S") != -1)
                {
                    //Buscamos contenido estático
                    int _layerStaticTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("S")+1,_layerDefinition.find("S")+1).c_str());
                    _layerStaticTypeID += (NUM_DYNAMIC_CONTENTS+1);
                    CONTENT_TYPES _layerStaticType = static_cast<CONTENT_TYPES>(_layerStaticTypeID);
                    switch(_layerStaticType)
                    {
                        case MUD_STATIC_CONTENT:
                        {
                            int _layerStaticSubTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("T")+1,_layerDefinition.find("T")+1).c_str());
                            
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellStaticContentMud(_parentBoard,zLayer,MUD_STATIC_CONTENT,posXCell,posYCell,sizeCell,getCellId(),_layerStaticSubTypeID);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case WOOD_STATIC_CONTENT:
                        {
                            int _layerStaticSubTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("T")+1,_layerDefinition.find("T")+1).c_str());

                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellStaticContentWood(_parentBoard,zLayer,WOOD_STATIC_CONTENT,posXCell,posYCell,sizeCell,getCellId(),_layerStaticSubTypeID);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case STONE_STATIC_CONTENT:
                        {
                            int _layerStaticSubTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("T")+1,_layerDefinition.find("T")+1).c_str());
                            
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellStaticContentStone(_parentBoard,zLayer,STONE_STATIC_CONTENT,posXCell,posYCell,sizeCell,getCellId(),_layerStaticSubTypeID);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case NET_STATIC_CONTENT:
                        {
                            int _layerStaticSubTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("T")+1,_layerDefinition.find("T")+1).c_str());
                            
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellStaticContentNetwork(_parentBoard,zLayer,NET_STATIC_CONTENT,posXCell,posYCell,sizeCell,getCellId(),_layerStaticSubTypeID);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case GLUE_STATIC_CONTENT:
                        {
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellStaticContentGlue(_parentBoard,zLayer,GLUE_STATIC_CONTENT,posXCell,posYCell,sizeCell,getCellId());
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        default:
                        {
                            CCLOG("Se intenta crear un contenido estático desconocido, revisa la definición del XML.");
                            abort();
                        }
                        break;
                    }
                }
                else if(_layerDefinition.find("D") != -1)
                {
                    //Buscamos contenido dinámico
                    int _layerDynamicTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("D")+1,_layerDefinition.find("D")+1).c_str());
                    CONTENT_TYPES _layerDynamicType = static_cast<CONTENT_TYPES>(_layerDynamicTypeID);
                    switch(_layerDynamicType)
                    {
                        case FIGURE_DYNAMIC_CONTENT:
                        case COMBO_VERTICAL_DYNAMIC_CONTENT:
                        case COMBO_HORIZONTAL_DYNAMIC_CONTENT:
                        case COMBO_AREA_DYNAMIC_CONTENT:
                        {
                            int _layerDynamicSubTypeID = atoi(_layerDefinition.substr(_layerDefinition.find("T")+1,_layerDefinition.find("T")+1).c_str());
    
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellDynamicContentFigure(_parentBoard,zLayer,_layerDynamicType,posXCell,posYCell,_column,_row,_totalColumns,_totalRows,sizeCell,_layerDynamicSubTypeID);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case COMBO_COLOR_DYNAMIC_CONTENT:
                        {
                            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
                            TTCellContent* _layer = new TTCellDynamicContentFigureComboColor(_parentBoard,zLayer,posXCell,posYCell,_column,_row,_totalColumns,_totalRows,sizeCell);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        case DOWN_OBJECT_DYNAMIC_CONTENT:
                        {
                            TTCellContent* _layer = new TTCellDynamicContentObjectDown(_parentBoard,zLayer,(CONTENT_TYPES)DOWN_OBJECT_DYNAMIC_CONTENT,posXCell,posYCell,_column,_row,_totalColumns,_totalRows,sizeCell);
                            _cellContentLayers.push_back(_layer);
                        }
                        break;
                        default:
                        {
                            CCLOG("Se intenta crear un contenido dinámico desconocido, revisa la definición del XML.");
                            abort();
                        }
                        break;
                    }
                }
            }
            
            loadCellGraph();
        }
        break;
            
        case CELL_WITH_EXTERNAL_RESOURCE:
        {
            
        }
        break;
        default:
        {
            CCLOG("Se intenta crear celda de tipo desconocido, revisa la definición del XML.");
            abort();
        }
        break;
    }
    
    // Carga la veladura de la celda, universal para todas las celdas
    _cellTutorialGlaze = CCSprite::create("pixelBg.png");
    _cellTutorialGlaze->setScale(_sizeCell);
    _cellTutorialGlaze->setVisible(false);
    _cellTutorialGlaze->setPosition(Vec2(_posXCell,_posYCell));
    _parentBoard->addChild(_cellTutorialGlaze,LAYER_TUTORIAL_GLAZE);
}

TTCell::~TTCell()
{
    CCLOG("Se elimina la celda %d,%d",_column,_row);
}

void TTCell::loadCellGraph()
{
    // Cargamos el fondo de la casilla
    _cellBackground = CCSprite::createWithSpriteFrameName("background_casilla.png");
    _cellBackground->setPosition(Vec2(_posXCell,_posYCell));
    float newScale = ((BACKGROUND_SIZE_PERCENTAGE*_sizeCell)/100)/_cellBackground->getBoundingBox().size.width;
    _cellBackground->setScale(newScale,newScale);
    _cellBackground->setVisible(false);
    _parentBoard->addChild(_cellBackground,CELL_BACKGROUND_LAYER);
    
    _cellSelectedMark = Sprite::createWithSpriteFrameName("circuloseleccion.png");
    _cellSelectedMark->setPosition(Vec2(_posXCell,_posYCell));
    newScale = ((SELECTION_MARK_SIZE_PERCENTAGE*_sizeCell)/100)/_cellSelectedMark->getBoundingBox().size.width;
    _cellSelectedMark->setScale(newScale,newScale);
    _cellSelectedMark->setVisible(false);
    if(_cellContentLayers.size() > 0)
    {
        _parentBoard->addChild(_cellSelectedMark,_cellContentLayers.back()->getZContent()+CELL_MARK_LAYER);
    }
    else
    {
        _parentBoard->addChild(_cellSelectedMark,CELL_MARK_LAYER);
    }
    
    _cellPreSelectedMark = CCSprite::createWithSpriteFrameName("help.png");
    _cellPreSelectedMark->setPosition(Vec2(_posXCell,_posYCell));
    newScale = _sizeCell/_cellPreSelectedMark->getBoundingBox().size.width;
    _cellPreSelectedMark->setScale(newScale,newScale);
    _cellPreSelectedMark->setVisible(false);
    if(_cellContentLayers.size() > 0)
    {
        _parentBoard->addChild(_cellPreSelectedMark,_cellContentLayers.back()->getZContent()+CELL_PREMARK_LAYER);
    }
    else
    {
        _parentBoard->addChild(_cellPreSelectedMark,CELL_PREMARK_LAYER);
    }
}

void TTCell::initCell()
{
    // Inicializamos los contenidos de la celda
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        _cellContentLayers[i]->initCell();
    }
    
    if(!isEmptyCell())
    {
        _cellBackground->setVisible(true);
    }
}

bool TTCell::isUpdatingCell()
{
    bool _updatingCell = false;
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        if(_cellContentLayers[i]->_isUpdatingContent)
        {
            _updatingCell = true;
            break;
        }
    }
    return _updatingCell;
}

bool TTCell::isCellTouched(float x, float y, std::vector<CONTENT_TYPES> _cellsAllowed)
{
    if(_cellContentLayers.size() == 0) return false;
    
    // Verificación de celdas permitas
    if(_cellsAllowed.size() != 0)
    {
        bool _allowedCell = false;
        for(int i = 0; i < _cellsAllowed.size(); i++)
        {
            if(_cellContentLayers.back()->_contentLayerType == _cellsAllowed[i])
            {
                _allowedCell = true;
            }
        }
        
        if(!_allowedCell) return false;
    }
    else
    {
        if(!isSelectableCell())
        {
            return false;
        }
    }
    
    // Verificación de tutorial
    if(TTHowToInGameManager::getInstance()->_isTutorialActive)
    {
        bool _tutotialAllowed = false;
        std::vector<TTCell*> _cellsAllowedTutorial = TTHowToInGameManager::getInstance()->getActualAllowedCells();
        for(int i = 0; i < _cellsAllowedTutorial.size(); i++)
        {
            if(_cellsAllowedTutorial[i] == this)
            {
                _tutotialAllowed = true;
                break;
            }
        }
        
        if(!_tutotialAllowed)
        {
            return false;
        }
    }
    
    
    Vec2 _posCellInWorld ;
    _posCellInWorld = _parentBoard->convertToWorldSpace(Point(_posXCell,_posYCell));
    int minX = _posCellInWorld.x - (_sizeCell/2);
    int maxX = _posCellInWorld.x + (_sizeCell/2);
    int minY = _posCellInWorld.y - (_sizeCell/2);
    int maxY = _posCellInWorld.y + (_sizeCell/2);
    
    if(x >= minX && x <= maxX && y >= minY && y <= maxY)
    {
        if(!isEmptyCell())
            return true;
    }
    
    return false;
}

bool TTCell::isFirstSelectableCell()
{
    // Verificamos si la celda es seleccionable, mirando la layer inmediatamente superior
    bool isFirstSelectable = false;
    if(!isEmptyCell() && _cellContentLayers.size() > 0)
    {
        isFirstSelectable = _cellContentLayers.back()->isFirstSelectable();
    }
    return isFirstSelectable;
}

bool TTCell::isSelectableCell()
{
    // Verificamos si la celda es seleccionable, mirando la layer inmediatamente superior
    bool isSelectable = false;
    if(!isEmptyCell() && _cellContentLayers.size() > 0)
    {
        isSelectable = _cellContentLayers.back()->isSelectable();
    }
    return isSelectable;
}

bool TTCell::isComboCell()
{
    // Verificamos si en la capa superior hay un combo
    if(_cellContentLayers.size() > 0 && _cellContentLayers.back()->isComboContent())
    {
        return true;
    }
    return false;
}

bool TTCell::isComboColorCell()
{
    if(_cellContentLayers.size() > 0 && typeid(*_cellContentLayers.back()) == typeid(TTCellDynamicContentFigureComboColor))
    {
        return true;
    }
    return false;
}

bool TTCell::isDownObjectCell()
{
    if(_cellContentLayers.size() > 0 && typeid(*_cellContentLayers.back()) == typeid(TTCellDynamicContentObjectDown))
    {
        return true;
    }
    return false;
}

bool TTCell::isEmptyCell()
{
    // Verificamos si la celda es de tipo vacío
    if(_cellTypeID == CELL_EMPTY)
    {
        return true;
    }
    return false;
}

bool TTCell::isRefillableCell()
{
    // Verificamos si la celda puede ser rellenada, consultamos todas sus capas para ver si admiten un rellenado
    bool refillable = true;
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        if(!_cellContentLayers[i]->isRefillable())
        {
            refillable = false;
            break;
        }
    }
    
    return refillable;
}

bool TTCell::hasMovableContent()
{
    if(_cellContentLayers.size() > 0 && _cellContentLayers[_cellContentLayers.size()-1]->isMovableContent())
    {
        return true;
    }
    return false;
}

std::vector<CONTENT_TYPES> TTCell::getCellContentTypes()
{
    std::vector<CONTENT_TYPES> _allCellContentTypes;
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        _allCellContentTypes.push_back(_cellContentLayers[i]->_contentLayerType);
    }
    return _allCellContentTypes;
}

cocos2d::Vec2 TTCell::getCellId()
{
    return cocos2d::Vec2(_column,_row);
}


int TTCell::getCellIdCode()
{
    int _codeCell = _column*100+_row;
    return _codeCell;
}

bool TTCell::isAffectedFromAreaCombination()
{
    if(_cellContentLayers.size() > 0)
    {
        return _cellContentLayers[_cellContentLayers.size()-1]->_affectAreaCombination;
    }
    return false;
}

float TTCell::moveDownObjectOutOfBoard(TTCell* _cellDestination)
{
    if(isDownObjectCell())
    {
        TTCellDynamicContentObjectDown *_content = static_cast<TTCellDynamicContentObjectDown*>(_cellContentLayers.back());
        float _timeToGoOut = _content->moveDownObjectOutOfBoard(_cellDestination->_posYCell,_cellDestination->_row,_row,_totalRows);
        
        // Eliminamos el contenido
        _cellContentLayers.erase(_cellContentLayers.begin()+(_cellContentLayers.size()-1));
        delete(_content);
        _content = nullptr;
        
        return _timeToGoOut;
    }
    
    return 0;
}

void TTCell::moveCell(TTCell* _cellDestination)
{
    // Movemos el contenido dinamico de la celda eliminando el que ya no nos interesa
    TTCellContent *_contentToMove = _cellContentLayers.back();
    _cellContentLayers.erase(_cellContentLayers.begin()+(_cellContentLayers.size()-1));
    _cellDestination->_cellContentLayers.push_back(_contentToMove);
    
    // Seteamos el movimiento de la celda logico y visual
    TTCellDynamicContent *_cellDynamicContentMoved = static_cast<TTCellDynamicContent*>(_contentToMove);
    _cellDynamicContentMoved->_parentContent->setPosition(Point(_posXCell,_posYCell));
    _cellDynamicContentMoved->_posXCell = _cellDestination->_posXCell;
    _cellDynamicContentMoved->_posYCell = _cellDestination->_posYCell;
    if(_cellDestination->isDownObjectCell())
    {
        int moves = _cellDestination->_row-_row;
        (static_cast<TTCellDynamicContentObjectDown*>(_cellDestination->_cellContentLayers.back()))->moveCellAlert(moves);
        
        _cellDynamicContentMoved->_cellDynamicState = DYNAMIC_CONTENT_STATE_NOTHING;
    }
    else
    {
       _cellDynamicContentMoved->_cellDynamicState = DYNAMIC_CONTENT_STATE_FALLING;
    }
    _cellDynamicContentMoved->_row = _cellDestination->_row;
    _cellDynamicContentMoved->_column = _cellDestination->_column;
    
    // Creamos movimiento visual
    int _totalCellsMove = _cellDestination->_row-_row;
    float _fallTime = FALL_TIME/_totalRows;
    _fallTime *= _totalCellsMove;
    
    MoveTo* moveCell = MoveTo::create(_fallTime,Point(_cellDestination->_posXCell,_cellDestination->_posYCell));
    CallFunc *finishMove = CallFunc::create(CC_CALLBACK_0(TTCellDynamicContent::finishFallCell,_cellDynamicContentMoved));
    _cellDynamicContentMoved->_parentContent->runAction(Sequence::create(moveCell,finishMove,NULL));
    
    // Reordenamos las Z
    _cellDestination->_cellTypeID = CELL_WITH_CONTENT;
    _parentBoard->reorderChild(_cellDestination->_cellSelectedMark,_cellDestination->_cellContentLayers.back()->getZContent()+CELL_MARK_LAYER);
    _parentBoard->reorderChild(_cellDestination->_cellPreSelectedMark,_cellDestination->_cellContentLayers.back()->getZContent()+CELL_MARK_LAYER);
}

void TTCell::generateNewCell(CONTENT_TYPES newCellType, int _numNewCells, int _idNewCell ,std::vector<int> _emptyCellsMap)
{
    switch(newCellType)
    {
        case FIGURE_DYNAMIC_CONTENT:
        {
            // Creamos el contenido y lo agregamos al vector de contenidos de la celda
            TTCellContent* _layer = new TTCellDynamicContentFigure(_parentBoard,_cellContentLayers.size(),FIGURE_DYNAMIC_CONTENT,_posXCell,_posYCell,_column,_row,_totalColumns,_totalRows,_sizeCell,6);
            _cellContentLayers.push_back(_layer);
        }
        break;
        case DOWN_OBJECT_DYNAMIC_CONTENT:
        {
            TTCellContent* _layer = new TTCellDynamicContentObjectDown(_parentBoard,_cellContentLayers.size(),(CONTENT_TYPES)DOWN_OBJECT_DYNAMIC_CONTENT,_posXCell,_posYCell,_column,_row,_totalColumns,_totalRows,_sizeCell);
            _cellContentLayers.push_back(_layer);
        }
        break;
        default:
        {
            CCLOG("Se intenta generar un contenido dinámico desconocido.");
            abort();
        }
        break;
    }
    
    // calculamos celdas vacías superiores
    int _noMapCells = 0;
    int _emptyUpCells = 0;
    bool _inMap = false;
    for(int i = 0; i <= _row; i++)
    {
        if(_emptyCellsMap[i] == 0)
            _inMap = true;
        
        if(_emptyCellsMap[i] == 1)
        {
            if(!_inMap) _noMapCells++;
            else _emptyUpCells++;
        }
    }

    // Situamos la nueva celda
    int _posYBorn = _posYCell + (_sizeCell*(_numNewCells+_emptyUpCells));
    TTCellDynamicContent *_generatedContent = static_cast<TTCellDynamicContent*>(_cellContentLayers.back());
    _generatedContent->_parentContent->setPosition(_posXCell,_posYBorn);
    
    // Creamos movimiento de bajada
    float _fallTime = FALL_TIME/_totalRows;
    _fallTime *= (_numNewCells+_emptyUpCells);
    MoveTo* moveCell = MoveTo::create(_fallTime,Point(_posXCell,_posYCell));
    CallFunc *finishMove = CallFunc::create(CC_CALLBACK_0(TTCellDynamicContent::finishFallCell,_generatedContent));
    _generatedContent->_parentContent->runAction(Sequence::create(moveCell,finishMove,NULL));
    
    // Creamos accion de entrada
    float _timeMoveCell = _fallTime/_numNewCells;
    float _timeDelayEnterScene = _timeMoveCell*_idNewCell;
    DelayTime *delayEnterOnScene = DelayTime::create(_timeDelayEnterScene);
    CallFunc *enterOnScene = CallFunc::create(std::bind([&](){
        TTCellDynamicContent *_generatedContent = static_cast<TTCellDynamicContent*>(_cellContentLayers.back());
        _generatedContent->_cellFigureSprite->setVisible(true);
    }));
    _generatedContent->_parentContent->runAction(Sequence::create(delayEnterOnScene,enterOnScene,NULL));
    
    
    // Seteamos el estado de animación y alertamos del movimiento
    if(isDownObjectCell())
    {
        _generatedContent->_cellDynamicState = DYNAMIC_CONTENT_STATE_NOTHING;
    }
    else
    {
        _generatedContent->_cellDynamicState = DYNAMIC_CONTENT_STATE_FALLING;
    }
    
    // Reordenamos las Z
    _cellTypeID = CELL_WITH_CONTENT;
    _parentBoard->reorderChild(_cellSelectedMark,_cellContentLayers.back()->getZContent()+CELL_MARK_LAYER);
    _parentBoard->reorderChild(_cellPreSelectedMark,_cellContentLayers.back()->getZContent()+CELL_MARK_LAYER);
}

void TTCell::convertCellToComboColor()
{
    // Eliminamos el contenido actual de figura y creamos uno nuevo de combo de color
    TTCellContent* _content = _cellContentLayers.back();
    _cellContentLayers.erase(_cellContentLayers.begin()+(_cellContentLayers.size()-1));
    delete(_content);
    _content = nullptr;
    
    TTCellContent* _layer = new TTCellDynamicContentFigureComboColor(_parentBoard,CELL_CONTENT_LAYER,_posXCell,_posYCell,_column,_row,_totalColumns,_totalRows,_sizeCell);
    _cellContentLayers.push_back(_layer);
    _layer->initCell();
}

void TTCell::convertCellToCombo(CONTENT_TYPES _newComboType)
{
    if(_cellContentLayers.size() > 0 && typeid(*_cellContentLayers.back()) == typeid(TTCellDynamicContentFigure))
    {
        (static_cast<TTCellDynamicContentFigure*>(_cellContentLayers.back()))->convertCellToCombo(_newComboType);
    }
}

void TTCell::switchCell(TTCell* _cellDestination)
{
    // Intercambiamos los contenidos de ambas celdas
    TTCellDynamicContentFigure* _contentCell = static_cast<TTCellDynamicContentFigure*>(_cellContentLayers.back());
    TTCellDynamicContentFigure* _contentCellDestination = static_cast<TTCellDynamicContentFigure*>(_cellDestination->_cellContentLayers.back());
    
    _cellContentLayers.erase(_cellContentLayers.begin()+(_cellContentLayers.size()-1));
    _cellDestination->_cellContentLayers.erase(_cellDestination->_cellContentLayers.begin()+(_cellDestination->_cellContentLayers.size()-1));
    
    _cellContentLayers.push_back(_contentCellDestination);
    _cellDestination->_cellContentLayers.push_back(_contentCell);
    
    _contentCell->_posXCell = _contentCellDestination->_posXCell;
    _contentCell->_posYCell = _contentCellDestination->_posYCell;
    _contentCell->_row = _contentCellDestination->_row;
    _contentCell->_column = _contentCellDestination->_column;
    
    _contentCellDestination->_posXCell = _posXCell;
    _contentCellDestination->_posYCell = _posYCell;
    _contentCellDestination->_row = _row;
    _contentCellDestination->_column = _column;
    
    MoveTo* moveAction = MoveTo::create(MOVE_MIX_TIME,Point(_contentCell->_posXCell,_contentCell->_posYCell));
    _contentCell->_parentContent->runAction(moveAction);
    
    MoveTo* moveActionDestination = MoveTo::create(MOVE_MIX_TIME,Point(_contentCellDestination->_posXCell,_contentCellDestination->_posYCell));
    _contentCellDestination->_parentContent->runAction(moveActionDestination);
    
}

void TTCell::prepareToAbduction(Point _abductionPoint)
{ 
    if(_cellContentLayers.size() > 0 && _cellContentLayers.back()->isSelectable())
    {
        // Hacemos un hit a los contenidos por debajo de la capa que va a ser abducida
        int _hits = 1;
        for(int i = _cellContentLayers.size()-2; i >= 0; i--)
        {
            _hits = _cellContentLayers[i]->hitCell(1);
            if(_hits <= 0) break;
        }
        
        static_cast<TTCellDynamicContent*>(_cellContentLayers.back())->prepareToAbduction(_abductionPoint);
    }
}

void TTCell::killCell()
{
    // Reseteamos los marcadores de celda
    unselectCell();
    unPreSelectCell();
    
    /* Lanzamos hits hasta eliminar el contenido
     */
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        _cellContentLayers[i]->hitCell(1000);
    }
}

void TTCell::hitCell(int _numHits)
{
    // Reseteamos los marcadores de celda
    unselectCell();
    unPreSelectCell();
    
    /* Repartimos los hit entre las celdas de la casilla
     * Tenemos que tener en cuenta los hits que sobran después de aplicar a un contenido,
     * los stopper ya estan incluidos en el recuento de hits sobrantes.
     */
    int _hits = _numHits;
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        if(!_cellContentLayers[i]->_contentIsBlocked)
        {
            _hits = _cellContentLayers[i]->hitCell(_numHits);
            if(_hits <= 0) break;
        }
    }
}

int TTCell::getCellFigure()
{
    // Buscamos el tipo de figura de la capa más inmediatamente superior
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        if(!_cellContentLayers[i]->isSelectable())
        {
            break;
        }
        
        
        //CCLOG("Tipo de variable %s",typeid(*_cellContentLayers[i]).name());
        if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigure))
        {
            return static_cast<TTCellDynamicContentFigure*>(_cellContentLayers[i])->_cellFigure;
        }
        else if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigureComboColor))
        {
            return static_cast<TTCellDynamicContentFigureComboColor*>(_cellContentLayers[i])->_cellFigure;
        }
    }
    return -1;
}

void TTCell::preSelectCell()
{
    if(!_cellPreSelectedMark->isVisible())
    {
        _cellPreSelectedMark->setVisible(true);
    }
}

void TTCell::unPreSelectCell()
{
    if(_cellPreSelectedMark->isVisible())
    {
        _cellPreSelectedMark->setVisible(false);
    }
}

void TTCell::selectCell()
{
    // Avisa el contenido que ha sido seleccionado por el usuario
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        _cellContentLayers[i]->selectedCell();
    }
    
    // Selección gráfica
    if(!_cellSelectedMark->isVisible())
    {
        _cellSelectedMark->setVisible(true);
    }
}

void TTCell::unselectCell()
{
    // Avisa el contenido que ha sido deseleccionado por el usuario
    for(int i = 0; i < _cellContentLayers.size(); i++)
    {
        _cellContentLayers[i]->unselectedCell();
    }
    
    // DesSelección gráfica
    if(_cellSelectedMark->isVisible())
    {
        _cellSelectedMark->setVisible(false);
    }
}

void TTCell::consolidateCell()
{
    if(isEmptyCell()) return;
    
    // Eliminamos las capas muertas de la celda
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        if(_cellContentLayers[i]->_contentIsBlocked)
        {
            _cellContentLayers[i]->_contentIsBlocked = false;
        }
        
        if(_cellContentLayers[i]->_contentIsDeath)
        {
            TTCellContent* _content = _cellContentLayers[i];
            _cellContentLayers.erase(_cellContentLayers.begin()+i);
            delete(_content);
            _content = nullptr;
            
            CCLOG("Se elimina contenido de la celda %d %d, ahora tiene %d contenidos",_column,_row,_cellContentLayers.size());
        }
    }
}

void TTCell::showPathCell(CELL_PATHS pathDirection)
{
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        //CCLOG("Tipo de variable %s",typeid(*_cellContentLayers[i]).name());
        if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigure))
        {
            static_cast<TTCellDynamicContentFigure*>(_cellContentLayers[i])->showPathCell(pathDirection);
            return;
        }
        else if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigureComboColor))
        {
            static_cast<TTCellDynamicContentFigureComboColor*>(_cellContentLayers[i])->showPathCell(pathDirection);
            return;
        }
    }
}

void TTCell::hidePathCell()
{
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        //CCLOG("Tipo de variable %s",typeid(*_cellContentLayers[i]).name());
        if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigure))
        {
            static_cast<TTCellDynamicContentFigure*>(_cellContentLayers[i])->hidePathCell();
        }
        else if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigureComboColor))
        {
            static_cast<TTCellDynamicContentFigureComboColor*>(_cellContentLayers[i])->hidePathCell();
        }
    }
}

bool TTCell::isWallCell()
{
    // Verificamos si la celda actua como bloque, impidiendo el paso
    if(_cellContentLayers.size() > 0 && _cellContentLayers[_cellContentLayers.size()-1]->isWallContent())
    {
        return true;
    }
    return false;
}

bool TTCell::isStrongWallCell()
{
    // Verificamos si la celda actua como bloque, impidiendo el paso
    if(_cellContentLayers.size() > 0 && _cellContentLayers[_cellContentLayers.size()-1]->isStrongWallContent())
    {
        return true;
    }
    return false;
}

void TTCell::launchCellAnimationStandBy()
{
    if(_cellContentLayers.size() > 0 && (typeid(*_cellContentLayers.back()) == typeid(TTCellDynamicContentFigure) || (typeid(*_cellContentLayers.back()) == typeid(TTCellDynamicContentFigureComboColor))))
    {
        TTCellDynamicContent* _cellToAnimate = static_cast<TTCellDynamicContent*>(_cellContentLayers.back());
        if(_cellToAnimate->_cellDynamicState == DYNAMIC_CONTENT_STATE_STANDBY)
        {
            _cellToAnimate->launchContentAnimation();
        }
    }
}

void TTCell::playSelectionSound(int _numSelection)
{
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        //CCLOG("Tipo de variable %s",typeid(*_cellContentLayers[i]).name());
        if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigure))
        {
            static_cast<TTCellDynamicContentFigure*>(_cellContentLayers[i])->playSelectSound(_numSelection);
            return;
        }
        else if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigureComboColor))
        {
            static_cast<TTCellDynamicContentFigureComboColor*>(_cellContentLayers[i])->playSelectSound(_numSelection);
            return;
        }
    }
}

void TTCell::playUnSelectionSound(int _numSelection)
{
    for(int i = _cellContentLayers.size()-1; i >= 0; i--)
    {
        //CCLOG("Tipo de variable %s",typeid(*_cellContentLayers[i]).name());
        if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigure))
        {
            static_cast<TTCellDynamicContentFigure*>(_cellContentLayers[i])->playUnSelectSound(_numSelection);
            return;
        }
        else if(typeid(*_cellContentLayers[i]) == typeid(TTCellDynamicContentFigureComboColor))
        {
            static_cast<TTCellDynamicContentFigureComboColor*>(_cellContentLayers[i])->playUnSelectSound(_numSelection);
            return;
        }
    }
}
