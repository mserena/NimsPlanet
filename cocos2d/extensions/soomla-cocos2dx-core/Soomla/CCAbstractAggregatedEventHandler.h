

#ifndef __CCAbstractAggregatedEventHandler_H_
#define __CCAbstractAggregatedEventHandler_H_


#include "cocos2d.h"

namespace soomla {

#define FOR_EACH_EVENT_HANDLER(__TYPE__) __SetIterator i;\
    for(i = mEventHandlers->begin(); i != mEventHandlers->end(); i++) {\
        __TYPE__ *eventHandler = dynamic_cast<__TYPE__ *>(*i);\

    template <class T> class CCAbstractAggregatedEventHandler {
    protected:
        cocos2d::__Set *mEventHandlers;
        cocos2d::__Set *tempAddEventHandlers;
        cocos2d::__Set *tempRemoveEventHandlers;
        int lockCount;
    public:
        CCAbstractAggregatedEventHandler() :
                mEventHandlers(nullptr), tempAddEventHandlers(nullptr),
                tempRemoveEventHandlers(nullptr), lockCount(0) {
        }
        virtual bool init() {
            mEventHandlers = cocos2d::__Set::create();
            mEventHandlers->retain();

            tempAddEventHandlers = cocos2d::__Set::create();
            tempAddEventHandlers->retain();

            tempRemoveEventHandlers = cocos2d::__Set::create();
            tempRemoveEventHandlers->retain();

            return true;
        }

        virtual ~CCAbstractAggregatedEventHandler() {
            CC_SAFE_RELEASE(mEventHandlers);
        }

        /**
        Add an event handler. This retains the event handler.
        \param eventHandler A pointer to the event handler you'd like to add.
        */
        void addEventHandler(T *eventHandler) {
            if (lockCount <= 0) {
                mEventHandlers->addObject(eventHandler);
            }
            else {
                tempAddEventHandlers->addObject(eventHandler);
            }
        }

        /**
        Remove an event handler. This releases the event handler.
        \param eventHandler A pointer to the event handler you'd like to remove.
        */
        void removeEventHandler(T *eventHandler) {
            if (lockCount <= 0) {
                mEventHandlers->removeObject(eventHandler);
            }
            else {
                tempRemoveEventHandlers->addObject(eventHandler);
            }
        }

        /**
         Removes all event handlers from the event handler
         */
        void purge() {
            mEventHandlers->removeAllObjects();

            lockCount = 0;

            tempAddEventHandlers->removeAllObjects();
            tempRemoveEventHandlers->removeAllObjects();
        }
    protected:
        void lockEventHandlers() {
            if (lockCount == 0) {
                tempAddEventHandlers->removeAllObjects();
                tempRemoveEventHandlers->removeAllObjects();
            }

            lockCount++;
        }

        void unlockEventHandlers() {
            for(cocos2d::__SetIterator i = tempAddEventHandlers->begin(); i != tempAddEventHandlers->end(); i++) {
                if (!(*i)) {
                    break;
                }
                
                T *addHandler = dynamic_cast<T *>(*i);
                addEventHandler(addHandler);
            }

            for(cocos2d::__SetIterator i = tempRemoveEventHandlers->begin(); i != tempRemoveEventHandlers->end(); i++) {
                if (!(*i)) {
                    break;
                }
                
                T *removeHandler = dynamic_cast<T *>(*i);
                removeEventHandler(removeHandler);
            }

            lockCount--;

            if (lockCount <= 0) {
                tempAddEventHandlers->removeAllObjects();
                tempRemoveEventHandlers->removeAllObjects();

                lockCount = 0;
            }
        }

    };
}

#endif //__CCAbstractAggregatedEventHandler_H_
