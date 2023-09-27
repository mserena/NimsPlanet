#include "CCSoomlaEventDispatcher.h"

namespace soomla {

    USING_NS_CC;

    static CCSoomlaEventDispatcher *s_SharedInstance = NULL;

    CCSoomlaEventDispatcher *CCSoomlaEventDispatcher::getInstance() {
        if (!s_SharedInstance) {
            s_SharedInstance = new CCSoomlaEventDispatcher();
        }

        return s_SharedInstance;
    }

	void CCSoomlaEventDispatcher::registerEventHandler(const char *key, std::function<void(__Dictionary *)> handler) {
		mEventHandlers[key] = handler;
	}

    void CCSoomlaEventDispatcher::unregisterEventHandler(const char *key) {
        mEventHandlers.erase(key);
    }

    void CCSoomlaEventDispatcher::ndkCallback(__Dictionary *parameters) {
        __String *eventName = dynamic_cast<__String *>(parameters->objectForKey("method"));
        CC_ASSERT(eventName);

        std::function<void(__Dictionary *)> handler = mEventHandlers[eventName->getCString()];

        if (handler) {
            handler(parameters);
        }
		else {
            log("Unregistered event happened: %s", eventName->getCString());
        }
    }
}
