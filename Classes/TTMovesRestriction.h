#ifndef __TTMovesRestriction__
#define __TTMovesRestriction__

#include "TTRestriction.h"

class TTMovesRestriction : public TTRestriction {
protected:
    
public:
    
    TTMovesRestriction();
    TTMovesRestriction(int limit, bool allowsFinish);
    ~TTMovesRestriction();
    
    void reset();
    
    int getLimitMoves();
    void addExtraMoves(int moves);
    void doMove();

public:
    
    bool    _finishedMoves;
    
    int     _limitMoves;
    int     _extraMoves;
    int     _currMoves;
};

#endif 
