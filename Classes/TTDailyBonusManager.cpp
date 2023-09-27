//
//  TTDailyBonusManager.cpp
//  animalplanet
//
//  Created by Marc Serena on 7/4/15.
//
//

#include "TTDailyBonusManager.h"
#include "thegame.h"
#include "TTUIStackManager.h"
#include "TTStoreManager.h"
#include "TTParseManager.h"
#include "TTUIPopupSmart.h"
#include "TTUIMap.h"
#include "TTUIShop.h"
#include "TTAudioManager.h"

#define DAILY_BONUS_XML "dailyBonus.xml"

//Singelton
static TTDailyBonusManager *_sharedDailyBonusManager = nullptr;

TTDailyBonusManager* TTDailyBonusManager::getInstance()
{
    if (!_sharedDailyBonusManager)
    {
        _sharedDailyBonusManager = new TTDailyBonusManager();
        _sharedDailyBonusManager->preinit();
    }
    
    return _sharedDailyBonusManager;
}

void TTDailyBonusManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedDailyBonusManager);
}

TTDailyBonusManager::~TTDailyBonusManager()
{
    CCLOG("Se destruyen los recursos del Bonus diario");
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    
    // Eliminamos las acciones
    removeAllActionsFromJSON("UI_DAILY_BONUS.ExportJson");
    
    // Esconde los créditos del usuario
    TTUIMap::getInstance()->hideUserCredits();
    
    // Eliminamos la layer contenedora
    _uiDBLayer->removeAllChildrenWithCleanup(true);
    _uiDBLayer->removeFromParentAndCleanup(true);
    _winParent->removeAllChildrenWithCleanup(true);
    _winParent->removeFromParentAndCleanup(true);
    _bgUIDailyBonus->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_DAILY_BONUS0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_DAILY_BONUS0.png");
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("animalCubes_DAILY_BONUS_FIGURES.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("animalCubes_DAILY_BONUS_FIGURES.png");
}

void TTDailyBonusManager::pauseUI()
{
    
}

void TTDailyBonusManager::restoreUI()
{
    TTUIMap::getInstance()->showUserCredits(_actualParent);
}

void TTDailyBonusManager::preloadResources()
{
    _generalResourcesToLoad = 2;
    Director::getInstance()->getTextureCache()->addImageAsync("UI_DAILY_BONUS0.png",CC_CALLBACK_1(TTDailyBonusManager::resourceLoaded, this));
    Director::getInstance()->getTextureCache()->addImageAsync("animalCubes_DAILY_BONUS_FIGURES.png",CC_CALLBACK_1(TTDailyBonusManager::resourceLoaded, this));
}

void TTDailyBonusManager::resourceLoaded(Texture2D* texture)
{
    _generalResourcesToLoad--;
    if(_generalResourcesToLoad == 0)
    {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UI_DAILY_BONUS0.plist","UI_DAILY_BONUS0.png");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animalCubes_DAILY_BONUS_FIGURES.plist","animalCubes_DAILY_BONUS_FIGURES.png");
        init();
    }
}

void TTDailyBonusManager::preinit()
{
    // Registramos los eventos
    _classListeners.push_back(registerListenerEvent(EVENT_TIME_SERVER,CC_CALLBACK_1(TTDailyBonusManager::onEvent_getTimeServer,this)));
    
    // Recuperamos la estructura guardada y si no existe la creamos
    _def = UserDefault::getInstance();
    _userInfo = new userDailyBonusInfo();
    
    _timeServerInfo._waitForTime = false;
    _timeServerInfo._timeRecived = false;
}

