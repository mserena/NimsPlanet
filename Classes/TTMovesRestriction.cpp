#include "TTMovesRestriction.h"
#include "TTUserLives.h"

TTMovesRestriction::TTMovesRestriction():
_limitMoves(0),
_currMoves(0),
_extraMoves(0),
_finishedMoves(false)
{
    _allowFinish = false;
}

TTMovesRestriction::TTMovesRestriction(int limit, bool allowsFinish):
_limitMoves(limit),
_currMoves(0),
_extraMoves(0),
_finishedMoves(false)
{
    _allowFinish = allowsFinish;
}

TTMovesRestriction::~TTMovesRestriction(){
    
}

void TTMovesRestriction::reset()
{
    _finishedMoves = false;
    _currMoves = 0;
    _extraMoves = 0;
}

int TTMovesRestriction::getLimitMoves()
{
    return _limitMoves+_extraMoves;
}

void TTMovesRestriction::addExtraMoves(int moves)
{
    _finishedMoves = false;
    _extraMoves += moves;
}

void TTMovesRestriction::doMove()
{
    if(_finishedMoves)
        return;
    
    if(_currMoves == 0)
    {
        // Es el primer movimiento
        TTUserLives::getInstance()->playOneLive();
    }
    
    _currMoves++;
    if(_currMoves == (_limitMoves+_extraMoves))
    {
        _finishedMoves = true;
    }
}
