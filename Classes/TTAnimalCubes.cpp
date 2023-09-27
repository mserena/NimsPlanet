#include "TTAnimalCubes.h"
#include "TTConfigParser.h"
#include "TTUserInfo.h"
#include "TTBoostersManager.h"
#include "TTUILoading.h"
#include "TTGameMenus.h"
#include "TTUIPopup.h"
#include "TTUIPopupSmart.h"
#include "TTParticleManager.h"
#include "TTGameBonus.h"
#include "TTUIStackManager.h"
#include "TTAudioManager.h"
#include "TTFacebook.h"
#include "TTNotificationManager.h"
#include "TTParseManager.h"

//Scenes
#include "Layers/TTMainMenuLayer.h"
#include "Layers/TTMapLayer.h"
#include "Layers/TTLoadingLayer.h"
#include "Layers/TTSplashLayer.h"

//Singleton
static TTAnimalCubes *_sharedAnimalCubes = nullptr;

TTAnimalCubes* TTAnimalCubes::getInstance()
{
    if (!_sharedAnimalCubes)
    {
        _sharedAnimalCubes = new TTAnimalCubes();
        _sharedAnimalCubes->init();
    }
    
    return _sharedAnimalCubes;
}

void TTAnimalCubes::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedAnimalCubes);
}

TTAnimalCubes::~TTAnimalCubes()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTAnimalCubes::init()
{
    // Registro de recepcion de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_SCENE_ON_LOADING,CC_CALLBACK_1(TTAnimalCubes::onEvent_onLoadingScene,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_LOADED,CC_CALLBACK_1(TTAnimalCubes::onEvent_onGameLoaded,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_MAP_LOADED,CC_CALLBACK_1(TTAnimalCubes::onEvent_onMapLoaded,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_MAIN_MENU_LOADED,CC_CALLBACK_1(TTAnimalCubes::onEvent_onMainMenuLoaded,this)));
    
    // Mostramos el splash screen
    _generalScene = Scene::create();
    Director::getInstance()->runWithScene(_generalScene);
    
    _gameState = GAME_STATE_SPLASH;
    _splashLayer = new TTSplashLayer();
    _generalScene->addChild(_splashLayer);
    
    //Al entrar borramos las notificaciones locales lanzadas.
    TTNotificationManager::getInstance()->cancel();
}

void TTAnimalCubes::loadAll()
{
    // Cargamos los recursos generales
    CCLOG("Cargando recursos generales");
    configGeneral* generalConf = TTConfigParser::getInstance()->_generalConfig;
    typedef std::map<std::string,std::vector<std::string> >::iterator it_generalResources;
    for(it_generalResources iterator = generalConf->_generalResources.begin(); iterator != generalConf->_generalResources.end(); iterator++)
    {
        if(iterator->first == "spriteSheet")
        {
            for(int i = 0; i < iterator->second.size(); i++)
            {
                std::string resourceName = iterator->second[i];
                // Load image
                char strResourceImage[100] = {0};
                sprintf(strResourceImage,"%s.png",resourceName.c_str());
                Director::getInstance()->getTextureCache()->addImage(strResourceImage);
                // Load plist
                char strResourcePlist[100] = {0};
                sprintf(strResourcePlist,"%s.plist",resourceName.c_str());
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(strResourcePlist);
            }
        }
        else if(iterator->first == "particle")
        {
            for(int i = 0; i < iterator->second.size(); i++)
            {
                // Load plist
                std::string resourceName = iterator->second[i];
                TTParticleManager::getInstance()->loadParticle(resourceName);
            }
        }
        else if(iterator->first == "animationSheet")
        {
            for(int i = 0; i < iterator->second.size(); i++)
            {
                // Load plist
                std::string resourceName = iterator->second[i];
                char strAnimPlist[100] = {0};
                sprintf(strAnimPlist,"%s.plist",resourceName.c_str());
                AnimationCache::getInstance()->addAnimationsWithFile(strAnimPlist);
            }
        }
    }
    
    // Inicializamos facebook
    TTFacebook::getInstance();

    // Cargamos los boosters
    TTBoostersManager::getInstance()->loadBoosters();
    
    // Inicialización de las UI
    TTGameBonus::getInstance();
    TTGameMenus::getInstance();
    TTUIPopup::getInstance();
    TTUIPopupSmart::getInstance();
    TTUILoading::getInstance();
    
    // Inicializamos el audio
    TTAudioManager::getInstance();
    
    // Cambiamos al estado inicial
    setState(GAME_STATE_MAINMENU);
}