void TTDailyBonusManager::init()
{
    _isPlaying = false;
    
    //Cargamos el fondo de veladura
    Size _screenSize = Director::getInstance()->getWinSize();
    _bgUIDailyBonus = Sprite::create("pixelBg.png");
    _bgUIDailyBonus->retain();
    _bgUIDailyBonus->setAnchorPoint(Vec2(0.5,0.5));
    _bgUIDailyBonus->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUIDailyBonus->setScale(_screenSize.width,_screenSize.height);
    
    // Cargamos la UI del bonus diario
    _uiDBLayer = Layer::create();
    _uiDBLayer->retain();
    _widgetDB = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_DAILY_BONUS.ExportJson");
    cocos2d::ui::Layout* _layoutDB = dynamic_cast<cocos2d::ui::Layout*>(_widgetDB);
    _uiDBLayer->addChild(_layoutDB);
    
    // Escalado Widget
    _widgetDB->setScale(scale_widget(_widgetDB,WIDTH_UI_DB_SPACE,HEIGHT_UI_DB_SPACE));
    
    // Posicionamiento Widget
    _widgetDB->setAnchorPoint(Vec2(0.5,0.5));
    _uiDBLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _uiDBLayer->setVisible(false);
    
    _widgetBackgroundWin = (cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("play_backgroundWin");
    _widgetBackgroundWin->setVisible(false);
    
    ((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_1"))->setAnchorPoint(Point(0,0));
    ((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_2"))->setAnchorPoint(Point(0,0));
    ((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_3"))->setAnchorPoint(Point(0,0));
    _rollerParents.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_1"));
    _rollerParents.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_2"));
    _rollerParents.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("play_background")->getChildByName("roller_3"));
    
    _btnExit = (cocos2d::ui::Button*)_widgetDB->getChildByName("exitBtn");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTDailyBonusManager::onClickExitBtn,this));
    _btnPlay = (cocos2d::ui::Button*)_widgetDB->getChildByName("machine_ui")->getChildByName("spinBtn");
    _btnPlay->addTouchEventListener(CC_CALLBACK_2(TTDailyBonusManager::onClickPlayBtn,this));
    
    _infoSpins.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("machine_ui")->getChildByName("info")->getChildByName("infoCoins"));
    _infoSpins.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("machine_ui")->getChildByName("info")->getChildByName("infox1"));
    _infoSpins.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("machine_ui")->getChildByName("info")->getChildByName("infox2"));
    _infoSpins.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("machine_ui")->getChildByName("info")->getChildByName("infox3"));
    
    for(int i = 0; i < 11; i++)
    {
        std::string _name = "bombilla_"+std::to_string(i+1);
        _lights.push_back((cocos2d::ui::Widget*)_widgetDB->getChildByName("machine_ui")->getChildByName(_name));
    }
    
    _freeSpins = (cocos2d::ui::TextBMFont*)_widgetDB->getChildByName("machine_ui")->getChildByName("infoMachine");
    _freeSpins->setString(TTLanguageManager::getInstance()->getTag(FREE_SPINS));
    
    // Creamos los rodillos
    parseDailyBonusXML();
    
    _winParent = Node::create();
    _winParent->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _winParent->setVisible(false);
    _actualParent->addChild(_winParent,40);
    
    // Cargamos los gráficos del win
    _winBackground = Sprite::createWithSpriteFrameName("halo_win.png");
    _winBackground->setScale(scale_sprite(_winBackground,90,70));
    auto _rotation = RotateBy::create(3.0f,360);
    _winBackground->runAction(RepeatForever::create(_rotation));
    _winParent->addChild(_winBackground,1);
    for(int i = 0; i < NUM_FIGURES_SLOT; i++)
    {
        std::string _nameFigure = "win_"+_figureSlotSpritesMap.find(i)->second;
        Sprite* _figure = Sprite::createWithSpriteFrameName(_nameFigure);
        _figure->setScale(scale_sprite(_figure,50,50));
        _figure->setVisible(false);
        _winParent->addChild(_figure,2);
        _winFigures.push_back(_figure);
    }
    
    // Mostramos el Bonus diario
    finishShowDailyBonus();
}

