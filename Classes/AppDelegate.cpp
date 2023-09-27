#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "TTAnimalCubes.h"
#include "TTNotificationManager.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    /* Resetea la semilla del random.
     * TODO: buscar un mejor sitio para hacerlo
     */
    unsigned int _seed;
    _seed = time(NULL);
    srand(_seed);
    CCLOG("seed: %u", _seed);
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview)
    {
        glview = GLView::create("AnimalCubes");
        director->setOpenGLView(glview);
    }
    
    std::string _folderResolution;
    int _screenWidth = Director::getInstance()->getWinSize().width;
    if(_screenWidth <= 480)
    {
        _folderResolution = "480";
    }
    else if(_screenWidth <= 720)
    {
        _folderResolution = "720";
    }
    else
    {
        _folderResolution = "1024";
    }
    
    // Agregamos los path para los recursos
    auto fileUtils = cocos2d::FileUtils::getInstance();
    std::vector<std::string> searchPaths;
    searchPaths.push_back("fonts");
    searchPaths.push_back("GUI");
    searchPaths.push_back("GUI/Fonts");
    searchPaths.push_back("GUI/"+_folderResolution);
    searchPaths.push_back("GUI/"+_folderResolution+"/HowToPlay");
    searchPaths.push_back("GUI/"+_folderResolution+"/MainMenu");
    
    searchPaths.push_back("Config");
    searchPaths.push_back("Config/Levels");
    searchPaths.push_back("Particle");
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    searchPaths.push_back("SoundiOS");
    searchPaths.push_back("SoundiOS/music");
    searchPaths.push_back("SoundiOS/effects");
#else
    searchPaths.push_back("SoundAndroid");
    searchPaths.push_back("SoundAndroid/music");
    searchPaths.push_back("SoundAndroid/effects");
#endif
    
    searchPaths.push_back("Worlds");
    searchPaths.push_back("Worlds/Map/"+_folderResolution);
    searchPaths.push_back("Worlds/Map/"+_folderResolution+"/Animations");
    searchPaths.push_back("Worlds/AllWorlds");
    searchPaths.push_back("Worlds/AllWorlds/"+_folderResolution);
    searchPaths.push_back("Worlds/World1/"+_folderResolution);
    searchPaths.push_back("Worlds/World1");
    searchPaths.push_back("Worlds/World2/"+_folderResolution);
    searchPaths.push_back("Worlds/World2");
    
    fileUtils->setSearchPaths(searchPaths);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0/60);
    
    // Inicializamos el juego
    TTAnimalCubes::getInstance();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause  
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    
    //Lanzamos notificaciones locales al entrar en background
    TTNotificationManager::getInstance()->show();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    
    //Borramos notificaciones locales al reanudar tras background
    TTNotificationManager::getInstance()->cancel();
}
