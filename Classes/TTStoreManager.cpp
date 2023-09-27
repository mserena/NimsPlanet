#include "TTStoreManager.h"
#include "TTEventsManager.h"
#include "TTUserInfo.h"
#include "pugixml.hpp"
#include "TTStoreAssets.h"
#include "TTStoreEventHandler.h"
#include "Cocos2dxStore.h"
#include "TTParseManager.h"

#define SHOP_XML    "Shop.xml"
#define DEFAULT_INIT_GAME_COINS 25

using namespace soomla;

//Singelton
static TTStoreManager *_sharedStoreManager = nullptr;

TTStoreManager* TTStoreManager::getInstance()
{
    if (!_sharedStoreManager)
    {
        _sharedStoreManager = new TTStoreManager();
        _sharedStoreManager->init();
    }
    
    return _sharedStoreManager;
}

void TTStoreManager::destroyInstance()
{
    CC_SAFE_DELETE(_sharedStoreManager);
}

TTStoreManager::~TTStoreManager()
{
    TTEventsManager::getInstance()->removeSafeListenerEvents("TTStoreManager");
    soomla::CCStoreEventDispatcher::getInstance()->removeEventHandler(handler);
}

bool TTStoreManager::init(void)
{
    
    TTEventsManager::getInstance()->addListenerEvent("TTStoreManager", EVENT_MARKET_ITEMS_REFRESHED, CC_CALLBACK_1(TTStoreManager::onEvent_MarketItemsRefreshed, this));
    
    handler = new TTStoreEventHandler();
    
    TTStoreAssets *assets = TTStoreAssets::create();
    
    loadVirtualItems(assets);
    
    __Dictionary *commonParams = __Dictionary::create();
    commonParams->setObject(__String::create("re3n7e#9IVu$h!0iR1c$l0h69Cfz#&ts"), "customSecret");
    soomla::CCServiceManager::getInstance()->setCommonParams(commonParams);
    
    //FOR ANDROID
    __Dictionary *storeParams = __Dictionary::create();
    storeParams->setObject(__String::create("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAieIDMSl9Z8duELnOgSwKKspRm/33XmsVvNwXZUvv1DrTKlFZfc3eh6fSpPzrepne9IVuzjhx0iRZuz0q++bgWM9z3iMt5t/hBf70/mCpmqco1JN+suy1/bjiC8oVx6J/c3Tamv9RD7fPfsj0Ct4x0EjgS65gzvNX2IwYM9Uec4/8ARpYlb0W6XnCpQA6wR/VwPGgcQAUu5NbAUWGTPiRAK0H5Tizxz9BhKXIBXsNJROasq0rjtwxm7vYQEC7wv/yzkxi2vXcYxIS8kecQTsdZfXbdlgGs42JkvRB6CLy4Zybfa4p9X8xYC/va98vsRGBkowOmnTOo/t1c09CfFOwhwIDAQAB"), "androidPublicKey");
    storeParams->setObject(__Bool::create(true), "testPurchases");
    
    soomla::CCStoreService::initShared(assets, storeParams);
    
    soomla::CCStoreEventDispatcher::getInstance()->addEventHandler(handler);
    
    soomla::CCError* error = nullptr;
    handler->_currentCoinBalance = soomla::CCStoreInventory::sharedStoreInventory()->getItemBalance(COIN_CURRENCY_ITEM_ID, &error);
    
    // Updateamos los precios desde la store
    updateItemsShopPrice();
    
    return true;
}

void TTStoreManager::addUserInitGameCoins()
{
    soomla::CCError* error = nullptr;
    
    // Resetamos las monedas del usuario, por si acaso
    int coins = getUserBalance();
    soomla::CCStoreInventory::sharedStoreInventory()->takeItem(COIN_CURRENCY_ITEM_ID, coins, &error);
    
    //Añadimos las monedas por defecto
    soomla::CCStoreInventory::sharedStoreInventory()->giveItem(COIN_CURRENCY_ITEM_ID,DEFAULT_INIT_GAME_COINS, &error);
}

