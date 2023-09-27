#include "TTUISettings.h"
#include "TTLanguageManager.h"
#include "TTEventsManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTAudioManager.h"
#include "TTUserInfo.h"
#include "TTUIPopup.h"
#include "TTConfigParser.h"
#include "TTParseManager.h"

#define SLIDER_MIN_PERCENTAGE   10
#define SLIDER_MAX_PERCENTAGE   90

//Singleton
static TTUISettings *_sharedSettingsUI = nullptr;

TTUISettings* TTUISettings::getInstance()
{
    if (!_sharedSettingsUI)
    {
        _sharedSettingsUI = new TTUISettings();
    }
    
    return _sharedSettingsUI;
}

void TTUISettings::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedSettingsUI);
}

TTUISettings::~TTUISettings()
{
    CCLOG("Se destruye la UI de ajustes/pausa");
    
    // Eliminamos las acciones
    removeAllActionsFromJSON("UI_SETTINGS.ExportJson");
    
    // Eliminamos la layer contenedora
    _uiSettingsLayer->removeAllChildrenWithCleanup(true);
    _uiSettingsLayer->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_SETTINGS0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_SETTINGS0.png");
    
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTUISettings::preloadResources()
{
    Director::getInstance()->getTextureCache()->addImageAsync("UI_SETTINGS0.png",CC_CALLBACK_1(TTUISettings::resourceLoaded, this));
}

void TTUISettings::resourceLoaded(Texture2D* texture)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UI_SETTINGS0.plist","UI_SETTINGS0.png");
    init();
    TTUIStackManager::getInstance()->hideUILoading();
    showMenu(_actualParent);
}

