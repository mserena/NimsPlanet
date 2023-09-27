#include "TTUIShop.h"
#include "TTLanguageManager.h"
#include "TTStoreManager.h"
#include "TTUIStackManager.h"
#include "TTUIMap.h"
#include "TTAudioManager.h"
#include "TTUIPopupSmart.h"
#include "TTParseManager.h"

//Singleton
static TTUIShop *_sharedShopUI = nullptr;

TTUIShop* TTUIShop::getInstance()
{
    if (!_sharedShopUI)
    {
        _sharedShopUI = new TTUIShop();
    }
    
    return _sharedShopUI;
}

void TTUIShop::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedShopUI);
}

TTUIShop::~TTUIShop()
{
    CCLOG("Se destruye la UI de tienda");
    
    // Eliminamos las acciones
    removeAllActionsFromJSON("UI_SHOP.ExportJson");
    
    // Eliminamos la layer contenedora
    _uiShopLayer->removeAllChildrenWithCleanup(true);
    _uiShopLayer->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_SHOP0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_SHOP0.png");
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTUIShop::preloadResources()
{
    Director::getInstance()->getTextureCache()->addImageAsync("UI_SHOP0.png",CC_CALLBACK_1(TTUIShop::resourceLoaded, this));
}

void TTUIShop::resourceLoaded(Texture2D* texture)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UI_SHOP0.plist","UI_SHOP0.png");
    init();
    TTUIStackManager::getInstance()->hideUILoading();
    printShop(_actualParent);
}

void TTUIShop::init()
{
    _blockMenus = false;
    TTUIMap::getInstance()->hideUserCredits();
    
    // Cargamos el XML de la UI Resume
    _uiShopLayer = Layer::create();
    _uiShopLayer->retain();
    _widgetShop = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_SHOP.ExportJson");
    cocos2d::ui::Layout* _layoutShop = dynamic_cast<cocos2d::ui::Layout*>(_widgetShop);
    _uiShopLayer->addChild(_layoutShop);
    
    // Escalado Widget
    _widgetShop->setScale(scale_widget(_widgetShop,WIDTH_UI_SHOP_SPACE,HEIGHT_UI_SHOP_SPACE));
                          
    // Posicionamiento Widget
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetShop->setAnchorPoint(Vec2(0.5,0.5));
    _uiShopLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_widgetShop->getBoundingBox().size.height/2)));
    _uiShopLayer->setVisible(false);
    
    // Recuperamos los items que necesitamos
    _shopTitleLabel = (cocos2d::ui::TextBMFont*)_widgetShop->getChildByName("shop")->getChildByName("shopHeader")->getChildByName("shopTitle");
    _shopMoneyTitleLabel = (cocos2d::ui::TextBMFont*)_widgetShop->getChildByName("shop")->getChildByName("shopHeader")->getChildByName("shopMoneyTitleLabel");
    _shopMoneyLabel = (cocos2d::ui::TextBMFont*)_widgetShop->getChildByName("shop")->getChildByName("shopHeader")->getChildByName("shopMoneyLabel");

    std::vector<storeItem*> _storeItems = TTStoreManager::getInstance()->_storeItems;
    for(int i = 0; i < 5; i++)
    {
        char stItem[100] = {0};
        sprintf(stItem,"shopShowcase%d",i+1);
    
        itemShop item;
        item._btnBuy = (cocos2d::ui::Button*)_widgetShop->getChildByName("shop")->getChildByName(stItem)->getChildByName("shopItem")->getChildByName("buyItemBtn");
        item._btnBuy->setTag(i);
        item._btnBuy->addTouchEventListener(CC_CALLBACK_2(TTUIShop::onClickBuyBtn, this));
        
        _shopItems.push_back(item);
        
        // Seteamos los precios de los productos
        cocos2d::ui::TextBMFont* _prize = (cocos2d::ui::TextBMFont*)_widgetShop->getChildByName("shop")->getChildByName(stItem)->getChildByName("shopItem")->getChildByName("buyItemBtn")->getChildByName("prizeLabel");
        
        std::string _prizeStr = "";
        char stItemPrize[100] = {0};
        if(_storeItems[i]->_currency == "EUR")
        {
            sprintf(stItemPrize,"%.2f €",_storeItems[i]->_priceCurrency);
            _prizeStr = stItemPrize;
        }
        else
        {
            sprintf(stItemPrize,"%.2f $",_storeItems[i]->_price);
            _prizeStr = stItemPrize;
        }
        _prize->setString(_prizeStr);
    }
    
    _btnExit = (cocos2d::ui::Button*)_widgetShop->getChildByName("shop")->getChildByName("shopHeader")->getChildByName("exitBtn");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTUIShop::onClickExitBtn, this));
    
    // Creamos el bg
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
    
    // Registro de recepcion de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_COINS_BALANCE_UPDATED,CC_CALLBACK_1(TTUIShop::onEvent_CoinsBalanceUpdated,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_PURCHASE_STORE_ERROR,CC_CALLBACK_1(TTUIShop::onEvent_PurchaseStoreError,this)));

}

