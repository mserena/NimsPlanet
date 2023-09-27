#include "TTUIMap.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTStoreManager.h"
#include "TTUISettings.h"
#include "TTUIShop.h"
#include "TTUserLives.h"
#include "TTUIPopup.h"
#include "TTUIFacebook.h"
#include "TTParseManager.h"

#include "TTDailyBonusManager.h"

#define UI_MAP_SCREEN_MARGIN    0.5f

//Singleton
static TTUIMap *_sharedMapUI = nullptr;

TTUIMap* TTUIMap::getInstance()
{
    if (!_sharedMapUI)
    {
        _sharedMapUI = new TTUIMap();
        _sharedMapUI->init();
    }
    
    return _sharedMapUI;
}

void TTUIMap::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedMapUI);
}

TTUIMap::~TTUIMap()
{
    CCLOG("Se destruye la UI del Mapa");
    
    // Eliminamos la layer contenedora
    _uiMapLayer->removeAllChildrenWithCleanup(true);
    _uiMapLayer->removeFromParentAndCleanup(true);

    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_MAP0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_MAP0.png");
}

void TTUIMap::init()
{
    _blockMenus = false;
    _buyCoinsOutAllowed = false;
    
    // Cargamos el XML de la UI de Mapa
    _uiMapLayer = Layer::create();
    _uiMapLayer->retain();
    
    _widgetMapCoins = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_MAP_COINS.ExportJson");
    cocos2d::ui::Layout* _layoutMapCoins = dynamic_cast<cocos2d::ui::Layout*>(_widgetMapCoins);
    _uiMapLayer->addChild(_layoutMapCoins);
    
    _widgetMapLive = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_MAP_LIVE.ExportJson");
    cocos2d::ui::Layout* _layoutMapLive = dynamic_cast<cocos2d::ui::Layout*>(_widgetMapLive);
    _uiMapLayer->addChild(_layoutMapLive);
    
    _widgetMapMenu = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_MAP_MENU.ExportJson");
    cocos2d::ui::Layout* _layoutMapMenu = dynamic_cast<cocos2d::ui::Layout*>(_widgetMapMenu);
    _uiMapLayer->addChild(_layoutMapMenu);
    
    // Calculo de margenes
    Size _screenSize = Director::getInstance()->getWinSize();
    float marginScreen = (UI_MAP_SCREEN_MARGIN*_screenSize.width)/100;

    // UI Vidas
    float _scale = scale_widget(_widgetMapLive,WIDTH_UI_LIVE_SPACE,HEIGHT_UI_LIVE_SPACE);
    _widgetMapLive->setScale(_scale);
    _widgetMapLive->setAnchorPoint(Vec2(0,1));
    _widgetMapLive->setPosition(Vec2(marginScreen,_screenSize.height-marginScreen));
    _widgetMapLive->setVisible(false);

    _btnLives = (cocos2d::ui::Button*)_widgetMapLive->getChildByName("uiLiveBtn");
    _btnLives->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickLivesBtn,this));
    _userLives = (cocos2d::ui::TextBMFont*)_widgetMapLive->getChildByName("uiLiveBtn")->getChildByName("Live")->getChildByName("LiveLabel");
    _livesInfo = (cocos2d::ui::TextBMFont*)_widgetMapLive->getChildByName("uiLiveBtn")->getChildByName("uiLiveLabel");
    
    // UI Daily Bonus
    _dailyBonusBtn = cocos2d::ui::Button::create("button_dailybonus.png","","",cocos2d::ui::Widget::TextureResType::PLIST);
    _uiMapLayer->addChild(_dailyBonusBtn,2);
    _dailyBonusBtn->setPosition(Point((marginScreen*4)+(_dailyBonusBtn->getContentSize().width/4),_screenSize.height-_livesInfo->getContentSize().height-(4*marginScreen)-(_dailyBonusBtn->getContentSize().height/4)));
    _dailyBonusBtn->setScale(scale_widget(_dailyBonusBtn,15,15));
    _dailyBonusBtn->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickDailyBonusBtn,this));
    _dailyBonusBtn->setVisible(false);
    
    _dailyBonusBtnHalo = Sprite::createWithSpriteFrameName("button_dailybonus_halo.png");
    _uiMapLayer->addChild(_dailyBonusBtnHalo,1);
    _dailyBonusBtnHalo->setPosition(_dailyBonusBtn->getPosition());
    _dailyBonusBtnHalo->setScale(scale_widget(_dailyBonusBtn,10,10));
    auto _rotation = RotateBy::create(2.5f,360);
    _dailyBonusBtnHalo->runAction(RepeatForever::create(_rotation));
    _dailyBonusBtnHalo->setVisible(false);
    
    // UI Monedas
    // _widgetMapCoins->setScale(scale_widget(_widgetMapCoins,WIDTH_UI_COINS_SPACE,HEIGHT_UI_COINS_SPACE));
    //opner la misma escala que el widget de las vidas
    _widgetMapCoins->setScale(_scale);
    _widgetMapCoins->setAnchorPoint(Vec2(1,1));
    _widgetMapCoins->setPosition(Vec2(_screenSize.width-marginScreen,_screenSize.height-marginScreen));
    _widgetMapCoins->setVisible(false);
    
    _btnCoins = (cocos2d::ui::Button*)_widgetMapCoins->getChildByName("uiCoins_btn");
    _btnCoins->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickCoinsBtn,this));
    _userCoins = (cocos2d::ui::TextBMFont*)_widgetMapCoins->getChildByName("uiCoins_btn")->getChildByName("userCoins");
    
    // UI Menu
    _widgetMapMenu->setScale(scale_widget(_widgetMapMenu,WIDTH_UI_MENU_SPACE,HEIGHT_UI_MENU_SPACE));
    _widgetMapMenu->setAnchorPoint(Vec2(0,0));
    _widgetMapMenu->setPosition(Vec2(marginScreen,marginScreen));
    _widgetMapMenu->setVisible(false);
    
    _btnMenuSettings = (cocos2d::ui::Button*)_widgetMapMenu->getChildByName("uimenu_bg")->getChildByName("BtnSettings");
    _btnMenuSettings->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickSettingsBtn,this));
    
    _btnMenuGiveLives = (cocos2d::ui::Button*)_widgetMapMenu->getChildByName("uimenu_bg")->getChildByName("BtnLive");
    _btnMenuGiveLives->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickGiveLivesBtn,this));
    
    _btnMenuInviteFriends = (cocos2d::ui::Button*)_widgetMapMenu->getChildByName("uimenu_bg")->getChildByName("BtnFriends");
    _btnMenuInviteFriends->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickInviteFriendsBtn,this));
    
    _btnMenuMail = (cocos2d::ui::Button*)_widgetMapMenu->getChildByName("uimenu_bg")->getChildByName("BtnSms");
    _btnMenuMail->addTouchEventListener(CC_CALLBACK_2(TTUIMap::onClickMailBtn,this));
    _alertMail = (cocos2d::ui::Widget*)_widgetMapMenu->getChildByName("uimenu_bg")->getChildByName("BtnSms")->getChildByName("alert");
    _alertMail->setVisible(false);

}

