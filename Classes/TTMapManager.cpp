#include "TTMapManager.h"
#include "TTMapParser.h"
#include "TTLevelParser.h"
#include "TTGameMenus.h"
#include "TTUIMap.h"
#include "TTConfigParser.h"
#include "TTUserInfo.h"
#include "TTAudioManager.h"
#include "TTFacebook.h"
#include "TTUIStackManager.h"
#include "TTParseManager.h"
#include "TTMapAnimationJump.h"
#include "TTUIPopup.h"
#include "TTMapStopperManager.h"

using namespace screw::facebook;

TTMapManager::TTMapManager(cocos2d::Layer* parentLayer):
_parentLayer(parentLayer),
_blockDownDisp(false),
_blockUpDisp(false),
_onMove(false),
_pauseMap(false),
_isWalking(false),
_waitForUpdateMap(false),
_totalDisp(0),
_timeDisp(0),
_meUserMap(nullptr),
_actualLevelBtnSelected(0)
{
    _mapLayer = Layer::create();
    _mapNode = Node::create();
    _mapNode->setPosition(Vec2(0,0));
    _mapLayer->addChild(_mapNode);
    _parentLayer->addChild(_mapLayer,MAP_LAYER);
    loadMapPieces();
    
    _classListeners.push_back(registerListenerEvent(EVENT_MAP_RESTORE,CC_CALLBACK_1(TTMapManager::onEvent_ReturnToMap,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_MAP_PAUSE,CC_CALLBACK_1(TTMapManager::onEvent_PauseMap,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_MAP_ANIM_LOADED,CC_CALLBACK_1(TTMapManager::onEvent_AnimMapLoaded,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_USERS_INFO_UPDATED,CC_CALLBACK_1(TTMapManager::onEvent_userFriendsUpdated,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_LANGUAGE_CHANGE,CC_CALLBACK_1(TTMapManager::onEvent_ChangeLanguage,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_USER_INFO_UPDATED,CC_CALLBACK_1(TTMapManager::onEvent_userInfoUpdated,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_MAP_LEVEL_UNLOCK,CC_CALLBACK_1(TTMapManager::onEvent_unlockActualLevel,this)));
    
    
    //Hacemos update de las Requests
    if(TTFacebook::getInstance()->isConnected())
    {
        TTFacebook::getInstance()->updateAppRequests();
    }
}

TTMapManager::~TTMapManager()
{
    // Paramos el update de multiamigos
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapManager::updateUsersMultiPhotos),this);
    
    // Quitamos la música el mapa
    TTAudioManager::getInstance()->stopAudio("MAP_MUSIC");
    
    //Ocultamos la UI del mapa
    TTUIMap::getInstance()->hideUIMap();
    
    // Se elimina la configuración del nivel
    delete(_gameBasicConfig);

    // Elimina las texturas
    char strMapAssetsPng[100] = {0};
    sprintf(strMapAssetsPng,"%s.png",MAP_ASSETS_FILE);
    char strMapAssetsPlist[100] = {0};
    sprintf(strMapAssetsPlist,"%s.plist",MAP_ASSETS_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(strMapAssetsPng);
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(strMapAssetsPlist);
    
    int numMapPieces = TTMapParser::getInstance()->_mapInfo->_mapPieces;
    for(int i = 0; i < numMapPieces; i++)
    {
        char strPieceName[100] = {0};
        sprintf(strPieceName,"%s%d%s",TTMapParser::getInstance()->_mapInfo->_mapPrefix.c_str(),i+1,TTMapParser::getInstance()->_mapInfo->_mapSuffix.c_str());
        Director::getInstance()->getTextureCache()->removeTextureForKey(strPieceName);
        _mapPieces[i]->removeFromParentAndCleanup(true);
    }
    _mapPieces.clear();
    
    // Elimina las animaciones
    for(int i = 0; i < _mapAnimations.size(); i++)
        delete(_mapAnimations[i]);
    _mapAnimations.clear();
    
    // Desregistra eventos
    removeSafeListenerEvent(_classListeners);
    
    //Eliminamos la UI del mapa
    TTUIMap::destroyInstance();
}

void TTMapManager::update(float dt)
{
    if(_onMove)
        _timeDisp += dt;
}

void TTMapManager::loadMapPieces()
{
    int numMapPieces = TTMapParser::getInstance()->_mapInfo->_mapPieces;
    for(int i = 0; i < numMapPieces; i++)
        _mapPieces.push_back(NULL);
        
    // Elementos a cargar = Piezas del mapa + Piezas de las nubes + assets
    _totalResourcesToLoad = numMapPieces;
    
    // Cargamos las piezas del mapa
    for(int i = 0; i < numMapPieces; i++)
    {
        char strPieceName[100] = {0};
        sprintf(strPieceName,"%s%d%s",TTMapParser::getInstance()->_mapInfo->_mapPrefix.c_str(),i+1,TTMapParser::getInstance()->_mapInfo->_mapSuffix.c_str());
        std::function<void(Texture2D*)> _funcMapPieceLoaded = std::bind(&TTMapManager::resourceMapPieceLoaded,this,std::placeholders::_1,i);
        Director::getInstance()->getTextureCache()->addImageAsync(strPieceName,_funcMapPieceLoaded);
    }
}

void TTMapManager::loadMapAnimations()
{
    int numMapAnimations = TTMapParser::getInstance()->_mapInfo->_mapAnimations.size();
    _totalResourcesToLoad += numMapAnimations;
    
    // Cargamos las animaciones del mapa
    for(int j = 0; j < numMapAnimations; j++)
    {
        //Posiciones según resolucion
        Size _designResolution = TTMapParser::getInstance()->_mapInfo->_mapDesignResolution;
        std::vector<int> _actualPosition = TTMapParser::getInstance()->_mapInfo->_mapAnimations[j]._animPosition;
        float _newXPosition = (_mapSize.width*_actualPosition[0])/_designResolution.width;
        float _newYPosition = (_mapSize.height*_actualPosition[1])/_designResolution.height;
        std::vector<int> _newPosition;
        _newPosition.push_back(_newXPosition);
        _newPosition.push_back(_newYPosition);
        
        mapAnimation _animMap = TTMapParser::getInstance()->_mapInfo->_mapAnimations[j];
        _animMap._animPosition = _newPosition;
        
        switch(TTMapParser::getInstance()->_mapInfo->_mapAnimations[j]._animType)
        {
            case MAP_TYPE_ANIM_ROTATION:
            {
                _mapAnimations.push_back(new TTMapAnimationRotation(_mapNode,_animMap,_mapScale));
            }
            break;
            case MAP_TYPE_ANIM_ANIMATOR:
            {
                _mapAnimations.push_back(new TTMapAnimationAnimator(_mapNode,_animMap,_mapScale));
            }
            break;
            case MAP_TYPE_ANIM_STATIC_SPRITE:
            {
                _mapAnimations.push_back(new TTMapAnimationStaticSprite(_mapNode,_animMap,_mapScale));
            }
            break;
            case MAP_TYPE_ANIM_PARTICLES:
            {
                _mapAnimations.push_back(new TTMapAnimationParticles(_mapNode,_animMap,_mapScale));
            }
            break;
            case MAP_TYPE_ANIM_JUMP:
            {
                _mapAnimations.push_back(new TTMapAnimationJump(_mapNode,_animMap,_mapScale));
            }
                break;
            default:
                CCLOG("Se intenta crear animación de mapa no definida.");
                abort();
            break;
        }
    }
}

void TTMapManager::loadMapAssets()
{
    _totalResourcesToLoad++;
    
    // Cargamos los assets del mapa
    char strMapAssets[100] = {0};
    sprintf(strMapAssets,"%s.png",MAP_ASSETS_FILE);
    Director::getInstance()->getTextureCache()->addImageAsync(strMapAssets, CC_CALLBACK_1(TTMapManager::assetsMapLoaded, this));
}

void TTMapManager::assetsMapLoaded(Texture2D* texture)
{
    char strMapAssetsPng[100] = {0};
    sprintf(strMapAssetsPng,"%s.png",MAP_ASSETS_FILE);
    char strMapAssetsPlist[100] = {0};
    sprintf(strMapAssetsPlist,"%s.plist",MAP_ASSETS_FILE);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(strMapAssetsPlist,strMapAssetsPng);
    
    controlResoucesLoaded();
}

void TTMapManager::resourceMapPieceLoaded(Texture2D* texture, int _mapId)
{
    Sprite* _piece = Sprite::createWithTexture(texture);
    _piece->setVisible(false);
    
    _mapSize = _piece->getBoundingBox().size;
    _mapSize.height *= _mapPieces.size();
    
    // Escalado y posicionado de la piezas del mapa
    Size _screenSize = Director::getInstance()->getWinSize();
    _mapScale = _screenSize.width/_piece->getBoundingBox().size.width;
    _piece->setScale(_mapScale);
    
    float _posYPiece = (_piece->getBoundingBox().size.height*_mapId)+(_piece->getBoundingBox().size.height/2);
    _piece->setPosition(Vec2(_screenSize.width/2,_posYPiece));
    
    _mapNode->addChild(_piece,INMAP_PIECES_LAYER);
    _mapPieces[_mapId] = _piece;
    
    _totalResourcesToLoad--;
    if(_totalResourcesToLoad == 0)
    {
        loadMapAssets();
        loadMapAnimations();
    }
}

void TTMapManager::controlResoucesLoaded()
{
    _totalResourcesToLoad--;
    if(_totalResourcesToLoad == 0)
    {
        // Cargamos la UI del mapa
        TTUIMap::getInstance();
        CCLOG("Todos los recursos del mapa cargados.");
        TTAudioManager::getInstance()->playAudio("MAP_MUSIC",true);
        createMapCells();
        createUsersUI();
        setVisibleMap();
        TTUIMap::getInstance()->updateUIValues();
        sendEvent(EVENT_MAP_LOADED);
    }
}

void TTMapManager::createUsersUI()
{
    // Usuario principal
    _meUserMap = new userMap();
    _meUserMap->_parent = Node::create();
    _meUserMap->_parent->setScale(_mapScale);
    _mapNode->addChild(_meUserMap->_parent,INMAP_USERS_LAYER);
    
    _meUserMap->_frame = Sprite::createWithSpriteFrameName("facebook_me.png");
    _meUserMap->_parent->addChild(_meUserMap->_frame,1);

    
    if(TTFacebook::getInstance()->isConnected())
    {
        Texture2D* _meTexture;
        _meTexture =  TTFacebook::getInstance()->getTexturePhotoMe();
        _meUserMap->_photo = Sprite::createWithTexture(_meTexture);
    }
    else
    {
        _meUserMap->_photo = Sprite::create("fb-default.png");
    }
    
    float _scaleUserPhoto = (_meUserMap->_frame->getContentSize().width*0.75)/_meUserMap->_photo->getContentSize().width;
    _meUserMap->_photo->setScale(_scaleUserPhoto);
    _meUserMap->_parent->addChild(_meUserMap->_photo,0);
    
    int _levelToShow = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
    if(TTUserInfo::getInstance()->_userDataVolatile.newLevelOpen && _levelToShow > 0)
    {
        _levelToShow--;
    }
    _meUserMap->_parent->setPosition(Point(_mapCells[_levelToShow]->_parentCell->getPosition().x,_mapCells[_levelToShow]->_parentCell->getPosition().y+((_mapCells[_levelToShow]->_cellOn->getBoundingBox().size.height*1.75)*_mapScale)));
    
    // Amigos
    std::map<std::string,int> friendsHighLevelInfo = TTFacebook::getInstance()->_friendsHighLevelInfo;
    if(TTFacebook::getInstance()->isConnected() && friendsHighLevelInfo.size() > 0)
    {
        
        std::map<std::string,int>::iterator _itrFriendsInfo;
        for(_itrFriendsInfo = friendsHighLevelInfo.begin(); _itrFriendsInfo != friendsHighLevelInfo.end(); ++_itrFriendsInfo)
        {
            std::string _actualIdUser = (*_itrFriendsInfo).first;
            int _levelToShow = (*_itrFriendsInfo).second;
            if(_levelToShow == -1) _levelToShow = 0;
            
            // Verificamos todos los niveles de los amigos
            if(_multiFriendsInfo.find(_levelToShow) != _multiFriendsInfo.end())
            {
                _multiFriendsInfo.find(_levelToShow)->second.push_back(_actualIdUser);
                continue;
            }
            else
            {
                std::vector<string> _userIds;
                _userIds.push_back(_actualIdUser);
                _multiFriendsInfo.insert(std::make_pair(_levelToShow,_userIds));
            }
            
            userMap* _userMap = new userMap();
            _userMap->_parent = Node::create();
            _userMap->_parent->setScale(_mapScale);
            _mapNode->addChild(_userMap->_parent,INMAP_FRIENDS_LAYER);
            
            _userMap->_frame = Sprite::createWithSpriteFrameName("facebook_friend.png");
            _userMap->_parent->addChild(_userMap->_frame,1);
            
            Texture2D* _meTexture =  TTFacebook::getInstance()->getTexturePhotoUser(_actualIdUser);
            _userMap->_photo = Sprite::createWithTexture(_meTexture);
            float _scaleUserPhoto = (_userMap->_frame->getContentSize().width*0.75)/_userMap->_photo->getContentSize().width;
            _userMap->_photo->setScale(_scaleUserPhoto);
            _userMap->_parent->addChild(_userMap->_photo,0);
            
            if(_levelToShow == TTUserInfo::getInstance()->getActualUserProgress()._actualLevel)
            {
                _userMap->_parent->setPosition(Point(_mapCells[_levelToShow]->_parentCell->getPosition().x,_mapCells[_levelToShow]->_parentCell->getPosition().y+((_mapCells[_levelToShow]->_cellOn->getBoundingBox().size.height*2.25)*_mapScale)));
            }
            else
            {
                _userMap->_parent->setPosition(Point(_mapCells[_levelToShow]->_parentCell->getPosition().x,_mapCells[_levelToShow]->_parentCell->getPosition().y+((_mapCells[_levelToShow]->_cellOn->getBoundingBox().size.height*1.75)*_mapScale)));
            }
            
            _actualFriendsUsersMap.insert(std::make_pair(_levelToShow,_userMap));
        }

        // Lanzamos update de las multiposiciones
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapManager::updateUsersMultiPhotos),this);
        Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTMapManager::updateUsersMultiPhotos),this,2.0f,false);
    }
}

void TTMapManager::updateUsersMultiPhotos(float dt)
{
    typedef std::map<int,std::vector<std::string>>::iterator itr;
    for(itr iterator = _multiFriendsInfo.begin(); iterator != _multiFriendsInfo.end(); iterator++)
    {
        if(iterator->second.size() > 1)
        {
            // Elegimos un nuevo usuario random que mostrar
            int randUser = rand()%iterator->second.size();
            if(_actualFriendsUsersMap.find(iterator->first) != _actualFriendsUsersMap.end())
            {
                userMap* _user = _actualFriendsUsersMap.find(iterator->first)->second;
                _user->_photo->setTexture(TTFacebook::getInstance()->getTexturePhotoUser(iterator->second[randUser]));
            }
        }
    }
}

void TTMapManager::createMapCells()
{
    std::vector<int> _stoppers = TTMapParser::getInstance()->getActiveMapStoppers();
    std::vector<std::vector<int> > _cellsPositions = TTMapParser::getInstance()->_mapInfo->_cellsPositions;
    
    // Aplicamos la escala a las posiciones
    Size _designResolution = TTMapParser::getInstance()->_mapInfo->_mapDesignResolution;
    for(int i = 0; i < _cellsPositions.size(); i++)
    {
        float _newXPosition = (_mapSize.width*_cellsPositions[i][0])/_designResolution.width;
        float _newYPosition = (_mapSize.height*_cellsPositions[i][1])/_designResolution.height;
        _cellsPositions[i][0] = _newXPosition;
        _cellsPositions[i][1] = _newYPosition;
    }
    
    // Creamos todas las celdas
    for(int i = 0; i < _cellsPositions.size(); i++)
    {
        mapCell *cell = new mapCell();
        
        // Creamos el nodo padre
        cell->_parentCell = Node::create();
        cell->_parentCell->setAnchorPoint(Vec2(0.5,0.5));
        
        // Cargamos las casillas
        cell->_cellOn = Sprite::createWithSpriteFrameName("nivel_on.png");
        cell->_parentCell->addChild(cell->_cellOn,CELL_LAYER);
        cell->_cellOn->setVisible(false);
        cell->_cellOff = Sprite::createWithSpriteFrameName("nivel_off.png");
        cell->_cellOff->setVisible(true);
        cell->_parentCell->addChild(cell->_cellOff,CELL_LAYER);
        
        // Cargamos los gráficos de las estrellas
        cell->_stars.push_back(Sprite::createWithSpriteFrameName("stars_1.png"));
        cell->_stars.push_back(Sprite::createWithSpriteFrameName("stars_2.png"));
        cell->_stars.push_back(Sprite::createWithSpriteFrameName("stars_3.png"));
        for(int i = 0; i < cell->_stars.size(); i++)
        {
            cell->_parentCell->addChild(cell->_stars[i],STARS_LAYER);
            cell->_stars[i]->setPosition(Point(cell->_stars[i]->getPosition().x,cell->_stars[i]->getPosition().y+(cell->_cellOn->getBoundingBox().size.height*0.65)));
            cell->_stars[i]->setVisible(false);
        }
        
        // Cargamos la fuente
        char strLevel[100] = {0};
        sprintf(strLevel,"%d",i+1);
        cell->_fntLevel = LabelBMFont::create(strLevel,"levelFnt.fnt");
        cell->_fntLevel->setAlignment(cocos2d::TextHAlignment::CENTER);
        cell->_fntLevel->setAnchorPoint(Vec2(0.5,0.5));
        cell->_fntLevel->setPosition(Point(5,-cell->_cellOn->getBoundingBox().size.height/2));
        cell->_fntLevel->setScale((_mapSize.width*0.75)/TTMapParser::getInstance()->_mapInfo->_mapDesignResolution.width);
        cell->_fntLevel->setVisible(false);
        cell->_parentCell->addChild(cell->_fntLevel,NUM_LAYER);
        
        // Miramos si la casilla tiene stopper
        for(int u = 0; u < _stoppers.size(); u++)
        {
            if(i == _stoppers[u])
            {
                //La casilla tiene stopper
                cell->_hasStopper = true;
                cell->_stopperOn = Sprite::createWithSpriteFrameName("stopper_map.png");
                cell->_stopperOn->setVisible(false);
                cell->_stopperOff = Sprite::createWithSpriteFrameName("stopper_map_gris.png");
                cell->_stopperOff->setVisible(true);
                cell->_parentCell->addChild(cell->_stopperOn,STOPPER_LAYER);
                cell->_parentCell->addChild(cell->_stopperOff,STOPPER_LAYER);
                break;
            }
        }
        
        // Escalado y posicionamiento
        cell->_parentCell->setScale(_mapScale);
        cell->_parentCell->setPosition(Point(_mapScale*_cellsPositions[i][0],_mapScale*_cellsPositions[i][1]));
        _mapNode->addChild(cell->_parentCell,INMAP_CELLS_LAYER);
        
        _mapCells.push_back(cell);
    }
}

bool TTMapManager::verifyButtons(Vec2 point, EventTouch::EventCode _typeTouch)
{
    Vec2 pointNode = _mapNode->convertToNodeSpace(point);
    
    // Calculamos el tamaño de una celda de mapa
    Size _sizeCell = Size(_mapScale*_mapCells[0]->_cellOn->getBoundingBox().size.width,_mapScale*_mapCells[0]->_cellOn->getBoundingBox().size.height);
    
    switch(_typeTouch)
    {
        case EventTouch::EventCode::BEGAN:
        {
            // Buscamos si hay pulsación/despulsación de botones.
            for(int i = 0; i < _mapCells.size(); i++)
            {
                if(!_mapCells[i]->_cellOn->isVisible())
                    continue;
                
                cocos2d::Rect _cellRect;
                _cellRect.origin = _mapCells[i]->_parentCell->getBoundingBox().origin;
                _cellRect.origin.x -= _sizeCell.width/2;
                _cellRect.origin.y -= _sizeCell.height/2;
                _cellRect.size = _sizeCell;
                
                if(_cellRect.containsPoint(pointNode))
                {
                    CCLOG("botón nivel %d pulsado",i);
                    _actualLevelBtnSelected = i+1;
                    ScaleTo* _scaleButtonBig = ScaleTo::create(0.1f,_mapScale*1.2f);
                    _mapCells[i]->_parentCell->runAction(_scaleButtonBig);
                    break;
                }
            }
            return false;
        }
        break;
            
        case EventTouch::EventCode::MOVED:
        {
            if(_actualLevelBtnSelected != 0)
            {
                cocos2d::Rect _cellRect;
                _cellRect.origin = _mapCells[_actualLevelBtnSelected-1]->_parentCell->getBoundingBox().origin;
                _cellRect.origin.x -= _sizeCell.width/2;
                _cellRect.origin.y -= _sizeCell.height/2;
                _cellRect.size = _sizeCell;
                
                if(!_cellRect.containsPoint(pointNode))
                {
                    // El botón ya no esta pulsado
                    ScaleTo* _scaleButtonNormal = ScaleTo::create(0.1f,_mapScale);
                    _mapCells[_actualLevelBtnSelected-1]->_parentCell->runAction(_scaleButtonNormal);
                    _actualLevelBtnSelected = 0;
                }
            }
            
            return false;
        }
        break;
            
        case EventTouch::EventCode::ENDED:
        {
            if(_actualLevelBtnSelected != 0)
            {
                // Animación de pulsado de botón y mostrar menú inicio juego
                TTAudioManager::getInstance()->playAudio("BUTTON_SELECT_STAGE");
                ScaleTo* _scaleButtonNormal = ScaleTo::create(0.1f,_mapScale);
                CallFunc* _showInitGameMenu = CallFunc::create(std::bind([&](){
                    verifyActualLevelToInit(_actualLevelBtnSelected-1);
                }));
                _mapCells[_actualLevelBtnSelected-1]->_parentCell->runAction(Sequence::create(_scaleButtonNormal,_showInitGameMenu,NULL));
            
                _pauseMap = true;
                return true;
            }
            
            return false;
        }
        break;
            
        default:
            return false;
        break;
    }
}

void TTMapManager::setVisibleMap()
{
    //Updateamos si toca, el usuario desde el cloud
    TTParseManager::getInstance()->getFriendsHighLevel(TTFacebook::getInstance()->getStringFriends());
    TTParseManager::getInstance()->verifyUserCloudUpdates();
    TTParseManager::getInstance()->storeUserInfo();
    TTParseManager::getInstance()->syncCloud();
    
    //Mostramos la UI de mapa
    TTUIMap::getInstance()->showUIMap(_mapLayer);
    
    // Mostramos las piezas del mapa
    for(int i = 0; i < _mapPieces.size(); i++)
        _mapPieces[i]->setVisible(true);
    
    // Mostramos los niveles logrados del mapa
    std::vector<int> _stoppers = TTMapParser::getInstance()->getActiveMapStoppers();
    std::vector<int> _stars = TTUserInfo::getInstance()->_userData->_levelStars;
    for(int i = 0; i < _stars.size(); i++)
    {
        _mapCells[i]->_cellOn->setVisible(true);
        _mapCells[i]->_cellOff->setVisible(false);
        _mapCells[i]->_fntLevel->setVisible(true);
        if(_stars[i] != 0)
            _mapCells[i]->_stars[_stars[i]-1]->setVisible(true);
    }
    
    // Mostramos el nivel actual del mapa
    if(TTUserInfo::getInstance()->_userDataVolatile.newLevelOpen)
    {
        int _newLevel = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
        Point _userUIPosition = Point(_mapCells[_newLevel]->_parentCell->getPosition().x,_mapCells[_newLevel]->_parentCell->getPosition().y+((_mapCells[_newLevel]->_cellOn->getBoundingBox().size.height*1.75)*_mapScale));
        
        if(_newLevel <= _mapCells.size())
        {
            _isWalking = true;
            _pauseMap = true;
            MoveTo* _moveUser = MoveTo::create(2,_userUIPosition);
            CallFunc *_showCell = CallFunc::create(CC_CALLBACK_0(TTMapManager::animateActiveCell,this));
            
            //iniciamos automaticamente el siguiente nivel
            CallFunc* _showInitGameMenu = CallFunc::create(std::bind([&](){
                _isWalking = false;
                // Verificamos si hay que updatear el mapa
                if(_waitForUpdateMap)
                {
                    reloadMapInfo();
                }
                
                TTAudioManager::getInstance()->stopAudio("WALK");
                
                //Verificamos el nivel actual
                verifyActualLevelToInit(TTUserInfo::getInstance()->getActualUserProgress()._actualLevel);
            }));
            _meUserMap->_parent->runAction(Sequence::create(_moveUser,_showCell,_showInitGameMenu,NULL));
            
            //Bloqueamos el menú del mapa
            TTUIMap::getInstance()->_blockMenus = true;
            
            TTAudioManager::getInstance()->playAudio("WALK");
        }
        else
        {
            CCLOG("Todos los niveles terminados");
        }
        
        TTUserInfo::getInstance()->_userDataVolatile.newLevelOpen = false;
        
    }
    else
    {
        animateActiveCell();
    }
    
    resituateMapForUserPosition();
    
    _textWinLevel=nullptr;
    //poner el texto de proximamente
    this->onEvent_ChangeLanguage(nullptr);
 
}

void TTMapManager::verifyActualLevelToInit(int _actualLevel)
{
    // Verificamos si el nivel tiene un stopper
    int _levelUserSelected = _actualLevel;
    std::vector<int> _stoppers = TTMapParser::getInstance()->getActiveMapStoppers();
    if(std::find(_stoppers.begin(), _stoppers.end(), _levelUserSelected) != _stoppers.end())
    {
        // Tenemos stopper
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
        TTUIPopup::getInstance()->showPopupStopper(_mapLayer);
    }
    else
    {
        // Cargamos el nivel actual
        std::string levelStr = TTConfigParser::getInstance()->getLevelStr(_levelUserSelected);
        _gameBasicConfig = TTLevelParser::getInstance()->parseLevel(levelStr,true);
        
        // Desbloqueamos el menú del mapa
        TTUIMap::getInstance()->_blockMenus = false;
        
        // Mostramos el menu principal del juego
        TTGameMenus::getInstance()->showInitMenu(_gameBasicConfig,_parentLayer);
    }
}

void TTMapManager::onEvent_ChangeLanguage(EventCustom* e)
{
    if (_textWinLevel!=nullptr)
    {
        _textWinLevel->stopAllActions();
        _textWinLevel->removeFromParent();
    }
    Size _screenSize = Director::getInstance()->getWinSize();

    //Mostramos el texto de proximamente
    _textWinLevel = Label::createWithBMFont("comicFnt.fnt",TTLanguageManager::getInstance()->getTag("COMING_SOON"));
    _textWinLevel->setAnchorPoint(Vec2(0.5,0.5));
    _textWinLevel->setPosition(Point(_screenSize.width/2,_mapPieces[_mapPieces.size()-3]->getPositionY()));
    
    //el cartel ocupe el 70% del ancho de la pantalla
    float _scale =  (70*_screenSize.width/100)/_textWinLevel->getContentSize().width;
    
    //si el texto es demasiado corto, evitamos que salga gigante
    if(_scale > 1)
        _scale = 1;
    
    _textWinLevel->setScale(_scale );
    
    _meUserMap->_parent->getParent()->addChild(_textWinLevel,21);
    auto _scaleUp = ScaleTo::create(1.0f,_textWinLevel->getScale()*0.80);
    auto _scaleDown = ScaleTo::create(1.0f,_textWinLevel->getScale()*0.70);
    _textWinLevel->runAction(RepeatForever::create(Sequence::create(_scaleUp,_scaleDown,NULL)));
}

void TTMapManager::animateActiveCell()
{

    if(!TTUIStackManager::getInstance()->isUIOnTop(UI_GAME_MENU_INIT) && !TTUIStackManager::getInstance()->isUIOnTop(UI_POPUP_SMART))
    {
        _pauseMap = false;
    }
    
    int _actualLevel = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
    std::vector<int> _stoppers = TTMapParser::getInstance()->getActiveMapStoppers();
    
    // Mostramos la celda
    _mapCells[_actualLevel]->_fntLevel->setVisible(true);
    _mapCells[_actualLevel]->_cellOff->setVisible(false);
    _mapCells[_actualLevel]->_cellOn->setVisible(true);
    
    if(std::find(_stoppers.begin(), _stoppers.end(), _actualLevel) != _stoppers.end())
    {
        _mapCells[_actualLevel]->_stopperOn->setVisible(true);
        _mapCells[_actualLevel]->_stopperOff->setVisible(false);
    }
    else
    {
        // Creamos la animación para llamar la atención del usuario en el útlimo nivel desbloqueado
        _halo1 = Sprite::createWithSpriteFrameName("halo.png");
        _halo2 = Sprite::createWithSpriteFrameName("halo.png");
        _halo1->setScale(_mapScale);
        _halo2->setScale(_mapScale);
        _mapCells[_actualLevel]->_parentCell->addChild(_halo1,HALO_1_LAYER);
        _mapCells[_actualLevel]->_parentCell->addChild(_halo2,HALO_2_LAYER);
        
        CallFunc *_launchHalo1 = CallFunc::create(std::bind([&](){
            ScaleTo* _scaleHaloBig = ScaleTo::create(1,2.5f);
            FadeTo* _fadeOutHalo = FadeTo::create(1,0);
            _halo1->runAction(_fadeOutHalo);
            _halo1->runAction(_scaleHaloBig);
            
            CallFunc *_resetHalo1 = CallFunc::create(std::bind([&](){
                _halo1->setScale(1.0f);
                _halo1->setOpacity(255);
            }));
            DelayTime* _delayAnimationHalo = DelayTime::create(1);
            _halo1->runAction(Sequence::create(_delayAnimationHalo,_resetHalo1,NULL));
        }));
        CallFunc *_launchHalo2 = CallFunc::create(std::bind([&](){
            ScaleTo* _scaleHaloBig = ScaleTo::create(1,2.5f);
            FadeTo* _fadeOutHalo = FadeTo::create(1,0);
            _halo2->runAction(_fadeOutHalo);
            _halo2->runAction(_scaleHaloBig);
            
            CallFunc *_resetHalo2 = CallFunc::create(std::bind([&](){
                _halo2->setScale(1.0f);
                _halo2->setOpacity(255);
            }));
            DelayTime* _delayAnimationHalo = DelayTime::create(1);
            _halo2->runAction(Sequence::create(_delayAnimationHalo,_resetHalo2,NULL));
        }));
        
        DelayTime* _delayAnimation = DelayTime::create(2.0f);
        ScaleTo* _scaleBig = ScaleTo::create(0.1f,1.2f);
        ScaleTo* _scaleNormal = ScaleTo::create(0.1f,1.0f);
        Sequence* _scaleSequence = Sequence::create(_launchHalo1,_scaleBig,_scaleNormal,DelayTime::create(0.1),_launchHalo2,_scaleBig,_scaleNormal,_delayAnimation,NULL);
        _mapCells[_actualLevel]->_cellOn->runAction(RepeatForever::create(_scaleSequence));
    }
}

void TTMapManager::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if(_pauseMap)
        return;
    
    // Inicializa variables de movimiento
    _totalDisp = 0;
    _timeDisp = 0;
    _onMove = true;
    _moveDirection = SCROLL_N;
    _mapNode->stopAllActions();
    
    // Verifica los botones
    verifyButtons(touches[0]->getLocation(),EventTouch::EventCode::BEGAN);
}

