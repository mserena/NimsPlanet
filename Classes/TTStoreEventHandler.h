//
//  TTStoreEventHandler.h
//  animalplanet
//
//  Created by Albert Aymerich on 14/11/14.
//
//

#ifndef __animalplanet__TTStoreEventHandler__
#define __animalplanet__TTStoreEventHandler__

#include "thegame.h"
#include "Cocos2dxStore.h"


class TTStoreEventHandler : public soomla::CCStoreEventHandler {
public:

    TTStoreEventHandler();
    
    virtual void onBillingNotSupported();
    
    virtual void onBillingSupported();
    
    virtual void onCurrencyBalanceChanged(soomla::CCVirtualCurrency *virtualCurrency, int balance, int amountAdded);
    
    virtual void onGoodBalanceChanged(soomla::CCVirtualGood *virtualGood, int balance, int amountAdded);
    
    virtual void onGoodEquipped(soomla::CCEquippableVG *equippableVG);
    
    virtual void onGoodUnEquipped(soomla::CCEquippableVG *equippableVG);
    
    virtual void onGoodUpgrade(soomla::CCVirtualGood *virtualGood, soomla::CCUpgradeVG *upgradeVG);
    
    virtual void onItemPurchased(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    
    virtual void onItemPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    
    virtual void onMarketPurchaseCancelled(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    
    virtual void onMarketPurchase(soomla::CCPurchasableVirtualItem *purchasableVirtualItem, cocos2d::__String *token, cocos2d::__String *payload);
    
    virtual void onMarketPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    
    virtual void onMarketPurchaseVerification(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    
    virtual void onRestoreTransactionsStarted();
    
    virtual void onRestoreTransactionsFinished(bool success);
    
    virtual void onUnexpectedErrorInStore();
    
    virtual void onStoreControllerInitialized();
    
    virtual void onMarketItemsRefreshed(cocos2d::__Array *virtualItems);
    
    virtual void onMarketItemsRefreshStarted();
    

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    virtual void onMarketRefund(soomla::CCPurchasableVirtualItem *purchasableVirtualItem);
    virtual void onIabServiceStarted();
    virtual void onIabServiceStopped();

#endif
    
public:
    int _currentCoinBalance;
    
private:
    bool isPurchasing;
};


#endif /* defined(__animalplanet__TTStoreEventHandler__) */
