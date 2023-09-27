#ifndef __TTNetworkGoal__
#define __TTNetworkGoal__

#include "TTGoal.h"

class TTNetworkGoal : public TTGoal {
public:
    
    TTNetworkGoal(int breaksToWin, bool opt = false);
    ~TTNetworkGoal();
    
    void reset();
    
    void onEvent_newBreak(EventCustom* e);
    
private:
    
    std::vector<EventListener*> _classListeners;
    
};

#endif
