#include "TTMainMenuLayer.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTFacebook.h"
#include "TTUserInfo.h"
#include "TTStoreManager.h"
#include "TTUIStackManager.h"
#include "TTParseManager.h"

#define RESOURCES_TO_LOAD   3

TTMainMenuLayer::TTMainMenuLayer()
{
    // agregamos la escena al update
    scheduleUpdate();
    
    TTEventsManager::getInstance()->addListenerEvent("MAINMENULAYER", EVENT_FACEBOOK_CONNECTION_REFRESH_BUTTON, CC_CALLBACK_1(TTMainMenuLayer::onFacebookConnection,this));
    TTEventsManager::getInstance()->addListenerEvent("MAINMENULAYER", EVENT_FACEBOOK_CONNECTION, CC_CALLBACK_1(TTMainMenuLayer::onFacebookConnection,this));
    
    // Cargamos los recursos
    _resourcesToLoad = RESOURCES_TO_LOAD;
    loadResources();
}


TTMainMenuLayer::~TTMainMenuLayer()
{
    CCLOG("Se destruye la escena del menú principal.");
    unscheduleUpdate();
    
    TTEventsManager::getInstance()->removeSafeListenerEvents("MAINMENULAYER");
    
    // Quitamos la música
    TTAudioManager::getInstance()->stopAudio("MAP_MUSIC");
    
    //Eliminamos los recursos
    Director::getInstance()->getTextureCache()->removeTextureForKey("bg_MainMenu.jpg");
    Director::getInstance()->getTextureCache()->removeTextureForKey("background_animals.png");
    Director::getInstance()->getTextureCache()->removeTextureForKey("background_logo.png");
    Director::getInstance()->getTextureCache()->removeTextureForKey("background_play.png");
    Director::getInstance()->getTextureCache()->removeTextureForKey("background_facebook.png");
    Director::getInstance()->getTextureCache()->removeTextureForKey("background_like_facebook.png");
    Director::getInstance()->getTextureCache()->removeTextureForKey("5coins.png");
    this->removeAllChildrenWithCleanup(true);
}

void TTMainMenuLayer::loadResources()
{
    // Cargamos los recursos gráficos
    auto _funcLoadBg = std::bind([&](){
        Size _screen = Director::getInstance()->getWinSize();
        _bgMainMenu = Sprite::createWithTexture(Director::getInstance()->getTextureCache()->getTextureForKey("bg_MainMenu.jpg"));
        _bgMainMenu->setAnchorPoint(Point(0.5,0.5));
        _bgMainMenu->setPosition(Point(_screen.width/2,_screen.height/2));
        this->addChild(_bgMainMenu,1);
        resourcesLoadedControl();
    });
    Director::getInstance()->getTextureCache()->addImageAsync("bg_MainMenu.jpg", _funcLoadBg);

    auto _funcLoadBgAnimals = std::bind([&](){
        Size _screen = Director::getInstance()->getWinSize();
        _animalsMainMenu = Sprite::createWithTexture(Director::getInstance()->getTextureCache()->getTextureForKey("background_animals.png"));
        _animalsMainMenu->setAnchorPoint(Point(0.5,0.5));
        _animalsMainMenu->setPosition(Point(_screen.width/2,_screen.height/2));
        this->addChild(_animalsMainMenu,3);
        resourcesLoadedControl();
    });
    Director::getInstance()->getTextureCache()->addImageAsync("background_animals.png", _funcLoadBgAnimals);
    
    auto _funcLoadBgLogo = std::bind([&](){
        Size _screen = Director::getInstance()->getWinSize();
        _logoMainMenu = Sprite::createWithTexture(Director::getInstance()->getTextureCache()->getTextureForKey("background_logo.png"));
        _logoMainMenu->setAnchorPoint(Point(0.5,0.5));
        _logoMainMenu->setPosition(Point(_screen.width/2,_screen.height*0.72));
        this->addChild(_logoMainMenu,4);
        resourcesLoadedControl();
    });
    Director::getInstance()->getTextureCache()->addImageAsync("background_logo.png", _funcLoadBgLogo);
    
    // Creamos los botones
    Size _screen = Director::getInstance()->getWinSize();
    _btnPlay = cocos2d::ui::Button::create("background_play.png");
    _btnPlay->setPosition(Point(_screen.width/2,_screen.height*0.40));
    _playLabel = cocos2d::ui::TextBMFont::create(TTLanguageManager::getInstance()->getTag(PLAY_TAG),"markerfeltwide.fnt");
    _playLabel->setPosition(Point(_btnPlay->getContentSize().width/2,_btnPlay->getContentSize().height/2));
    _playLabel->setScale((_btnPlay->getContentSize().height*0.85)/_playLabel->getContentSize().height);
    _btnPlay->addChild(_playLabel);
    _btnPlay->addTouchEventListener(CC_CALLBACK_2(TTMainMenuLayer::onTouchPlayBtn,this));
    this->addChild(_btnPlay,4);
    
    _btnFacebook = cocos2d::ui::Button::create("background_facebook.png");
    _btnFacebook->setPosition(Point(_screen.width/2,_screen.height*0.28));
    std::string _buttonLabel = "";
    if (TTFacebook::getInstance()->isConnected())
    {
        _buttonLabel = TTLanguageManager::getInstance()->getTag(DISCONNECT_TAG);
    }
    else
    {
        _buttonLabel = TTLanguageManager::getInstance()->getTag(CONNECT_TAG);
    }

    _facebookLabel = cocos2d::ui::TextBMFont::create(_buttonLabel,"markerfeltwide.fnt");
    _facebookLabel->setPosition(Point(_btnFacebook->getContentSize().width*0.4,_btnFacebook->getContentSize().height/2));
    _facebookLabel->setScale((_btnFacebook->getContentSize().height*0.55)/_facebookLabel->getContentSize().height);
    _btnFacebook->addChild(_facebookLabel);
    _btnFacebook->addTouchEventListener(CC_CALLBACK_2(TTMainMenuLayer::onTouchFacebookBtn,this));
    this->addChild(_btnFacebook,4);
    
    _btnFacebookLike = cocos2d::ui::Button::create("background_like_facebook.png");
    _btnFacebookLike->addTouchEventListener(CC_CALLBACK_2(TTMainMenuLayer::onTouchFacebookLikeBtn,this));
    _btnFacebook->addChild(_btnFacebookLike,1);
    _btnFacebookLike->setScale(0.8);
    _btnFacebookLike->setAnchorPoint(Point(1.0f,0.5f));
    _btnFacebookLike->setPosition(Point(-_btnFacebookLike->getContentSize().width/6,_btnFacebook->getContentSize().height/2));
}

