#include "TTGameManager.h"
#include "TTConfigParser.h"
#include "TTAnimalCubes.h"
#include "TTUserInfo.h"
#include "TTStatistics.h"
#include "TTParticleManager.h"
#include "TTUIStackManager.h"
#include "TTBoardBoosterCombos.h"
#include "TTAudioManager.h"
#include "TTUserLives.h"
#include "TTUIPopup.h"
#include "TTUIBonus.h"
#include "TTFacebook.h"
#include "TTParseManager.h"
#include "TTHowToInGameManager.h"

TTGameManager::TTGameManager()
{
    // Init de variables
    _parentLayer = this;
    _inGame = false;
    _pause = false;
    _waitingForFinishBoardSteps = false;
    
    //Iniciamos clase de tutoriales
    TTHowToInGameManager::getInstance()->init(_parentLayer);
    
    // Inicializamos clase de tablero y juego
    TTBoard::getInstance()->initBoardInstance(_parentLayer);
    _game = TTGame::getInstance();
    
    // Registro de recepcion de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_FINISH,CC_CALLBACK_1(TTGameManager::onEvent_FinishGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_CONTINUEBUY,CC_CALLBACK_1(TTGameManager::onEvent_ContinueBuyGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_RETRY,CC_CALLBACK_1(TTGameManager::onEvent_RetryGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_PAUSE,CC_CALLBACK_1(TTGameManager::onEvent_pauseGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_CONTINUE,CC_CALLBACK_1(TTGameManager::onEvent_continueGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_FINISHED_BOARD,CC_CALLBACK_1(TTGameManager::onEvent_FinishedBoard,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_ALL_STEPS_FINISHED,CC_CALLBACK_1(TTGameManager::onEvent_boardStepsFinished,this)));
    
    // activate touches on this scene
    _touchListener = EventListenerTouchAllAtOnce::create();
    _touchListener->onTouchesBegan = CC_CALLBACK_2(TTGameManager::onTouchesBegan, this);
    _touchListener->onTouchesMoved = CC_CALLBACK_2(TTGameManager::onTouchesMoved, this);
    _touchListener->onTouchesEnded = CC_CALLBACK_2(TTGameManager::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
    
    // Activamos el teclado
    this->setKeypadEnabled(true);
    
    // agregamos la escena al update
    scheduleUpdate();
    
    // Cargamos el juego
    loadGame();
}

TTGameManager::~TTGameManager()
{
    CCLOG("Se destruye la escena de juego.");
    
    //Quitamos la música de background
    TTAudioManager::getInstance()->stopAudio("GAME_MUSIC");
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    _eventDispatcher->removeEventListener(_touchListener);
    
    // Eliminamos el update
    unscheduleUpdate();
    
    // Eliminamos las clases creadas
    TTBoard::destroyInstance();
    TTGame::destroyInstance();
    
    // Eliminamos todos los hijos
    this->removeAllChildrenWithCleanup(true);
    
    // Eliminamos las animaciones
    for(int i = 0; i < NUM_CELL_ANIMATIONS_STANDBY; i++)
    {
        std::string animName = _cellAnimationsInfo.find(CELL_ANIMATIONS_STANDBY_ID(i))->second;
        for(int j = 0; j < NUM_CELL_NORMAL_FIGURES; j++)
        {
            char strAnim[100] = {0};
            sprintf(strAnim,"figura%d_%s",j+1,animName.c_str());
            AnimationCache::getInstance()->removeAnimation(strAnim);
        }
    }
    for(int j = 0; j < NUM_CELL_NORMAL_FIGURES; j++)
    {
        char strAnim[100] = {0};
        sprintf(strAnim,"figura%d_%s",j+1,ANIM_SELECTION_SCARY);
        AnimationCache::getInstance()->removeAnimation(strAnim);
    }
    for(int j = 0; j < NUM_CELL_NORMAL_FIGURES; j++)
    {
        char strAnim[100] = {0};
        sprintf(strAnim,"figura%d_%s",j+1,ANIM_SELECTION_REBOUND);
        AnimationCache::getInstance()->removeAnimation(strAnim);
    }
    for(int j = 0; j < NUM_CELL_NORMAL_FIGURES; j++)
    {
        char strAnim[100] = {0};
        sprintf(strAnim,"figura%d_%s",j+1,ANIM_SELECTION_JUMP);
        AnimationCache::getInstance()->removeAnimation(strAnim);
    }
    
    // Eliminamos las texturas que hemos cargado
    std::map<std::string,std::vector<std::string> > _worldResources = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->getActualUserProgress()._actualWorld]._resources;
    for(auto iterator = _worldResources.begin(); iterator != _worldResources.end(); iterator++)
    {
        if(iterator->first == "spriteSheet")
        {
            for(int i = 0; i < iterator->second.size(); i++)
            {
                std::string resourceName = iterator->second[i];
                // Load plist
                char strResourceImage[100] = {0};
                sprintf(strResourceImage,"%s.png",resourceName.c_str());
                
                char strResourcePlist[100] = {0};
                sprintf(strResourcePlist,"%s.plist",resourceName.c_str());
                
                SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(strResourcePlist);
                Director::getInstance()->getTextureCache()->removeTextureForKey(strResourceImage);
            }
        }
    }
    
    int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld]._charactersPack;
    std::map<std::string,std::vector<std::string> > _charactersResources = TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._graphs;
    for(auto iterator = _charactersResources.begin(); iterator != _charactersResources.end(); iterator++)
    {
        if(iterator->first == "spriteSheet")
        {
            for(int i = 0; i < iterator->second.size(); i++)
            {
                std::string resourceName = iterator->second[i];
                // Load plist
                char strResourceImage[100] = {0};
                sprintf(strResourceImage,"%s.png",resourceName.c_str());
                
                char strResourcePlist[100] = {0};
                sprintf(strResourcePlist,"%s.plist",resourceName.c_str());
                
                SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(strResourcePlist);
                Director::getInstance()->getTextureCache()->removeTextureForKey(strResourceImage);
            }
        }
    }
    
    // Eliminamos los sonidos
    TTAudioManager::getInstance()->unLoadActualWorldEffects();
    
    //Eliminamos las UI de juego
    TTUIBonus::destroyInstance();
    TTGameUIManager::destroyInstance();
    
}