void TTMapManager::onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if(_pauseMap)
        return;

    // Verifica los botones
    verifyButtons(touches[0]->getLocation(),EventTouch::EventCode::MOVED);
    
    float _verticalDisp = touches[0]->getDelta().y;
    
    // Control de límites del mapa
    if(blockDisplacement(_verticalDisp))
        return;
    
    // Control cambio de dirección dirección
    SCROLL_MOVE_DIRECTION _actualMoveDirection;
    if(_verticalDisp < 0) _actualMoveDirection = SCROLL_S;
    else _actualMoveDirection = SCROLL_N;
    
    if(_actualMoveDirection != _moveDirection)
    {
        //CCLOG("Cambio de dirección");
        _moveDirection = _actualMoveDirection;
        _totalDisp = 0;
        _timeDisp = 0;
    }
    
    _totalDisp += _verticalDisp;
    
    // Movimiento del mapa acelerado
    _mapNode->setPosition(Point(_mapNode->getPosition().x,_mapNode->getPosition().y+_verticalDisp));
}

void TTMapManager::onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if(_pauseMap)
        return;
    
    // Verifica la pulsación de botones
    if(verifyButtons(touches[0]->getLocation(),EventTouch::EventCode::ENDED))
        return;
    
    //CCLOG("El total de distancia recorrida en el último scroll es %f en un tiempo de %f",_totalDisp,_timeDisp);
    _onMove = false;
    
    // Procedemos al movimiento de desaceleración
    if(_timeDisp == 0)
        return;
    
    float velocity = _totalDisp/_timeDisp;
    float _timeToDeacceleration = velocity/DESACCELERATION;
    if(_timeToDeacceleration < 0)
        _timeToDeacceleration *= (-1);
    
    // Limite inferior
    float _posFinish = _mapNode->getPosition().y+(velocity*_timeToDeacceleration)+((DESACCELERATION*(_timeToDeacceleration*_timeToDeacceleration))/2);
    if(_posFinish > 0)
    {
        auto _moveMap = MoveTo::create(TIME_TO_DESCCELERATION,Point(_mapNode->getPosition().x,0));
        auto _moveEaseMap = EaseExponentialOut::create(_moveMap);
        _mapNode->runAction(_moveEaseMap);
        return;
    }
    
    // Limite superior
    int numMapPieces = TTMapParser::getInstance()->_mapInfo->_mapPieces;
    Size _screenSize = Director::getInstance()->getWinSize();
    float _mapHeight = (-1)*(((_mapPieces[0]->getBoundingBox().size.height)*numMapPieces) - _screenSize.height);
    if(_posFinish < _mapHeight)
    {
        auto _moveMap = MoveTo::create(TIME_TO_DESCCELERATION,Point(_mapNode->getPosition().x,_mapHeight));
        auto _moveEaseMap = EaseExponentialOut::create(_moveMap);
        _mapNode->runAction(_moveEaseMap);
        return;
    }
    
    // Caso libre
    auto _moveMap = MoveTo::create(TIME_TO_DESCCELERATION,Point(_mapNode->getPosition().x,_posFinish));
    auto _moveEaseMap = EaseExponentialOut::create(_moveMap);
    _mapNode->runAction(_moveEaseMap);
}

