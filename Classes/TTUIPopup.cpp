#include "TTUIPopup.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTStoreManager.h"
#include "TTUIShop.h"
#include "TTUIMap.h"
#include "TTUserInfo.h"
#include "TTUserLives.h"
#include "TTAudioManager.h"
#include "TTHowToParser.h"
#include "TTUIFacebook.h"
#include "TTUIPopupSmart.h"
#include "TTParseManager.h"
#include "TTMapStopperManager.h"
#include "TTEventsManager.h"

//Singleton
static TTUIPopup *_sharedPopupUI = nullptr;

TTUIPopup* TTUIPopup::getInstance()
{
    if (!_sharedPopupUI)
    {
        _sharedPopupUI = new TTUIPopup();
        _sharedPopupUI->init();
        _sharedPopupUI->initHowTo();
        _sharedPopupUI->initStopper();
    }
    
    return _sharedPopupUI;
}

void TTUIPopup::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedPopupUI);
}

void TTUIPopup::init()
{
    _blockMenus = false;
    _buyDone    = false;
    
    // Cargamos el XML de la UI Resume
    _uiPopupLayer = Layer::create();
    _uiPopupLayer->retain();
    _widgetPopup = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_POPUP.ExportJson");
    
    cocos2d::ui::Layout* _layoutPopup = dynamic_cast<cocos2d::ui::Layout*>(_widgetPopup);
    _uiPopupLayer->addChild(_layoutPopup);
    
    // Escalado Widget
    _scaleForInfo = scale_widget(_widgetPopup,WIDTH_UI_POPUP_SPACE,HEIGHT_UI_POPUP_SPACE);
    _scaleForHowTo = scale_widget(_widgetPopup,WIDTH_UI_POPUP_HOWTO_SPACE,HEIGHT_UI_POPUP_HOWTO_SPACE);
    _widgetPopup->setAnchorPoint(Point(0.5,0.5));
                          
    // Posicionamiento Widget
    _uiPopupLayer->setVisible(false);
    
    _infoPopup = (cocos2d::ui::TextBMFont*)_widgetPopup->getChildByName("bg_popup")->getChildByName("infoPopup");
    _infoPopup->setVisible(false);
    
    // Recuperamos los items que necesitamos popup compra boosters
    _popupTitle = (cocos2d::ui::TextBMFont*)_infoPopup->getChildByName("popupTitle");
    
    _referenceText = (cocos2d::ui::Text*)_infoPopup->getChildByName("popupText");
    _referenceText->setVisible(false);
    _popupText = Label::createWithBMFont("markerfeltwide.fnt","");
    _popupText->setPosition(_referenceText->getPosition());
    _popupText->setScale(_popupText->getScale()*1.5f);
    _referenceText->getParent()->addChild(_popupText);
    _popupText->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
    
    _btnExit = (cocos2d::ui::Button*)_infoPopup->getChildByName("exitInfoPopupBtn");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickExitPopupBtn,this));
    
    _popupImgParent = (cocos2d::ui::Widget*)_infoPopup->getChildByName("popupImg");
    _popupImgTimeParent = (cocos2d::ui::Widget*)_infoPopup->getChildByName("popupImg")->getChildByName("popupImgBlock");
    
    _btnPopup = (cocos2d::ui::Button*)_infoPopup->getChildByName("popupBtnShop");
    _btnPopup->setVisible(false);
    _btnPopup->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickBuyPopupBtn,this));
    _btnPopupLabel = (cocos2d::ui::TextBMFont*)_infoPopup->getChildByName("popupBtnShop")->getChildByName("prize");
    
    _btnPopupFacebook = (cocos2d::ui::Button*)_infoPopup->getChildByName("popupBtnFacebook");
    _btnPopupFacebook->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickFacebookAskForLivesPopupBtn,this));
    _btnPopupFacebook->setVisible(false);
    _btnPopupFacebookLabel = (cocos2d::ui::TextBMFont*)_infoPopup->getChildByName("popupBtnFacebook")->getChildByName("label");

    // Temporal: ocultamos tutorial
    _widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->setVisible(false);
    
    // Creamos el bg
    Size _screenSize = Director::getInstance()->getWinSize();
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
}

