#ifndef __TTUIStackManager_SCENE_H__
#define __TTUIStackManager_SCENE_H__

#include "thegame.h"

using namespace cocos2d;

typedef enum UI_TYPES
{
    UI_POPUP,
    UI_POPUP_SMART,
    UI_MAP,
    UI_SETTINGS,
    UI_INGAME,
    UI_GAME_MENU_INIT,
    UI_GAME_MENU_RESUME,
    UI_GAME_MENU_EXIT,
    UI_SHOP,
    UI_FACEBOOK,
    UI_DAILY_BONUS
}UI_TYPES;

class CC_DLL TTUIStackManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIStackManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUIStackManager(){}
    
public:
    
    void init();
    ~TTUIStackManager();
    
    void clearStack();
    
    void goToNewUI(UI_TYPES _menu);
    void closeUI();
    
    // UI Loading
    void showUILoading(Node* _parent);
    void hideUILoading();

    // UI Connecting
    void showUIConnecting(Node* _parent);
    void hideUIConnecting(bool _error = false);
    
    //devuelve si la UI que se le pasa es la activa actual
    bool isUIOnTop(UI_TYPES _uiType);
    
    bool isStackEmpty();
    
    void killTopUI();
    
    void blockUITouches(Node *parentConnectingUI);
    void unblockUITouches();
    
    bool onTouchesUIBegan(cocos2d::Touch* touch, cocos2d::Event *event);
    void onTouchesUI(cocos2d::Touch* touch, cocos2d::Event *event);
    
private:
    
    void pauseUI(UI_TYPES _stackUI);
    void restoreUI(UI_TYPES _stackUI);

private:
    
    std::vector<UI_TYPES>   _uiStack;
    
    Sprite*                 _loadingSpriteUI;
    Sprite*                 _bgLoadingUI;
    Node*                   _parentLoadingUI;
    
    Sprite*                 _connectingSpriteUI;
    Sprite*                 _bgConnectingUI;
    Node*                   _parentConnectingUI;
    Sprite*                 _spriteFailConnection;
    
    EventListenerTouchOneByOne *_touchListenerBg;

};

#endif  // __TTUIStackManager_SCENE_H__