bool TTMapManager::blockDisplacement(float _verticalDisp)
{
    int numMapPieces = TTMapParser::getInstance()->_mapInfo->_mapPieces;
    
    // Control limite inferior
    if(_verticalDisp < 0 && _blockDownDisp)
        _blockDownDisp = false;
    
    if(_blockDownDisp && _verticalDisp > 0)
        return true;
    
    if(_mapNode->getPosition().y+_verticalDisp > 0)
    {
        // Resituamos y bloqueamos desplazamiento vertical inferior
        _mapNode->setPosition(Point(_mapNode->getPosition().x,0));
        
        _blockDownDisp = true;
        return true;
    }
    
    // Control limite superior
    if(_verticalDisp > 0 && _blockUpDisp)
        _blockUpDisp = false;
    
    if(_blockUpDisp && _verticalDisp < 0)
        return true;
    
    Size _screenSize = Director::getInstance()->getWinSize();
    float _mapHeight = (-1)*(((_mapPieces[0]->getBoundingBox().size.height)*numMapPieces) - _screenSize.height);
    if(_mapNode->getPosition().y+_verticalDisp < _mapHeight)
    {
        // Resituamos y bloqueamos desplazamiento vertical superior
        _mapNode->setPosition(Point(_mapNode->getPosition().x,_mapHeight));
        
        _blockUpDisp = true;
        return true;
    }

    return false;
}

