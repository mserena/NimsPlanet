#ifndef __CCCoreEventDispatcher_H_
#define __CCCoreEventDispatcher_H_

#include "cocos2d.h"
#include "CCAbstractAggregatedEventHandler.h"
#include "CCCoreEventHandler.h"

namespace soomla {

	/** \class CCCoreEventDispatcher
		\brief Calls event handler functions when events are fired, also contains settings for StoreController.

		This class calls event handler functions when events are fired, to tie
		it to your event handler call addEventHandler(). You also set parameters
		for StoreController through this class.
	 */
    class CCCoreEventDispatcher : public CCAbstractAggregatedEventHandler<CCCoreEventHandler>, public CCCoreEventHandler {
    public:
		/**
		   This class is singleton, access it with this function.
		 */
        static CCCoreEventDispatcher *getInstance();

        bool init();

        virtual void onRewardGivenEvent(CCReward *reward);
        virtual void onRewardTakenEvent(CCReward *reward);
    };
};

#endif //__CCCoreEventDispatcher_H_
