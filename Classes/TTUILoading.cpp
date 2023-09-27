#include "TTUILoading.h"
#include "TTLanguageManager.h"
#include "TTEventsManager.h"

//Singleton
static TTUILoading *_sharedUILoading = nullptr;

TTUILoading* TTUILoading::getInstance()
{
    if (!_sharedUILoading)
    {
        _sharedUILoading = new TTUILoading();
        _sharedUILoading->init();
    }
    
    return _sharedUILoading;
}

void TTUILoading::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedUILoading);
}

void TTUILoading::init()
{
    Size _screenSize = Director::getInstance()->getWinSize();
    _loadingParent = Node::create();
    _loadingParent->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _loadingParent->retain();
    
    
    // Cargamos el widget
    _widgetLoading = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_LOADING.ExportJson");
    _widgetLoading->setScale(scale_widget(_widgetLoading,WIDTH_UI_LOADING_SPACE,HEIGHT_UI_LOADING_SPACE));
    _widgetLoading->setAnchorPoint(Vec2(0.5,0.5));
    _widgetLoading->setPosition(Point(0,-_screenSize.height*0.05f));
    
    // Recuperamos los items que usaremos
    _loadingTextLabel = (cocos2d::ui::TextBMFont*)_widgetLoading->getChildByName("loading_text");
    
    // Creamos los fondos del Loading
    _bgLoading = Sprite::create("bg_MainMenu.jpg");
    _bgLoading->setAnchorPoint(Point(0.5,0.5));
    _bgLoading->setScale(_screenSize.height/_bgLoading->getContentSize().height);
    
    _logoBgLoading = Sprite::create("background_logo.png");
    _logoBgLoading->setAnchorPoint(Point(0.5,0.5));
    _logoBgLoading->setPosition(Point(0,_screenSize.height*0.22));
    _logoBgLoading->setScale(_screenSize.height/_bgLoading->getContentSize().height);
    
    _loadingParent->addChild(_bgLoading,1);
    _loadingParent->addChild(_logoBgLoading,2);
    _loadingParent->addChild(_widgetLoading,2);
}

TTUILoading::~TTUILoading()
{

}

void TTUILoading::showLoading(Node* _parent)
{
    if(_loadingParent->getParent() != nullptr)
        _loadingParent->removeFromParentAndCleanup(false);
    _parent->addChild(_loadingParent);
    _loadingParent->setVisible(true);
    
    _loadingTextLabel->setString(TTLanguageManager::getInstance()->getTag(LOADING_TAG));
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_LOADING.ExportJson","loading");
    
    // Envia evento para indicar que esta en loading
    sendEvent(EVENT_SCENE_ON_LOADING);
}
