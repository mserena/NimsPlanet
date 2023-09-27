#ifndef __TTStoneGoal__
#define __TTStoneGoal__

#include "TTGoal.h"

class TTStoneGoal : public TTGoal {
public:
    
    TTStoneGoal(int breaksToWin, bool opt = false);
    ~TTStoneGoal();
    
    void reset();
    
    void onEvent_newBreak(EventCustom* e);
    
private:
    
    std::vector<EventListener*> _classListeners;
    
};

#endif 