void TTMapManager::reloadMapInfo()
{
    CCLOG("Actualizamos la UI de mapa por evento de update de datos de Facebook");
    
    // Eliminamos las fotos actuales y recargamos con la nueva información
    _meUserMap->_parent->removeFromParentAndCleanup(true);
    delete(_meUserMap);
    
    // Paramos el update de multiamigos
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTMapManager::updateUsersMultiPhotos),this);
    
    _multiFriendsInfo.clear();
    typedef std::map<int,userMap*>::iterator itr;
    for(itr iterator = _actualFriendsUsersMap.begin(); iterator != _actualFriendsUsersMap.end(); iterator++)
    {
        iterator->second->_parent->removeFromParentAndCleanup(true);
        delete(iterator->second);
    }
    _actualFriendsUsersMap.clear();
    
    createUsersUI();
    
    _waitForUpdateMap = false;
}

void TTMapManager::onEvent_ReturnToMap(EventCustom* e)
{
    _pauseMap = false;
    _actualLevelBtnSelected = 0;
}

void TTMapManager::onEvent_PauseMap(EventCustom* e)
{
    _pauseMap = true;
}

void TTMapManager::onEvent_AnimMapLoaded(EventCustom* e)
{
    controlResoucesLoaded();
}

void TTMapManager::onEvent_userFriendsUpdated(EventCustom* e)
{
    // Verifica si aun no se ha creado la foto de usuario
    if(_meUserMap == nullptr || !TTFacebook::getInstance()->isConnected())
        return;
    
    if(_isWalking)
    {
        // Hay que esperar a que termine de andar
        _waitForUpdateMap = true;
    }
    else
    {
        reloadMapInfo();
    }
}