void TTDailyBonusManager::parseDailyBonusXML()
{
    CCLOG("Parseando XML Daily Bonus");
    
    // Abrimos XML HowTo
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(DAILY_BONUS_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer el XML Daily Bonus");
        abort();
    }
    
    // Leemos XML del DailyBonus
    pugi::xml_node dailyBonusNode = XMLFile->child("dailyBonus");
    if(!dailyBonusNode.empty())
    {
        pugi::xml_node _combinationNode = dailyBonusNode.child("rollerCombinations");
        for(pugi::xml_node combination = _combinationNode.child("rollerCombination"); combination; combination = combination.next_sibling("rollerCombination"))
        {
            _combinationsWeight.push_back(combination.attribute("weight").as_int());
        }
        
        int _countRollers = 0;
        pugi::xml_node _rollersNode = dailyBonusNode.child("rollerDefinitions");
        for(pugi::xml_node roller = _rollersNode.child("rollerDefinition"); roller; roller = roller.next_sibling("rollerDefinition"))
        {
            rollerInfo _roller;
            _roller._parentRoller = Node::create();
            _rollerParents[_countRollers]->addChild(_roller._parentRoller);
            _roller._parentRoller->setPosition(Point(0,0));
            
            std::vector<int> _rollerDef = tokenizeStringCommaDelimitedToInt(roller.attribute("slots").as_string());
            for(int i = 0; i < _rollerDef.size(); i++)
            {
                slotInfo _slot;
                
                // Cargamos los gráficos
                int _idFigure = _rollerDef[i];
                Sprite* _slotFigure = Sprite::createWithSpriteFrameName(_figureSlotSpritesMap.find(_idFigure)->second);
                
                // Creamos el padre del slot
                Node* _slotParentNode = Node::create();
                _roller._parentRoller->addChild(_slotParentNode);
                _slotParentNode->setPosition(Point(0,_slotFigure->getContentSize().height*i));
                _slotParentNode->addChild(_slotFigure,2);
                if(i < 3) _slotParentNode->setVisible(true);
                else _slotParentNode->setVisible(false);
                
                _slot._idFigureSlot = _idFigure;
                _slot._spriteFigureSlot = _slotFigure;
                _slot._slotParentNode = _slotParentNode;
                _roller._rollerSlots.push_back(_slot);
            }
            _roller._isRolling = false;
            
            auto _rollerSpace = _rollerParents[_countRollers]->getContentSize().height;
            auto _figureSize = _roller._rollerSlots[0]._spriteFigureSlot->getContentSize().height;
            auto _scale = (_rollerSpace/1.7f)/_figureSize;
            _roller._parentRoller->setScale(_scale);
            
            _rollers.push_back(_roller);
            _countRollers++;
        }
    }
}

void TTDailyBonusManager::finishShowDailyBonus()
{
    if(!_timeServerInfo._timeRecived)
    {
        _timeServerInfo._waitForTime = true;
    }
    else
    {
        if(_timeServerInfo._error)
        {
            TTUIStackManager::getInstance()->hideUILoading();
            
            // Mostramos el popup de error de conexión
            TTUIStackManager::getInstance()->closeUI();
            TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
            TTUIPopupSmart::getInstance()->showPopupState(_actualParent,SMART_POPUP_GENERAL_CONNECTION_ERRROR_STATE,false);
            
            // Destruimos la instancia
            destroyInstance();
        }
        else
        {
            // Verificamos los datos de usuario del bonus diario
            verifyUserDailyBonusInfo();
            setIconUserSpins();
            
            TTUIStackManager::getInstance()->hideUILoading();
            
            if(_bgUIDailyBonus->getParent() != nullptr)
                _bgUIDailyBonus->removeFromParentAndCleanup(false);
            _actualParent->addChild(_bgUIDailyBonus,20);
            _bgUIDailyBonus->setVisible(true);
            
            if(_uiDBLayer->getParent() != nullptr)
                _uiDBLayer->removeFromParentAndCleanup(false);
            _actualParent->addChild(_uiDBLayer,30);
            _uiDBLayer->setVisible(true);
            _widgetDB->setVisible(true);
            
            setMachineState(MACHINE_STATE_STANBY);
            
            TTAudioManager::getInstance()->playAudio("DAILY_BONUS_MUSIC");
        }
    }
}

