//
//  TTStoreAssets.h
//  animalplanet
//
//  Created by Albert Aymerich on 03/11/14.
//
//

#ifndef __animalplanet__TTStoreAssets__
#define __animalplanet__TTStoreAssets__

#define COIN_CURRENCY_ITEM_ID "coin_currency_id"

#include "thegame.h"
#include "CCStoreAssets.h"
#include "Cocos2dxStore.h"


class TTStoreAssets: public soomla::CCStoreAssets {
    cocos2d::Array *mCurrencies;
    cocos2d::Array *mGoods;
    cocos2d::Array *mCurrencyPacks;
    cocos2d::Array *mCategories;
    
public:
    TTStoreAssets():
    mCurrencies(NULL),
    mGoods(NULL),
    mCurrencyPacks(NULL),
    mCategories(NULL)
    {}
    
    virtual ~TTStoreAssets();
    
    static TTStoreAssets *create();
    bool init();
    
    
    void addVirtualGood(const std::string name, const std::string description, const std::string itemId, int price);
    
    virtual int getVersion();
    
    virtual cocos2d::CCArray *getCurrencies();
    
    virtual cocos2d::CCArray *getGoods();
    
    virtual cocos2d::CCArray *getCurrencyPacks();
    
    virtual cocos2d::CCArray *getCategories();

private:
    
    soomla::CCVirtualCurrencyPack* createCurrencyPack(const std::string name, const std::string description, const std::string itemID, int numOfItems, const std::string marketItemId, float marketPrice) ;
};


#endif /* defined(__animalplanet__TTStoreAssets__) */
