//
//  TTUIPopupSmart.cpp
//  animalplanet
//
//  Created by Marc Serena on 05/12/14.
//
//

#include "TTUIPopupSmart.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTFacebook.h"

//Singleton
static TTUIPopupSmart *_sharedPopupSmartUI = nullptr;

TTUIPopupSmart* TTUIPopupSmart::getInstance()
{
    if (!_sharedPopupSmartUI)
    {
        _sharedPopupSmartUI = new TTUIPopupSmart();
        _sharedPopupSmartUI->init();
    }
    
    return _sharedPopupSmartUI;
}

void TTUIPopupSmart::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedPopupSmartUI);
}

void TTUIPopupSmart::init()
{
    _blockMenus = false;
    
    // Registramos los eventos
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_CONNECTION,CC_CALLBACK_1(TTUIPopupSmart::onEvent_FacebookConnect,this)));
    
    // Cargamos el XML de la UI Resume
    _uiPopupLayer = Layer::create();
    _uiPopupLayer->retain();
    _widgetPopup = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_POPUP_SMART.ExportJson");
    
    cocos2d::ui::Layout* _layoutPopup = dynamic_cast<cocos2d::ui::Layout*>(_widgetPopup);
    _uiPopupLayer->addChild(_layoutPopup);
    
    // Escalado Widget
    _widgetPopup->setScale(scale_widget(_widgetPopup,WIDTH_UI_POPUP_SMART_SPACE,HEIGHT_UI_POPUP_SMART_SPACE));
    _widgetPopup->setAnchorPoint(Point(0.5,0.5));
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetPopup->setPosition(Point(_screenSize.width/2,_screenSize.height/2));
    
    // Recuperamos los items que necesitamos popup compra boosters
    _popupTitle = (cocos2d::ui::TextBMFont*)_widgetPopup->getChildByName("bg_popup")->getChildByName("titlePopup");
    
    _referenceText = (cocos2d::ui::Text*)_widgetPopup->getChildByName("bg_popup")->getChildByName("textPopup");
    _referenceText->setVisible(false);
    _popupText = Label::createWithBMFont("markerfeltwide.fnt","");
    _popupText->setPosition(_referenceText->getPosition());
    _popupText->setScale(_popupText->getScale()*1.5f);
    _referenceText->getParent()->addChild(_popupText);
    _popupText->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
    
    _btnExit = (cocos2d::ui::Button*)_widgetPopup->getChildByName("bg_popup")->getChildByName("exitPopup");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTUIPopupSmart::onClickExitPopupBtn,this));
    
    _btnAccept = (cocos2d::ui::Button*)_widgetPopup->getChildByName("bg_popup")->getChildByName("acceptBtn");
    _btnAccept->addTouchEventListener(CC_CALLBACK_2(TTUIPopupSmart::onClickAcceptPopupBtn,this));
    
    _btnAcceptLabel = (cocos2d::ui::TextBMFont*)_widgetPopup->getChildByName("bg_popup")->getChildByName("acceptBtn")->getChildByName("acceptLabel");
    
    _parentFacebookInfo = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("imgPopup")->getChildByName("facebook");
    _parentFacebookInfoDone = (cocos2d::ui::Widget*)_parentFacebookInfo->getChildByName("confirmation");
    _parentFacebookInfoFail = (cocos2d::ui::Widget*)_parentFacebookInfo->getChildByName("problem");
    _parentFacebookInfoDone->setVisible(false);
    _parentFacebookInfoFail->setVisible(false);
    _parentFacebookInfo->setVisible(false);

    _parentStoreInfo = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("imgPopup")->getChildByName("tienda");
    _parentStoreInfoDone = (cocos2d::ui::Widget*)_parentStoreInfo->getChildByName("confirmation");
    _parentStoreInfoFail = (cocos2d::ui::Widget*)_parentStoreInfo->getChildByName("problem");
    _parentStoreInfoDone->setVisible(false);
    _parentStoreInfoFail->setVisible(false);
    _parentStoreInfo->setVisible(false);
    
    _parentProximamente = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("imgPopup")->getChildByName("proximamente");
    _parentProximamenteImg = (cocos2d::ui::Widget*)_parentProximamente->getChildByName("problem");
    _parentProximamente->setVisible(false);
    
    _parentGeneralConnection = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("imgPopup")->getChildByName("generalConnection");
    _parentGeneralConnection->setVisible(false);
    
    // Creamos el bg
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
}

TTUIPopupSmart::~TTUIPopupSmart()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTUIPopupSmart::pauseUI()
{
    _blockMenus = true;
    _bgUI->setVisible(false);
}