bool TTGameManager::loadGame()
{
    CCLOG("Inicia carga de recursos de la partida");
    
    // Resetea el juego
    _game->reset();
    
    // Parsea el XML del juego y crea las estructuras de datos
    configWorld* world = &TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld];
    std::string actualLevel = TTConfigParser::getInstance()->getLevelStr(TTUserInfo::getInstance()->_userData->_selectedLevel);
    _game->_gameConfig = TTLevelParser::getInstance()->parseLevel(actualLevel);
    
    //poner los valores por defecto de scores en el gameconfig
    if(_game->_gameConfig->_scoreDetahFigure==0)
    {
        _game->_gameConfig->_scoreDetahFigure=world->_scoreDetahFigure;
    }
    if(_game->_gameConfig->_scoreDetahCombo==0)
    {
        _game->_gameConfig->_scoreDetahCombo=world->_scoreDetahCombo;
    }
    if(_game->_gameConfig->_scoreDetahComboColor==0)
    {
        _game->_gameConfig->_scoreDetahComboColor=world->_scoreDetahComboColor;
    }
    if(_game->_gameConfig->_scoreOvni==0)
    {
        _game->_gameConfig->_scoreOvni=world->_scoreOvni;
    }
    if(_game->_gameConfig->_scoreCreateCombo==0)
    {
        _game->_gameConfig->_scoreCreateCombo=world->_scoreCreateCombo;
    }
    if(_game->_gameConfig->_scoreCreateComboColor==0)
    {
        _game->_gameConfig->_scoreCreateComboColor=world->_scoreCreateComboColor;
    }
    if(_game->_gameConfig->_scoreBonusCatch==0)
    {
        _game->_gameConfig->_scoreBonusCatch=world->_scoreBonusCatch;
    }
    if(_game->_gameConfig->_scoreBonusNoCatch==0)
    {
        _game->_gameConfig->_scoreBonusNoCatch=world->_scoreBonusNoCatch;
    }
    
    // Cargamos las texturas del mundo
    std::map<std::string,std::vector<std::string> > _worldResources = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->getActualUserProgress()._actualWorld]._resources;
    for(auto iterator = _worldResources.begin(); iterator != _worldResources.end(); iterator++)
    {
        if(iterator->first == "spriteSheet")
        {
            totalResourcesToLoad = iterator->second.size();
            for(int i = 0; i < iterator->second.size(); i++)
            {
                std::string resourceName = iterator->second[i];
                // Load image
                char strResourceImage[100] = {0};
                sprintf(strResourceImage,"%s.png",resourceName.c_str());
                CCLOG("Cargando recurso: %s",strResourceImage);
                Director::getInstance()->getTextureCache()->addImageAsync(strResourceImage, CC_CALLBACK_1(TTGameManager::resourceLoaded, this));
            }
        }
    }
    
    // Cargamos las texturas del paquete gráfico de carácteres
    int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld]._charactersPack;
    std::map<std::string,std::vector<std::string> > _charactersResources = TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._graphs;
    for(auto iterator = _charactersResources.begin(); iterator != _charactersResources.end(); iterator++)
    {
        if(iterator->first == "spriteSheet")
        {
            totalResourcesToLoad += iterator->second.size();
            for(int i = 0; i < iterator->second.size(); i++)
            {
                std::string resourceName = iterator->second[i];
                // Load image
                char strResourceImage[100] = {0};
                sprintf(strResourceImage,"%s.png",resourceName.c_str());
                CCLOG("Cargando recurso: %s",strResourceImage);
                Director::getInstance()->getTextureCache()->addImageAsync(strResourceImage, CC_CALLBACK_1(TTGameManager::resourceLoaded, this));
            }
        }
    }

    // Cargamos el background
    totalResourcesToLoad++;
    Director::getInstance()->getTextureCache()->addImageAsync(_game->_gameConfig->boardInit.background, CC_CALLBACK_1(TTGameManager::backgroundBoardLoaded, this));

    return true;
}

