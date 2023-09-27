#include "TTMapLayer.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTGameMenus.h"

#define WIDTH_UI_LOADING_MARGIN 10

TTMapLayer::TTMapLayer()
{
    _mapMgr = new TTMapManager(this);
    
    // activate touches on this scene
    _touchListener = EventListenerTouchAllAtOnce::create();
    _touchListener->onTouchesBegan = CC_CALLBACK_2(TTMapLayer::onTouchesBegan, this);
    _touchListener->onTouchesMoved = CC_CALLBACK_2(TTMapLayer::onTouchesMoved, this);
    _touchListener->onTouchesEnded = CC_CALLBACK_2(TTMapLayer::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

    this->setKeypadEnabled(true);
    // agregamos la escena al update
    scheduleUpdate();
}

TTMapLayer::~TTMapLayer()
{
    CCLOG("Se destruye la escena del mapa.");
    _eventDispatcher->removeEventListener(_touchListener);
    unscheduleUpdate();
    delete(_mapMgr);
}

void TTMapLayer::update( float dt )
{
    _mapMgr->update(dt);
}

void TTMapLayer::onEnter()
{
    Layer::onEnter();
    log("Scene TTMapLayer onEnter");
}

void TTMapLayer::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    log("Scene TTMapLayer onEnterTransitionDidFinish");
}

void TTMapLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    log("Scene TTMapLayer onExitTransitionDidStart");
}

void TTMapLayer::onExit()
{
    Layer::onExit();
    log("Scene TTMapLayer onExit");
}

void TTMapLayer::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    _mapMgr->onTouchesBegan(touches,event);
}

void TTMapLayer::onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    _mapMgr->onTouchesMoved(touches,event);
}

void TTMapLayer::onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    _mapMgr->onTouchesEnded(touches,event);
}

void TTMapLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        if(!_mapMgr->_pauseMap && TTUIStackManager::getInstance()->isUIOnTop(UI_MAP))
        {
            //Mostramos cierre de la aplicación desde el mapa
            TTGameMenus::getInstance()->showExitMenu(this,true);
        }
        else
        {
            //Matamos la UI abierta, si la hay
            TTUIStackManager::getInstance()->killTopUI();
        }
    }
#endif
}