void TTDailyBonusManager::initShowDailyBonus(Node* _parent)
{
    _actualParent = _parent;
    
    // Mostramos el loading
    TTUIStackManager::getInstance()->showUILoading(_parent);

    // Intentamos recuperar el tiempo del servidor
    TTParseManager::getInstance()->getServerTime();
    
    // Cargamos los recursos
    preloadResources();
}

void TTDailyBonusManager::play()
{
    // Aplicamos la jugada
    if(_userInfo->_freeSpins > 0)
    {
        _userInfo->_freeSpins--;
        _userInfo->_lastDayPlayed = _timeServerInfo._timeServer._day;
        _userInfo->_lastMonthPlayed = _timeServerInfo._timeServer._month;
        saveUserDailyBonusInfo();
        setIconUserSpins();
    }
    else
    {
        // Cobramos una moneda por la tirada
        if(TTStoreManager::getInstance()->buyVirtualItem("DAILY_BONUS_PLAY"))
        {
            TTUIMap::getInstance()->updateUIValues();
        }
        else
        {
            // No hay monedas suficientes, mostramos la tienda
            TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
            TTUIShop::getInstance()->showShop(_uiDBLayer->getParent());
            return;
        }
    }
    
    _isPlaying = true;
    _widgetBackgroundWin->setVisible(false);
    
    // Decidimos el resultado de la jugada según la estadístics
    calcActualCombination();
    
    // Ponemos los rodillos a girar
    TTAudioManager::getInstance()->playAudio("SLOT_LOOP",true);
    _timeRolling = 0;
    for(int i = 0; i < _rollers.size(); i++)
    {
        _rollers[i]._isRolling = true;
        _rollers[i]._isFinished = false;
    }
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTDailyBonusManager::spinRollers),this,0.01f,false);
}