void TTUIPopupSmart::restoreUI()
{
    _blockMenus = false;
    _bgUI->setVisible(true);
}

void TTUIPopupSmart::showPopupState(Node* _parent, SMART_POPUPS _smartPopup, bool _state)
{
    _blockMenus = false;
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_IN");
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,51);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiPopupLayer->getParent() != nullptr)
        _uiPopupLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiPopupLayer,52);
    
    _uiPopupLayer->setVisible(true);
    _widgetPopup->setVisible(true);
    
    _parentProximamente->setVisible(false);
    _parentStoreInfo->setVisible(false);
    _parentFacebookInfo->setVisible(false);
    _parentGeneralConnection->setVisible(false);
    
    _btnAcceptLabel->setString(TTLanguageManager::getInstance()->getTag("ACCEPT"));
    
    switch(_smartPopup)
    {
        case SMART_POPUP_STORE_STATE:
        {
            _parentStoreInfo->setVisible(true);
            _popupTitle->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_STORE_TITLE));
            _btnExit->setVisible(false);
            if(_state)
            {
                _popupText->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_STORE_DONE));
                _parentStoreInfoDone->setVisible(true);
                _parentStoreInfoFail->setVisible(false);
            }
            else
            {
                _popupText->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_STORE_FAIL));
                _parentStoreInfoFail->setVisible(true);
                _parentStoreInfoDone->setVisible(false);
            }
        }
        break;
        case SMART_POPUP_FACEBOOK_STATE:
        {
            _parentFacebookInfo->setVisible(true);
            _popupTitle->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_FACEBOOK_TITLE));
            _btnExit->setVisible(false);
            if(_state)
            {
                _popupText->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_FACEBOOK_DONE));
                _parentFacebookInfoDone->setVisible(true);
                _parentFacebookInfoFail->setVisible(false);
            }
            else
            {
                _popupText->setString(TTLanguageManager::getInstance()->getTag(INFO_POPUP_FACEBOOK_FAIL));
                _parentFacebookInfoFail->setVisible(true);
                _parentFacebookInfoDone->setVisible(false);
            }
        }
        break;
        case SMART_POPUP_COMINGSOON_STATE:
        {
            _popupTitle->setString(TTLanguageManager::getInstance()->getTag("COMING_SOON"));
            _btnExit->setVisible(false);
            
            _popupText->setString(TTLanguageManager::getInstance()->getTag("COMING_SOON_TEXT"));
            _parentProximamente->setVisible(true);
        }
        break;
        case SMART_POPUP_GENERAL_CONNECTION_ERRROR_STATE:
        {
            _popupTitle->setString(TTLanguageManager::getInstance()->getTag("ERROR"));
            _btnExit->setVisible(false);
            
            _popupText->setString(TTLanguageManager::getInstance()->getTag("NETWORK_ERROR"));
            _parentGeneralConnection->setVisible(true);
        }
        break;
        default:
        break;
    }
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_POPUP_SMART.ExportJson","panel_enter");
}

void TTUIPopupSmart::exitMenu()
{
    _uiPopupLayer->setVisible(false);
    _widgetPopup->setVisible(false);
    _bgUI->setVisible(false);
    
    _parentFacebookInfoDone->setVisible(false);
    _parentFacebookInfoFail->setVisible(false);
    _parentFacebookInfo->setVisible(false);
    _parentStoreInfoDone->setVisible(false);
    _parentStoreInfoFail->setVisible(false);
    _parentStoreInfo->setVisible(false);
}

void TTUIPopupSmart::onClickExitPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;

    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP_SMART.ExportJson","panel_enter")->stop();
    
    _blockMenus = true;
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    forceExit();
}

void TTUIPopupSmart::forceExit()
{
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP_SMART.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
        
        // Cerramos el stack
        TTUIStackManager::getInstance()->closeUI();
    }));
    _uiPopupLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_OUT");
}

void TTUIPopupSmart::onClickAcceptPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    _blockMenus = true;
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_BOOSTER");
    forceExit();
}

void TTUIPopupSmart::onEvent_FacebookConnect(EventCustom* e)
{
    // Verificar si el usuario se ha conectado o ha sido automático
    if(TTFacebook::getInstance()->isConnected() && TTFacebook::getInstance()->isUserConnection())
    {
        //No hacemos nada si ya hemos abierto el popup
        if(!TTUIStackManager::getInstance()->isUIOnTop(UI_POPUP_SMART))
        {
            TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
            showPopupState(Director::getInstance()->getRunningScene(),SMART_POPUP_FACEBOOK_STATE,true);
            TTFacebook::getInstance()->userConecctionAlerted();
        }
    }
}