void TTStoreManager::giftCoinsToUser(int numCoins)
{
    soomla::CCError* error = nullptr;
    
    //Añadimos las monedas por defecto
    soomla::CCStoreInventory::sharedStoreInventory()->giveItem(COIN_CURRENCY_ITEM_ID,numCoins, &error);
    
    //Actualizamos en parse las monedas
    TTParseManager::getInstance()->storeUserInfo();
}

void TTStoreManager::giftBoosterToUser(std::string _virtualItemId)
{
    soomla::CCError* error = nullptr;
    
    //Añadimos el item virtual
    soomla::CCStoreInventory::sharedStoreInventory()->giveItem(_virtualItemId.c_str(),1,&error);
}

void TTStoreManager::onEvent_MarketItemsRefreshed(EventCustom* e)
{
    this->updateItemsShopPrice();
}


void TTStoreManager::loadVirtualItems(TTStoreAssets* _assets)
{
    CCLOG("Parseando XML de items virtuales");
    
    // Abrimos XML configuracion de la tienda
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(SHOP_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer el XML de ítems virtuales");
        abort();
    }
    
    // Shop Items
    pugi::xml_node ConfigNode = XMLFile->child("shopConfig");
    if(!ConfigNode.empty())
    {
        for(pugi::xml_node Node = ConfigNode.child("ShopItems").child("item"); Node; Node = Node.next_sibling("item"))
        {
            std::string _id = Node.attribute("id").as_string();
            int _price = Node.attribute("price").as_int();

            //SOOMLA
            _assets->addVirtualGood(_id, "", _id, _price);
        
        }
    }
    
    // Store Items
    if(!ConfigNode.empty())
    {
        for(pugi::xml_node Node = ConfigNode.child("StoreItems").child("item"); Node; Node = Node.next_sibling("item"))
        {
            int _numCoins = Node.attribute("numCoins").as_int();
            float _price = Node.attribute("price").as_float();
            storeItem* item = new storeItem();
            item->_numCoins = _numCoins;
            item->_price = _price;
            item->_currency = "$";
            item->_priceCurrency = _price;
            
            char stItemPrize[100] = {0};
            sprintf(stItemPrize,"%.2f$",_price);
            item->_priceCurrencyDesc = stItemPrize;

            _storeItems.push_back(item);
        }
    }
}

int TTStoreManager::getVirtualItemPrice(std::string _virtualItemId)
{
    soomla::CCError* error=nullptr;
    
    CCVirtualItem* item = soomla::CCStoreInfo::sharedStoreInfo()->getItemByItemId(_virtualItemId.c_str(), &error);
    if (error==nullptr)
    {
        return ((CCPurchaseWithVirtualItem*)((CCVirtualGood*)item)->getPurchaseType())->getAmount()->getValue();
    }
    else
    {
        soomla::CCSoomlaUtils::logException("getVirtualItemPrice", error);
    }
    return 66;
}

bool TTStoreManager::buyVirtualItem(std::string _virtualItemId)
{
    int _itemPrice = getVirtualItemPrice(_virtualItemId);
    int coins = getUserBalance();
    
    if(coins >= _itemPrice)
    {
        //Gestionamos la compra
        soomla::CCError* error = nullptr;
        soomla::CCStoreInventory::sharedStoreInventory()->buyItem(_virtualItemId.c_str(), &error);
        if (error==nullptr)
        {
            TTParseManager::getInstance()->storeUserInfo();
            return true;
        }
    }
    
    return false;
}

int TTStoreManager::getNumberOfUserVirtualItems(std::string _virtualItemId)
{
    soomla::CCError* error = nullptr;
    int numItems = soomla::CCStoreInventory::sharedStoreInventory()->getItemBalance(_virtualItemId.c_str(),&error);
    if (error!=nullptr)
    {
        CCLOG("error al consultar el número de ítem %s en soomla.",_virtualItemId.c_str());
        numItems = 0;
    }
    
    return numItems;
}

