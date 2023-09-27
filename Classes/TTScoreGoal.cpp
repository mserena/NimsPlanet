#include "TTScoreGoal.h"

TTScoreGoal::TTScoreGoal(int scoreToWin, bool opt)
{
    _optional = opt;
    _typeGoal = SCORE_GOAL;
    _goalValue = scoreToWin,
    _actualValue =0;
}

TTScoreGoal::~TTScoreGoal()
{
    
}

void TTScoreGoal::reset()
{
    _actualValue = 0;
}

void TTScoreGoal::scoreWin(int scoreWin)
{
    bool _beforeAchieved = isAchieved();
    
    _actualValue += scoreWin;
    if(_actualValue >= _goalValue && !_beforeAchieved)
    {
         sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
    }
}
