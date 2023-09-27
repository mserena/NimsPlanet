#ifndef __TTUIShop_SCENE_H__
#define __TTUIShop_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "TTGameInfo.h"


using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_SHOP_SPACE             100
#define HEIGHT_UI_SHOP_SPACE            100

typedef struct itemShop
{
    cocos2d::ui::Button*        _btnBuy;
}itemShop;

class CC_DLL TTUIShop: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIShop* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUIShop(){}
    
public:
    
    void init();
    ~TTUIShop();
    
    void preloadResources();
    void resourceLoaded(Texture2D* texture);

    void pauseUI();
    void restoreUI();
    
    void showShop(Node* _parent);
    
    void onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
private:
    
    void printShop(Node* _parent);
    
    void onClickBuyBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void exitMenu();
    
    void onEvent_CoinsBalanceUpdated(EventCustom* e);
    void onEvent_PurchaseStoreError(EventCustom* e);
    
public:
    
    bool _blockMenus;

    cocos2d::Layer*         _uiShopLayer;
    cocos2d::ui::Widget*    _widgetShop;
    
    Sprite*                 _bgUI;
    
    cocos2d::ui::TextBMFont*                _shopTitleLabel;
    cocos2d::ui::TextBMFont*                _shopMoneyTitleLabel;
    cocos2d::ui::TextBMFont*                _shopMoneyLabel;
    std::vector<itemShop>                   _shopItems;
    cocos2d::ui::Button*                    _btnExit;
    std::vector< cocos2d::ui::TextBMFont*>  _labelsPrizes;
    
    Node*           _actualParent;
    
    
private:
    std::vector<EventListener*> _classListeners;
};

#endif  // __TTUIShop_SCENE_H__
