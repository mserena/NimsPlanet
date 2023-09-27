#include "TTUIStackManager.h"
#include "TTAnimalCubes.h"
#include "TTGameMenus.h"
#include "TTUIPopup.h"
#include "TTUIMap.h"
#include "TTUISettings.h"
#include "TTUIShop.h"
#include "TTUIFacebook.h"
#include "TTGameUIManager.h"
#include "TTUIPopupSmart.h"
#include "TTDailyBonusManager.h"

//Singleton
static TTUIStackManager *_sharedUIStackManager = nullptr;

TTUIStackManager* TTUIStackManager::getInstance()
{
    if (!_sharedUIStackManager)
    {
        _sharedUIStackManager = new TTUIStackManager();
        _sharedUIStackManager->init();
    }
    
    return _sharedUIStackManager;
}

void TTUIStackManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedUIStackManager);
}

void TTUIStackManager::init()
{
    Size _screenSize = Director::getInstance()->getWinSize();
    _parentLoadingUI = Node::create();
    
    _loadingSpriteUI = Sprite::create("loadingClock.png");
    _loadingSpriteUI->setAnchorPoint(Point(0.5,0.5));
    _loadingSpriteUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
   
    //el loading es un 25% del ancho de la pantalla
    float _scale = scale_sprite(_loadingSpriteUI, 25, 100);
    
    _loadingSpriteUI->setScale(_scale);
    _parentLoadingUI->addChild(_loadingSpriteUI,2);
    
    auto _rotate = RotateBy::create(2,360);
    _loadingSpriteUI->runAction(RepeatForever::create(_rotate));
    
    _bgLoadingUI = Sprite::create("pixelBg.png");
    _bgLoadingUI->setAnchorPoint(Point(0.5,0.5));
    _bgLoadingUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgLoadingUI->setScale(_screenSize.width,_screenSize.height);
    _parentLoadingUI->addChild(_bgLoadingUI,1);
    
    _parentLoadingUI->setVisible(false);
    _parentLoadingUI->retain();
    
    // Animación conexión WIFI
    _parentConnectingUI = Node::create();
    
    Sprite* _connectingSpriteUI = Sprite::createWithSpriteFrameName("wifi_naranja.png");
    Sprite* _spriteAnimation = Sprite::create();
    _spriteAnimation->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimation->setPosition(Vec2(_connectingSpriteUI->getContentSize().width/2,_connectingSpriteUI->getContentSize().height/2));
    _connectingSpriteUI->addChild(_spriteAnimation);
    
    _spriteFailConnection = Sprite::createWithSpriteFrameName("wifi_fail.png");
    _spriteFailConnection->setAnchorPoint(Vec2(0.5,0.5));
    _spriteFailConnection->setPosition(Vec2(_connectingSpriteUI->getContentSize().width-(_connectingSpriteUI->getContentSize().width/5),_connectingSpriteUI->getContentSize().height/5));
    _spriteFailConnection->setVisible(false);
    _connectingSpriteUI->addChild(_spriteFailConnection,2);
    
    Animate* animNode = Animate::create(AnimationCache::getInstance()->getAnimation("wifi_connection"));
    Sequence* _animSequence = Sequence::create(animNode,DelayTime::create(0.2),NULL);
    _spriteAnimation->runAction(RepeatForever::create(_animSequence));
    
    float _scaleConnecting = scale_sprite(_connectingSpriteUI, 25, 100);
    _connectingSpriteUI->setScale(_scaleConnecting);
    _connectingSpriteUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _parentConnectingUI->addChild(_connectingSpriteUI,2);
    
    _bgConnectingUI = Sprite::create("pixelBg.png");
    _bgConnectingUI->setAnchorPoint(Point(0.5,0.5));
    _bgConnectingUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgConnectingUI->setScale(_screenSize.width,_screenSize.height);
    _parentConnectingUI->addChild(_bgConnectingUI,1);
    
    _parentConnectingUI->setVisible(false);
    _parentConnectingUI->retain();
}

TTUIStackManager::~TTUIStackManager()
{
    _parentLoadingUI->release();
}

