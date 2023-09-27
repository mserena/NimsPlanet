#include "TTCollectFiguresGoal.h"

TTCollectFiguresGoal::TTCollectFiguresGoal(int figureToCollect, int figuresToWin, bool opt):
_figureToCollect(figureToCollect-1)
{
    _optional = opt;
    _typeGoal = COLLECT_FIGURES_GOAL;
    _goalValue = figuresToWin;
    _actualValue = 0;
}

TTCollectFiguresGoal::~TTCollectFiguresGoal()
{

}

void TTCollectFiguresGoal::reset()
{
    _actualValue = 0;
}

void TTCollectFiguresGoal::newCollect(int figure, int collect)
{
    if (figure == _figureToCollect) {
        bool _beforeAchieved = isAchieved();

        _actualValue += collect;
        
        //informar a la UI con un evento
        CollectFiguresDataEvent dataEvent;
        dataEvent.currentValue = _actualValue;
        dataEvent.goalValue = _goalValue;
        dataEvent.figure = _figureToCollect;
        dataEvent.recentAchieved = (_beforeAchieved != isAchieved());
        
        sendEvent(EVENT_COLLECT_FIGURE_UI, &dataEvent);
        
        if (_actualValue>=_goalValue && !_beforeAchieved)
        {
            sendEvent(EVENT_GOAL_ACHIEVED,&_optional);
        }
    }
}
