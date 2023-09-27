
#ifndef __CCVirtualItemReward_H_
#define __CCVirtualItemReward_H_

#include "CCReward.h"
#include "CCCoreConsts.h"
#include "CCStoreConsts.h"

namespace soomla {
    class CCVirtualItemReward : public CCReward {
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__Integer *, mAmount, Amount, CCStoreConsts::JSON_AMOUNT);
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__String *, mAssociatedItemId, AssociatedItemId, CCStoreConsts::JSON_ASSOCIATED_ITEM_ID);
    public:
        static CCVirtualItemReward *create(
                cocos2d::__String *rewardId,
                cocos2d::__String *name,
                cocos2d::__Integer * amount,
                cocos2d::__String * associatedItemId
        );

        SL_CREATE_WITH_DICTIONARY(CCVirtualItemReward);

        virtual bool init(
                cocos2d::__String *rewardId,
                cocos2d::__String *name,
                cocos2d::__String *associatedItemId,
                cocos2d::__Integer *amount
        );

        virtual bool initWithDictionary(cocos2d::__Dictionary *dict);

        virtual cocos2d::__Dictionary *toDictionary();

        virtual ~CCVirtualItemReward();

        virtual const char *getType() const;


    protected:
        virtual bool takeInner();

        virtual bool giveInner();
    };
};

#endif //__CCVirtualItemReward_H_