void TTMapManager::onEvent_userInfoUpdated(EventCustom* e)
{
    if(_mapCells.size() == 0 || !TTFacebook::getInstance()->isConnected() || _meUserMap == nullptr )
        return;
    
    TTMapStopperManager::getInstance()->updateStoppersInfoFromPosition();
    TTMapStopperManager::destroyInstance();
    
    std::vector<int> _stoppers = TTMapParser::getInstance()->_mapInfo->_mapStoppers;
    std::vector<int> _actualStoppers = TTMapParser::getInstance()->getActiveMapStoppers();
    
    // Mostramos los niveles logrados del mapa
    for(int i = 0; i < _mapCells.size(); i++)
    {
        _mapCells[i]->_cellOn->setVisible(false);
        _mapCells[i]->_cellOn->stopAllActions();
        _mapCells[i]->_cellOff->setVisible(true);
        _mapCells[i]->_fntLevel->setVisible(false);
        for(int j = 0; j < _mapCells[i]->_stars.size(); j++)
        {
            _mapCells[i]->_stars[j]->setVisible(false);
        }
        
        // Update de los stoppers de tablero
        if(std::find(_stoppers.begin(), _stoppers.end(), i) != _stoppers.end())
        {
            if(std::find(_actualStoppers.begin(), _actualStoppers.end(), i) != _actualStoppers.end())
            {
                if( i == TTUserInfo::getInstance()->getActualUserProgress()._actualLevel)
                {
                    _mapCells[i]->_stopperOn->setVisible(true);
                    _mapCells[i]->_stopperOff->setVisible(false);
                }
                else
                {
                    _mapCells[i]->_stopperOn->setVisible(false);
                    _mapCells[i]->_stopperOff->setVisible(true);
                }
            }
            else
            {
                _mapCells[i]->_stopperOn->setVisible(false);
                _mapCells[i]->_stopperOff->setVisible(false);
            }
        }
    }
    
    std::vector<int> _stars = TTUserInfo::getInstance()->_userData->_levelStars;
    for(int i = 0; i < _stars.size(); i++)
    {
        _mapCells[i]->_cellOn->setVisible(true);
        _mapCells[i]->_cellOn->stopAllActions();
        _mapCells[i]->_cellOff->setVisible(false);
        _mapCells[i]->_fntLevel->setVisible(true);
        if(_stars[i] != 0)
            _mapCells[i]->_stars[_stars[i]-1]->setVisible(true);
    }
    
    
    reloadMapInfo();
    animateActiveCell();
    resituateMapForUserPosition();
}