void TTDailyBonusManager::spinRollers(float _time)
{
    _timeRolling += _time;
    if(_timeRolling > TIME_ROLLING)
    {
        // Los rodillos deben terminar de rodar, indicamos que el primer rodillo se detenga
        _rollers[0]._isRolling = false;
    }
    
    for(int j = 0; j < _rollers.size(); j++)
    {
        rollerInfo _roller = _rollers[j];
        
        // Si el rodillo ha finalizado no hacemos nada
        if(_roller._isFinished) continue;
        
        float _totalDistance = _roller._rollerSlots.size()*_roller._rollerSlots[0]._spriteFigureSlot->getContentSize().height;
        float _distanceToMove = (_time*_totalDistance)/TIME_ONE_ROLL;
        int _idxRoller = 0;
        for(int i = 0; i < _roller._rollerSlots.size(); i++)
        {
            // Verificamos si hay que parar los rodillos
            if(!_roller._isRolling && _roller._rollerSlots[1]._idFigureSlot == _actualCombinationInfo._actualCombinationIds[j])
            {
                // Ajustamos los rodillos a su posición correcta
                for(int w = 0; w < _roller._rollerSlots.size(); w++)
                {
                    if(w < 3) _roller._rollerSlots[w]._slotParentNode->setVisible(true);
                    else _roller._rollerSlots[w]._slotParentNode->setVisible(false);
                    _roller._rollerSlots[w]._slotParentNode->setPosition(Point(_roller._rollerSlots[w]._slotParentNode->getPosition().x,_roller._rollerSlots[w]._spriteFigureSlot->getContentSize().height*w));
                }
                _roller._isFinished = true;
                
                // Reproducimos el audio de parada del rodillo
                std::string _slotStopAudio = "SLOT_STOP_"+std::to_string(j+1);
                TTAudioManager::getInstance()->playAudio(_slotStopAudio);
                
                // Creamos la animación de rebote del rodillo
                float _posYDown = _roller._parentRoller->getPosition().y-(_roller._rollerSlots[0]._spriteFigureSlot->getContentSize().height/6);
                auto _moveDown = MoveTo::create(0.1f,Point(_roller._parentRoller->getPosition().x,_posYDown));
                auto _moveUp = MoveTo::create(0.1f,Point(_roller._parentRoller->getPosition().x,0));
                if(j == _rollers.size()-1)
                {
                    // Es el útlimo rodillo
                    auto _callFinishCallback = CallFunc::create(CC_CALLBACK_0(TTDailyBonusManager::finishSpinRollers,this));
                    _roller._parentRoller->runAction(Sequence::create(_moveDown,_moveUp,_callFinishCallback,NULL));
                }
                else
                {
                    _roller._parentRoller->runAction(Sequence::create(_moveDown,_moveUp,NULL));
                }
                
                break;
            }
            
            // Mover la figura del rodillo
            _roller._rollerSlots[_idxRoller]._slotParentNode->setPosition(Point(_roller._rollerSlots[_idxRoller]._slotParentNode->getPosition().x,_roller._rollerSlots[_idxRoller]._slotParentNode->getPosition().y-_distanceToMove));
            
            // Setear la visiblidad de la figura
            if(_roller._rollerSlots[_idxRoller]._slotParentNode->getPosition().y >= 0 && _roller._rollerSlots[_idxRoller]._slotParentNode->getPosition().y <= _roller._rollerSlots[_idxRoller]._spriteFigureSlot->getContentSize().height*2)
            {
                _roller._rollerSlots[_idxRoller]._slotParentNode->setVisible(true);
            }
            else
            {
                _roller._rollerSlots[_idxRoller]._slotParentNode->setVisible(false);
            }
            
            // Verificamos si hay que reposicionar la figura
            if(_roller._rollerSlots[0]._slotParentNode->getPosition().y <= _roller._parentRoller->getPosition().y)
            {
                // Posición
                float _newY = _roller._rollerSlots[_roller._rollerSlots.size()-1]._slotParentNode->getPosition().y+_roller._rollerSlots[0]._spriteFigureSlot->getContentSize().height;
                _roller._rollerSlots[0]._slotParentNode->setPosition(Point(_roller._rollerSlots[0]._slotParentNode->getPosition().x,_newY));
                
                // Switch Vector
                _roller._rollerSlots.push_back(_roller._rollerSlots[0]);
                _roller._rollerSlots.erase(_roller._rollerSlots.begin());
            }
            else
            {
                _idxRoller++;
            }
        }
        
        _rollers[j] = _roller;
    }
    
    // Miramos si quedan rodillos por parar
    int _numStopedRollers = 0;
    for(int i = 0; i < _rollers.size(); i++)
    {
        if(_rollers[i]._isFinished) _numStopedRollers++;
    }
    if(_numStopedRollers != 0 && _numStopedRollers < _rollers.size())
    {
        //Lanzamos evento para parar el rodillo al cabo de un rato
        auto _stopRoller = CallFunc::create(std::bind([&](int _idRoller){
            _rollers[_idRoller]._isRolling = false;
        },_numStopedRollers));
        _rollers[_numStopedRollers-1]._parentRoller->runAction(Sequence::create(DelayTime::create(TIME_ROLL_STOP),_stopRoller, NULL));
    }
    
    //Verificamos si hay que parar la tirada
    bool _allRollersFinished = true;
    for(int j = 0; j < _rollers.size(); j++)
    {
        if(!_rollers[j]._isFinished) _allRollersFinished = false;
    }
    if(_allRollersFinished)
    {
        //TTAudioManager::getInstance()->stopAudio("SLOT_LOOP");
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTDailyBonusManager::spinRollers),this);
    }
}

void TTDailyBonusManager::finishSpinRollers()
{
    // Mostramos los shows de ganado/perdido
    TTAudioManager::getInstance()->stopAudio("SLOT_LOOP");
    if(_actualCombinationInfo._actualCombinationType == COMBINATION_TYPE_NO_PRIZE)
    {
        //Show de perdido
        _uiDBLayer->runAction(Sequence::create(DelayTime::create(0.1f),CallFunc::create(CC_CALLBACK_0(TTDailyBonusManager::showLosePrize,this)),NULL));
    }
    else
    {
        //Show de ganado con retardo
        _uiDBLayer->runAction(Sequence::create(DelayTime::create(0.1f),CallFunc::create(CC_CALLBACK_0(TTDailyBonusManager::showWinPrize,this)),NULL));
    }
}