void TTUIMap::pauseUI()
{
    _blockMenus = true;
    sendEvent(EVENT_MAP_PAUSE);
}

void TTUIMap::restoreUI()
{
    _blockMenus = false;
    sendEvent(EVENT_MAP_RESTORE);
    
    updateUIValues();
    
    setShowDailyBonusVisiblity();
}

void TTUIMap::setShowDailyBonusVisiblity()
{
    // Verificamos la iluminación del icono del regalo
    UserDefault *_def = UserDefault::getInstance();
    if(_def->getBoolForKey(USER_DATA_DAILY_BONUS_HAS_DATA,false))
    {
        int _spins = _def->getIntegerForKey(USER_DATA_DAILY_BONUS_FREE_SPINS);
        _dailyBonusBtnHalo->setVisible(_spins > 0);
    }
    else
    {
        // Si no hay datos de usuario los creamos
        _dailyBonusBtnHalo->setVisible(true);
    }

}

void TTUIMap::hideUIMap()
{
    _widgetMapCoins->setVisible(false);
    _widgetMapLive->setVisible(false);
    _dailyBonusBtn->setVisible(false);
    _dailyBonusBtnHalo->setVisible(false);
    _widgetMapMenu->setVisible(false);
    
    TTUIStackManager::getInstance()->closeUI();
}

