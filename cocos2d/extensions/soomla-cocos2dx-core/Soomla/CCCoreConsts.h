#ifndef __CCCoreConsts__
#define __CCCoreConsts__

namespace soomla {
    class CCCoreConsts {
    public:
        static char const *JSON_ITEM_NAME;
        static char const *JSON_ITEM_DESCRIPTION;
        static char const *JSON_ITEM_ITEM_ID;

        static char const* JSON_JSON_TYPE;

        static char const *JSON_SCHEDULE;
        static char const *JSON_ICON_URL;
        static char const *JSON_REWARDS;

        /** Schedule **/
        static char const *JSON_SCHE_REC;
        static char const *JSON_SCHE_RANGES;
        static char const *JSON_SCHE_RANGE_START;
        static char const *JSON_SCHE_RANGE_END;
        static char const *JSON_SCHE_APPROVALS;

        static char const *JSON_JSON_TYPE_BADGE;
        static char const *JSON_JSON_TYPE_RANDOM;
        static char const *JSON_JSON_TYPE_SEQUENCE;
        static char const *JSON_JSON_TYPE_SCHEDULE;
        static char const *JSON_JSON_TYPE_DATE_TIME_RANGE;

        static char const *EVENT_REWARD_GIVEN;
        static char const *EVENT_REWARD_TAKEN;
    };
}

#endif /* defined(__CCCoreConsts__) */