void TTDailyBonusManager::showLosePrize()
{
    _isPlaying = false;
    setMachineState(MACHINE_STATE_STANBY);
}

void TTDailyBonusManager::showWinPrize()
{
    setMachineState(MACHINE_STATE_WIN);
    _widgetBackgroundWin->setVisible(true);
    
    auto _showWin = CallFunc::create(std::bind([&](){
        _winParent->setVisible(true);
        int _figure = (int)_actualCombinationInfo._actualCombinationType-1;
        _winFigures[_figure]->setVisible(true);
        
        _winParent->setScale(0);
        auto _scaleWinUp = ScaleTo::create(0.2f,1.1f);
        auto _scaleWinDown = ScaleTo::create(0.1f,1.0f);
        _winParent->runAction(Sequence::create(_scaleWinUp,_scaleWinDown,NULL));
    }));
    auto _hideWin = CallFunc::create(std::bind([&](){
        auto _fadeOutBackground = FadeTo::create(0.3f,0);
        auto _fadeOutFigure = FadeTo::create(0.3f,0);
        int _figure = (int)_actualCombinationInfo._actualCombinationType-1;
        _winFigures[_figure]->runAction(_fadeOutFigure);
        _winBackground->runAction(_fadeOutBackground);
    }));
    
    auto _finishWin = CallFunc::create(std::bind([&](){
        //Entregamos el premio ganado
        giveGift();
        
        //Finilizamos el show
        _isPlaying = false;
        _winBackground->setOpacity(255);
        int _figure = (int)_actualCombinationInfo._actualCombinationType-1;
        _winFigures[_figure]->setOpacity(255);
        _winParent->setVisible(false);
        for(int i = 0; i < _winFigures.size(); i++)
        {
            _winFigures[i]->setVisible(false);
        }
        setMachineState(MACHINE_STATE_STANBY);
    }));
    
    _uiDBLayer->runAction(Sequence::create(DelayTime::create(0.5f),_showWin,DelayTime::create(4.0f),_hideWin,DelayTime::create(0.4f),_finishWin,NULL));
}

void TTDailyBonusManager::giveGift()
{
    if(_actualCombinationInfo._actualCombinationType == COMBINATION_TYPE_MONEYS)
    {
        // Regalamos monedas al usuario
        TTStoreManager::getInstance()->giftCoinsToUser(3);
        TTUIMap::getInstance()->updateUIValues();
    }
    else
    {
        // Regalamos un booster al usuario
        std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"1_"+std::to_string(_actualCombinationInfo._actualCombinationType-2));
        int _numActualItems = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
        if(_numActualItems < 9)
        {
            // Regalamos un booster al usuario
            TTStoreManager::getInstance()->giftBoosterToUser(_boosterItemId);
        }
    }
}

void TTDailyBonusManager::calcActualCombination()
{
    // Calcula el tipo de combinación
    COMBINATION_TYPE _actualCombinationType;
    int _maxWeight = 0;
    for(int i = 0; i < _combinationsWeight.size(); i++)
    {
        _maxWeight += _combinationsWeight[i];
    }
    int _randSelection = rand()%_maxWeight + 1;
    int _acumulationSum = 0;
    for(int i = 0; i < _combinationsWeight.size(); i++)
    {
        _acumulationSum += _combinationsWeight[i];
        if(_randSelection <= _acumulationSum)
        {
            _actualCombinationType = (COMBINATION_TYPE)i;
            break;
        }
    }
    
    _actualCombinationInfo._actualCombinationIds.clear();
    _actualCombinationInfo._actualCombinationType = _actualCombinationType;
    if(_actualCombinationType == COMBINATION_TYPE_NO_PRIZE)
    {
        // Creamos una combinación sin premio
        for(int i = 0; i < _rollers.size(); i++)
        {
            int _randFigure = rand()%NUM_FIGURES_SLOT;
            if(i == _rollers.size()-1)
            {
                while(_randFigure == _actualCombinationInfo._actualCombinationIds[_actualCombinationInfo._actualCombinationIds.size()-1])
                {
                    _randFigure = rand()%NUM_FIGURES_SLOT;
                }
            }
            _actualCombinationInfo._actualCombinationIds.push_back(_randFigure);
        }
    }
    else
    {
        // Creamos el premio que toque
        int idFiguresPrize = _actualCombinationType-1;
        for(int i = 0; i < _rollers.size(); i++)
        {
            _actualCombinationInfo._actualCombinationIds.push_back(idFiguresPrize);
        }
    }
    
    CCLOG("Tirada de rodillo tipo %d: combinación final = %d,%d,%d",_actualCombinationType,_actualCombinationInfo._actualCombinationIds[0],_actualCombinationInfo._actualCombinationIds[1],_actualCombinationInfo._actualCombinationIds[2]);
    
}