void TTUIPopup::initHowTo()
{
    _blockMenus = false;
    
    _howToPopup = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup");
    _howToPopup->setVisible(false);
    
    // Recuperamos los items que necesitamos popup How To
    _howtoTitle =(cocos2d::ui::TextBMFont*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("popupTitle");
    _howtoReferenceText = (cocos2d::ui::Text*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("popupText");
    _howtoReferenceText->setVisible(false);
    _howtoText = Label::createWithBMFont("markerfeltwide.fnt","");
    _howtoText->setPosition(_howtoReferenceText->getPosition());
    _howtoText->setScale(_howtoText->getScale()*1.3f);
    _howtoReferenceText->getParent()->addChild(_howtoText);
    _howtoText->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
    
    _howtoBtnNext = (cocos2d::ui::Button*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("nextHowToPopupBtn");
    _howtoBtnNext->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickNextHowToPopupBtn,this));
    _howtoBtnBack = (cocos2d::ui::Button*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("backHowToPopupBtn");
    _howtoBtnBack->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickBackHowToPopupBtn,this));
    
    _howtoBtnExit = (cocos2d::ui::Button*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("exitHowToPopupBtn");
    _howtoBtnExit->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickExitHowtoBtn,this));

    _howtoImg = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("popupImg");
    _howtoTeacherImg = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("howToPopup")->getChildByName("popupImgTeacher");
    
    _sprHowTo = Sprite::create();
    _sprHowTo->setPosition(_howtoImg->getBoundingBox().size.width/1.75,_howtoImg->getBoundingBox().size.height/1.4);
    _howToPopup->addChild(_sprHowTo,2);
    
    //HowToParser XML
    TTHowToParser::getInstance()->parseXML();
}

void TTUIPopup::initStopper()
{
    // Registramos los eventos
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_CONNECTION,CC_CALLBACK_1(TTUIPopup::onFacebookConnection,this)));
    
    _blockMenus = false;
    
    _stopperPopup = (cocos2d::ui::Widget*)_widgetPopup->getChildByName("bg_popup")->getChildByName("stopperPopup");
    _stopperPopup->setVisible(false);
    
    _stopperPopupLoked = (cocos2d::ui::Widget*)_stopperPopup->getChildByName("blockMenu");
    _stopperPopupLoked->setVisible(false);
    _stopperTime = (cocos2d::ui::TextBMFont*)_stopperPopupLoked->getChildByName("clockBg")->getChildByName("time");
    _buyStopperButton = (cocos2d::ui::Button*)_stopperPopupLoked->getChildByName("popupBtnShop");
    _buyStopperButton->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickBuyStopperBtn,this));
    _buyStopperButtonLabel = (cocos2d::ui::TextBMFont*)_stopperPopupLoked->getChildByName("popupBtnShop")->getChildByName("prize");
    _padlockImgParents.push_back((cocos2d::ui::Widget*)_stopperPopupLoked->getChildByName("lockImg0"));
    _padlockImgParents.push_back((cocos2d::ui::Widget*)_stopperPopupLoked->getChildByName("lockImg1"));
    _padlockImgParents.push_back((cocos2d::ui::Widget*)_stopperPopupLoked->getChildByName("lockImg2"));
    _facebookStopperButton = (cocos2d::ui::Button*)_stopperPopupLoked->getChildByName("popupBtnFacebook");
    _facebookStopperButton->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickFacebookStopperBtn,this));
    
    _stopperPopupUnloked = (cocos2d::ui::Widget*)_stopperPopup->getChildByName("unblockMenu");
    _stopperPopupUnloked->setVisible(false);
    _stopperHaloKey = (cocos2d::ui::Widget*)_stopperPopupUnloked->getChildByName("haloKeyImg");
    _unlockButton = (cocos2d::ui::Button*)_stopperPopupUnloked->getChildByName("unblockBtn");
    _unlockButton->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickExitStopperUnlockBtn,this));
    _unlockButtonLabel = (cocos2d::ui::TextBMFont*)_unlockButton->getChildByName("unblockLabel");
    
    _stopperTitle =(cocos2d::ui::TextBMFont*)_stopperPopup->getChildByName("popupTitle");
    _stopperReferenceText = (cocos2d::ui::Text*)_stopperPopup->getChildByName("popupText");
    _stopperReferenceText->setVisible(false);
    _stopperText = Label::createWithBMFont("markerfeltwide.fnt","");
    _stopperText->setPosition(_stopperReferenceText->getPosition());
    _stopperText->setScale(_stopperText->getScale()*1.3f);
    _stopperReferenceText->getParent()->addChild(_stopperText);
    _stopperText->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
    _labelUnlock = (cocos2d::ui::TextBMFont*)_stopperPopupLoked->getChildByName("popupBtnShop")->getChildByName("unblockNowLabel");
    
    _stopperBtnExit = (cocos2d::ui::Button*)_stopperPopup->getChildByName("stopperExitBtn");
    _stopperBtnExit->addTouchEventListener(CC_CALLBACK_2(TTUIPopup::onClickExitStopperBtn,this));
}