void TTMainMenuLayer::resourcesLoadedControl()
{
    _resourcesToLoad--;
    if(_resourcesToLoad != 0) return;
    
    // Seteamos la escala
    Size _screen = Director::getInstance()->getWinSize();
    _bgMainMenu->setScale(_screen.height/_bgMainMenu->getContentSize().height);
    _animalsMainMenu->setScale(_screen.height/_bgMainMenu->getContentSize().height);
    _logoMainMenu->setScale(_screen.height/_bgMainMenu->getContentSize().height);
    _btnPlay->setScale(_screen.height/_bgMainMenu->getContentSize().height);
    _btnFacebook->setScale(_screen.height/_bgMainMenu->getContentSize().height);
    
    auto _scaleUp = ScaleTo::create(1,(_screen.height/_bgMainMenu->getContentSize().height)+0.05f);
    auto _scaleDown = ScaleTo::create(1,(_screen.height/_bgMainMenu->getContentSize().height)-0.05f);
    _btnPlay->runAction(RepeatForever::create(Sequence::create(_scaleUp,_scaleDown, NULL)));
    
    // Todos los recursos han sido cargados
    sendEvent(EVENT_MAIN_MENU_LOADED);
}

void TTMainMenuLayer::showMainMenu()
{
    TTAudioManager::getInstance()->playAudio("MAP_MUSIC",true);
    
    _bgMainMenu->setVisible(true);
    _logoMainMenu->setVisible(true);
    _animalsMainMenu->setVisible(true);
    
    _animalsMainMenu->setOpacity(0);
    auto _fadeIn = FadeTo::create(0.5f,250);
    _animalsMainMenu->runAction(_fadeIn);
}

void TTMainMenuLayer::onTouchPlayBtn(Ref *sender,ui::Widget::TouchEventType touchEvent)
{
    if (touchEvent != ui::Widget::TouchEventType::ENDED)
        return;
    
    if (!TTUIStackManager::getInstance()->isStackEmpty())
    {
        return;
    }

    TTAudioManager::getInstance()->playAudio("BUTTON_PLAY");
    
    // Nos vamos al mapa
    TTAnimalCubes::getInstance()->setState(GAME_STATE_MAP);
}

void TTMainMenuLayer::onTouchFacebookBtn(Ref *sender,ui::Widget::TouchEventType touchEvent)
{
    if (touchEvent != ui::Widget::TouchEventType::ENDED)
        return;
    
    if (!TTUIStackManager::getInstance()->isStackEmpty())
    {
        return;
    }
    
    TTAudioManager::getInstance()->playAudio("BUTTON_PLAY");

    if (TTFacebook::getInstance()->isConnected())
    {
        TTParseManager::getInstance()->disconnectUserToFacebook();
    }
    else
    {
        TTFacebook::getInstance()->connectUserToFacebook();
    }
}

void TTMainMenuLayer::onTouchFacebookLikeBtn(Ref *sender,ui::Widget::TouchEventType touchEvent)
{
    if (touchEvent != ui::Widget::TouchEventType::ENDED)
        return;
    
    if (!TTUIStackManager::getInstance()->isStackEmpty())
    {
        return;
    }
    
    TTAudioManager::getInstance()->playAudio("BUTTON_PLAY");
    
    // Marcamos la variable de like
    if(!TTUserInfo::getInstance()->_userData->_likeOnFacebook)
    {
        // Marcamos como like
        TTUserInfo::getInstance()->_userData->_likeOnFacebook = true;
        TTUserInfo::getInstance()->saveData();
        
        // Enviamos evento de analitica a Parse
        std::map<std::string,std::string> _dimensions;
        TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_LIKE_FACEBOOK,_dimensions);
    }
    
    // Abrimos link en el navegador
    Application::getInstance()->openURL("http://www.facebook.com/NimsPlanet");
}

void TTMainMenuLayer::update( float dt )
{
}

void TTMainMenuLayer::onEnter()
{
    Layer::onEnter();
    log("Scene TTMainMenuLayer onEnter");
}

void TTMainMenuLayer::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    log("Scene TTMainMenuLayer onEnterTransitionDidFinish");
    
    showMainMenu();
}

void TTMainMenuLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    log("Scene TTMainMenuLayer onExitTransitionDidStart");
}

void TTMainMenuLayer::onExit()
{
    Layer::onExit();
    log("Scene TTMainMenuLayer onExit");
}


void TTMainMenuLayer::onFacebookConnection(EventCustom *event)
{
    CCLOG("onFacebookConnection Event");
    if (TTFacebook::getInstance()->isConnected())
    {
        _facebookLabel->setString(TTLanguageManager::getInstance()->getTag(DISCONNECT_TAG));
    }
    else
    {
        _facebookLabel->setString(TTLanguageManager::getInstance()->getTag(CONNECT_TAG));
    }
}