void TTGameManager::resourceLoaded(Texture2D* texture)
{
    SpriteBatchNode* gameBatchNode = SpriteBatchNode::createWithTexture(texture);
    _parentLayer->addChild(gameBatchNode);
    resourcesLoadControl();
}

void TTGameManager::backgroundBoardLoaded(Texture2D* texture)
{
    TTBoard::getInstance()->setBackgroundBoard(texture);
    resourcesLoadControl();
}

void TTGameManager::resourcesLoadControl()
{
    totalResourcesToLoad--;
    if(totalResourcesToLoad != 0)
        return;
    
    // Reproducimos la múscia de nivel
    TTAudioManager::getInstance()->playAudio("GAME_MUSIC",true);
    
    std::map<std::string,std::vector<std::string> > _worldResources = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld]._resources;
    
    // Cargamos los recursos del mundo  - spriteSheets
    auto iteratorSS = _worldResources.find("spriteSheet");
    if(iteratorSS != _worldResources.end())
    {
        for(int i = 0; i < iteratorSS->second.size(); i++)
        {
            std::string resourceName = iteratorSS->second[i];
            // Load plist
            char strResourceImage[100] = {0};
            sprintf(strResourceImage,"%s.png",resourceName.c_str());
            char strResourcePlist[100] = {0};
            sprintf(strResourcePlist,"%s.plist",resourceName.c_str());
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(strResourcePlist,strResourceImage);
        }
    }
    
    // Cargamos los recursos del mundo  - animaciones
    auto iteratorAS = _worldResources.find("animationSheet");
    if(iteratorAS != _worldResources.end())
    {
        for(int i = 0; i < iteratorAS->second.size(); i++)
        {
            // Load plist
            std::string resourceName = iteratorAS->second[i];
            char strAnimPlist[100] = {0};
            sprintf(strAnimPlist,"%s.plist",resourceName.c_str());
            AnimationCache::getInstance()->addAnimationsWithFile(strAnimPlist);
        }
    }

    // Cargamos los recursos del mundo  - particulas
    auto iteratorP = _worldResources.find("particle");
    if(iteratorP != _worldResources.end())
    {
        for(int i = 0; i < iteratorP->second.size(); i++)
        {
            // Load plist
            std::string resourceName = iteratorP->second[i];
            TTParticleManager::getInstance()->loadParticle(resourceName);
        }
    }
    
    // Cargamos los recursos de los personajes
    int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld]._charactersPack;
    std::map<std::string,std::vector<std::string> > _charactersResources = TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._graphs;
    
    auto iteratorSSCharacters = _charactersResources.find("spriteSheet");
    if(iteratorSSCharacters != _charactersResources.end())
    {
        for(int i = 0; i < iteratorSSCharacters->second.size(); i++)
        {
            std::string resourceName = iteratorSSCharacters->second[i];
            // Load plist
            char strResourceImage[100] = {0};
            sprintf(strResourceImage,"%s.png",resourceName.c_str());
            char strResourcePlist[100] = {0};
            sprintf(strResourcePlist,"%s.plist",resourceName.c_str());
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(strResourcePlist,strResourceImage);
        }
    }

    auto iteratorASCharacters = _charactersResources.find("animationSheet");
    if(iteratorASCharacters != _charactersResources.end())
    {
        for(int i = 0; i < iteratorASCharacters->second.size(); i++)
        {
            // Load plist
            std::string resourceName = iteratorASCharacters->second[i];
            char strAnimPlist[100] = {0};
            sprintf(strAnimPlist,"%s.plist",resourceName.c_str());
            AnimationCache::getInstance()->addAnimationsWithFile(strAnimPlist);
        }
    }
    
    // Cargamos las UI
    TTUIBonus::getInstance();
    TTGameUIManager::getInstance();
    
    // Cargamos los sonidos del mundo actual
    TTAudioManager::getInstance()->loadActualWorldEffects();
    
    CCLOG("Cargados todos los recursos de la partida");
    initGame();
    sendEvent(EVENT_GAME_LOADED);
}

