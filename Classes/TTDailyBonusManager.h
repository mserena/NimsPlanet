//
//  TTDailyBonusManager.h
//  animalplanet
//
//  Created by Marc Serena on 7/4/15.
//
//

#ifndef __animalplanet__TTDailyBonusManager__
#define __animalplanet__TTDailyBonusManager__

#include "thegame.h"
#include "cocos2d.h"
#include "pugixml.hpp"
#include "TTDailyBonusDefines.h"
#include "TTEventsManager.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#define WIDTH_UI_DB_SPACE             90
#define HEIGHT_UI_DB_SPACE            90

class CC_DLL TTDailyBonusManager : public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTDailyBonusManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTDailyBonusManager(){}
    
public:
    
    ~TTDailyBonusManager();
    void init();
    void preinit();
    void initShowDailyBonus(Node* _parent);
    
    void pauseUI();
    void restoreUI();
    
private:
    
    void resourceLoaded(Texture2D* texture);
    void preloadResources();
    void finishShowDailyBonus();
    
    void parseDailyBonusXML();
    void calcActualCombination();
    void play();
    void spinRollers(float _time);
    
    void finishSpinRollers();
    void showWinPrize();
    void showLosePrize();
    void giveGift();
    
    void setMachineState(MACHINE_STATE _state);
    void initStanbyShow(int _idxLight);
    
    void onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickPlayBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void doStatistics(int _numPlays);
    
    void verifyUserDailyBonusInfo();
    void readUserDailyBonusInfo();
    void saveUserDailyBonusInfo();
    void setIconUserSpins();
    
    //Eventos
    void onEvent_getTimeServer(EventCustom* e);
    
private:
    
    int                         _generalResourcesToLoad;
    Node*                       _actualParent;
    std::vector<int>            _combinationsWeight;
    Sprite*                     _bgUIDailyBonus;
    std::vector<rollerInfo>     _rollers;
    float                       _timeRolling;
    combinationInfo             _actualCombinationInfo;
    bool                        _isPlaying;
    
    cocos2d::Layer*                     _uiDBLayer;
    cocos2d::ui::Widget*                _widgetDB;
    cocos2d::ui::Widget*                _widgetBackgroundWin;
    std::vector<cocos2d::ui::Widget*>   _rollerParents;
    cocos2d::ui::Button*                _btnPlay;
    cocos2d::ui::Button*                _btnExit;
    std::vector<cocos2d::ui::Widget*>   _lights;
    std::vector<cocos2d::ui::Widget*>   _infoSpins;
    cocos2d::ui::TextBMFont*            _freeSpins;
    
    Node*                       _winParent;
    Sprite*                     _winBackground;
    std::vector<Sprite*>        _winFigures;
    
    std::vector<EventListener*> _classListeners;
    
    timeInfo _timeServerInfo;
    
    UserDefault*            _def;
    userDailyBonusInfo*     _userInfo;

};


#endif /* defined(__animalplanet__TTDailyBonusManager__) */