void TTUIStackManager::clearStack()
{
    _uiStack.clear();
}

bool TTUIStackManager::isStackEmpty()
{
    return _uiStack.empty();
}

void TTUIStackManager::goToNewUI(UI_TYPES _menu)
{
    _uiStack.push_back(_menu);
    

    if(_uiStack.size() > 1)
    {
        // Cambiamos a nueva UI
        pauseUI(_uiStack[_uiStack.size()-2]);
    }
}

void TTUIStackManager::closeUI()
{
    if(_uiStack.size() == 0) return;
    
    if(_uiStack.size() > 1)
    {
        restoreUI(_uiStack[_uiStack.size()-2]);
    }
    
    _uiStack.erase(_uiStack.begin()+(_uiStack.size()-1));
}

//devuelve si la UI que se le pasa es la activa actual
bool TTUIStackManager::isUIOnTop(UI_TYPES _uiType)
{
    if(_uiStack.size() == 0) return false;
    return (_uiStack[_uiStack.size()-1] == _uiType);
}

void TTUIStackManager::pauseUI(UI_TYPES _stackUI)
{
    CCLOG("PauseUI[%d]",_stackUI);
    switch(_stackUI)
    {
        case UI_GAME_MENU_INIT:
        case UI_GAME_MENU_RESUME:
        {
            TTGameMenus::getInstance()->pauseUI();
        }
        break;
        case UI_MAP:
        {
            TTUIMap::getInstance()->pauseUI();
        }
        break;
        case UI_SETTINGS:
        {
            TTUISettings::getInstance()->pauseUI();
        }
        break;
        case UI_SHOP:
        {
            TTUIShop::getInstance()->pauseUI();
        }
        break;
        case UI_INGAME:
        {
            TTGameUIManager::getInstance()->pauseUI();
        }
        break;
        case UI_POPUP:
        {
            TTUIPopup::getInstance()->pauseUI();
        }
        break;
        case UI_FACEBOOK:
        {
            TTUIFacebook::getInstance()->pauseUI();
        }
        break;
        case UI_POPUP_SMART:
        {
            TTUIPopupSmart::getInstance()->pauseUI();
        }
        break;
        case UI_DAILY_BONUS:
        {
            TTDailyBonusManager::getInstance()->pauseUI();
        }
        break;
        default:
        {
            CCLOG("Se intenta pausar UI no definida");
            abort();
        }
        break;
    }
}

void TTUIStackManager::restoreUI(UI_TYPES _stackUI)
{
    CCLOG("RestoreUI[%d]",_stackUI);

    switch(_stackUI)
    {
        case UI_GAME_MENU_INIT:
        case UI_GAME_MENU_RESUME:
        {
            TTGameMenus::getInstance()->restoreUI();
        }
        break;
        case UI_MAP:
        {
            TTUIMap::getInstance()->restoreUI();
        }
        break;
        case UI_SETTINGS:
        {
            TTUISettings::getInstance()->restoreUI();
        }
        break;
        case UI_SHOP:
        {
            TTUIShop::getInstance()->restoreUI();
        }
        break;
        case UI_INGAME:
        {
            TTGameUIManager::getInstance()->restoreUI();
        }
        break;
        case UI_POPUP:
        {
            TTUIPopup::getInstance()->restoreUI();
        }
        break;
        case UI_FACEBOOK:
        {
            TTUIFacebook::getInstance()->restoreUI();
        }
        break;
        case UI_POPUP_SMART:
        {
            TTUIPopupSmart::getInstance()->restoreUI();
        }
        break;
        case UI_DAILY_BONUS:
        {
            TTDailyBonusManager::getInstance()->restoreUI();
        }
        break;
        default:
        {
            CCLOG("Se intenta reactivar UI no definida");
            abort();
        }
        break;
    }
}

void TTUIStackManager::showUILoading(Node* _parent)
{
    if(_parentLoadingUI->getParent() != nullptr)
        _parentLoadingUI->removeFromParentAndCleanup(false);
    _parent->addChild(_parentLoadingUI,60);
    _parentLoadingUI->setVisible(true);
}