void TTAnimalCubes::update(float dt)
{

}

Layer* TTAnimalCubes::getActualLayer()
{
    // Eliminamos el estado anterior
    switch(_gameState)
    {
        case GAME_STATE_SPLASH:
        {
            return _splashLayer;
        }
        break;
        case GAME_STATE_MAINMENU:
        {
            return _mainMenuLayer;
        }
        break;
        case GAME_STATE_INGAME:
        {
            return _gameLayer;
        }
        break;
        case GAME_STATE_MAP:
        {
            return _mapLayer;
        }
        break;
        default:
        break;
    }
    
}

void TTAnimalCubes::setState(GAME_STATE gameState)
{
    // Limpiamos el stack de menus
    TTUIStackManager::getInstance()->clearStack();
    
    if(_gameState == gameState)
        CCLOG("Se intenta setear el mismo estado en el que ya estamos.");
    
    // Eliminamos el estado anterior
    switch(_gameState)
    {
        case GAME_STATE_SPLASH:
        {
            _generalScene->removeChild(_splashLayer);
            _splashLayer->release();
        }
        break;
        case GAME_STATE_MAINMENU:
        {
            _generalScene->removeChild(_mainMenuLayer);
            _mainMenuLayer->release();
        }
        break;
        case GAME_STATE_INGAME:
        {
            _generalScene->removeChild(_gameLayer);
            _gameLayer->release();
        }
        break;
        case GAME_STATE_MAP:
        {
            _generalScene->removeChild(_mapLayer);
            _mapLayer->release();
        }
        break;
        default:
        break;
    }
    
    // Seteamos el estado del juego
    _gameState = gameState;
    
    // Switch a la layer de loading
    _loadingLayer = new TTLoadingLayer();
    _generalScene->addChild(_loadingLayer);
}

void TTAnimalCubes::onEvent_onLoadingScene(EventCustom* e)
{
    switch(_gameState)
     {
         case GAME_STATE_SPLASH:
         {
             // Creamos la instancia del juego
             _splashLayer = new TTSplashLayer();
         }
        break;
        case GAME_STATE_MAINMENU:
        {
            // Creamos la instancia del juego
            _mainMenuLayer = new TTMainMenuLayer();
        }
        break;
        case GAME_STATE_INGAME:
        {
            // Creamos la instancia del juego
            _gameLayer = new TTGameManager();
        }
        break;
        case GAME_STATE_MAP:
        {
            // Creamos la instancia del mapa
            _mapLayer = new TTMapLayer();
        }
        break;
        default:
            CCLOG("Se intenta cambiar a estado no implementado. Finalizamos app.");
            abort();
        break;
     }
}

void TTAnimalCubes::onEvent_onGameLoaded(EventCustom* e)
{
    // Cambiamos a la escena de juego
    _generalScene->removeChild(_loadingLayer);
    _loadingLayer->release();
    _generalScene->addChild(_gameLayer);
}

void TTAnimalCubes::onEvent_onMapLoaded(EventCustom* e)
{
    // Cambiamos a la escena de mapa
    _generalScene->removeChild(_loadingLayer);
    _loadingLayer->release();
    _generalScene->addChild(_mapLayer);
}

void TTAnimalCubes::onEvent_onMainMenuLoaded(EventCustom* e)
{
    // Cambiamos a la escena de mapa
    _generalScene->removeChild(_loadingLayer);
    _loadingLayer->release();
    _generalScene->addChild(_mainMenuLayer);
}
