

#include "CCSequenceReward.h"
#include "CCRewardStorage.h"

soomla::CCSequenceReward *soomla::CCSequenceReward::create(
        cocos2d::__String *id,
        cocos2d::__String *name,
        cocos2d::__Array *rewards
) {

    CCSequenceReward *ret = new CCSequenceReward();
    if (ret->init(id, name, rewards, NULL)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

soomla::CCSequenceReward::~CCSequenceReward() {
}

const char *soomla::CCSequenceReward::getType() const {
    return CCCoreConsts::JSON_JSON_TYPE_SEQUENCE;
}

soomla::CCReward *soomla::CCSequenceReward::getLastGivenReward() {
    int idx = CCRewardStorage::getInstance()->getLastSeqIdxGiven(this);
    if (idx < 0) {
        return NULL;
    }
    return (CCReward *) getRewards()->getObjectAtIndex(idx);
}

bool soomla::CCSequenceReward::hasMoreToGive() {
    return CCRewardStorage::getInstance()->getLastSeqIdxGiven(this) < getRewards()->count();
}

bool soomla::CCSequenceReward::forceNextRewardToGive(soomla::CCReward *reward) {
    for (unsigned int i = 0; i < getRewards()->count(); i++) {
        if (reward->equals(getRewards()->getObjectAtIndex(i))) {
            CCRewardStorage::getInstance()->setLastSeqIdxGiven(this, i - 1);
            return true;
        }
    }
    return false;
}

bool soomla::CCSequenceReward::takeInner() {
    int idx = CCRewardStorage::getInstance()->getLastSeqIdxGiven(this);
    if (idx <= 0) {
        return false; // all rewards in the sequence were taken
    }
    CCRewardStorage::getInstance()->setLastSeqIdxGiven(this, (unsigned int) --idx);
    return true;
}

bool soomla::CCSequenceReward::giveInner() {
    int idx = CCRewardStorage::getInstance()->getLastSeqIdxGiven(this);
    if (idx >= getRewards()->count()) {
        return false; // all rewards in the sequence were given
    }
    CCRewardStorage::getInstance()->setLastSeqIdxGiven(this, (unsigned int) ++idx);
    return true;
}
