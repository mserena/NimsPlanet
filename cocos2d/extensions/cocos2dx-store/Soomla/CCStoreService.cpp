

#include "CCStoreService.h"
#include "CCDomainFactory.h"
#include "CCStoreConsts.h"
#include "CCVirtualItem.h"
#include "CCMarketItem.h"
#include "CCVirtualCategory.h"
#include "CCVirtualCurrency.h"
#include "CCVirtualCurrencyPack.h"
#include "CCEquippableVG.h"
#include "CCSingleUsePackVG.h"
#include "CCUpgradeVG.h"
#include "CCSingleUseVG.h"
#include "CCDomainHelper.h"
#include "CCNdkBridge.h"
#include "CCSoomlaUtils.h"
#include "CCStoreInfo.h"
#include "CCStoreEventDispatcher.h"
#include "CCVirtualItemReward.h"

USING_NS_CC;


namespace soomla {

#define TAG "SOOMLA CCStoreService"

    static CCStoreService *sInstance = nullptr;

    soomla::CCStoreService *soomla::CCStoreService::getInstance() {
        if (!sInstance)
        {
            sInstance = new CCStoreService();
            sInstance->retain();
        }
        return sInstance;
    }

    void soomla::CCStoreService::initShared(CCStoreAssets *storeAssets, cocos2d::__Dictionary *storeParams) {
        CCStoreService *storeService = CCStoreService::getInstance();
        if (!storeService->init(storeAssets, storeParams)) {
            exit(1);
        }
    }

    soomla::CCStoreService::CCStoreService() {

    }

    bool soomla::CCStoreService::init(CCStoreAssets *storeAssets, cocos2d::__Dictionary *storeParams) {

        CCStoreEventDispatcher::getInstance();    // to get sure it's inited

        CCDomainFactory *domainFactory = CCDomainFactory::getInstance();
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_MARKET_ITEM, CCMarketItem::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_VIRTUAL_CATEGORY, CCVirtualCategory::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_VIRTUAL_CURRENCY, CCVirtualCurrency::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_VIRTUAL_CURRENCY_PACK, CCVirtualCurrencyPack::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_EQUIPPABLE_VG, CCEquippableVG::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_LIFETIME_VG, CCLifetimeVG::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_SINGLE_USE_PACK_VG, CCSingleUsePackVG::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_SINGLE_USE_VG, CCSingleUseVG::createWithDictionary);
        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_UPGRADE_VG, CCUpgradeVG::createWithDictionary);

        domainFactory->registerCreator(CCStoreConsts::JSON_JSON_TYPE_ITEM, &CCVirtualItemReward::createWithDictionary);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        __Bool *SSV = dynamic_cast<__Bool *>(storeParams->objectForKey("SSV"));
        if (SSV == NULL) {
            SSV = __Bool::create(false);
        }

        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCSoomlaStore::setSSV"), "method");
            params->setObject(SSV, "ssv");
            CCNdkBridge::callNative (params, NULL);
        }
#endif

        CCStoreInfo::createShared(storeAssets);

        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreService::init"), "method");
            CCError *error = NULL;
            CCNdkBridge::callNative (params, &error);

            if (error) {
                CCSoomlaUtils::logError(TAG, error->getInfo());
                return false;
            }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            {
                __String *androidPublicKey = dynamic_cast<__String *>(storeParams->objectForKey("androidPublicKey"));
                if (androidPublicKey != NULL && androidPublicKey->length()>0) {
                    __Dictionary *params = __Dictionary::create();
                    params->setObject(__String::create("CCSoomlaStore::setAndroidPublicKey"), "method");
                    params->setObject(androidPublicKey, "androidPublicKey");
                    CCNdkBridge::callNative (params, NULL);
                }

            }

            {
                __Bool *testPurchases = dynamic_cast<__Bool *>(storeParams->objectForKey("testPurchases"));
                if (testPurchases == NULL) {
                    testPurchases = __Bool::create(false);
                }
                __Dictionary *params = __Dictionary::create();
                params->setObject(__String::create("CCSoomlaStore::setTestPurchases"), "method");
                params->setObject(testPurchases, "testPurchases");
                CCNdkBridge::callNative (params, NULL);
            }
#endif
        }

        return true;
    }


}
