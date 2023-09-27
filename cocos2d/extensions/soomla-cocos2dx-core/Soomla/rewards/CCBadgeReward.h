


#ifndef __CCBadgeReward_H_
#define __CCBadgeReward_H_

#include "cocos2d.h"
#include "CCSoomlaMacros.h"
#include "CCReward.h"

namespace soomla {
    class CCBadgeReward : public CCReward {
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__String *, mIconUrl, IconUrl, CCCoreConsts::JSON_ICON_URL);
    public:
        CCBadgeReward() : CCReward(), mIconUrl(NULL) {
        };

        static CCBadgeReward *create(
                cocos2d::__String *id,
                cocos2d::__String *name,
                cocos2d::__String *iconUrl = NULL);

        SL_CREATE_WITH_DICTIONARY(CCBadgeReward);

        virtual bool init(
                cocos2d::__String *id,
                cocos2d::__String *name,
                cocos2d::__String *iconUrl = NULL);

        virtual bool initWithDictionary(cocos2d::__Dictionary *dict);

        virtual cocos2d::__Dictionary *toDictionary();

        virtual ~CCBadgeReward();

        virtual const char *getType() const;

    protected:
        virtual bool takeInner() override;

        virtual bool giveInner() override;
    };
}

#endif //__CCBadgeReward_H_