void TTUIStackManager::hideUILoading()
{
    _parentLoadingUI->setVisible(false);
    _parentLoadingUI->removeFromParentAndCleanup(false);
}

void TTUIStackManager::blockUITouches(Node *parentConnectingUI)
{
    // Agregamos el listener del touch
    _touchListenerBg = EventListenerTouchOneByOne::create();
    _touchListenerBg->setSwallowTouches(true);
    _touchListenerBg->onTouchBegan = CC_CALLBACK_2(TTUIStackManager::onTouchesUIBegan, this);
    _touchListenerBg->onTouchMoved = CC_CALLBACK_2(TTUIStackManager::onTouchesUI, this);
    _touchListenerBg->onTouchEnded = CC_CALLBACK_2(TTUIStackManager::onTouchesUI, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListenerBg,parentConnectingUI);
}

void TTUIStackManager::unblockUITouches()
{
    Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListenerBg);
}

void TTUIStackManager::showUIConnecting(Node* _parent)
{
    if(_parentConnectingUI->getParent() != nullptr)
        _parentConnectingUI->removeFromParentAndCleanup(false);
    _parent->addChild(_parentConnectingUI,60);
    _parentConnectingUI->setVisible(true);
    _spriteFailConnection->setVisible(false);
    
    blockUITouches(_parentConnectingUI);
}

void TTUIStackManager::hideUIConnecting(bool _error)
{
    if(_error)
    {
        _spriteFailConnection->setVisible(true);
        auto _scaleUp = ScaleTo::create(0.2f,_spriteFailConnection->getScale()+0.1f);
        auto _scaleDown = ScaleTo::create(0.2f,_spriteFailConnection->getScale());
        auto _funcHideUI = CallFunc::create(std::bind([&](){
            unblockUITouches();
            _parentConnectingUI->setVisible(false);
            _parentConnectingUI->removeFromParentAndCleanup(false);
        }));

        _spriteFailConnection->runAction(Sequence::create(_scaleUp,_scaleDown,DelayTime::create(2.0f),_funcHideUI,NULL));
    }
    else
    {
        unblockUITouches();
        _parentConnectingUI->setVisible(false);
        _parentConnectingUI->removeFromParentAndCleanup(false);
    }
}

void TTUIStackManager::killTopUI()
{
    if(_uiStack.size() == 0) return;
    if(_parentLoadingUI->isVisible()) return;
    
    switch(_uiStack[_uiStack.size()-1])
    {
        case UI_GAME_MENU_INIT:
        {
            TTGameMenus::getInstance()->onClickInitExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        break;
        case UI_GAME_MENU_RESUME:
        {
            TTGameMenus::getInstance()->onClickExitLevelDoneBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        break;
        case UI_SETTINGS:
        {
            TTUISettings::getInstance()->onClickExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        break;
        case UI_SHOP:
        {
            TTUIShop::getInstance()->onClickExitBtn(nullptr, ui::Widget::TouchEventType::ENDED);
        }
        break;
        case UI_POPUP:
        {
            if(TTUIPopup::getInstance()->_howToPopup->isVisible())
            {
                TTUIPopup::getInstance()->onClickExitHowtoBtn(nullptr,ui::Widget::TouchEventType::ENDED);
            }
            else
            {
                TTUIPopup::getInstance()->onClickExitPopupBtn(nullptr,ui::Widget::TouchEventType::ENDED);
            }
        }
        break;
        case UI_FACEBOOK:
        {
            TTUIFacebook::getInstance()->onClickExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        break;
        case UI_POPUP_SMART:
        {
            TTUIPopupSmart::getInstance()->onClickExitPopupBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        break;
        default:
        {
        }
        break;
    
    }
}

bool TTUIStackManager::onTouchesUIBegan(cocos2d::Touch* touch, cocos2d::Event *event)
{
    event->stopPropagation();
}

void TTUIStackManager::onTouchesUI(cocos2d::Touch* touch, cocos2d::Event *event)
{
    event->stopPropagation();
}


