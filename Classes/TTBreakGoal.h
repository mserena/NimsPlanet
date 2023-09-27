#ifndef __TTBreakGoal__
#define __TTBreakGoal__

#include "TTGoal.h"

class TTBreakGoal : public TTGoal {
public:
    
    TTBreakGoal(int breaksToWin, bool opt = false);
    ~TTBreakGoal();

    void reset();
    
    void onEvent_newBreak(EventCustom* e);
    
private:
    
    std::vector<EventListener*> _classListeners;

};

#endif 
