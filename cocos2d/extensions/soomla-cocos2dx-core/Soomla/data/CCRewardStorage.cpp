
#include "CCRewardStorage.h"
#include "CCReward.h"
#include "CCSequenceReward.h"
#include "CCCoreService.h"

#define TAG "SOOMLA RewardStorage"

namespace soomla {
    static CCRewardStorage *s_SharedRewardStorage = NULL;

    CCRewardStorage *CCRewardStorage::getInstance() {
        if (!s_SharedRewardStorage) {
            s_SharedRewardStorage = new CCRewardStorage();
            s_SharedRewardStorage->retain();
        }

        return s_SharedRewardStorage;
    }

    int CCRewardStorage::getTimesGiven(CCReward *reward) {
        return CCCoreService::getInstance()->getTimesGiven(reward);
    }

    bool CCRewardStorage::isRewardGiven(CCReward *reward) {
        return getTimesGiven(reward) > 0;
    }

    void CCRewardStorage::setRewardStatus(CCReward *reward, bool give) {
        setRewardStatus(reward, give, true);
    }

    void CCRewardStorage::setRewardStatus(CCReward *reward, bool give, bool notify) {
        CCCoreService::getInstance()->setRewardStatus(reward, give, notify);
    }

    int CCRewardStorage::getLastSeqIdxGiven(CCSequenceReward *sequenceReward) {
        return CCCoreService::getInstance()->getLastSeqIdxGiven(sequenceReward);
    }

    void CCRewardStorage::setLastSeqIdxGiven(CCSequenceReward *sequenceReward, unsigned int idx) {
        CCCoreService::getInstance()->setLastSeqIdxGiven(sequenceReward, idx);
    }
}
