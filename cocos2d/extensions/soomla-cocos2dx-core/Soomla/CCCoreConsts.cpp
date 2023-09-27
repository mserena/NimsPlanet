#include "CCCoreConsts.h"

namespace soomla {
    char const *CCCoreConsts::JSON_ITEM_NAME = "name";
    char const *CCCoreConsts::JSON_ITEM_DESCRIPTION = "description";
    char const *CCCoreConsts::JSON_ITEM_ITEM_ID = "itemId";

    char const *CCCoreConsts::JSON_JSON_TYPE = "className";

    char const *CCCoreConsts::JSON_SCHEDULE = "schedule";
    char const *CCCoreConsts::JSON_ICON_URL = "iconUrl";
    char const *CCCoreConsts::JSON_REWARDS = "rewards";

    char const *CCCoreConsts::JSON_SCHE_REC = "schedRecurrence";
    char const *CCCoreConsts::JSON_SCHE_RANGES = "schedTimeRanges";
    char const *CCCoreConsts::JSON_SCHE_RANGE_START = "schedTimeRangeStart";
    char const *CCCoreConsts::JSON_SCHE_RANGE_END = "schedTimeRangeEnd";
    char const *CCCoreConsts::JSON_SCHE_APPROVALS = "schedApprovals";

    char const *CCCoreConsts::JSON_JSON_TYPE_BADGE = "BadgeReward";
    char const *CCCoreConsts::JSON_JSON_TYPE_RANDOM = "RandomReward";
    char const *CCCoreConsts::JSON_JSON_TYPE_SEQUENCE = "SequenceReward";
    char const *CCCoreConsts::JSON_JSON_TYPE_SCHEDULE = "Schedule";
    char const *CCCoreConsts::JSON_JSON_TYPE_DATE_TIME_RANGE = "DateTimeRange";

    char const *CCCoreConsts::EVENT_REWARD_GIVEN = "com.soomla.events.RewardGivenEvent";
    char const *CCCoreConsts::EVENT_REWARD_TAKEN = "com.soomla.events.RewardTakenEvent";

}