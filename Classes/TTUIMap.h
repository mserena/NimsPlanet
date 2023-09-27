#ifndef __TTUIMap_SCENE_H__
#define __TTUIMap_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_COINS_SPACE             40
#define HEIGHT_UI_COINS_SPACE            10

#define WIDTH_UI_LIVE_SPACE             40
#define HEIGHT_UI_LIVE_SPACE            10

#define WIDTH_UI_MENU_SPACE             50
#define HEIGHT_UI_MENU_SPACE            10

class CC_DLL TTUIMap: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIMap* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUIMap(){}
    
public:
    
    void init();
    ~TTUIMap();

    void pauseUI();
    void restoreUI();
    
    void showUIMap(Node* _parent);
    void hideUIMap();
    
    void updateUIValues();
    void showUserCredits(Node* _parent, int zValue = 0);
    void hideUserCredits();
    void setShowDailyBonusVisiblity();
    
private:
    
    void onClickLivesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickDailyBonusBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickCoinsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickSettingsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickGiveLivesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickInviteFriendsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickMailBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
public:
    
    bool    _blockMenus;
    bool    _buyCoinsOutAllowed;
    
    cocos2d::Layer*         _uiMapLayer;
    cocos2d::ui::Widget*    _widgetMapCoins;
    cocos2d::ui::Widget*    _widgetMapLive;
    cocos2d::ui::Widget*    _widgetMapMenu;
    
    // uiDailyBonus
    cocos2d::ui::Button*        _dailyBonusBtn;
    Sprite*                     _dailyBonusBtnHalo;
    
    // uiLives
    cocos2d::ui::Button*        _btnLives;
    cocos2d::ui::TextBMFont*    _userLives;
    cocos2d::ui::TextBMFont*    _livesInfo;
    
    // uiCoins
    cocos2d::ui::Button*        _btnCoins;
    cocos2d::ui::TextBMFont*    _userCoins;
    
    // uiSettings
    cocos2d::ui::Button*    _btnMenuSettings;
    cocos2d::ui::Button*    _btnMenuGiveLives;
    cocos2d::ui::Button*    _btnMenuInviteFriends;
    cocos2d::ui::Button*    _btnMenuMail;
    cocos2d::ui::Widget*    _alertMail;
};

#endif  // __TTUIMap_SCENE_H__
