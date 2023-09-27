#include "TTEventsManager.h"

void sendEvent(int eventId,void *optionalUserData)
{
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(std::to_string(eventId),optionalUserData);
}

EventListener* registerListenerEvent(int eventId,const std::function<void (EventCustom *)> &callback)
{
    return Director::getInstance()->getEventDispatcher()->addCustomEventListener(std::to_string(eventId),callback);
}

void removeListenerEvent(int eventId)
{
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(std::to_string(eventId));
}

void removeSafeListenerEvent(EventListener* eventListener)
{
    if (eventListener!=nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
}

void removeSafeListenerEvent(std::vector<EventListener*>  &eventListeners)
{
    for(int i = 0; i < eventListeners.size(); i++) {
        removeSafeListenerEvent(eventListeners[i]);
    }
}


//Singleton
static TTEventsManager* _sharedEventsManager = nullptr;

TTEventsManager* TTEventsManager::getInstance()
{
    if (!_sharedEventsManager)
    {
        _sharedEventsManager = new TTEventsManager();
        _sharedEventsManager->init();
    }
    
    return _sharedEventsManager;
}

void TTEventsManager::destroyInstance()
{
    CC_SAFE_DELETE(_sharedEventsManager);
}

TTEventsManager::~TTEventsManager()
{
    for (auto it=_classListeners.begin(); it!=_classListeners.end(); ++it) {
        removeSafeListenerEvent(it->second);
    }
}

void TTEventsManager::init()
{
    CCLOG("Initializing Events Manager");
    _classListeners.clear();
    
}

EventListener* TTEventsManager::addListenerEvent(const std::string groupId, int eventId,const std::function<void (EventCustom *)> &callback)
{
    EventListener* _eventListener = registerListenerEvent(eventId, callback);
    
    std::vector<EventListener*> _data =  _classListeners[groupId];
    _data.push_back(_eventListener);
    _classListeners[groupId] = _data;
    
    return _eventListener;
}

void TTEventsManager::removeSafeListenerEvents(std::string groupId)
{
    std::vector<EventListener*> _data = _classListeners[groupId];
    
    removeSafeListenerEvent(_data);
    
    _classListeners.erase(groupId);
}




