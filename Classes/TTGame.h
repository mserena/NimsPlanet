#ifndef __TTGame_SCENE_H__
#define __TTGame_SCENE_H__

#include "TTRestriction.h"
#include "TTTimeRestriction.h"
#include "TTMovesRestriction.h"

#include "TTGoal.h"
#include "TTScoreGoal.h"
#include "TTBreakGoal.h"
#include "TTDownObjectGoal.h"
#include "TTCollectFiguresGoal.h"

#include "TTGameInfo.h"
#include "TTGameUIManager.h"

#include "cocos2d.h"

using namespace cocos2d;

class CC_DLL TTGame: public Ref
{

public:
    
    // Retorna la instancia Singleton de la clase
    static TTGame* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTGame(){};
    
public:

    ~TTGame();

    void init();
    void reset();
    void initGame();
    void update(float dt);
    void finishGame();
    void reactivateGame();
    
    int calcScoreWined(finishPlayInfo _playInfo);
    
    bool isFinished();
    
private:
    
    //Events
    void onEvent_RestrictionTimeFinish(EventCustom* e);
    void onEvent_RestrictionMovesFinish(EventCustom* e);
    void onEvent_ReadyToPlay(EventCustom* e);
    void onEvent_FinishWinPlay(EventCustom* e);
    void onEvent_GoalAchieved(EventCustom* e);

    void onEvent_GamePause(EventCustom* e);
    void onEvent_GameContinue(EventCustom* e);

public:
    
    gameInfo*       _gameConfig;
    bool            _paused;
    
private:
    
    bool _finishedGame;
    std::vector<EventListener*> _classListeners;
};

#endif  // __TTGame_SCENE_H__
