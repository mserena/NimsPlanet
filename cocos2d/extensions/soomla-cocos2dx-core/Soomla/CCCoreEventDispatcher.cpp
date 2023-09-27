#include "CCCoreEventDispatcher.h"
#include "CCDomainFactory.h"
#include "CCSoomlaEventDispatcher.h"

namespace soomla {

    USING_NS_CC;

    static CCCoreEventDispatcher *s_SharedInstance = NULL;

    CCCoreEventDispatcher *CCCoreEventDispatcher::getInstance() {
        if (!s_SharedInstance) {
            s_SharedInstance = new CCCoreEventDispatcher();
            s_SharedInstance->init();
        }

        return s_SharedInstance;
    }

    bool CCCoreEventDispatcher::init() {

        if (!CCAbstractAggregatedEventHandler::init()) {
            return false;
        }
        
        CCSoomlaEventDispatcher *eventDispatcher = CCSoomlaEventDispatcher::getInstance();

        eventDispatcher->registerEventHandler(CCCoreConsts::EVENT_REWARD_GIVEN,
                [this](__Dictionary *parameters) {
                    __Dictionary *rewardDict = dynamic_cast<__Dictionary *>(parameters->objectForKey("reward"));
                    CC_ASSERT(rewardDict);
                    CCReward *reward  = dynamic_cast<CCReward *>(CCDomainFactory::getInstance()->createWithDictionary(rewardDict));
                    CC_ASSERT(reward);
                    this->onRewardGivenEvent(reward);
                });
        eventDispatcher->registerEventHandler(CCCoreConsts::EVENT_REWARD_TAKEN,
                [this](__Dictionary *parameters) {
                    __Dictionary *rewardDict = dynamic_cast<__Dictionary *>(parameters->objectForKey("reward"));
                    CC_ASSERT(rewardDict);
                    CCReward *reward  = dynamic_cast<CCReward *>(CCDomainFactory::getInstance()->createWithDictionary(rewardDict));
                    CC_ASSERT(reward);
                    this->onRewardTakenEvent(reward);
                });

        return true;
    }

    void CCCoreEventDispatcher::onRewardGivenEvent(CCReward *reward) {
        FOR_EACH_EVENT_HANDLER(CCCoreEventHandler)
            eventHandler->onRewardGivenEvent(reward);
        }
    }

    void CCCoreEventDispatcher::onRewardTakenEvent(CCReward *reward) {
        FOR_EACH_EVENT_HANDLER(CCCoreEventHandler)
            eventHandler->onRewardTakenEvent(reward);
        }
    }
}
