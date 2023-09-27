#ifndef __TTStoreManager_SCENE_H__
#define __TTStoreManager_SCENE_H__

#include "cocos2d.h"
#include "TTStoreAssets.h"
#include "TTStoreEventHandler.h"

using namespace cocos2d;

typedef struct storeItem
{
    int     _numCoins;
    float   _price; //price in usS
    std::string _priceCurrencyDesc; //descripction price with currency
    
    std::string _currency; //currency of the item
    float _priceCurrency; //price in the currency
    
}storeItem;

#define ITEM_BOOSTER "BOOSTER_"

class TTStoreManager
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTStoreManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();

protected:
    
    TTStoreManager(){}

public:
    
    virtual ~TTStoreManager();
    bool init(void);
    
    int getVirtualItemPrice(std::string _virtualItemId);
    bool buyVirtualItem(std::string _virtualItemId);
    void giftBoosterToUser(std::string _virtualItemId);
    int getNumberOfUserVirtualItems(std::string _virtualItemId);
    bool takeUserVirtualItem(std::string _virtualItemId);
    
    void updateItemsShopPrice();
    void buyShopItem(int idShopItem);
    int getUserBalance();
    void addUserInitGameCoins();
    void giftCoinsToUser(int numCoins);
    
    void syncWithParseInfo(int _coins);
    
    void onEvent_MarketItemsRefreshed(EventCustom* e);
    
private:
    
    void loadVirtualItems(TTStoreAssets* _assets);

public:
    
    std::vector<storeItem*> _storeItems;
    
private:
    
    TTStoreEventHandler *handler;
    
};

#endif  // __TTStoreManager_SCENE_H__