void TTDailyBonusManager::onClickPlayBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if(eEventType == ui::Widget::TouchEventType::ENDED && !_isPlaying)
    {
        setMachineState(MACHINE_STATE_SPIN);
        play();
    }
}

void TTDailyBonusManager::onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if(eEventType == ui::Widget::TouchEventType::ENDED && !_isPlaying)
    {
        TTAudioManager::getInstance()->playAudio("MAP_MUSIC");
        TTUIStackManager::getInstance()->closeUI();
        destroyInstance();
    }
}

void TTDailyBonusManager::setMachineState(MACHINE_STATE _state)
{
    //Apagamos todas las bombillas
    for(int i = 0; i < _lights.size(); i++)
    {
        _lights[i]->getChildByName("bombilla_on")->setOpacity(255);
        _lights[i]->getChildByName("bombilla_on")->setVisible(false);
        _lights[i]->getChildByName("bombilla_on")->stopAllActions();
    }
    
    switch(_state)
    {
        case MACHINE_STATE_STANBY:
        {
            initStanbyShow(0);
        }
        break;
            
        case MACHINE_STATE_SPIN:
        {
            TTAudioManager::getInstance()->playAudio("START_SLOT_MACHINE");
            
            for(int i = 0; i < _lights.size(); i++)
            {
                _lights[i]->getChildByName("bombilla_on")->setVisible(true);
            }
        }
        break;
            
        case MACHINE_STATE_WIN:
        {
            TTAudioManager::getInstance()->playAudio("SLOT_WIN");
            
            for(int i = 0; i < _lights.size(); i++)
            {
                auto _fadeIn = FadeTo::create(0.2f,255);
                auto _fadeOut = FadeTo::create(0.2f,0);
                _lights[i]->getChildByName("bombilla_on")->setVisible(true);
                _lights[i]->getChildByName("bombilla_on")->runAction(RepeatForever::create(Sequence::create(_fadeIn,_fadeOut, NULL)));
            }
        }
        break;
            
        default:
        break;
    }
}

void TTDailyBonusManager::initStanbyShow(int _idxLight)
{
    _lights[_idxLight]->getChildByName("bombilla_on")->setVisible(true);
    
    auto _fadeIn = FadeTo::create(0.1f,255);
    auto _fadeOut = FadeTo::create(0.1f,0);
    auto _recall = CallFunc::create(std::bind([&](int _currIdx){
        if(_currIdx == _lights.size()-1) _currIdx = 0;
        else _currIdx++;
        initStanbyShow(_currIdx);
    },_idxLight));
    auto _normalFunc = Sequence::create(_fadeIn,_fadeOut,_recall,NULL);
    _lights[_idxLight]->getChildByName("bombilla_on")->runAction(_normalFunc);
}