void TTUIShop::pauseUI()
{
    _blockMenus = true;
}

void TTUIShop::restoreUI()
{
    _blockMenus = false;
    _shopMoneyLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(TTStoreManager::getInstance()->getUserBalance()));
}

void TTUIShop::showShop(Node* _parent)
{
    _actualParent = _parent;
    TTUIStackManager::getInstance()->showUILoading(_parent);
    preloadResources();
    
    // Enviamos evento de analitica a Parse
    std::map<std::string,std::string> _dimensions;
    TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_SHOP_MENU,_dimensions);
}

void TTUIShop::printShop(Node* _parent)
{
    TTAudioManager::getInstance()->playAudio("PANEL_BUY_DOWN");
    
    _blockMenus = false;
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,31);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiShopLayer->getParent() != nullptr)
        _uiShopLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiShopLayer,32);
    _uiShopLayer->setVisible(true);
    _widgetShop->setVisible(true);
    
    _shopMoneyLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(TTStoreManager::getInstance()->getUserBalance()));
    
    //Seteamos el idioma
    _shopTitleLabel->setString(TTLanguageManager::getInstance()->getTag(TITLE_SHOP_TAG));
    _shopMoneyTitleLabel->setString(TTLanguageManager::getInstance()->getTag(BALANCE_TAG));
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_SHOP.ExportJson","shop_enter");
}

void TTUIShop::exitMenu()
{
    _uiShopLayer->setVisible(false);
    _widgetShop->setVisible(false);
    _bgUI->setVisible(false);
    
    TTUIStackManager::getInstance()->closeUI();
}

void TTUIShop::onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{

    if (_blockMenus )
        return;

    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_SHOP.ExportJson","shop_enter")->stop();
    // ocultamos el loading de las UI
    TTUIStackManager::getInstance()->hideUILoading();
    
    TTAudioManager::getInstance()->playAudio("BUTTON_CLOSE_SHOP");
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_SHOP.ExportJson","shop_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideShop = CallFunc::create(std::bind([&](){
        exitMenu();
        destroyInstance();
    }));
    _uiShopLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideShop,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_BUY_UP");
}

void TTUIShop::onClickBuyBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_ITEM");
    
    int _itemShopId = ((cocos2d::ui::Widget*)pSender)->getTag();
    
    
    TTUIStackManager::getInstance()->showUILoading(_actualParent);
    TTStoreManager::getInstance()->buyShopItem(_itemShopId);
}

void TTUIShop::onEvent_CoinsBalanceUpdated(cocos2d::EventCustom *e)
{
    if (_uiShopLayer->isVisible())
    {
        TTAudioManager::getInstance()->playAudio("COIN_INCREMENT");
        _shopMoneyLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(TTStoreManager::getInstance()->getUserBalance()));
    }
    TTUIMap::getInstance()->updateUIValues();
    _blockMenus = false;
    
    //cerrar la tienda
    if (_uiShopLayer->isVisible())
    {
        this->onClickExitBtn(this, ui::Widget::TouchEventType::ENDED);
    }
 
}

void TTUIShop::onEvent_PurchaseStoreError(cocos2d::EventCustom *e)
{
    //ERROR en la compra
    MarketDataEvent *data = static_cast<MarketDataEvent*>(e->getUserData());

    if (data->cancelled)
    {
        CCLOG("COMPRA CANCELADA");
        
        //debloquear menus
        _blockMenus = false;
    }
    else if (data->success==false)
    {
        CCLOG("ERROR al COMPRAR item");
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
        TTUIPopupSmart::getInstance()->showPopupState(_actualParent,SMART_POPUP_STORE_STATE,false);
    }
    
    // ocultamos el loading
    TTUIStackManager::getInstance()->hideUILoading();
}
