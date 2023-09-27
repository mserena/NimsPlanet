#ifndef __TTGameUIManager_SCENE_H__
#define __TTGameUIManager_SCENE_H__

#include "thegame.h"
#include "TTGameInfo.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "TTUIShows.h"
#include "TTUIDefines.h"
#include "TTBoostersManager.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_UP_SPACE              100
#define HEIGHT_UI_UP_SPACE             20
#define WIDTH_UI_DOWN_SPACE            100
#define HEIGHT_UI_DOWN_SPACE           10

class CC_DLL TTGameUIManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTGameUIManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTGameUIManager(){}
    
public:

    void init();
    ~TTGameUIManager();
    
    void pauseUI();
    void restoreUI();
    
    void resetPrinter();
    void showPrinter(gameInfo* gameInfo, Node* _parent);
    void finishPrinter();
    
    // printers for labels
    void printTimeLabel(int remainTime);
    void printScoreLabel(int score);
    void printMovementsLabel(int actualMovements, int totalMovements);
    
    void setBoosters();
    void setTouchEnabledUI(bool _isEnabled);
    
    void activateBoosterUI(int _boosterSubType);
    
    // callback botones
    void onClickMoreRestrictions(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickPause(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickCancelApplyBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickDoneApplyBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    // gestión de eventos
    void onEvent_languageChange(EventCustom* e);
    void onEvent_continueGame(EventCustom* e);
    void onEvent_collectFigures(EventCustom* e);
    void onEvent_goalUpdate(EventCustom* e);
    void onEvent_gamePause(EventCustom* e);
    
private:
    
    void showGoalStep(goalUI _goal);
    void showGoalComplete(goalUI _goal);
    
private:
    
    Node*   _actualParent;
    
    bool _blockMenus;
    gameInfo* _gameInfo;

    cocos2d::Layer* _uiUpLayer;
    cocos2d::ui::Widget* _widgetUp;
    cocos2d::Layer* _uiDownLayer;
    cocos2d::ui::Widget* _widgetDown;
    
    cocos2d::ui::TextBMFont*                _restrictionLabel;
    cocos2d::ui::LoadingBar*                _scoreBar;
    cocos2d::ui::TextBMFont*                _scoreLabel;
    std::vector<std::vector<Sprite*> >      _starsSprites;
    
    cocos2d::ui::Button*                    _btnMoreRestrictions;
    cocos2d::ui::Button*                    _btnPause;
    
    std::vector<boosterUI>                  _boosters;
    
    cocos2d::ui::Widget*                    _boostersParent;
    cocos2d::ui::Widget*                    _boosterApplyUI;
    cocos2d::ui::Button*                    _btnApply;
    cocos2d::ui::Button*                    _btnCancel;
    cocos2d::ui::Widget*                    _boosterApplyIcon;
    
    std::vector<EventListener*> _classListeners;
    
    int _remainTime;

public:
    
    std::vector<goalUI>                     _goals;
    Vec2                                    _UISpace;
};

#endif  // __TTGameUIManager_SCENE_H__
