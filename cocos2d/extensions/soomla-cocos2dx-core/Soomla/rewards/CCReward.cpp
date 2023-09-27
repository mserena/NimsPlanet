
#include "CCReward.h"
#include "CCRewardStorage.h"
#include "CCSoomlaUtils.h"

#define TAG "SOOMLA Reward"

bool soomla::CCReward::init(cocos2d::__String *id, cocos2d::__String *name, CCSchedule *schedule) {
    bool ret = CCSoomlaEntity::init(id, name);

    if (ret) {
        setSchedule(schedule != NULL ? schedule : CCSchedule::createAnyTimeOnce());
        return true;
    }

    return ret;
}

bool soomla::CCReward::initWithDictionary(cocos2d::__Dictionary *dict) {
    bool ret = CCSoomlaEntity::initWithDictionary(dict);

    if (ret) {
        CCSchedule *schedule;
        if (dict->objectForKey(CCCoreConsts::JSON_SCHEDULE)) {
            schedule = CCSchedule::createWithDictionary((cocos2d::__Dictionary *) dict->objectForKey(CCCoreConsts::JSON_SCHEDULE));
        }
        else {
            schedule = CCSchedule::createAnyTimeOnce();
        }
        setSchedule(schedule);

        return true;
    }

    return ret;
}

cocos2d::__Dictionary *soomla::CCReward::toDictionary() {
    cocos2d::__Dictionary* dict = CCSoomlaEntity::toDictionary();

    if (mSchedule) {
        dict->setObject(mSchedule->toDictionary(), CCCoreConsts::JSON_SCHEDULE);
    }

    return dict;
}

soomla::CCReward::~CCReward() {
    CC_SAFE_RELEASE(mSchedule);
}

bool soomla::CCReward::take() {
    if (!CCRewardStorage::getInstance()->isRewardGiven(this)) {
        CCSoomlaUtils::logDebug(TAG,
                cocos2d::__String::createWithFormat("Reward not given. id: %s", getId()->getCString())->getCString());
        return false;
    }

    if (takeInner()) {
        CCRewardStorage::getInstance()->setRewardStatus(this, false);
        return true;
    }

    return false;
}

bool soomla::CCReward::give() {
    if (!mSchedule->approve(CCRewardStorage::getInstance()->getTimesGiven(this))) {
        CCSoomlaUtils::logDebug(TAG,
                cocos2d::__String::createWithFormat("(Give) Reward is not approved by Schedule. id: %s", getId()->getCString())->getCString());
        return false;
    }

    if (giveInner()) {
        CCRewardStorage::getInstance()->setRewardStatus(this, true);
        return true;
    }

    return false;
}

bool soomla::CCReward::isOwned() {
    return CCRewardStorage::getInstance()->isRewardGiven(this);
}
