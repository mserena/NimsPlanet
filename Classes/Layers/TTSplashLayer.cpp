#include "TTSplashLayer.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"


TTSplashLayer::TTSplashLayer()
{
    // Cargamos los recursos
    Size _screen = Director::getInstance()->getWinSize();
    _bgSplash = Sprite::create("bg_MainMenu.jpg");
    _bgSplash->setAnchorPoint(Point(0.5,0.5));
    _bgSplash->setPosition(Point(_screen.width/2,_screen.height/2));
    _bgSplash->setScale(_screen.height/_bgSplash->getContentSize().height);
    this->addChild(_bgSplash,1);
    
    _logoSplash = Sprite::create("background_logo.png");
    _logoSplash->setAnchorPoint(Point(0.5,0.5));
    _logoSplash->setPosition(Point(_screen.width/2,_screen.height*0.72));
    _logoSplash->setScale(_screen.height/_bgSplash->getContentSize().height);
    this->addChild(_logoSplash,2);
    
    _labelSplash = cocos2d::ui::TextBMFont::create(TTLanguageManager::getInstance()->getTag(LOADING_TAG),"markerfeltwide.fnt");
    _labelSplash->setPosition(Point(_screen.width/2,_screen.height*0.50));
    
    //30% del ancho de la pantalla
    float _widthDesired = 30 *_screen.width/100;
    _labelSplash->setScale(_widthDesired/_labelSplash->getContentSize().width);
    this->addChild(_labelSplash,2);
    
    
    // Cargamos los recursos
    _bgSplashAndroid = Sprite::create("background_splash.jpg");
    _bgSplashAndroid->setAnchorPoint(Point(0.5,0.5));
    _bgSplashAndroid->setPosition(Point(_screen.width/2,_screen.height/2));
    _bgSplashAndroid->setScale(_screen.height/_bgSplashAndroid->getContentSize().height);
    _bgSplashAndroid->setVisible(false);
    this->addChild(_bgSplashAndroid,1);
    
    _logoSplashAndroid = Sprite::create("logo_splash.png");
    _logoSplashAndroid->setAnchorPoint(Point(1,0));
    _logoSplashAndroid->setPosition(Point(_screen.width,0));
    _logoSplashAndroid->setScale(_screen.height/_bgSplashAndroid->getContentSize().height);
    _logoSplashAndroid->setVisible(false);
    this->addChild(_logoSplashAndroid,1);
    
    
    // Mostramos el splash screen del logo en Android
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    TTSplashLayer::showSplashScreen();
    #endif
   
}

TTSplashLayer::~TTSplashLayer()
{
    CCLOG("Se destruye la escena del splash layer.");
    this->removeAllChildrenWithCleanup(true);
}

void TTSplashLayer::onEnter()
{
    Layer::onEnter();
    log("Scene TTSplashLayer onEnter");
}

void TTSplashLayer::onEnterTransitionDidFinish()
{
 
    Layer::onEnterTransitionDidFinish();
    log("Scene TTSplashLayer onEnterTransitionDidFinish");
    
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto _showLoading = CallFunc::create(std::bind([&](){
        TTSplashLayer::showLoading();
    }));
    
    //Splash screen en pantalla durante 1,5 segundos
    this->runAction(Sequence::create(DelayTime::create(1.5),_showLoading,NULL));
    
    #else
    // Empezamos la carga de recursos
    auto _initLoad = CallFunc::create(std::bind([&](){
        TTAnimalCubes::getInstance()->loadAll();
    }));
    this->runAction(_initLoad);
    
    #endif

}

void TTSplashLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    log("Scene TTSplashLayer onExitTransitionDidStart");
}

void TTSplashLayer::onExit()
{
    Layer::onExit();
    log("Scene TTSplashLayer onExit");
}

void TTSplashLayer::showSplashScreen()
{
    _bgSplash->setVisible(false);
    _logoSplash->setVisible(false);
    _labelSplash->setVisible(false);
    _bgSplashAndroid->setVisible(true);
    _logoSplashAndroid->setVisible(true);
}

void TTSplashLayer::showLoading()
{
    _bgSplash->setVisible(true);
    _logoSplash->setVisible(true);
    _labelSplash->setVisible(true);
    _bgSplashAndroid->setVisible(false);
    _logoSplashAndroid->setVisible(false);
    
    // Empezamos la carga de recursos
    auto _initLoad = CallFunc::create(std::bind([&](){
        TTAnimalCubes::getInstance()->loadAll();
    }));
    //Dejamos un delay de 0,1 segundos para que se muestren los cambios de setVisible
    this->runAction(Sequence::create(DelayTime::create(0.1),_initLoad,NULL));
}