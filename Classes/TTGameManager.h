#ifndef __TTGameManager_SCENE_H__
#define __TTGameManager_SCENE_H__

#include "TTBoard.h"
#include "cocos2d.h"
#include "TTGame.h"
#include "TTGameUIManager.h"
#include "TTLevelParser.h"
#include "TTGameMenus.h"

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocostudio;
using namespace cocos2d;

// Clase singleton
class TTGameManager: public cocos2d::Layer
{
public:
    TTGameManager();
    ~TTGameManager();
    void update( float dt);
    
    bool loadGame();
    void initGame();
    void showGame();
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    virtual void onExit() override;
    
    void onGameFinish();
    
    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

private:
    
    bool isInGame();
    void resourcesLoadControl();
    void resourceLoaded(Texture2D* texture);
    void backgroundBoardLoaded(Texture2D* texture);
    void updateUserInfo();
    
    //events
    void onEvent_FinishGame(EventCustom* e);
    void onEvent_ContinueBuyGame(EventCustom* e);
    void onEvent_FinishedBoard(EventCustom* e);
    void onEvent_RetryGame(EventCustom* e);
    void onEvent_pauseGame(EventCustom* e);
    void onEvent_continueGame(EventCustom* e);
    void onEvent_boardStepsFinished(EventCustom* e);
    
public:
    
    cocos2d::Layer* _parentLayer;
    bool    _inGame;
    bool    _pause;
    bool    _waitingForFinishBoardSteps;
    
    TTGame*             _game;
    
    int totalResourcesToLoad;
    
private:
    
    EventListenerTouchAllAtOnce *_touchListener;
    std::vector<EventListener*> _classListeners;

};

#endif  // __TTGameManager_SCENE_H__