TTUIPopup::~TTUIPopup()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTUIPopup::pauseUI()
{
    _blockMenus = true;
    _bgUI->setVisible(false);
    
    // Ocultamos los créditos del usuario
    TTUIMap::getInstance()->hideUserCredits();
}

void TTUIPopup::restoreUI()
{
    _blockMenus = false;
    _bgUI->setVisible(true);
    
    // Mostramos los créditos del usuario
    TTUIMap::getInstance()->showUserCredits(_uiPopupLayer->getParent(),23);
}

void TTUIPopup::showPopupBooster(Node* _parent, int typeBooster, int subTypeBooster, std::string _boosterIcon)
{
    _blockMenus = false;
    _buyDone    = false;
    
    _howToPopup->setVisible(false);
    _infoPopup->setVisible(true);
    
    //Reproduce audio de entrada
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_IN");
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,21);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiPopupLayer->getParent() != nullptr)
        _uiPopupLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiPopupLayer,22);
    _uiPopupLayer->setVisible(true);
    _widgetPopup->setVisible(true);
    
    // Escalado Widget y posicionado
    _widgetPopup->setScale(_scaleForInfo);
    
    Size _screenSize = Director::getInstance()->getWinSize();
    _uiPopupLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    
    // Seteamos el icono
    _popupImgTimeParent->setVisible(false);
    Sprite* _icon = Sprite::createWithSpriteFrameName(_boosterIcon);
    _icon->setPosition(_popupImgParent->getBoundingBox().size.width/2,_popupImgParent->getBoundingBox().size.height/2);
    _icon->setName("icon");
    float _iconScale = _popupImgParent->getContentSize().width/_icon->getContentSize().width;
    _icon->setScale(_iconScale*1.5f);
    _popupImgParent->addChild(_icon);
    
    // Seteamos el título
    std::string _boosterTitleTag = BOOSTER_TITLE;
    _boosterTitleTag += std::to_string(typeBooster)+"_"+std::to_string(subTypeBooster);
    std::string _titleBooster = TTLanguageManager::getInstance()->getTag(_boosterTitleTag);
    _popupTitle->setString(_titleBooster);
    
    // Seteamos la descripción del booster
    std::string _boosterDescTag = BOOSTER_DESC;
    _boosterDescTag += std::to_string(typeBooster)+"_"+std::to_string(subTypeBooster);
    std::string _descBooster = TTLanguageManager::getInstance()->getTag(_boosterDescTag);
    _popupText->setString(_descBooster);
    _popupText->setPosition(_referenceText->getPosition());
    
    // Verificamos si tenemos boosters de este tipo
    _btnPopupFacebook->setVisible(false);
    _btnPopup->setVisible(true);
    
    // Seteamos el precio del booster
    std::string _boosterIdPrice = ITEM_BOOSTER;
    _boosterIdPrice += std::to_string(typeBooster)+"_"+std::to_string(subTypeBooster);
    int priceBooster = TTStoreManager::getInstance()->getVirtualItemPrice(_boosterIdPrice);
    _btnPopupLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(priceBooster));
        
    // Seteamos el identificador del booster a comprar
    _btnPopup->setName(_boosterIdPrice);
    
    // Mostramos los créditos del usuario
    TTUIMap::getInstance()->showUserCredits(_parent,23);
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_POPUP.ExportJson","AnimEnterPopup");
}

