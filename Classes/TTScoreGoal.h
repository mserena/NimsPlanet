#ifndef __TTScoreGoal__
#define __TTScoreGoal__

#include "TTGoal.h"

class TTScoreGoal : public TTGoal {
public:
    
    TTScoreGoal(int scoreToWin, bool opt = false);
    ~TTScoreGoal();
    void reset();
    
    void scoreWin(int scoreWin);

};

#endif 
