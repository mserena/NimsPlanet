//
//  TTStoreEventHandler.cpp
//  animalplanet
//
//  Created by Albert Aymerich on 14/11/14.
//
//

#include "TTStoreEventHandler.h"
#include "TTEventsManager.h"

TTStoreEventHandler::TTStoreEventHandler()
{
    isPurchasing = false;
}

void TTStoreEventHandler::onBillingNotSupported() {
    CCLOG("onBillingNotSupported");
}

void TTStoreEventHandler::onBillingSupported() {
    CCLOG("onBillingSupported");
}

void TTStoreEventHandler::onCurrencyBalanceChanged(soomla::CCVirtualCurrency *virtualCurrency, int balance, int amountAdded) {
    CCLOG("onCurrencyBalanceChanged");
    //TODO: actualizar la tienda UI ?
    isPurchasing = false;
    _currentCoinBalance = balance;
    sendEvent(EVENT_COINS_BALANCE_UPDATED);
}

void TTStoreEventHandler::onGoodBalanceChanged(soomla::CCVirtualGood *virtualGood, int balance, int amountAdded) {
    CCLOG("onGoodBalanceChanged [%s] Balance[%d] AmountAdded[%d]",virtualGood->getName()->getCString(),balance, amountAdded);
}

void TTStoreEventHandler::onGoodEquipped(soomla::CCEquippableVG *equippableVG) {
    CCLOG("onGoodEquipped");
}

void TTStoreEventHandler::onGoodUnEquipped(soomla::CCEquippableVG *equippableVG) {
    CCLOG("onGoodUnEquipped");
}

void TTStoreEventHandler::onGoodUpgrade(soomla::CCVirtualGood *virtualGood, soomla::CCUpgradeVG *upgradeVG) {
    CCLOG("onGoodUpgrade");
}

void TTStoreEventHandler::onItemPurchased(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onItemPurchased");
}

void TTStoreEventHandler::onItemPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onItemPurchaseStarted");
}

void TTStoreEventHandler::onMarketPurchaseCancelled(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onMarketPurchaseCancelled");
    
    isPurchasing = false;
    MarketDataEvent dataEvent;
    dataEvent.success=false;
    dataEvent.cancelled=true;
    sendEvent(EVENT_PURCHASE_STORE_ERROR,&dataEvent);
}

void TTStoreEventHandler::onMarketPurchase(soomla::CCPurchasableVirtualItem *purchasableVirtualItem, cocos2d::__String *token, cocos2d::__String *payload) {
    CCLOG("onMarketPurchase");
}

void TTStoreEventHandler::onMarketPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onMarketPurchaseStarted");
    
    isPurchasing = true;
}

void TTStoreEventHandler::onMarketPurchaseVerification(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onMarketPurchaseVerification");
}

void TTStoreEventHandler::onRestoreTransactionsFinished(bool success) {
    CCLOG("onRestoreTransactionsFinished");
}

void TTStoreEventHandler::onRestoreTransactionsStarted() {
    CCLOG("onRestoreTransactionsStarted");
}

void TTStoreEventHandler::onUnexpectedErrorInStore() {
    CCLOG("onUnexpectedErrorInStore");
    if (isPurchasing)
    {
        isPurchasing = false;
        MarketDataEvent dataEvent;
        dataEvent.success=false;
        dataEvent.cancelled=false;
        sendEvent(EVENT_PURCHASE_STORE_ERROR,&dataEvent);
    }
}

void TTStoreEventHandler::onStoreControllerInitialized() {
    CCLOG("onStoreControllerInitialized");
}

void TTStoreEventHandler::onMarketItemsRefreshed(cocos2d::__Array *virtualItems) {
    CCLOG("onMarketItemsRefreshed");
    sendEvent(EVENT_MARKET_ITEMS_REFRESHED);
}

void TTStoreEventHandler::onMarketItemsRefreshStarted() {
    CCLOG("onMarketItemsRefreshStarted");
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void TTStoreEventHandler::onMarketRefund(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    CCLOG("onMarketRefund");
}

void TTStoreEventHandler::onIabServiceStarted() {
    CCLOG("onIabServiceStarted");
}

void TTStoreEventHandler::onIabServiceStopped() {
    CCLOG("onIabServiceStopped");
}
#endif