void TTUISettings::init()
{
    _blockMenus = false;
    
    // Cargamos el XML de la UI Resume
    _uiSettingsLayer = Layer::create();
    _uiSettingsLayer->retain();
    _widgetSettings = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_SETTINGS.ExportJson");
    cocos2d::ui::Layout* _layoutShop = dynamic_cast<cocos2d::ui::Layout*>(_widgetSettings);
    _uiSettingsLayer->addChild(_layoutShop);
    
    // Escalado Widget
    _widgetSettings->setScale(scale_widget(_widgetSettings,WIDTH_UI_SETTINGS_SPACE,HEIGHT_UI_SETTINGS_SPACE));
                          
    // Posicionamiento Widget
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetSettings->setAnchorPoint(Vec2(0.5,0.5));
    _uiSettingsLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_widgetSettings->getBoundingBox().size.height/2)));
    _uiSettingsLayer->setVisible(false);
    
    // Seteamos la versión
    ((cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("versionLabel"))->setString(TTConfigParser::getInstance()->_idVersionApp);
    ((cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("keyLabel"))->setString(TTParseManager::getInstance()->getUserParseID());
    
    // Recuperamos los items que necesitamos
    _pauseTitle = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("titleMenu");
    
    _btnExit = (cocos2d::ui::Button*)_widgetSettings->getChildByName("settingsBg")->getChildByName("exitBtn");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onClickExitBtn,this));
    
    _musicSlider = (cocos2d::ui::Slider*)_widgetSettings->getChildByName("settingsBg")->getChildByName("musicOption")->getChildByName("selector");
    _musicSlider->addEventListener(CC_CALLBACK_2(TTUISettings::onSliderPercentChange,this));
    _musicSlider->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onSliderTouch,this));
    
    _musicSlider->setPercent(TTUserInfo::getInstance()->_userData->_musicLevel);
   
    
    _musicFXSlider = (cocos2d::ui::Slider*)_widgetSettings->getChildByName("settingsBg")->getChildByName("FxOption")->getChildByName("selector");
    _musicFXSlider->addEventListener(CC_CALLBACK_2(TTUISettings::onSliderPercentChange,this));
    _musicFXSlider->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onSliderTouch,this));
    
    _musicFXSlider->setPercent(TTUserInfo::getInstance()->_userData->_effectsLevel);

    
    _musicOptionLabel = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("musicOption")->getChildByName("title");
    _musicFXOptionLabel = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("FxOption")->getChildByName("title");
    _musicOptionLabel->setString(TTLanguageManager::getInstance()->getTag(MUSIC_PAUSE_TAG));
    _musicFXOptionLabel->setString(TTLanguageManager::getInstance()->getTag(FX_PAUSE_TAG));
    
    _languagesSelection = (cocos2d::ui::Widget*)_widgetSettings->getChildByName("settingsBg")->getChildByName("LanguagesSelection");
    _languagesSelection->setVisible(false);
    _btnLanguages = (cocos2d::ui::Button*)_widgetSettings->getChildByName("settingsBg")->getChildByName("LanguageBtn");
    _btnLanguages->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onClickLanguagesBtn,this));
    _btnLanguages->setVisible(true);
    for(int i = 0; i < LANGUAGES_NUM; i++)
    {
        char stLang[100] = {0};
        sprintf(stLang,"language%d",i+1);
        cocos2d::ui::Button* _btnLang = (cocos2d::ui::Button*)_languagesSelection->getChildByName(stLang)->getChildByName("languageBtn");
        _btnLang->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onClickChangeLanguageBtn,this));
        _languagesSelection->getChildByName(stLang)->setTag(i);
    }
    _btnLanguagesLabel = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("LanguageBtn")->getChildByName("label");
    _btnLanguagesLabel->setString(TTLanguageManager::getInstance()->getTag(LANGUAGES_PAUSE_TAG));
    
    _btnHowToPlay = (cocos2d::ui::Button*)_widgetSettings->getChildByName("settingsBg")->getChildByName("HowToPlayBtn");
    _btnHowToPlay->setVisible(true);
     _btnHowToPlay->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onClickHowToBtn,this));
    _btnHowToPlayLabel = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("HowToPlayBtn")->getChildByName("label");
    _btnHowToPlayLabel->setString(TTLanguageManager::getInstance()->getTag(HOWTOPLAY_TAG));

    _btnExitLevel = (cocos2d::ui::Button*)_widgetSettings->getChildByName("settingsBg")->getChildByName("ExitLevelBtn");
    _btnExitLevel->setVisible(true);
    _btnExitLevel->addTouchEventListener(CC_CALLBACK_2(TTUISettings::onClickExitMenuBtn,this));
    _btnExitLevelLabel = (cocos2d::ui::TextBMFont*)_widgetSettings->getChildByName("settingsBg")->getChildByName("ExitLevelBtn")->getChildByName("label");
    
    // Creamos el bg
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
    
    // Eventos
    _classListeners.push_back(registerListenerEvent(EVENT_LANGUAGE_CHANGE,CC_CALLBACK_1(TTUISettings::onEvent_languageChange,this)));
}

void TTUISettings::pauseUI()
{
    _blockMenus = true;
    _bgUI->setVisible(false);
}

void TTUISettings::restoreUI()
{
    _blockMenus = false;
    _bgUI->setVisible(true);
}

void TTUISettings::showMenu(Node* _parent)
{
    _blockMenus = false;
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,19);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiSettingsLayer->getParent() != nullptr)
        _uiSettingsLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiSettingsLayer,20);
    _uiSettingsLayer->setVisible(true);
    _widgetSettings->setVisible(true);
    
    if(_activeMenu == PAUSE)
    {
        _pauseTitle->setString(TTLanguageManager::getInstance()->getTag(TITLE_PAUSE_TAG));
        _btnExitLevelLabel->setString(TTLanguageManager::getInstance()->getTag(QUIT_LEVEL_TAG));
    }
    else
    {
        _pauseTitle->setString(TTLanguageManager::getInstance()->getTag(TITLE_SETTINGS_TAG));
        _btnExitLevelLabel->setString(TTLanguageManager::getInstance()->getTag(QUIT_GAME_TAG));
    }
    
    _languagesSelection->setVisible(false);
    _btnLanguages->setVisible(true);
    _btnHowToPlay->setVisible(true);
    _btnExitLevel->setVisible(true);
    
    // Activamos el tick del idioma actual
    for(int i = 0; i < LANGUAGES_NUM; i++)
    {
        char stLang[100] = {0};
        sprintf(stLang,"language%d",i+1);
        _languagesSelection->getChildByName(stLang)->getChildByName("tick")->setVisible(false);
    }
    char stLang[100] = {0};
    sprintf(stLang,"language%d",TTLanguageManager::getInstance()->_language+1);
    _languagesSelection->getChildByName(stLang)->getChildByName("tick")->setVisible(true);
    
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_SETTINGS.ExportJson","panel_enter");
    TTAudioManager::getInstance()->playAudio("PANEL_DOWN");
    
    // Enviamos evento de analitica a Parse
    std::map<std::string,std::string> _dimensions;
    TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_SETTINGS_MENU,_dimensions);
}