bool TTStoreManager::takeUserVirtualItem(std::string _virtualItemId)
{
    soomla::CCError* error = nullptr;
    soomla::CCStoreInventory::sharedStoreInventory()->takeItem(_virtualItemId.c_str(),1,&error);
    
    if (error!=nullptr)
    {
        CCLOG("error al usar ítem %s en soomla.",_virtualItemId.c_str());
        return false;
    }
    else
    {
        TTParseManager::getInstance()->storeUserInfo();
    }

    return true;
}

void TTStoreManager::buyShopItem(int idShopItem)
{
    //TODO: change this!
    char idItem[20] = {0};
    
    sprintf(idItem,"coins_%d",_storeItems[idShopItem]->_numCoins);
    
    soomla::CCError* error = nullptr;

#if TEST_GAME
    soomla::CCStoreInventory::sharedStoreInventory()->giveItem(COIN_CURRENCY_ITEM_ID,_storeItems[idShopItem]->_numCoins, &error);
#else
    soomla::CCStoreInventory::sharedStoreInventory()->buyItem(idItem, &error);
#endif
    
    if (error!=nullptr)
    {
        //TODO: tratamiento del error
        MarketDataEvent dataEvent;
        dataEvent.success=false;
        dataEvent.cancelled=false;
        sendEvent(EVENT_PURCHASE_STORE_ERROR,&dataEvent);
    }
    else
    {
        // Enviamos evento de analitica a Parse
        std::map<std::string,std::string> _dimensions = {{"Shop_Item",std::to_string(idShopItem)}};
        TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_BUY_COINS,_dimensions);
        
        TTParseManager::getInstance()->storeUserInfo();
    }
}

void TTStoreManager::updateItemsShopPrice()
{
    //Update items list prices
    for(int i = 0; i < _storeItems.size(); i++)
    {
        std::string _itemId;
        _itemId += "coins_"+std::to_string(_storeItems[i]->_numCoins);
        
        soomla::CCError* error=nullptr;
        CCVirtualItem* item = soomla::CCStoreInfo::sharedStoreInfo()->getItemByItemId(_itemId.c_str(), &error);
        if (error==nullptr)
        {
            CCMarketItem* _marketItem =  ((CCPurchaseWithMarket*)((CCVirtualGood*)item)->getPurchaseType())->getMarketItem();
            CCLOG("ITEM[%s] MARKET PRICE [%s]",_itemId.c_str(), _marketItem->getMarketPriceAndCurrency()->_string.c_str());
            _storeItems[i]->_price = _marketItem->getPrice()->getValue();
            _storeItems[i]->_currency = _marketItem->getMarketCurrencyCode()->_string;
            _storeItems[i]->_priceCurrency = _marketItem->getMarketPriceMicros()->getValue()/1000000.0;
            _storeItems[i]->_priceCurrencyDesc = _marketItem->getMarketPriceAndCurrency()->_string;

            if (_storeItems[i]->_priceCurrencyDesc.length()==0)
            {
                char stItemPrize[100] = {0};
                sprintf(stItemPrize,"%.2f$",_storeItems[i]->_price);
                _storeItems[i]->_priceCurrencyDesc = stItemPrize;
            }
        }
    }
}

int TTStoreManager::getUserBalance()
{
    return handler->_currentCoinBalance;
}

void TTStoreManager::syncWithParseInfo(int _coinsAux)
{
    // Sincronizamos las monedas
    if(_coinsAux > 0)
    {
        soomla::CCError* error = nullptr;
        soomla::CCStoreInventory::sharedStoreInventory()->giveItem(COIN_CURRENCY_ITEM_ID,_coinsAux, &error);
    }
    else if(_coinsAux < 0)
    {
        soomla::CCError* error = nullptr;
        int _coins = (-1)*_coinsAux;
        if((getUserBalance() - _coins) < 0)
        {
            soomla::CCStoreInventory::sharedStoreInventory()->takeItem(COIN_CURRENCY_ITEM_ID,getUserBalance(), &error);
        }
        else
        {
            soomla::CCStoreInventory::sharedStoreInventory()->takeItem(COIN_CURRENCY_ITEM_ID,_coins, &error);
        }
    }
    
    TTParseManager::getInstance()->storeUserInfo();
}