void TTGameManager::initGame()
{
    // Enviamos evento de analitica a Parse
    std::map<std::string,std::string> _dimensions = {{"Level_ID",std::to_string(_game->_gameConfig->level)}};
    TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_LEVEL_INIT,_dimensions);
    
    // Inicializamos tablero
    TTBoard::getInstance()->initBoard(_game->_gameConfig->boardInit,_game->_gameConfig->goals);
    
    // Inicializamos el juego
    _game->initGame();
    
    TTBoostersManager::getInstance()->applyActiveBoardBoosters();
}

void TTGameManager::showGame()
{
    // Visible el printer de la UI
    TTGameUIManager::getInstance()->showPrinter(_game->_gameConfig,_parentLayer);
    
    // Visible el tablero
    TTBoard::getInstance()->showBoard();

    // Verifica si hay que mostrar el tutorial
    if(TTHowToInGameManager::getInstance()->hasTutorial(_game->_gameConfig->level) && _game->_gameConfig->level > TTUserInfo::getInstance()->_userData->_levelStars.size())
    {
        //TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
        //TTUIPopup::getInstance()->showHowToPopup(_parentLayer,_game->_gameConfig->howToPages, false);
        
        // Llamada al nuevo sistema de HowToPlayInGame
        CCLOG("El nivel tiene tutorial");
        TTHowToInGameManager::getInstance()->showTutorial(_game->_gameConfig->level);
        TTBoard::getInstance()->stopShowHelpCombination();
    }
    
    _inGame = true;
    _waitingForFinishBoardSteps = false;
}

