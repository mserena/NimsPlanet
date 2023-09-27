

#ifndef __CCRewardStorage_H_
#define __CCRewardStorage_H_

#include "cocos2d.h"
#include "CCReward.h"
#include "CCSequenceReward.h"

namespace soomla {
    class CCRewardStorage: public cocos2d::Ref {
    public:
        static CCRewardStorage *getInstance();

        void setRewardStatus(CCReward *reward, bool give);
        void setRewardStatus(CCReward *reward, bool give, bool notify);
        bool isRewardGiven(CCReward *reward);
        int getTimesGiven(CCReward *reward);
        int getLastSeqIdxGiven(CCSequenceReward *sequenceReward);
        void setLastSeqIdxGiven(CCSequenceReward *sequenceReward, unsigned int idx);
    };
}

#endif //__CCRewardStorage_H_