void TTUISettings::showPause(Node* _parent)
{
    _activeMenu = PAUSE;
    _actualParent = _parent;
    TTUIStackManager::getInstance()->showUILoading(_parent);
    preloadResources();
}

void TTUISettings::showSettings(Node* _parent)
{
    _activeMenu = SETTINGS;
    _actualParent = _parent;
    TTUIStackManager::getInstance()->showUILoading(_parent);
    preloadResources();
}

void TTUISettings::exitMenu()
{
    _uiSettingsLayer->setVisible(false);
    _widgetSettings->setVisible(false);
    _bgUI->setVisible(false);
    TTUserInfo::getInstance()->saveData();
    TTUIStackManager::getInstance()->closeUI();
}

void TTUISettings::onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (_blockMenus )
        return;
    
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_SETTINGS.ExportJson","panel_enter")->stop();

    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_SETTINGS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
        sendEvent(EVENT_GAME_CONTINUE);
        destroyInstance();
    }));
    _uiSettingsLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTUISettings::onSliderPercentChange(Ref *sender,ui::Slider::EventType controlEvent)
{
    /*if (_blockMenus)
        return;
    
    if(sender == _musicSlider)
    {
        int _percent = _musicSlider->getPercent();
        TTAudioManager::getInstance()->changeVolumeMusic(_percent);
    }
    else if(sender == _musicFXSlider)
    {
        int _percent = _musicFXSlider->getPercent();
        TTAudioManager::getInstance()->changeVolumeFx(_percent);
    }*/
}

void TTUISettings::onSliderTouch(Ref *sender,ui::Widget::TouchEventType touchEvent)
{
    if(touchEvent == ui::Widget::TouchEventType::ENDED)
    {
        if (_blockMenus)
            return;
        
        if(sender == _musicSlider)
        {
            int _percent = _musicSlider->getPercent();
            TTAudioManager::getInstance()->changeVolumeMusic(_percent);
            
            if(_percent == 0)
            {
                std::map<std::string,std::string> _dimensions;
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_MUSIC_OFF,_dimensions);
            }
        }
        else if(sender == _musicFXSlider)
        {
            int _percent = _musicFXSlider->getPercent();
            TTAudioManager::getInstance()->changeVolumeFx(_percent);

            if(_percent == 0)
            {
                std::map<std::string,std::string> _dimensions;
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_FX_OFF,_dimensions);
            }
        }

        TTAudioManager::getInstance()->playAudio("BUTTON_ON");
    }
}

void TTUISettings::onClickLanguagesExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    _languagesSelection->setVisible(false);
    _btnLanguages->setVisible(true);
    _btnHowToPlay->setVisible(true);
    _btnExitLevel->setVisible(true);
}