void TTGameManager::update(float dt)
{
    if(!_inGame)
        return;
        
    TTBoard::getInstance()->update(dt);
    _game->update(dt);
}

void TTGameManager::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if (!_inGame)
        return;
    
    if (!TTUIStackManager::getInstance()->isUIOnTop(UI_INGAME))
    {
        return;
    }

    if(!_pause)
    {
        TTBoard::getInstance()->selectBoardCells(touches[0]->getLocation().x,touches[0]->getLocation().y);
        return;
    }
    // Verifica la activación de boosters
    TTBoostersManager::getInstance()->verifyBoostersSelection(Point(touches[0]->getLocation().x,touches[0]->getLocation().y));
}

void TTGameManager::onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if (!_inGame )
    {
        return;
    }
    
    if (!TTUIStackManager::getInstance()->isUIOnTop(UI_INGAME))
    {
        return;
    }

    if (_pause)
    {
        // Verifica la activación de boosters
        TTBoostersManager::getInstance()->verifyBoostersSelection(Point(touches[0]->getLocation().x,touches[0]->getLocation().y));
    }
    else
    {
        TTBoard::getInstance()->selectBoardCells(touches[0]->getLocation().x,touches[0]->getLocation().y);
    }
}

void TTGameManager::onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    if (!_inGame)
        return;

    if (!TTUIStackManager::getInstance()->isUIOnTop(UI_INGAME))
    {
        return;
    }

    if(_pause)
    {
        // Verifica si has dejado de presionar una casilla para aplicar un booster
        TTBoostersManager::getInstance()->releaseBoostersSelection();
    }
    
    TTBoard::getInstance()->finishSelectBoardCells();
}

void TTGameManager::onEnter()
{
    Layer::onEnter();
    log("Scene GameScene onEnter");
    showGame();
}

void TTGameManager::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    log("Scene GameScene onEnterTransitionDidFinish");
}

void TTGameManager::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    log("Scene GameScene onExitTransitionDidStart");
}

void TTGameManager::onExit()
{
    Layer::onExit();
    log("Scene GameScene onExit");
}

void TTGameManager::onEvent_FinishGame(EventCustom* e)
{
    if (!_inGame)
    {
        //para evitar que lleguen mas de un evento de finish
        return;
    }
    _inGame = false;
    
    // finalización de tablero
    if(TTBoard::getInstance()->_stepsManager->isRunningSteps())
    {
        //Esperamos la finalización de los pasos del tablero
        _waitingForFinishBoardSteps = true;
    }
    else
    {
        onGameFinish();
    }
}

void TTGameManager::onGameFinish()
{
    // Finalizamos el tablero y  esperamos su respuesta para finalizar el juego
    int _restrictionBonus = 0;
    if(_game->_gameConfig->timeRestriction != NULL)
    {
        //Un animal por cada 2 segundos que han sobrado de juego
        _restrictionBonus = (int)_game->_gameConfig->timeRestriction->getTimeToFinish()/2;
    }
    else if(_game->_gameConfig->moveRestriction != NULL)
    {
        _restrictionBonus = (_game->_gameConfig->moveRestriction->getLimitMoves()-_game->_gameConfig->moveRestriction->_currMoves);
    }
    
    // Enviamos las estadísticas de nivel a Parse
    TTParseManager::getInstance()->setLevelStatistics(_game->_gameConfig->level,_game->_gameConfig->winGame);
    
    if(_game->_gameConfig->winGame)
    {
        TTUserLives::getInstance()->restoreActualLive();
        TTBoard::getInstance()->finishBoardControl(_restrictionBonus);
    }
    else
    {
        //Cuando perdemos, antes de mostrar el cartel resumen ponemos un Delay de 0.9 segundos 
        DelayTime* _delayEnterMenu = DelayTime::create(0.9f);
        
        CallFunc *showResumeFunc = CallFunc::create(std::bind([&](){
            TTUIStackManager::getInstance()->goToNewUI(UI_GAME_MENU_RESUME);
            TTGameMenus::getInstance()->showResumeMenu(_game->_gameConfig,_parentLayer);
        }));
        runAction(Sequence::create(_delayEnterMenu,showResumeFunc, NULL));
    }
}