void TTDailyBonusManager::verifyUserDailyBonusInfo()
{
    if(_def->getBoolForKey(USER_DATA_DAILY_BONUS_HAS_DATA,false))
    {
        readUserDailyBonusInfo();
        
        // Verificamos si hay que regalar tres tiradas al usuario
        if(_userInfo->_lastDayPlayed != _timeServerInfo._timeServer._day ||
           (_userInfo->_lastDayPlayed == _timeServerInfo._timeServer._day && _userInfo->_lastMonthPlayed != _timeServerInfo._timeServer._month))
        {
            _userInfo->_freeSpins = DEFAULT_DAILY_FREE_SPINS;
            _userInfo->_lastDayPlayed = _timeServerInfo._timeServer._day;
            _userInfo->_lastMonthPlayed = _timeServerInfo._timeServer._month;
            saveUserDailyBonusInfo();
        }
    }
    else
    {
        // Si no hay datos de usuario los creamos
        _userInfo->_freeSpins = DEFAULT_DAILY_FREE_SPINS;
        _userInfo->_lastDayPlayed = _timeServerInfo._timeServer._day;
        _userInfo->_lastMonthPlayed = _timeServerInfo._timeServer._month;
        
        saveUserDailyBonusInfo();
    }
}

void TTDailyBonusManager::readUserDailyBonusInfo()
{
    _userInfo->_freeSpins = _def->getIntegerForKey(USER_DATA_DAILY_BONUS_FREE_SPINS);
    _userInfo->_lastDayPlayed = _def->getIntegerForKey(USER_DATA_DAILY_BONUS_LAST_DAY);
    _userInfo->_lastMonthPlayed = _def->getIntegerForKey(USER_DATA_DAILY_BONUS_LAST_MONTH);
}

void TTDailyBonusManager::saveUserDailyBonusInfo()
{
    _def->setBoolForKey(USER_DATA_DAILY_BONUS_HAS_DATA,true);
    _def->setIntegerForKey(USER_DATA_DAILY_BONUS_FREE_SPINS,_userInfo->_freeSpins);
    _def->setIntegerForKey(USER_DATA_DAILY_BONUS_LAST_DAY,_userInfo->_lastDayPlayed);
    _def->setIntegerForKey(USER_DATA_DAILY_BONUS_LAST_MONTH,_userInfo->_lastMonthPlayed);
    _def->flush();
}

void TTDailyBonusManager::setIconUserSpins()
{
    for(int i = 0; i < _infoSpins.size(); i++)
    {
        _infoSpins[i]->setVisible(false);
    }
    _infoSpins[_userInfo->_freeSpins]->setVisible(true);
    
    if(_userInfo->_freeSpins == 0)
    {
        //Mostramos las monedas del usuario
        TTUIMap::getInstance()->showUserCredits(_actualParent,30);
    }
}

void TTDailyBonusManager::doStatistics(int _numPlays)
{
    // Recopilación estadística
    int _playStatistics = _numPlays;
    std::map<int,int> _statistics;
    for(int i = 0; i < _playStatistics; i ++)
    {
        calcActualCombination();
        
        if(_statistics.find(_actualCombinationInfo._actualCombinationType) != _statistics.end())
        {
            _statistics.find(_actualCombinationInfo._actualCombinationType)->second++;
        }
        else
        {
            _statistics.insert(std::make_pair(_actualCombinationInfo._actualCombinationType,1));
        }
    }
    
    CCLOG("Número de tiradas = %d",_playStatistics);
    std::map<int,int>::iterator _statisticsItr;
    for(_statisticsItr = _statistics.begin(); _statisticsItr != _statistics.end(); ++_statisticsItr)
    {
        CCLOG("Tipo de combinación: %d número de casos: %d",_statisticsItr->first,_statisticsItr->second);
    }
}

void TTDailyBonusManager::onEvent_getTimeServer(EventCustom* e)
{
    TimeServer* _serverTime = static_cast<TimeServer*>(e->getUserData());
    _timeServerInfo._timeRecived = true;
    _timeServerInfo._error = _serverTime->_error;
    if(!_timeServerInfo._error) _timeServerInfo._timeServer = parseTimeToTimeStruct(_serverTime->_date);
    if(_timeServerInfo._waitForTime)
    {
        finishShowDailyBonus();
    }
}

