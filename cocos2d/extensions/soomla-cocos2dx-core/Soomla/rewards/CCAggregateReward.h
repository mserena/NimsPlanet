

#ifndef __CCAggregateReward_H_
#define __CCAggregateReward_H_

#include "cocos2d.h"
#include "CCSoomlaMacros.h"
#include "CCCoreConsts.h"
#include "CCReward.h"

namespace soomla {
    class CCAggregateReward : public CCReward {
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__Array *, mRewards, Rewards, CCCoreConsts::JSON_REWARDS);
    public:
        CCAggregateReward() : CCReward(), mRewards(NULL) {
        };

        virtual bool init(
                cocos2d::__String *id,
                cocos2d::__String *name,
                cocos2d::__Array *rewards,
                CCSchedule *schedule = NULL);

        virtual bool initWithDictionary(cocos2d::__Dictionary *dict);

        virtual cocos2d::__Dictionary *toDictionary();

        virtual ~CCAggregateReward();
    };
}

#endif //__CCAggregateReward_H_