void TTMapManager::onEvent_unlockActualLevel(EventCustom* e)
{
    int _actualLevel = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
    _mapCells[_actualLevel]->_stopperOn->setVisible(false);
    _mapCells[_actualLevel]->_stopperOff->setVisible(false);
    animateActiveCell();
}

void TTMapManager::resituateMapForUserPosition()
{
    _mapNode->setPosition(Point(0,0));
    int _actualLevel = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
    Point _userUIPosition = Point(_mapCells[_actualLevel]->_parentCell->getPosition().x,_mapCells[_actualLevel]->_parentCell->getPosition().y+((_mapCells[_actualLevel]->_cellOn->getBoundingBox().size.height*1.75)*_mapScale));
    
    // Resituamos el mapa según la casilla activa
    int numMapPieces = TTMapParser::getInstance()->_mapInfo->_mapPieces;
    Size _screenSize = Director::getInstance()->getWinSize();
    float _mapHeight = ((_mapPieces[0]->getBoundingBox().size.height)*numMapPieces) - _screenSize.height;
    float _disp = _userUIPosition.y-(_screenSize.height/2);
    if(_disp < 0) _disp = 0;
    if(_disp > _mapHeight) _disp = _mapHeight;
    _mapNode->setPosition(Point(_mapNode->getPosition().x,_mapNode->getPosition().y-_disp));
}