void TTGameManager::onEvent_ContinueBuyGame(EventCustom* e)
{
    _inGame = true;
}

void TTGameManager::onEvent_FinishedBoard(EventCustom* e)
{
    // Finalizamos la UI
    TTGameUIManager::getInstance()->finishPrinter();
    
    //Paramos la restriccion de tiempo
    if(_game->_gameConfig->timeRestriction != NULL)
        _game->_gameConfig->timeRestriction->reset();
    
    // Guardamos la información del nivel en la información de usuario
    updateUserInfo();
    
    //Mostramos el panel de resumen
    TTGameMenus::getInstance()->showResumeMenu(_game->_gameConfig,_parentLayer);
}

void TTGameManager::updateUserInfo()
{
    // Seteamos las estrellas por nivel
    int stars = 0;
    for(int i = 0; i < _game->_gameConfig->scoreStars.size(); i++)
    {
        if(_game->_gameConfig->scoreGame >= _game->_gameConfig->scoreStars[i])
            stars++;
    }
    
    
    // Seteamos el progreso
    if(TTUserInfo::getInstance()->_userData->_levelStars.size() > TTUserInfo::getInstance()->_userData->_selectedLevel)
    {
        if(TTUserInfo::getInstance()->_userData->_levelStars[TTUserInfo::getInstance()->_userData->_selectedLevel] < stars)
        {
            TTUserInfo::getInstance()->_userData->_levelStars[TTUserInfo::getInstance()->_userData->_selectedLevel] = stars;
            TTUserInfo::getInstance()->saveData();
        }
    }
    else
    {
        int _actualLevel = TTUserInfo::getInstance()->getActualUserProgress()._actualLevel;
        int _totalWorldLevels = 0;
        for(int i = 0; i < TTConfigParser::getInstance()->_generalConfig->_worlds.size(); i++)
        {
            _totalWorldLevels += TTConfigParser::getInstance()->_generalConfig->_worlds[i]._levels.size();
        }
        
        if(_actualLevel >= (_totalWorldLevels-1))
        {
            //No hay mas mundos
            TTUserInfo::getInstance()->_userDataVolatile.newLevelOpen = false;
            TTParseManager::getInstance()->setUserActualLevel(_actualLevel);
        }
        else
        {
            TTUserInfo::getInstance()->_userDataVolatile.newLevelOpen = true;
            TTParseManager::getInstance()->setUserActualLevel(_actualLevel);
            _actualLevel++;
        }
        
        TTUserInfo::getInstance()->_userData->_levelStars.push_back(stars);
        TTUserInfo::getInstance()->saveData();
    }

    // Seteamos en Parse el nivel anterior
    TTParseManager::getInstance()->storeUserLevelInfo(TTUserInfo::getInstance()->_userData->_selectedLevel,_game->_gameConfig->scoreGame,stars,_game->_gameConfig->moveRestriction->_currMoves,_game->_gameConfig->currentTime);
}

void TTGameManager::onEvent_RetryGame(EventCustom* e)
{
    //Resetea el stack de UI
    TTUIStackManager::getInstance()->clearStack();
    
    // Resetea la estructura del juego
    _game->_gameConfig->reset();
    
    // Inicia el juego
    initGame();
    
    // Muestra el nuevo juego
    showGame();
}

void TTGameManager::onEvent_pauseGame(EventCustom* e)
{
    _pause = true;
}

void TTGameManager::onEvent_continueGame(EventCustom* e)
{
    _pause = false;
}

void TTGameManager::onEvent_boardStepsFinished(EventCustom* e)
{
    if(_waitingForFinishBoardSteps)
    {
        onGameFinish();
        _waitingForFinishBoardSteps = false;
    }
}

void TTGameManager::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        //Matamos la UI abierta, si la hay
        TTUIStackManager::getInstance()->killTopUI();
    }
#endif
}