void TTUIPopup::showPopupStopper(Node* _parent)
{
    _blockMenus = false;
    _parentLayer = _parent;
    _stopperPopup->setVisible(true);
    _howToPopup->setVisible(false);
    _infoPopup->setVisible(false);
    
    bool _stopperFinished = TTMapStopperManager::getInstance()->isStopperFinished();
    
    //Reproduce audio de entrada
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_IN");
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,21);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiPopupLayer->getParent() != nullptr)
        _uiPopupLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiPopupLayer,22);
    _uiPopupLayer->setVisible(true);
    _widgetPopup->setVisible(true);
    
    // Escalado Widget y posicionado
    _widgetPopup->setScale(_scaleForInfo);
    
    Size _screenSize = Director::getInstance()->getWinSize();
    _uiPopupLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    
    // Seteamos el título
    _stopperTitle->setString(TTLanguageManager::getInstance()->getTag(CONTINUE_TAG));
    _labelUnlock->setString(TTLanguageManager::getInstance()->getTag(STOPPER_UNLOCK_NOW));
    
    if(!_stopperFinished)
    {
        _stopperPopupLoked->setVisible(true);
        _stopperPopupUnloked->setVisible(false);
        
        if(TTFacebook::getInstance()->isConnected())
        {
            ((cocos2d::ui::TextBMFont*)_facebookStopperButton->getChildByName("label"))->setString(INVITE_TAG);
        }
        else
        {
            ((cocos2d::ui::TextBMFont*)_facebookStopperButton->getChildByName("label"))->setString(CONNECT_TAG);
        }
        
        // Verificamos los candados a mostrar
        for(int i = 0; i < TTMapStopperManager::getInstance()->getUserInvitedFriends(); i++)
        {
            _padlockImgParents[i]->getChildByName("lock_off")->setVisible(false);
            _padlockImgParents[i]->getChildByName("lock_on")->setVisible(true);
        }
        
        // Mostramos los créditos del usuario
        TTUIMap::getInstance()->showUserCredits(_parent,23);
        _buyStopperButtonLabel->setString(std::to_string(TTStoreManager::getInstance()->getVirtualItemPrice("STOPPER")));
        
        TTMapStopperManager::getInstance()->verifyServerTimeForStopper();
        
        // Mostramos la UI de loading mientras intentamos recuperar el tiempo del servidor
        TTUIStackManager::getInstance()->showUIConnecting(_parent);
        
        // Seteamos la descripción
        _stopperText->setString(TTLanguageManager::getInstance()->getTag(STOPPER_LOCK_TEXT));
    }
    else
    {
        _stopperPopupLoked->setVisible(false);
        _stopperPopupUnloked->setVisible(true);
        
        _stopperHaloKey->stopAllActions();
        auto _rotateHalo = RotateBy::create(2.5f,360);
        _stopperHaloKey->runAction(RepeatForever::create(_rotateHalo));
        
        TTAudioManager::getInstance()->playAudio("STOPPER_UNLOCK");
        _unlockButtonLabel->setString(TTLanguageManager::getInstance()->getTag(UNLOCK));
        
        // Seteamos la descripción
        _stopperText->setString(TTLanguageManager::getInstance()->getTag(STOPPER_UNLOCK_TEXT));
    }
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_POPUP.ExportJson","AnimEnterPopup");
}

