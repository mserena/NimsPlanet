#ifndef __TTTimeRestriction__
#define __TTTimeRestriction__

#include "TTRestriction.h"
#include "cocos2d.h"

using namespace cocos2d;

class TTTimeRestriction : public TTRestriction {
protected:
    
public:
    
    TTTimeRestriction();
    TTTimeRestriction(int limit, bool allowsFinish);
    ~TTTimeRestriction();

    void update(float dt);
    void reset();
    
    int getTimeToFinish();
    void addExtraTime(int time);
    
    void onEvent_pauseGame(EventCustom* e);
    void onEvent_continueGame(EventCustom* e);

public:
    
    float _extraTime;
    float _limitTime;
    float _currTime;
    bool _finishedTime;
    bool _pause;
    
private:
    
    std::vector<EventListener*> _classListeners;
};


#endif 