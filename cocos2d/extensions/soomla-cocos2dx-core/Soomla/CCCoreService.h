
#ifndef __CCCoreService_H_
#define __CCCoreService_H_

#include "cocos2d.h"
#include "CCError.h"
#include "CCReward.h"
#include "CCSequenceReward.h"

namespace soomla {

    class CCCoreService: public cocos2d::Ref {
    public:
		/**
		   This class is singleton, use this function to access it.
		*/
        static CCCoreService* getInstance();

        static void initShared();

        CCCoreService();

        virtual bool init();

        int getTimesGiven(CCReward *reward);
        void setRewardStatus(CCReward *reward, bool give, bool notify);
        int getLastSeqIdxGiven(soomla::CCSequenceReward *sequenceReward);
        void setLastSeqIdxGiven(CCSequenceReward *sequenceReward, unsigned int idx);

        const char *kvStorageGetValue(const char *key) const;
        void kvStorageSetValue(const char *key, const char *val);
        void kvStorageDeleteKeyValue(const char *key);
        void kvStoragePurge();
    };
};

#endif // !__CCCoreService_H_
