#ifndef __TTEventsManager_SCENE_H__
#define __TTEventsManager_SCENE_H__

#include "TTEventsName.h"
#include "cocos2d.h"

using namespace cocos2d;

#ifndef ADD_EVENT_FUNC
#define ADD_EVENT_FUNC
void sendEvent(int eventId,void *optionalUserData = 0);
#endif

#ifndef REGISTER_LISTENER_EVENT_FUNC
#define REGISTER_LISTENER_EVENT_FUNC
EventListener* registerListenerEvent(int eventId,const std::function<void (EventCustom *)> &callback);
#endif

#ifndef REMOVE_LISTENER_EVENT_FUNC
#define REMOVE_LISTENER_EVENT_FUNC
//elimina TODOS los listeners asociados al evento que se le pasa
void removeListenerEvent(int eventId);
#endif

//eliminar un evenlistener previamente creado de manera segura
void removeSafeListenerEvent(EventListener* eventListener);

//eliminar todos los eventListener que se les pasa al vector
void removeSafeListenerEvent(std::vector<EventListener*>  &eventListeners);


// Clase singleton
class TTEventsManager {
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTEventsManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTEventsManager(){}
    
public:
    
    ~TTEventsManager();
    void init();
    
    EventListener* addListenerEvent(const std::string groupId, int eventId,const std::function<void (EventCustom *)> &callback);
    
    void removeSafeListenerEvents(std::string groupId);
    
private:
    
    std::map<std::string, std::vector<EventListener*> > _classListeners;
    
    
public:
    
    //properties
    
};



#endif  // __TTEventsManager_SCENE_H__
