#ifndef __TTDownObjectGoal__
#define __TTDownObjectGoal__

#include "TTGoal.h"

class TTDownObjectGoal : public TTGoal {
public:
    
    TTDownObjectGoal(int downObjectsToWin, int initWeight, int maxWeight,int increaseWeight, bool opt = false);
    ~TTDownObjectGoal();

    void reset();
    
private:
    
    void onEvent_objectDown(EventCustom* e);
    void onEvent_objectMove(EventCustom* e);

public:
    
    int _initWeight;
    int _actualWeight;
    int _increaseWeight;
    int _maxWeight;

private:
    std::vector<EventListener*> _classListeners;

};

#endif 
