#ifndef __CCEventDispatcher_H_
#define __CCEventDispatcher_H_

#include "cocos2d.h"

namespace soomla {

    class CCSoomlaEventDispatcher : public cocos2d::Ref {
	private:
        std::map<std::string, std::function<void(cocos2d::__Dictionary *)>> mEventHandlers;
    public:
        static CCSoomlaEventDispatcher *getInstance();

        void ndkCallback(cocos2d::__Dictionary *parameters);

		void registerEventHandler(const char *key, std::function<void(cocos2d::__Dictionary *)> handler);
		void unregisterEventHandler(const char *key);
    };
};

#endif //__CCEventDispatcher_H_