void TTUISettings::onClickExitMenuBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (_blockMenus)
        return;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    if(_activeMenu == PAUSE)
    {
        _blockMenus = true;
        ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_SETTINGS.ExportJson","panel_exit");
        DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
        CallFunc *hideSettings = CallFunc::create(std::bind([&](){
            exitMenu();
            TTGameMenus::getInstance()->showExitMenu(_uiSettingsLayer->getParent());
            destroyInstance();
        }));
        _uiSettingsLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
        action->play();
    }
    else if(_activeMenu == SETTINGS)
    {
        // Salir del juego
        TTGameMenus::getInstance()->showExitMenu(_actualParent,true);
    }
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTUISettings::onClickLanguagesBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    //delay
    auto _delay = DelayTime::create(0.2);
    auto _setVisible = CallFunc::create(std::bind([&](){
        _languagesSelection->setVisible(true);
        _btnLanguages->setVisible(false);
        _btnHowToPlay->setVisible(false);
        _btnExitLevel->setVisible(false);
    }));

    _uiSettingsLayer->getParent()->runAction(Sequence::create(_delay, _setVisible, NULL));

    //reproducimos audio
    TTAudioManager::getInstance()->playAudio("BUTTON_LANG");
    
    _blockMenus = false;
}

void TTUISettings::onClickChangeLanguageBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    // Escondemos todos los ticks
    for(int i = 0; i < LANGUAGES_NUM; i++)
    {
        char stLang[100] = {0};
        sprintf(stLang,"language%d",i+1);
        _languagesSelection->getChildByName(stLang)->getChildByName("tick")->setVisible(false);
    }
    
    // Activamos el tick seleccionado
    cocos2d::ui::Widget* _parent = (cocos2d::ui::Widget*)((cocos2d::ui::Widget*)pSender)->getParent();
    std::string name = _parent->getName();
    _languagesSelection->getChildByName(name)->getChildByName("tick")->setVisible(true);
    
    int idLanguage = _languagesSelection->getChildByName(name)->getTag();
    TTLanguageManager::getInstance()->setLanguage((LANGUAGES)idLanguage);
    
    //delay
    auto _delay = DelayTime::create(0.2);
    auto _setVisible = CallFunc::create(std::bind([&](){
        //cerrar
        _languagesSelection->setVisible(false);
        _btnLanguages->setVisible(true);
        _btnHowToPlay->setVisible(true);
        _btnExitLevel->setVisible(true);

    }));
    
    _uiSettingsLayer->getParent()->runAction(Sequence::create(_delay, _setVisible, NULL));
    
    //reproducimos audio
    TTAudioManager::getInstance()->playAudio("BUTTON_LANG_SELECT");
    
    _blockMenus = false;
    
    //añadimos a las dimensiones del analytics el idioma seleccionado
    std::map<std::string,std::string> _dimensions = {{"Language_Selection",TTLanguageManager::getInstance()->getActualLanguage()}};
    TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_LANGUAGE_MENU,_dimensions);
}

void TTUISettings::onEvent_languageChange(EventCustom* e)
{
    // Seteamos de nuevo los labels de textos
    _musicOptionLabel->setString(TTLanguageManager::getInstance()->getTag(MUSIC_PAUSE_TAG));
    _musicFXOptionLabel->setString(TTLanguageManager::getInstance()->getTag(FX_PAUSE_TAG));
    _btnHowToPlayLabel->setString(TTLanguageManager::getInstance()->getTag(HOWTOPLAY_TAG));
    _btnLanguagesLabel->setString(TTLanguageManager::getInstance()->getTag(LANGUAGES_PAUSE_TAG));
    
    if(_activeMenu == PAUSE)
    {
        _btnExitLevelLabel->setString(TTLanguageManager::getInstance()->getTag(QUIT_LEVEL_TAG));
        _pauseTitle->setString(TTLanguageManager::getInstance()->getTag(TITLE_PAUSE_TAG));
    }
    else if(_activeMenu == SETTINGS)
    {
        _btnExitLevelLabel->setString(TTLanguageManager::getInstance()->getTag(QUIT_GAME_TAG));
        _pauseTitle->setString(TTLanguageManager::getInstance()->getTag(TITLE_SETTINGS_TAG));
    }
    
}

void TTUISettings::onClickHowToBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
    
    std::vector<int> _idsHowTo;
    for(int i = 0; i < NUM_HOWTO_PAGES; i++)
    {
        _idsHowTo.push_back(i+1);
    }
    
    TTUIPopup::getInstance()->showHowToPopup(_uiSettingsLayer->getParent(),_idsHowTo, true);
}
