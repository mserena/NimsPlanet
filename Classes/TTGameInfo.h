#ifndef __TTGameInfo_SCENE_H__
#define __TTGameInfo_SCENE_H__

#include "TTBoard.h"
#include "TTGoal.h"
#include "TTTimeRestriction.h"
#include "TTMovesRestriction.h"
#include "TTScoreGoal.h"
#include "TTBreakGoal.h"
#include "TTDownObjectGoal.h"
#include "TTCollectFiguresGoal.h"
#include "TTBoosterDefines.h"

typedef struct gameInfo
{
    // Level info
    int level;
    
    // Board info
    initBoardInfo boardInit;
    
    // Boosters
    std::map<BOOSTERS_TYPES,std::vector<int>> _boosters;
    
    // HowTo pages
    std::vector<int> howToPages;
    
    // Restrictions
    TTTimeRestriction*  timeRestriction;
    TTMovesRestriction* moveRestriction;
    
    //Goals vector
    std::vector<TTGoal*> goals;
    
    // Scores
    std::vector<int> scoreStars;
    int _scoreDetahFigure;
    int _scoreDetahCombo;
    int _scoreDetahComboColor;
    int _scoreOvni;
    int _scoreCreateCombo;
    int _scoreCreateComboColor;
    int _scoreBonusCatch;
    int _scoreBonusNoCatch;
    
    // Win
    bool winGame;
    int  scoreGame;
    int  movesGame;
    float currentTime;
    
    //Destructor
    ~gameInfo()
    {
        // delete restrictions
        if(timeRestriction != NULL)
            delete(timeRestriction);
        if(moveRestriction != NULL)
            delete(moveRestriction);
        
        // delete goals
        for(int i = 0; i < goals.size(); i++)
            delete(goals[i]);
    }
    
    //scorestars
    int getScoreStars()
    {
        int stars = 0;
        for(int i = 0; i < scoreStars.size() && scoreGame >= scoreStars[i]; i++)
        {
           stars = i+1;
        }
        return stars;
    }
    
    // Reset
    void reset()
    {
        winGame = false;
        scoreGame = 0;
        movesGame = 0;
        
        // reset restrictions
        if(timeRestriction != NULL)
            timeRestriction->reset();
        if(moveRestriction != NULL)
            moveRestriction->reset();
        
        // reset goals
        for(int i = 0; i < goals.size(); i++)
        {
            goals[i]->reset();
        }

    }
    
    //devuelve si un determinado booster esta definido en la actual configuracion del game
    bool isBoosterDefined(BOOSTERS_TYPES _type, int _subtype)
    {
        bool ret = false;
        std::vector<int> _typeVec = _boosters[_type];
        ret =  (std::find(_typeVec.begin(), _typeVec.end(), _subtype) != _typeVec.end());
        return ret;
    }
    
}gameInfo;

#endif  // __TTGameInfo_SCENE_H__
