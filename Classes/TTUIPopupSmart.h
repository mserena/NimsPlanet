//
//  TTUIPopupSmart.h
//  animalplanet
//
//  Created by Marc Serena on 05/12/14.
//
//

#ifndef __animalplanet__TTUIPopupSmart__
#define __animalplanet__TTUIPopupSmart__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_POPUP_SMART_SPACE             80
#define HEIGHT_UI_POPUP_SMART_SPACE            80

typedef enum SMART_POPUPS{
    SMART_POPUP_FACEBOOK_STATE,
    SMART_POPUP_STORE_STATE,
    SMART_POPUP_COMINGSOON_STATE,
    SMART_POPUP_GENERAL_CONNECTION_ERRROR_STATE
}SMART_POPUPS;

class CC_DLL TTUIPopupSmart: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIPopupSmart* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTUIPopupSmart(){}
    
public:
    
    void init();
    ~TTUIPopupSmart();
    
    void pauseUI();
    void restoreUI();
    
    void showPopupState(Node* _parent, SMART_POPUPS _smartPopup, bool _state);
    
    void onClickExitPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
private:
    
    void onClickAcceptPopupBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void exitMenu();
    void forceExit();
    
    void onEvent_FacebookConnect(EventCustom* e);
    
public:
    
    bool _blockMenus;
    
    cocos2d::Layer*         _uiPopupLayer;
    cocos2d::ui::Widget*    _widgetPopup;
    Sprite*                 _bgUI;
    cocos2d::ui::Text*      _referenceText;
    
    cocos2d::ui::TextBMFont*    _popupTitle;
    Label*                      _popupText;
    cocos2d::ui::Button*        _btnExit;
    cocos2d::ui::Button*        _btnAccept;
    cocos2d::ui::TextBMFont*    _btnAcceptLabel;
    
    cocos2d::ui::Widget*    _parentFacebookInfo;
    cocos2d::ui::Widget*    _parentFacebookInfoDone;
    cocos2d::ui::Widget*    _parentFacebookInfoFail;

    cocos2d::ui::Widget*    _parentStoreInfo;
    cocos2d::ui::Widget*    _parentStoreInfoDone;
    cocos2d::ui::Widget*    _parentStoreInfoFail;
    
    cocos2d::ui::Widget*    _parentProximamente;
    cocos2d::ui::Widget*    _parentProximamenteImg;
    
    cocos2d::ui::Widget*    _parentGeneralConnection;
    
private:
    
    std::vector<EventListener*> _classListeners;

};

#endif /* defined(__animalplanet__TTUIPopupSmart__) */
