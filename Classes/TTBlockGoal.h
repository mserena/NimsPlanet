#ifndef __TTBlockGoal__
#define __TTBlockGoal__

#include "TTGoal.h"

class TTBlockGoal : public TTGoal {
public:
    
    TTBlockGoal(int breaksToWin, bool opt = false);
    ~TTBlockGoal();

    void reset();
    
    void onEvent_newBreak(EventCustom* e);
    
private:
    
    std::vector<EventListener*> _classListeners;

};

#endif 
