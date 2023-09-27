#ifndef __TTGameMenus_SCENE_H__
#define __TTGameMenus_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "TTGameInfo.h"
#include "TTUIDefines.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_RESUME_SPACE           100
#define HEIGHT_UI_RESUME_SPACE          90

class CC_DLL TTGameMenus: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTGameMenus* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTGameMenus(){}
    
public:
    
    void init();
    ~TTGameMenus();

    void reset();
    void showResumeMenu(gameInfo* _gameInfo, Node* _parent);
    void showInitMenu(gameInfo* _gameInfo, Node* _parent, bool retryMode = false);
    void showExitMenu(Node* _parent, bool exitGame = false);
    
    void pauseUI();
    void restoreUI();
    void forceExit();
    
    void onClickInitExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickRetryExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitLevelDoneBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitLevelCancelBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
private:
    
    void showBoosters();
    void showFacebookInfo();
    
    void onClickPlayBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickRetryBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickContinueBuyBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickContinueBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickConnectFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickInviteFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickScoreFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void exitMenu();
    
    void onEvent_FacebookConnect(EventCustom* e);
    void onEvent_FacebookRanking(EventCustom* e);
    
public:
    
    bool _blockMenus;
    bool _retryMode;
    bool _coinsInMenu;
    bool _exitGame;

    cocos2d::Layer* _uiLayer;
    cocos2d::ui::Widget* _widgetResume;
    
    Sprite*                 _bgUI;
    
    cocos2d::ui::TextBMFont*    _levelTitleLabel;
    cocos2d::ui::Widget*        _resumeParent;
    cocos2d::ui::Widget*        _initParent;
    cocos2d::ui::Widget*        _exitParent;
    
    //Items para el menu inicio partida
    cocos2d::ui::TextBMFont*    _btnPlayLabel;
    cocos2d::ui::Button*        _btnPlay;
    cocos2d::ui::Button*        _btnExitInit;
    
    cocos2d::ui::TextBMFont*    _objectivesLabel;
    cocos2d::ui::Widget*        _objectivesIcons;
    
    cocos2d::ui::TextBMFont*    _boostersTitle;
    
    std::vector<boosterUI>      _boosters;
    
    // Items para el menu de salir
    cocos2d::ui::Button*        _btnCancel;
    cocos2d::ui::Button*        _btnApply;
    cocos2d::ui::TextBMFont*    _labelExit1;
    cocos2d::ui::TextBMFont*    _labelExit2;
    cocos2d::ui::Widget*        _birdExitGame;
    cocos2d::ui::Widget*        _birdExitLevel;
    cocos2d::ui::Widget*        _heartExitLevel;
    
    // Items para el menu de partida ganada
    cocos2d::ui::Widget*        _winResumeParent;
    cocos2d::ui::TextBMFont*    _btnContinueLabel;
    cocos2d::ui::Button*        _btnContinue;
    std::vector<std::vector<cocos2d::ui::Widget*> > _stars;
    std::vector<ParticleSystemQuad*>                _starsParticles;
    cocos2d::ui::TextBMFont*    _scoreTitle;
    cocos2d::ui::TextBMFont*    _scoreLabel;
    
    // Items para el menu de partida perdida
    cocos2d::ui::Widget*        _loseResumeParent;
    cocos2d::ui::Button*        _btnExitRetry;
    cocos2d::ui::TextBMFont*    _btnRetryLabel;
    cocos2d::ui::Button*        _btnRetry;
    cocos2d::ui::TextBMFont*    _loseTitleLabel;
    cocos2d::ui::Widget*        _objectivesIconsLose;
    cocos2d::ui::Button*        _btnBuy;
    cocos2d::ui::Widget*        _iconTimeBuy;
    cocos2d::ui::Widget*        _iconMovesBuy;
    cocos2d::ui::TextBMFont*    _buyLabel1;
    cocos2d::ui::TextBMFont*    _buyLabel2;
    cocos2d::ui::TextBMFont*    _buyPrice;
    
    // Ranking
    Animation*                  _wifiConnection;
    Sprite*                     _spriteAnimationWiFi;
    cocos2d::ui::Widget*        _rankingParent;
    cocos2d::ui::Widget*        _rankingFacebookNoConnect;
    cocos2d::ui::Button*        _btnConnect;
    cocos2d::ui::TextBMFont*    _btnConnectLabel;
    cocos2d::ui::TextBMFont*    _connectLabel;
    cocos2d::ui::Widget*        _rankingFacebookConnect;
    
    cocos2d::ui::Widget*        _rankingFacebookInvite;
    cocos2d::ui::TextBMFont*    _inviteLabel;
    cocos2d::ui::Button*        _btnInvite;
    cocos2d::ui::TextBMFont*    _btnInviteLabel;
    
    gameInfo* _currentGameInfo;
    
    std::vector<EventListener*> _classListeners;
};

#endif  // __TTGameMenus_SCENE_H__