void TTUIPopup::showPopupLives(Node* _parent)
{
    _blockMenus = false;
    _buyDone    = false;
    
    _howToPopup->setVisible(false);
    _infoPopup->setVisible(true);
    _stopperPopup->setVisible(false);
    
    //Reproduce audio de entrada
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_IN");
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,21);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiPopupLayer->getParent() != nullptr)
        _uiPopupLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiPopupLayer,22);
    _uiPopupLayer->setVisible(true);
    _widgetPopup->setVisible(true);
    
    // Escalado Widget y posicionado
    _widgetPopup->setScale(_scaleForInfo);
    
    Size _screenSize = Director::getInstance()->getWinSize();
    _uiPopupLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    
    // Seteamos el icono
    _popupImgTimeParent->setVisible(false);
    Sprite* _icon = Sprite::createWithSpriteFrameName("lives_ui.png");
    _icon->setPosition(_popupImgParent->getBoundingBox().size.width/2,_popupImgParent->getBoundingBox().size.height/2);
    _icon->setName("icon");
    float _iconScale = _popupImgParent->getContentSize().width/_icon->getContentSize().width;
    _icon->setScale(_iconScale);
    _popupImgParent->addChild(_icon);
    
    // Seteamos el título
    _popupTitle->setString(TTLanguageManager::getInstance()->getTag(LIVES_TAG));
    
    // Seteamos la descripción
    _popupText->setString(TTLanguageManager::getInstance()->getTag(LIVES_DESC_TAG));
    _popupText->setPosition(Point(_referenceText->getPosition().x,_referenceText->getPosition().y*1.1f));
    
    _btnPopupFacebook->setVisible(true);
    _btnPopupFacebookLabel->setString(TTLanguageManager::getInstance()->getTag(ORDER_LIVES_TAG));
    _btnPopup->setVisible(true);
    
    // Seteamos el precio del booster
    std::string _livesIdPrice = "ALL_LIVES";
    int priceBooster = TTStoreManager::getInstance()->getVirtualItemPrice(_livesIdPrice);
    _btnPopupLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(priceBooster));
    
    // Seteamos el identificador del booster a comprar
    _btnPopup->setName(_livesIdPrice);
 
    // Mostramos los créditos del usuario
    TTUIMap::getInstance()->showUserCredits(_parent,23);

    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_POPUP.ExportJson","AnimEnterPopup");
}

void TTUIPopup::showHowToPopup(Node* _parent, std::vector<int> _pageIds, bool _fromSettings)
{
    _howToPages = _pageIds;
    _howToFromSettings = _fromSettings;
    showHowToPopup(_parent,0);
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_POPUP.ExportJson","AnimEnterPopup");
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_IN");
}

void TTUIPopup::showHowToPopup(Node* _parent, int _nextPage)
{
    //Si abrimos el tutorial desde el menú de ajustes, enviamos el evento a analytics
    if(_howToFromSettings)
    {
        // Enviamos evento de analitica a Parse
        std::map<std::string,std::string> _dimensions = {{"Tutorial_Page",std::to_string(_nextPage)}};
        TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_TUTORIAL_MENU,_dimensions);
    }
    
    _howToPopup->setVisible(true);
    _infoPopup->setVisible(false);
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,21);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiPopupLayer->getParent() != nullptr)
        _uiPopupLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiPopupLayer,22);
    _uiPopupLayer->setVisible(true);
    _widgetPopup->setVisible(true);
    
    _actualLevel = _nextPage;
    _parentLayer = _parent;
    
    _widgetPopup->setScale(_scaleForHowTo);
    Size _screenSize = Director::getInstance()->getWinSize();
    _uiPopupLayer->setPosition(_screenSize.width/2,_screenSize.height/2.25);
    
    //obtenemos info del tutorial desde howto.xml
    std::ostringstream _IDString;
    _IDString << _howToPages[_nextPage];
    std::string tutorialID = TUTORIAL_ID + _IDString.str();
    std::string _textTag = TTHowToParser::getInstance()->getTextFromTutorial(tutorialID);
    std::string _text = TTLanguageManager::getInstance()->getTag(_textTag);
    std::string _imgURL = TTHowToParser::getInstance()->getImgFromTutorial(tutorialID);
    
    // Seteamos la imagen
    _sprHowTo->setTexture(_imgURL);
    float scale = (_howToPopup->getContentSize().height/2)/_sprHowTo->getContentSize().height;
    _sprHowTo->setScale(scale);
    
    // Seteamos el título
    _howtoTitle->setString(TTLanguageManager::getInstance()->getTag("HOWTO_TITLE"));
    
    // Seteamos la descripción
    _howtoText->setString(_text);
    _howtoText->setPosition(Point(_howtoReferenceText->getPosition().x,_howtoReferenceText->getPosition().y));
    
    
    if(_howToPages.size() > 1)
    {
        _howtoBtnNext->setVisible(true);
        _howtoBtnBack->setVisible(true);
    }
    else
    {
        _howtoBtnNext->setVisible(false);
        _howtoBtnBack->setVisible(false);
    }
    
    _blockMenus = false;
}


