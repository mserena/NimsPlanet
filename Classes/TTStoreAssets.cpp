//
//  TTStoreAssets.cpp
//  animalplanet
//
//  Created by Albert Aymerich on 03/11/14.
//
//

#include "TTStoreAssets.h"



USING_NS_CC;
using namespace soomla;

TTStoreAssets * TTStoreAssets::create() {
    TTStoreAssets* ret = new TTStoreAssets();
    
    ret->autorelease();
    
    ret->init();
    
    return ret;
}

bool TTStoreAssets::init() {
    //TODO: create ALL Items and put into properties
    
    /** Virtual Currencies **/
    
    CCVirtualCurrency *currencyItemId = CCVirtualCurrency::create(
                                                                  CCString::create("Coins"),
                                                                  CCString::create(""),
                                                                  CCString::create(COIN_CURRENCY_ITEM_ID)
                                                                  );
    mCurrencies = CCArray::create(currencyItemId, NULL);
    mCurrencies->retain();
    
    /** Virtual Currency Packs **/
    CCVirtualCurrencyPack *pack15 = createCurrencyPack("15 Coins","15 Coins Pack","coins_15",15,"pack_15_coin",0.99);
    CCVirtualCurrencyPack *pack60 = createCurrencyPack("60 Coins","60 Coins Pack","coins_60",60,"pack_60_coin",2.99);
    CCVirtualCurrencyPack *pack130 = createCurrencyPack("130 Coins","130 Coins Pack","coins_130",130,"pack_130_coin",6.99);
    CCVirtualCurrencyPack *pack340 = createCurrencyPack("340 Coins","340 Coins Pack","coins_340",340,"pack_340_coin",16.99);
    CCVirtualCurrencyPack *pack660 = createCurrencyPack("660 Coins","660 Coins Pack","coins_660",660,"pack_660_coin",31.99);
    
    
    mCurrencyPacks = CCArray::create(pack15, pack60, pack130, pack340, pack660, NULL);
     mCurrencyPacks->retain();

    
    return true;
}

CCVirtualCurrencyPack* TTStoreAssets::createCurrencyPack(const std::string name, const std::string description, const std::string itemID, int numOfItems, const std::string marketItemId, float marketPrice) {
    
   
    CCVirtualCurrencyPack *currencyPack = CCVirtualCurrencyPack::create(
                        CCString::create(name), // name
                        CCString::create(description), // description
                        CCString::create(itemID),  // item id
                        CCInteger::create(numOfItems),	// number of currencies in the pack
                        CCString::create(COIN_CURRENCY_ITEM_ID), // the currency associated with this pack
                        CCPurchaseWithMarket::create(
                                     CCString::create(marketItemId),
                                     CCDouble::create(marketPrice))
                       );

    return currencyPack;

}


TTStoreAssets::~TTStoreAssets() {
    CC_SAFE_RELEASE(mCurrencies);
    CC_SAFE_RELEASE(mGoods);
    CC_SAFE_RELEASE(mCurrencyPacks);
    CC_SAFE_RELEASE(mCategories);
}




void TTStoreAssets::addVirtualGood(const std::string name, const std::string description, const std::string itemId, int price) {
    CCVirtualGood *vg = CCSingleUseVG::create(
                        CCString::create(name), //name
                        CCString::create(description), // description
                        CCString::create(itemId), // item id
                        CCPurchaseWithVirtualItem::create(
                                CCString::create(COIN_CURRENCY_ITEM_ID),
                                CCInteger::create(price)
                        ) // the way this virtual good is purchased
   );
    if (this->mGoods==nullptr) {
        mGoods = CCArray::create( vg,NULL);
        mGoods->retain();
        
    } else {
        this->mGoods->addObject(vg);
    }
    
    CCLOG("AddVirtualGood[%s] Price[%d]",name.c_str(),price);

}

int TTStoreAssets::getVersion() {
    return 10; 
}

cocos2d::CCArray *TTStoreAssets::getCurrencies() {
    return mCurrencies;
}

cocos2d::CCArray *TTStoreAssets::getGoods() {
    return mGoods;
}

cocos2d::CCArray *TTStoreAssets::getCurrencyPacks() {
    return mCurrencyPacks;
}

cocos2d::CCArray *TTStoreAssets::getCategories() {
    return mCategories;
}