void TTUIMap::showUIMap(Node* _parent)
{
    _blockMenus = false;
    TTUIStackManager::getInstance()->goToNewUI(UI_MAP);
    
    //Widget
    if(_uiMapLayer->getParent() != nullptr)
        _uiMapLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiMapLayer,5);
    _uiMapLayer->setVisible(true);
    _widgetMapCoins->setVisible(true);
    _widgetMapLive->setVisible(true);
    _dailyBonusBtn->setVisible(true);
    _widgetMapMenu->setVisible(true);
    
    setShowDailyBonusVisiblity();
    
    updateUIValues();
}

void TTUIMap::updateUIValues()
{
    if (_widgetMapCoins->isVisible())
    {
        _userCoins->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(TTStoreManager::getInstance()->getUserBalance()));
    }
    if (_widgetMapLive->isVisible())
    {
        livesInfo infoLives = TTUserLives::getInstance()->getLivesInfo();
        _userLives->setString(std::to_string(infoLives._numActiveLives));
        _livesInfo->setString(infoLives._timeToRegenerate);
    }
    if(_widgetMapMenu->isVisible())
    {
        if(AppRequests::getInstance()->getRequests().size() > 0)
        {
            //Mostramos la alerta en el sobre de MAIL
            _alertMail->setVisible(true);
        }
    }
}

void TTUIMap::showUserCredits(Node* _parent, int zValue)
{
    _widgetMapCoins->retain();
    if(_widgetMapCoins->getParent() != nullptr)
        _widgetMapCoins->removeFromParentAndCleanup(false);
    _parent->addChild(_widgetMapCoins,zValue);
    _widgetMapCoins->release();
    
    _widgetMapCoins->setVisible(true);
    _buyCoinsOutAllowed = true;
    updateUIValues();
}

void TTUIMap::hideUserCredits()
{
    _widgetMapCoins->retain();
    if(_widgetMapCoins->getParent() != nullptr)
        _widgetMapCoins->removeFromParentAndCleanup(false);
    cocos2d::ui::Layout* _layoutMapCoins = dynamic_cast<cocos2d::ui::Layout*>(_widgetMapCoins);
    _uiMapLayer->addChild(_layoutMapCoins,1);
    _widgetMapCoins->release();
    
    _buyCoinsOutAllowed = false;
}

void TTUIMap::onClickLivesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    if(!TTUserLives::getInstance()->allLivesActive())
    {
        TTUIPopup::getInstance()->showPopupLives(_widgetMapLive->getParent());
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
        
        // Enviamos evento de analitica a Parse
        std::map<std::string,std::string> _dimensions;
        TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_LIVES_MENU,_dimensions);
    }
}

void TTUIMap::onClickDailyBonusBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_DAILY_BONUS);
    TTDailyBonusManager::getInstance()->initShowDailyBonus(_widgetMapLive->getParent());
}

void TTUIMap::onClickCoinsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus && !_buyCoinsOutAllowed)
        return;
    
    TTUIShop::getInstance()->showShop(_widgetMapCoins->getParent());
    TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
}

void TTUIMap::onClickSettingsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_SETTINGS);
    TTUISettings::getInstance()->showSettings(_uiMapLayer->getParent());
}

void TTUIMap::onClickGiveLivesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_FACEBOOK);
    TTUIFacebook::getInstance()->showFacebookPage(_uiMapLayer->getParent(),FACEBOOK_SEND_HEART_PAGE);
}

void TTUIMap::onClickInviteFriendsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    if(TTFacebook::getInstance()->isConnected())
    {
        TTFacebook::getInstance()->showInviteFirendsDialog();
    }
    else
    {
        TTUIStackManager::getInstance()->goToNewUI(UI_FACEBOOK);
        TTUIFacebook::getInstance()->showFacebookPage(_uiMapLayer->getParent(),FACEBOOK_CONNECT_PAGE);
    }
}

void TTUIMap::onClickMailBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    if(_blockMenus)
        return;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_FACEBOOK);
    TTUIFacebook::getInstance()->showFacebookPage(_uiMapLayer->getParent(),FACEBOOK_MAIL_PAGE);
}