void TTUIPopup::exitMenu()
{
    _uiPopupLayer->setVisible(false);
    _widgetPopup->setVisible(false);
    _bgUI->setVisible(false);
    
    // Ocultamos los créditos del usuario
    TTUIMap::getInstance()->hideUserCredits();
}

void TTUIPopup::onClickExitPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus )
        return;
    
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimEnterPopup")->stop();

    _blockMenus = true;
    
    forceExit();
}

void TTUIPopup::forceExit()
{
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimExitPopup");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
        
        //Eliminamos el sprite del icono
        if(_popupImgParent->getChildByName("icon"))
            _popupImgParent->getChildByName("icon")->removeFromParentAndCleanup(true);
        
        // Cerramos el stack
        TTUIStackManager::getInstance()->closeUI();
        
        // Mostramos el menú de compra completada
        if(_buyDone)
        {
            TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
            TTUIPopupSmart::getInstance()->showPopupState(_uiPopupLayer->getParent(),SMART_POPUP_STORE_STATE,true);
        }
    }));
    _uiPopupLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_OUT");
}

void TTUIPopup::onClickBuyStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_BOOSTER");
    
    if(TTStoreManager::getInstance()->buyVirtualItem("STOPPER"))
    {
        _buyDone = true;
        
        // Eliminamos el stopper manualmente
        TTMapStopperManager::getInstance()->forceFinishStopper();
    }
    else
    {
        // No hay monedas suficientes, mostramos la tienda
        TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
        TTUIShop::getInstance()->showShop(_uiPopupLayer->getParent());
    }
}

void TTUIPopup::onClickBuyPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_BOOSTER");
    
    std::string _itemBuyId = ((cocos2d::ui::Widget*)pSender)->getName();
    if(TTStoreManager::getInstance()->buyVirtualItem(_itemBuyId))
    {
        // Compra realizada con exito
        _buyDone = true;
        TTUIMap::getInstance()->updateUIValues();
        
        //Dependiendo del tipo de ítem comprado...
        if(_itemBuyId == "ALL_LIVES")
        {
            // Vidas compradas, las aplicamos
            TTStoreManager::getInstance()->takeUserVirtualItem("ALL_LIVES");
            TTUserLives::getInstance()->restoreAllLives();
            
            // Enviamos evento de analitica a Parse
            int _level = TTUserInfo::getInstance()->_userData->_selectedLevel + 1;
            std::map<std::string,std::string> _dimensions={{"Level_ID",std::to_string(_level)}};
            TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_LIVES_PURCHASE,_dimensions);
        }
        else
        {
            // Aplicamos el booster
            if(TTAnimalCubes::getInstance()->_gameState == GAME_STATE_INGAME)
            {
                if (TTGame::getInstance()->isFinished())
                {
                    // Preactivamos el Booster
                    TTBoostersManager::getInstance()->activateBoosterById(_itemBuyId);
                }
                else
                {
                    // Aplicamos el booster
                    TTBoostersManager::getInstance()->applyBoosterById(_itemBuyId);
                }
                
                // Enviamos evento de analitica a Parse
                int _level = TTUserInfo::getInstance()->_userData->_selectedLevel + 1;
                std::map<std::string,std::string> _dimensions={{"Level_Booster_ID",_itemBuyId},{"Level_ID",std::to_string(_level)}};
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_BOOSTER_INGAME_PURCHASE,_dimensions);
            }
            else if(TTAnimalCubes::getInstance()->_gameState == GAME_STATE_MAP)
            {
                // Preactivamos el Booster
                TTBoostersManager::getInstance()->activateBoosterById(_itemBuyId);
                
                // Enviamos evento de analitica a Parse
                int _level = TTUserInfo::getInstance()->_userData->_selectedLevel + 1;
                std::map<std::string,std::string> _dimensions={{"Board_Booster_ID",_itemBuyId},{"Level_ID",std::to_string(_level)}};
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_BOOSTER_BOARD_PURCHASE,_dimensions);
            }
        }
        
        // Cerramos el menú
        forceExit();
        
    }
    else
    {
        // No hay monedas suficientes, mostramos la tienda
        TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
        TTUIShop::getInstance()->showShop(_uiPopupLayer->getParent());
    }
}

