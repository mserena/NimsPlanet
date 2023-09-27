#ifndef __TTUIPopup_SCENE_H__
#define __TTUIPopup_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_POPUP_SPACE             80
#define HEIGHT_UI_POPUP_SPACE            80

#define WIDTH_UI_POPUP_HOWTO_SPACE             90
#define HEIGHT_UI_POPUP_HOWTO_SPACE            90

#define TUTORIAL_ID                     "TUTORIAL_"

#define NUM_HOWTO_PAGES     15

class CC_DLL TTUIPopup: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIPopup* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUIPopup(){}
    
public:
    
    void init();
    void initHowTo();
    void initStopper();
    ~TTUIPopup();

    void pauseUI();
    void restoreUI();
    
    void showPopupBooster(Node* _parent, int typeBooster, int subTypeBooster, std::string _boosterIcon);
    void showPopupLives(Node* _parent);
    void showPopupStopper(Node* _parent);
    void showHowToPopup(Node* _parent, std::vector<int> _pageIds, bool _fromSettings);
    
    void onClickExitPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitHowtoBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitStopperUnlockBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickBuyStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickFacebookStopperBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
private:
    
    void showHowToPopup(Node* _parent, int _nextPage);
    
    void onClickBuyPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickFacebookAskForLivesPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickNextHowToPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickBackHowToPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void exitMenu();
    void forceExit();
    
    void onFacebookConnection(EventCustom *event);
    
public:
    
    bool _blockMenus;
    
    cocos2d::Layer*         _uiPopupLayer;
    cocos2d::ui::Widget*    _widgetPopup;
    Sprite*                 _bgUI;
    cocos2d::ui::Text*      _referenceText;
    
    cocos2d::ui::Widget*        _infoPopup;
    cocos2d::ui::TextBMFont*    _popupTitle;
    Label*                      _popupText;
    cocos2d::ui::Button*        _btnExit;
    cocos2d::ui::Widget*        _popupImgParent;
    cocos2d::ui::Widget*        _popupImgTimeParent;
    cocos2d::ui::Button*        _btnPopup;
    cocos2d::ui::TextBMFont*    _btnPopupLabel;
    cocos2d::ui::Button*        _btnPopupFacebook;
    cocos2d::ui::TextBMFont*    _btnPopupFacebookLabel;
    
    //How To
    cocos2d::ui::Widget*         _howToPopup;
    
    cocos2d::ui::TextBMFont*    _howtoTitle;
    Label*                      _howtoText;
    cocos2d::ui::Text*          _howtoReferenceText;
    cocos2d::ui::Button*        _howtoBtnNext;
    cocos2d::ui::Button*        _howtoBtnBack;
    cocos2d::ui::Button*        _howtoBtnExit;
    cocos2d::ui::Widget*        _howtoImg;
    cocos2d::ui::Widget*        _howtoTeacherImg;
    
    //Stopper
    cocos2d::ui::Widget*                _stopperPopup;
    cocos2d::ui::Widget*                _stopperPopupLoked;
    cocos2d::ui::TextBMFont*            _stopperTime;
    cocos2d::ui::TextBMFont*            _labelUnlock;
    cocos2d::ui::Button*                _buyStopperButton;
    cocos2d::ui::TextBMFont*            _buyStopperButtonLabel;
    std::vector<cocos2d::ui::Widget*>   _padlockImgParents;
    cocos2d::ui::Button*                _facebookStopperButton;
    
    cocos2d::ui::Widget*         _stopperPopupUnloked;
    cocos2d::ui::Widget*         _stopperHaloKey;
    cocos2d::ui::Button*         _unlockButton;
    cocos2d::ui::TextBMFont*     _unlockButtonLabel;
    
    cocos2d::ui::TextBMFont*     _stopperTitle;
    Label*                       _stopperText;
    cocos2d::ui::Text*           _stopperReferenceText;
    cocos2d::ui::Button*         _stopperBtnExit;
    
    Node*                       _parentLayer;
    
private:
    
    float                       _scaleForInfo;
    float                       _scaleForHowTo;
    Sprite*                     _sprHowTo;
    std::vector<int>            _howToPages;
    int                         _actualLevel;
    
    bool                        _buyDone;
    
    bool                        _howToFromSettings;
    
    std::vector<EventListener*> _classListeners;

};

#endif  // __TTUIPopup_SCENE_H__
