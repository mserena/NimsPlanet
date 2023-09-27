
#include "CCRandomReward.h"

soomla::CCRandomReward *soomla::CCRandomReward::create(
        cocos2d::__String *id,
        cocos2d::__String *name,
        cocos2d::__Array *rewards,
        CCSchedule *schedule) {

    CCRandomReward *ret = new CCRandomReward();
    if (ret->init(id, name, rewards, schedule)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

soomla::CCRandomReward::~CCRandomReward() {
    CC_SAFE_RELEASE(mLastGivenReward);
}

const char *soomla::CCRandomReward::getType() const {
    return CCCoreConsts::JSON_JSON_TYPE_RANDOM;
}


bool soomla::CCRandomReward::takeInner() {
    // for now is able to take only last given
    if (mLastGivenReward == NULL) {
        return false;
    }

    bool taken = mLastGivenReward->take();
    setLastGivenReward(NULL);

    return taken;
}

bool soomla::CCRandomReward::giveInner() {
    CCReward *randomReward = (CCReward *) getRewards()->getRandomObject();
    randomReward->give();
    setLastGivenReward(randomReward);

    return true;
}
