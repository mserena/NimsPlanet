#ifndef __TTCollectFiguresGoal__
#define __TTCollectFiguresGoal__

#include "TTGoal.h"

class TTCollectFiguresGoal : public TTGoal {
public:
    
    TTCollectFiguresGoal(int figureToCollect, int figuresToWin, bool opt = false);
    ~TTCollectFiguresGoal();
    void reset();
    void newCollect(int figure, int collect);
    
public:
    
    int _figureToCollect;
    
};

#endif
