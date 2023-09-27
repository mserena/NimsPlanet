
#ifndef __CCSchedule_H_
#define __CCSchedule_H_

#include "CCDomain.h"
#include "CCSoomlaMacros.h"
#include "CCCoreConsts.h"

namespace soomla {
    class CCSchedule : public CCDomain {
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__Integer*, mRequiredRecurrence, RequiredRecurrence, CCCoreConsts::JSON_SCHE_REC);
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__Array*, mTimeRanges, TimeRanges, CCCoreConsts::JSON_SCHE_RANGES);
        SL_SYNTHESIZE_RETAIN_WITH_DICT(cocos2d::__Integer*, mActivationLimit, ActivationLimit, CCCoreConsts::JSON_SCHE_APPROVALS);
    public:
        enum Recurrence {
            EVERY_MONTH,
            EVERY_WEEK,
            EVERY_DAY,
            EVERY_HOUR,
            NONE
        };

        class CCDateTimeRange: public CCDomain {
        public:
            timeval mStart;
            timeval mEnd;

            static CCDateTimeRange *create(timeval start, timeval end);
            SL_CREATE_WITH_DICTIONARY(CCDateTimeRange);
            virtual bool init(timeval start, timeval end);
            virtual bool initWithDictionary(cocos2d::__Dictionary* dict);
            virtual ~CCDateTimeRange();
            virtual cocos2d::__Dictionary* toDictionary();
        };

        CCSchedule(): CCDomain(), mRequiredRecurrence(NULL), mTimeRanges(NULL), mActivationLimit(NULL) {}

        static CCSchedule *create(cocos2d::__Integer *requiredRecurrence, cocos2d::__Array *timeRanges, cocos2d::__Integer *activationLimit);
        static CCSchedule *create(cocos2d::__Integer *activationLimit);
        static CCSchedule *create(cocos2d::__Integer *requiredRecurrence, timeval start, timeval end, cocos2d::__Integer *activationLimit);

        static CCSchedule *createAnyTimeOnce();
        static CCSchedule *createAnyTimeLimited(int activationLimit);
        static CCSchedule *createAnyTimeUnLimited();

        SL_CREATE_WITH_DICTIONARY(CCSchedule);

        virtual bool init(cocos2d::__Integer *requiredRecurrence, cocos2d::__Array *timeRanges, cocos2d::__Integer *activationLimit);
        virtual bool initWithDictionary(cocos2d::__Dictionary* dict);

        virtual ~CCSchedule();

        virtual cocos2d::__Dictionary* toDictionary();

        bool approve(int activationTimes);
    };
}

#endif //__CCSchedule_H_