void TTUIPopup::onClickFacebookStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_LANG");
    
    if(TTFacebook::getInstance()->isConnected())
    {
        TTFacebook::getInstance()->showInviteFirendsDialog();
    }
    else
    {
        TTFacebook::getInstance()->connectUserToFacebook();
    }
}

void TTUIPopup::onClickNextHowToPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_LANG");
    
    int _nextLevel;
    if(_actualLevel == _howToPages.size()-1)
        _nextLevel = 0;
    else
        _nextLevel = _actualLevel+1;
    
    Director::getInstance()->getTextureCache()->removeTexture(_sprHowTo->getTexture());
    showHowToPopup(_parentLayer, _nextLevel);
}

void TTUIPopup::onClickBackHowToPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_LANG");
    
    int _nextLevel;
    if(_actualLevel == 0)
    {
        _nextLevel = _howToPages.size()-1;
    }
    else
    {
        _nextLevel = _actualLevel-1;
    }
    
    Director::getInstance()->getTextureCache()->removeTexture(_sprHowTo->getTexture());
    
    showHowToPopup(_parentLayer,_nextLevel);
}

void TTUIPopup::onClickExitHowtoBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus )
        return;
    
	cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimEnterPopup")->stop();

	_blockMenus = true;

    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimExitPopup");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
        
        // Eliminamos la útlima imágen del popup
        Director::getInstance()->getTextureCache()->removeTexture(_sprHowTo->getTexture());
        
        // Cerramos el stack
        TTUIStackManager::getInstance()->closeUI();
    }));
    _uiPopupLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_OUT");
}

void TTUIPopup::onClickExitStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus )
        return;
    
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimEnterPopup")->stop();
    
    _blockMenus = true;
    
    TTAudioManager::getInstance()->stopAudio("STOPPER_UNLOCK");
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimExitPopup");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
    }));
    _uiPopupLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    // Cerramos el stack
    TTUIStackManager::getInstance()->closeUI();
    
    // Eliminamos el manager de stoppers
    TTMapStopperManager::destroyInstance();
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_OUT");
}

void TTUIPopup::onClickExitStopperUnlockBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus )
        return;
    
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimEnterPopup")->stop();
    
    _blockMenus = true;
    
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_POPUP.ExportJson","AnimExitPopup");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
    }));
    _uiPopupLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    // Cerramos el stack
    TTUIStackManager::getInstance()->closeUI();
    
    // Resteamos y eliminamos el manager de stoppers
    TTMapStopperManager::getInstance()->resetStopper();
    TTMapStopperManager::destroyInstance();
    
    // Reseteamos los gráficos de los candados
    for(int i = 0; i < _padlockImgParents.size(); i++)
    {
        _padlockImgParents[i]->getChildByName("lock_off")->setVisible(true);
        _padlockImgParents[i]->getChildByName("lock_on")->setVisible(false);
    }
    
    //Enviamos evento de desbloqueo al mapa
    sendEvent(EVENT_MAP_LEVEL_UNLOCK);
    
    TTAudioManager::getInstance()->playAudio("PANEL_POPUP_OUT");
}

void TTUIPopup::onClickFacebookAskForLivesPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_BOOSTER");
    
    // Abrimos la UI de facebook de solicitar vidas
    TTUIStackManager::getInstance()->goToNewUI(UI_FACEBOOK);
    TTUIFacebook::getInstance()->showFacebookPage(_uiPopupLayer->getParent(), FACEBOOK_ASK_FOR_HEARTS_PAGE);
}

void TTUIPopup::onFacebookConnection(EventCustom *event)
{
    CCLOG("onFacebookConnection Event");
    if(TTFacebook::getInstance()->isConnected())
    {
        ((cocos2d::ui::TextBMFont*)_facebookStopperButton->getChildByName("label"))->setString(INVITE_TAG);
    }
    else
    {
        ((cocos2d::ui::TextBMFont*)_facebookStopperButton->getChildByName("label"))->setString(CONNECT_TAG);
    }
}
