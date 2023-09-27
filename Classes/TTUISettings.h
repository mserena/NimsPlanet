#ifndef __TTUISettings_SCENE_H__
#define __TTUISettings_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_SETTINGS_SPACE             95
#define HEIGHT_UI_SETTINGS_SPACE            85

typedef enum ACTIVE_SETTINGS_MENU
{
    PAUSE,
    SETTINGS,
}ACTIVE_SETTINGS_MENU;

class CC_DLL TTUISettings: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUISettings* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUISettings(){}
    
public:
    
    void preloadResources();
    void resourceLoaded(Texture2D* texture);
    
    void init();
    ~TTUISettings();
    
    void pauseUI();
    void restoreUI();

    void showSettings(Node* _parent);
    void showPause(Node* _parent);
    
    void onClickExitMenuBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);

private:
    
    void showMenu(Node* _parent);
    
    void onSliderPercentChange(Ref *sender,ui::Slider::EventType controlEvent);
    void onSliderTouch(Ref *sender,ui::Widget::TouchEventType touchEvent);
    void onClickLanguagesExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickLanguagesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickChangeLanguageBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickHowToBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void exitMenu();
    
    void onEvent_languageChange(EventCustom* e);
    
public:
    
    bool _blockMenus;
    
    cocos2d::Layer*         _uiSettingsLayer;
    cocos2d::ui::Widget*    _widgetSettings;
    Sprite*                 _bgUI;
    
    cocos2d::ui::TextBMFont*    _pauseTitle;
    
    cocos2d::ui::Button*        _btnExit;
    cocos2d::ui::Slider*        _musicSlider;
    cocos2d::ui::Slider*        _musicFXSlider;
    cocos2d::ui::TextBMFont*    _musicOptionLabel;
    cocos2d::ui::TextBMFont*    _musicFXOptionLabel;
    
    cocos2d::ui::Widget*        _languagesSelection;
    cocos2d::ui::Button*        _btnLanguages;
    cocos2d::ui::TextBMFont*    _btnLanguagesLabel;
    cocos2d::ui::Button*        _btnExitLanguages;
    
    cocos2d::ui::Button*        _btnHowToPlay;
    cocos2d::ui::TextBMFont*    _btnHowToPlayLabel;
    
    cocos2d::ui::Button*        _btnExitLevel;
    cocos2d::ui::TextBMFont*    _btnExitLevelLabel;
    
private:
    
    ACTIVE_SETTINGS_MENU        _activeMenu;
    Node*                       _actualParent;
    
    std::vector<EventListener*> _classListeners;
};

#endif  // __TTUISettings_SCENE_H__
