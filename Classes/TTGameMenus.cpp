#include "TTGameMenus.h"
#include "TTLanguageManager.h"
#include "TTAnimalCubes.h"
#include "TTBoardBoosters.h"
#include "TTUserInfo.h"
#include "TTBoostersManager.h"
#include "TTUIPopup.h"
#include "TTUIStackManager.h"
#include "TTStoreManager.h"
#include "TTAudioManager.h"
#include "TTUserLives.h"
#include "TTUIMap.h"
#include "TTFacebook.h"
#include "TTUIShop.h"
#include "TTUIPopupSmart.h"
#include "TTNotificationManager.h"
#include "TTParseManager.h"

#define MAX_GOALS                           5
#define MAX_BOOSTERS_BOARD                  4
#define STAR_DELAY                         0.5
#define SECONDS_TO_REGENERATE              1800    //30min

//Singleton
static TTGameMenus *_sharedGameMenus = nullptr;

TTGameMenus* TTGameMenus::getInstance()
{
    if (!_sharedGameMenus)
    {
        _sharedGameMenus = new TTGameMenus();
        _sharedGameMenus->init();
    }
    
    return _sharedGameMenus;
}

void TTGameMenus::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedGameMenus);
}

void TTGameMenus::init()
{
    // Registramos los eventos
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_CONNECTION,CC_CALLBACK_1(TTGameMenus::onEvent_FacebookConnect,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_RANKING, CC_CALLBACK_1(TTGameMenus::onEvent_FacebookRanking,this)));
    
    //Inicializa varibles
    _blockMenus = false;
    _retryMode = false;
    
    // Cargamos el XML de la UI Resume
    _uiLayer = Layer::create();
    _uiLayer->retain();
    _widgetResume = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_GAME_MENUS.ExportJson");
    cocos2d::ui::Layout* _layoutResume = dynamic_cast<cocos2d::ui::Layout*>(_widgetResume);
    _uiLayer->addChild(_layoutResume);
    
    // Escalado Widget
    _widgetResume->setScale(scale_widget(_widgetResume,WIDTH_UI_RESUME_SPACE,HEIGHT_UI_RESUME_SPACE));
    
    // Posicionado Widget
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetResume->setAnchorPoint(Vec2(0.5,0.5));
    _uiLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_widgetResume->getBoundingBox().size.height/2)));
    _uiLayer->setVisible(false);
    
    // Comunes
    _levelTitleLabel = (cocos2d::ui::TextBMFont*)_widgetResume->getChildByName("bg_menu")->getChildByName("levelTitle");
    _initParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("bg_menu")->getChildByName("initGameMenu");
    _initParent->setVisible(false);
    _resumeParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("bg_menu")->getChildByName("finishGameMenu");
    _resumeParent->setVisible(false);
    _exitParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("bg_menu")->getChildByName("exitGameMenu");
    _exitParent->setVisible(false);
    
    // Exit
    _btnCancel = (cocos2d::ui::Button*)_exitParent->getChildByName("cancelBtn");
    _btnCancel->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickExitLevelCancelBtn, this));
    _btnApply = (cocos2d::ui::Button*)_exitParent->getChildByName("acceptBtn");
    _btnApply->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickExitLevelDoneBtn, this));
    _labelExit1 = (cocos2d::ui::TextBMFont*)_exitParent->getChildByName("infoWood")->getChildByName("label1");
    _labelExit2 = (cocos2d::ui::TextBMFont*)_exitParent->getChildByName("infoWood")->getChildByName("label2");
    _birdExitGame = (cocos2d::ui::Widget*)_exitParent->getChildByName("cryBirdExitGame");
    _birdExitLevel  = (cocos2d::ui::Widget*)_exitParent->getChildByName("cryBird");;
    _heartExitLevel  = (cocos2d::ui::Widget*)_exitParent->getChildByName("breakHeart");;
    
    // Init
    _btnPlay = (cocos2d::ui::Button*)_initParent->getChildByName("play_btn");
    _btnPlayLabel = (cocos2d::ui::TextBMFont*)_initParent->getChildByName("play_btn")->getChildByName("text_btn");
    _btnPlay->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickPlayBtn, this));
    
    _btnExitInit = (cocos2d::ui::Button*)_initParent->getChildByName("exit_btn");
    _btnExitInit->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickInitExitBtn, this));
    
    _boostersTitle = (cocos2d::ui::TextBMFont*)_initParent->getChildByName("boosters")->getChildByName("boostersTitle");
    
    // Objetives
    _objectivesLabel = (cocos2d::ui::TextBMFont*)_initParent->getChildByName("objetivos")->getChildByName("objetivosTitle");
    _objectivesIcons = (cocos2d::ui::Widget*)_initParent->getChildByName("objetivos")->getChildByName("objetivosIconos");
    
    //Boosters
    for(int i = 0; i < MAX_BOOSTERS_BOARD; i++)
    {
        boosterUI _newBoosterUI;
        char strBooster[100] = {0};
        sprintf(strBooster,"booster%d",i);
        cocos2d::ui::Widget* _booster = (cocos2d::ui::Widget*)_initParent->getChildByName("boosters")->getChildByName(strBooster);
        _newBoosterUI._boosterOff = (cocos2d::ui::Widget*)_booster->getChildByName("booster_off");
        _newBoosterUI._boosterOnParent = (cocos2d::ui::Widget*)_booster->getChildByName("booster_onBtn");
        _newBoosterUI._boosterMore = (cocos2d::ui::Widget*)_booster->getChildByName("booster_onBtn")->getChildByName("moreBooster");
        _newBoosterUI._boosterInfo = (cocos2d::ui::TextBMFont*)_booster->getChildByName("booster_onBtn")->getChildByName("infoBooster");
        _newBoosterUI._labelNumBoosters = (cocos2d::ui::TextBMFont*)_booster->getChildByName("booster_onBtn")->getChildByName("infoBooster")->getChildByName("num_label");
        _newBoosterUI._boosterApplyed = (cocos2d::ui::Widget*)_booster->getChildByName("booster_onBtn")->getChildByName("applyedMark");
        
        _newBoosterUI._boosterInfo->setVisible(false);
        
        _newBoosterUI._boosterOnParent->reorderChild(_newBoosterUI._boosterApplyed,3);
        _newBoosterUI._boosterOnParent->reorderChild(_newBoosterUI._boosterMore,3);
        _newBoosterUI._boosterOnParent->reorderChild(_newBoosterUI._boosterInfo,3);
        
        _boosters.push_back(_newBoosterUI);
    }
    
    // Win
    _winResumeParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("bg_menu")->getChildByName("finishGameMenu")->getChildByName("winMenu");
    _winResumeParent->setVisible(false);
    _btnContinue = (cocos2d::ui::Button*)_winResumeParent->getChildByName("continue_btn");
    _btnContinueLabel = (cocos2d::ui::TextBMFont*)_winResumeParent->getChildByName("continue_btn")->getChildByName("text_btn");
    _btnContinue->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickContinueBtn, this));
    for(int i = 0; i < 3; i++)
    {
        // animación
        char starNameOnStr[10] = {0};
        sprintf(starNameOnStr,"star_%d_on",i+1);
        char starNameOffStr[10] = {0};
        sprintf(starNameOffStr,"star_%d_off",i+1);
        std::vector<ui::Widget* > stars;
        stars.push_back((cocos2d::ui::Widget*)_winResumeParent->getChildByName("stars")->getChildByName(starNameOnStr));
        stars.push_back((cocos2d::ui::Widget*)_winResumeParent->getChildByName("stars")->getChildByName(starNameOffStr));
        _stars.push_back(stars);
        
        // particulas
        ParticleSystemQuad* starParticle = ParticleSystemQuad::create("star_win.plist");
        stars[0]->addChild(starParticle,10);
        _starsParticles.push_back(starParticle);
    }
    _scoreLabel = (cocos2d::ui::TextBMFont*)_winResumeParent->getChildByName("score")->getChildByName("scoreLabel");
    _scoreTitle = (cocos2d::ui::TextBMFont*)_winResumeParent->getChildByName("score")->getChildByName("scoreTitle");
    
    //Lose
    _loseResumeParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("bg_menu")->getChildByName("finishGameMenu")->getChildByName("loseMenu");
    _loseResumeParent->setVisible(false);
    _btnRetry = (cocos2d::ui::Button*)_loseResumeParent->getChildByName("retry_btn");
    _btnRetryLabel = (cocos2d::ui::TextBMFont*)_loseResumeParent->getChildByName("retry_btn")->getChildByName("text_btn");
    _btnRetry->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickRetryBtn, this));
    _loseTitleLabel = (cocos2d::ui::TextBMFont*)_loseResumeParent->getChildByName("loseTitleLabel");
    _btnExitRetry = (cocos2d::ui::Button*)_loseResumeParent->getChildByName("exit_btn");
    _btnExitRetry->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickRetryExitBtn, this));
    _objectivesIconsLose = (cocos2d::ui::Widget*)_loseResumeParent->getChildByName("wood_info_bg")->getChildByName("objectivesLabel");
    
    _btnBuy = (cocos2d::ui::Button*)_loseResumeParent->getChildByName("buyBtn");
    _btnBuy->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickContinueBuyBtn, this));
    _iconTimeBuy = (cocos2d::ui::Widget*)_loseResumeParent->getChildByName("buyBtn")->getChildByName("imgContinueTime");
    _iconMovesBuy = (cocos2d::ui::Widget*)_loseResumeParent->getChildByName("buyBtn")->getChildByName("imgContinueMov");
    _buyLabel1 = (cocos2d::ui::TextBMFont*)_loseResumeParent->getChildByName("buyBtn")->getChildByName("continueLabel1");
    _buyLabel2 = (cocos2d::ui::TextBMFont*)_loseResumeParent->getChildByName("buyBtn")->getChildByName("continueLabel2");
    _buyPrice = (cocos2d::ui::TextBMFont*)_loseResumeParent->getChildByName("buyBtn")->getChildByName("continuePrice");
    
    // Ranking
    _rankingParent = (cocos2d::ui::Widget*)_widgetResume->getChildByName("rankingGameMenu");
    _rankingFacebookNoConnect = (cocos2d::ui::Widget*)_widgetResume->getChildByName("rankingGameMenu")->getChildByName("bgRanking")->getChildByName("connectToFacebook");
    _rankingFacebookInvite = (cocos2d::ui::Widget*)_widgetResume->getChildByName("rankingGameMenu")->getChildByName("bgRanking")->getChildByName("inviteFriends");
    _rankingFacebookConnect = (cocos2d::ui::Widget*)_widgetResume->getChildByName("rankingGameMenu")->getChildByName("bgRanking")->getChildByName("rankingFacebook");
    _rankingParent->setVisible(false);
    _rankingFacebookNoConnect->setVisible(false);
    _rankingFacebookConnect->setVisible(false);
    _rankingFacebookInvite->setVisible(false);
    for(int i = 0; i < 3; i++)
    {
        std::string _rankingId = "ranking_"+std::to_string(i+1);
        ((cocos2d::ui::Button*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("inviteBtn")->getChildByName("Btn"))->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickInviteFacebookBtn,this));
    }
    
    _btnConnect = (cocos2d::ui::Button*)_rankingFacebookNoConnect->getChildByName("btnFacebookConnect");
    _btnConnect->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickConnectFacebookBtn,this));
    _btnConnectLabel = (cocos2d::ui::TextBMFont*)_rankingFacebookNoConnect->getChildByName("btnFacebookConnect")->getChildByName("btnFacebookConnectLabel");
    _connectLabel = (cocos2d::ui::TextBMFont*)_rankingFacebookNoConnect->getChildByName("connectLabel");
    
    _inviteLabel = (cocos2d::ui::TextBMFont*)_rankingFacebookInvite->getChildByName("inviteLabel");
    _btnInvite = (cocos2d::ui::Button*)_rankingFacebookInvite->getChildByName("btnFacebookInvite");
    _btnInvite->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickInviteFacebookBtn,this));
    _btnInviteLabel = (cocos2d::ui::TextBMFont*)_rankingFacebookInvite->getChildByName("btnFacebookInvite")->getChildByName("btnFacebookInviteLabel");
    
    // Creamos el bg
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
    
    // Animación conexión WIFI
    cocos2d::ui::Widget* _WifiBg = ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting")->getChildByName("Wifi_BackGround"));
    _spriteAnimationWiFi = Sprite::create();
    _spriteAnimationWiFi->setAnchorPoint(Vec2(0.5,0.5));
    _spriteAnimationWiFi->setScale(1.5f);
    _spriteAnimationWiFi->setPosition(Vec2(_WifiBg->getContentSize().width/2,_WifiBg->getContentSize().height/2));
    _WifiBg->addChild(_spriteAnimationWiFi);
    
    //Animator
    Animate* animNode = Animate::create(AnimationCache::getInstance()->getAnimation("wifi_connection"));
    Sequence* _animSequence = Sequence::create(animNode,DelayTime::create(0.2),NULL);
    _spriteAnimationWiFi->runAction(RepeatForever::create(_animSequence));
}

TTGameMenus::~TTGameMenus()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
}

void TTGameMenus::reset()
{
    _initParent->setVisible(false);
    _resumeParent->setVisible(false);
    
    //Init panel
    _objectivesIcons->removeAllChildrenWithCleanup(true);
    
    //Resume panel
    _winResumeParent->setVisible(false);
    _loseResumeParent->setVisible(false);
    
    _rankingParent->setVisible(false);
    _rankingFacebookNoConnect->setVisible(false);
    _rankingFacebookConnect->setVisible(false);
}

void TTGameMenus::showInitMenu(gameInfo* _gameInfo, Node* _parent, bool retryMode)
{
    TTUIStackManager::getInstance()->goToNewUI(UI_GAME_MENU_INIT);
    
    reset();
    _blockMenus = false;
    _retryMode = retryMode;
    
    _currentGameInfo = _gameInfo;

    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,20);
    _bgUI->setVisible(true);
    
    if(_uiLayer->getParent() != nullptr)
        _uiLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiLayer,21);
    _uiLayer->setVisible(true);
    
    _widgetResume->setVisible(true);
    _initParent->setVisible(true);
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","panel_enter");
    TTAudioManager::getInstance()->playAudio("PANEL_DOWN");
    
    char levelStr[10] = {0};
    sprintf(levelStr,"%s %d",TTLanguageManager::getInstance()->getTag(LEVEL_TAG).c_str(),_gameInfo->level);
    _levelTitleLabel->setString(levelStr);
    _levelTitleLabel->setVisible(true);
    
    _objectivesLabel->setString(TTLanguageManager::getInstance()->getTag(OBJECTIVES_TAG));
    
    _btnPlayLabel->setString(TTLanguageManager::getInstance()->getTag(PLAY_TAG));
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","btn_play");
    _btnPlayLabel->setVisible(true);
    _boostersTitle->setString(TTLanguageManager::getInstance()->getTag("SELECCION_BOOSTERS"));
    
    // Objectivos
    for(int i = 0; i < _gameInfo->goals.size(); i++)
    {
        Sprite* _icon;
        switch(_gameInfo->goals[i]->_typeGoal)
        {
            case SCORE_GOAL:
            {
                _icon = Sprite::createWithSpriteFrameName("objetivo_puntos.png");
            }
            break;
            case BREAKS_GOAL:
            {
                _icon = Sprite::createWithSpriteFrameName("objetivo_barro.png");
            }
            break;
            case DOWN_OBJECT_GOAL:
            {
                int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                char objectStr[100] = {0};
                sprintf(objectStr,"objetivo_bajable_world%d.png",_actualWorld+1);
                _icon = Sprite::createWithSpriteFrameName(objectStr);
            }
            break;
            case COLLECT_FIGURES_GOAL:
            {
                int _figureToCollect = (static_cast<TTCollectFiguresGoal*>(_gameInfo->goals[i]))->_figureToCollect;
                char idFigureStr[100] = {0};
                int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                sprintf(idFigureStr,"objetivo_figura%d_world%d.png",_figureToCollect+1,_actualWorld+1);
                _icon = Sprite::createWithSpriteFrameName(idFigureStr);
            }
            break;
            case BLOCK_GOAL:
            {
                _icon = Sprite::createWithSpriteFrameName("objetivo_madera.png");
            }
            break;
            case STONE_GOAL:
            {
                _icon = Sprite::createWithSpriteFrameName("objetivo_piedra.png");
            }
            break;
            case NETWORK_GOAL:
            {
                _icon = Sprite::createWithSpriteFrameName("objetivo_red.png");
            }
            break;
            default:
            break;
        }
        float _scaleIcon = (_objectivesIcons->getBoundingBox().size.width/5)/_icon->getBoundingBox().size.width;
        _icon->setScale(_scaleIcon);
        
        float _xSpace = _objectivesIcons->getBoundingBox().size.width/MAX_GOALS;
        float _offsetIcons = i*_xSpace;
        float _goalXPosition = _xSpace/2;
        float _goalYPosition = _objectivesIcons->getBoundingBox().size.height/2;
        
        // Desplazamiento para centrar los objetivos
        float _globalOffset = _xSpace*2 - ((_xSpace/2)*(_gameInfo->goals.size()-1));
        _objectivesIcons->addChild(_icon);
        _icon->setPosition(Point(_goalXPosition+_offsetIcons+_globalOffset,_goalYPosition));
    }
    
    // Boosters
    showBoosters();
    
    // Facebook
    showFacebookInfo();
}

void TTGameMenus::showFacebookInfo()
{
    _rankingParent->setVisible(true);
    if(TTFacebook::getInstance()->isConnected())
    {
        _rankingFacebookInvite->setVisible(false);
        _rankingFacebookNoConnect->setVisible(false);
        _rankingFacebookConnect->setVisible(true);
        
        _spriteAnimationWiFi->stopAllActions();
        Animate* animNode = Animate::create(AnimationCache::getInstance()->getAnimation("wifi_connection"));
        Sequence* _animSequence = Sequence::create(animNode,DelayTime::create(0.2),NULL);
        _spriteAnimationWiFi->runAction(RepeatForever::create(_animSequence));
        
        // Activamos la animación conexión con Parse para esperar al evento con los datos
        ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting")->getChildByName("Wifi_BackGround")->getChildByName("tick_fail"))->stopAllActions();
        ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting")->getChildByName("Wifi_BackGround")->getChildByName("tick_fail"))->setVisible(false);
        ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking"))->setVisible(false);
        ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting"))->setVisible(true);

        // Consultamos el ranking a Parse
        TTParseManager::getInstance()->getRankingLevel(TTUserInfo::getInstance()->_userData->_selectedLevel,TTFacebook::getInstance()->getStringFriends(),TTFacebook::getInstance()->getMeId());
    }
    else
    {
        _rankingFacebookNoConnect->setVisible(true);
        _btnConnectLabel->setString(TTLanguageManager::getInstance()->getTag("CONNECT"));
        _connectLabel->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_NOCONNECT_LABEL"));
    }
}

void TTGameMenus::showBoosters()
{
    for(int i = 0; i < _boosters.size(); i++)
    {
        if(_boosters[i]._boosterOnParent->getChildByName("icon") != nullptr)
            _boosters[i]._boosterOnParent->getChildByName("icon")->removeFromParentAndCleanup(true);
    }
    
    int _activeBoosters = 0;
    std::map<BOOSTERS_TYPES,std::vector<int>>::const_iterator _itrBoosters;
    for(_itrBoosters = _currentGameInfo->_boosters.begin(); _itrBoosters != _currentGameInfo->_boosters.end(); ++_itrBoosters)
    {
        if(_itrBoosters->first != BOARD_BOOSTERS)
            continue;
        
        std::vector<int> _boostersBoard = _itrBoosters->second;
        for(int i = 0; i < _boostersBoard.size(); i++)
        {
            BOARD_BOOSTERS_TYPE typeBooster = BOARD_BOOSTERS_TYPE(_boostersBoard[i]);
            TTBoardBoosters* _boardBooster = TTBoostersManager::getInstance()->_boardBoostersInstances.find(typeBooster)->second;
            
            Sprite* _icon = Sprite::createWithSpriteFrameName(_boardBooster->_boosterIcon);
            _icon->setName("icon");
            _icon->setPosition(Point(_boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.width/2,_boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.height/2));
            float _scaleIcon = _boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.width/_icon->getBoundingBox().size.width;
            _icon->setScale(_scaleIcon*1.5);
            _boosters[_activeBoosters]._boosterOnParent->addChild(_icon,0);
            
            _boosters[_activeBoosters]._boosterOnParent->setTag(_boardBooster->_boosterSubType);
            _boosters[_activeBoosters]._boosterOnParent->addTouchEventListener(CC_CALLBACK_2(TTGameMenus::onClickBoosterBtn, this));
            
            std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"0_"+std::to_string(typeBooster));
            int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
            if(_boardBooster->_boosterActivated || _numBoosters > 0)
            {
                TTBoostersManager::getInstance()->activateBoosterById(_boosterItemId);
                _boosters[_activeBoosters]._boosterMore->setVisible(false);
                _boosters[_activeBoosters]._boosterApplyed->setVisible(true);
            }
            else
            {
                _boosters[_activeBoosters]._boosterMore->setVisible(true);
                _boosters[_activeBoosters]._boosterApplyed->setVisible(false);
            }
            
            _boosters[_activeBoosters]._boosterOnParent->setVisible(true);
            _boosters[_activeBoosters]._boosterOff->setVisible(false);
            _activeBoosters++;
        }
    }
    
    for(int i = _activeBoosters; i < MAX_BOOSTERS_BOARD;i++)
    {
        _boosters[i]._boosterOnParent->setVisible(false);
        _boosters[i]._boosterOff->setVisible(true);
    }
}

void TTGameMenus::showResumeMenu(gameInfo* _gameInfo, Node* _parent)
{
    reset();
    _blockMenus = false;
    _currentGameInfo = _gameInfo;
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,20);
    _bgUI->setVisible(true);
    
    if(_uiLayer->getParent() != nullptr)
        _uiLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiLayer,31);
    _uiLayer->setVisible(true);
    
    _widgetResume->setVisible(true);
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_enter");
    action->play();
    TTAudioManager::getInstance()->playAudio("PANEL_DOWN");
    DelayTime* _delayEnterMenu = DelayTime::create(action->getTotalTime()*0.75f);
    
    char levelStr[10] = {0};
    sprintf(levelStr,"%s %d",TTLanguageManager::getInstance()->getTag(LEVEL_TAG).c_str(),_gameInfo->level);
    _levelTitleLabel->setString(levelStr);
    _levelTitleLabel->setVisible(true);
    
    if(_gameInfo->winGame)
    {
        TTUIStackManager::getInstance()->goToNewUI(UI_GAME_MENU_RESUME);
        
        _resumeParent->setVisible(true);
        _winResumeParent->setVisible(true);
        _loseResumeParent->setVisible(false);
        
        _scoreLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(_gameInfo->scoreGame));
        _scoreTitle->setString(TTLanguageManager::getInstance()->getTag("SCORE"));
        cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","happyBirdParty");
        _btnContinueLabel->setString(TTLanguageManager::getInstance()->getTag(CONTINUE_TAG));
        cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","btn_continue");
        _btnContinueLabel->setVisible(true);
        
        for(int i = 0; i < _stars.size(); i++)
        {
            _stars[i][0]->setVisible(false);
            _stars[i][1]->setVisible(true);
        }
        
        for(int i = 0; i < _gameInfo->scoreStars.size(); i++)
        {
            if(_gameInfo->scoreGame >= _gameInfo->scoreStars[i])
            {
                //Iluminamos estrella
                DelayTime *delayStar = DelayTime::create(STAR_DELAY*(i+1));
                CallFunc *playStar = CallFunc::create(std::bind([&](int star){
                    // Animacion
                    _stars[star][0]->setVisible(true);
                    char starAnimStr[10] = {0};
                    sprintf(starAnimStr,"star%d_win",star+1);
                    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson",starAnimStr);
                    // Particulas
                    _starsParticles[star]->resetSystem();
                    // Sonido
                    TTAudioManager::getInstance()->playAudio("WIN_STAR");
                },i));
                _uiLayer->getParent()->runAction(Sequence::create(delayStar,playStar,NULL));
            }
        }
        
        CallFunc *playEffects = CallFunc::create(std::bind([&](){
            TTAudioManager::getInstance()->playAudio("LEVEL_WIN");
        }));
        _uiLayer->getParent()->runAction(Sequence::create(_delayEnterMenu,playEffects, NULL));
    }
    else
    {
        //Background
        if(_bgUI->getParent() != nullptr)
            _bgUI->removeFromParentAndCleanup(false);
        _parent->addChild(_bgUI,20);
        _bgUI->setVisible(true);
        
        _resumeParent->setVisible(true);
        _loseResumeParent->setVisible(true);
        _winResumeParent->setVisible(false);
        _loseTitleLabel->setString(TTLanguageManager::getInstance()->getTag(LOSE_TITLE_TAG));
        _btnRetryLabel->setString(TTLanguageManager::getInstance()->getTag(RETRY_TAG));
        cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","btn_retry");
        _btnRetry->setVisible(true);
        
        // Rellenamos los objectivos
        for(int i = 0; i < _gameInfo->goals.size(); i++)
        {
            Sprite* _bgIcon;
            Sprite* _bgIconTick;
            cocos2d::ui::TextBMFont* _objectiveLabel;
            int _value = _gameInfo->goals[i]->_goalValue-_gameInfo->goals[i]->_actualValue;
            if (_value<0)
            {
                _value = 0 ;
            }
            _objectiveLabel = cocos2d::ui::TextBMFont::create(std::to_string(_value),"chalkboard.fnt");
            
            Sprite* _icon;
            switch(_gameInfo->goals[i]->_typeGoal)
            {
                case SCORE_GOAL:
                {
                    _icon = Sprite::createWithSpriteFrameName("objetivo_puntos.png");
                    
                    float pointsWined = _gameInfo->goals[i]->_actualValue;
                    float pointsToGoal = _gameInfo->goals[i]->_goalValue;
                    int _starsToWinGoal = 0;
                    int _starsWined = 0;
                    for(int i = 0; i < _gameInfo->scoreStars.size(); i++)
                    {
                        if(pointsWined >= _gameInfo->scoreStars[i]) _starsWined++;
                        if(pointsToGoal >= _gameInfo->scoreStars[i]) _starsToWinGoal++;
                    }
                    
                    int _stars = _starsToWinGoal-_starsWined;
                    if(_stars < 0)
                    {
                        _stars = 0;
                    }
                    _objectiveLabel = cocos2d::ui::TextBMFont::create(std::to_string(_stars),"chalkboard.fnt");
                }
                break;
                case BREAKS_GOAL:
                {
                    _icon = Sprite::createWithSpriteFrameName("objetivo_barro.png");
                }
                break;
                case DOWN_OBJECT_GOAL:
                {
                    int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                    char objectStr[100] = {0};
                    sprintf(objectStr,"objetivo_bajable_world%d.png",_actualWorld+1);
                    _icon = Sprite::createWithSpriteFrameName(objectStr);
                }
                break;
                case COLLECT_FIGURES_GOAL:
                {
                    int _figureToCollect = (static_cast<TTCollectFiguresGoal*>(_gameInfo->goals[i]))->_figureToCollect;
                    char idFigureStr[100] = {0};
                    int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                    sprintf(idFigureStr,"objetivo_figura%d_world%d.png",_figureToCollect+1,_actualWorld+1);
                    _icon = Sprite::createWithSpriteFrameName(idFigureStr);
                }
                break;
                case BLOCK_GOAL:
                {
                    _icon = Sprite::createWithSpriteFrameName("objetivo_madera.png");
                }
                break;
                case STONE_GOAL:
                {
                    _icon = Sprite::createWithSpriteFrameName("objetivo_piedra.png");
                }
                break;
                case NETWORK_GOAL:
                {
                    _icon = Sprite::createWithSpriteFrameName("objetivo_red.png");
                }
                break;
                default:
                break;
            }
            
            if(_gameInfo->goals[i]->isAchieved())
            {
                _bgIcon = Sprite::createWithSpriteFrameName("objetivo_verde.png");
                _bgIconTick = Sprite::createWithSpriteFrameName("boton_objectives_verde.png");
                _objectiveLabel->setColor(cocos2d::Color3B::GREEN);
            }
            else
            {
                _bgIcon = Sprite::createWithSpriteFrameName("objetivo_rojo.png");
                _bgIconTick = Sprite::createWithSpriteFrameName("boton_objectives_rojo.png");
                _objectiveLabel->setColor(cocos2d::Color3B::RED);
            }
            
            //Posicionado
            float _xSpace = _objectivesIconsLose->getBoundingBox().size.width/MAX_GOALS;
            float _offsetIcons = i*_xSpace;
            float _goalXPosition = _xSpace/2;
            float _goalYPosition = _objectivesIconsLose->getBoundingBox().size.height/2;
            float _globalOffset = _xSpace*2 - ((_xSpace/2)*(_gameInfo->goals.size()-1));
            _objectivesIconsLose->addChild(_bgIcon);
            _bgIcon->setPosition(Point(_goalXPosition+_offsetIcons+_globalOffset,_goalYPosition));
            _bgIcon->addChild(_bgIconTick,2);
            float _posXTick = _bgIcon->getBoundingBox().size.width;
            float _posYTick = _bgIcon->getBoundingBox().size.height-(_bgIconTick->getBoundingBox().size.height/2);
            _bgIconTick->setPosition(Point(_posXTick,_posYTick));
            
            _objectiveLabel->setScale(0.6f*_bgIcon->getBoundingBox().size.width/100);
            _objectiveLabel->setPosition(Point(_bgIcon->getBoundingBox().size.width/2,_bgIcon->getBoundingBox().size.height/4.75));
            _icon->setPosition(Point(_bgIcon->getBoundingBox().size.width/2,_bgIcon->getBoundingBox().size.height-(_bgIcon->getBoundingBox().size.height/3)));
            _icon->setScale(_bgIcon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
            _bgIcon->addChild(_icon,1);
            _bgIcon->setScale((_xSpace*0.75)/_bgIcon->getContentSize().width);
            _bgIcon->addChild(_objectiveLabel,1);
            
        }
        
        //Botón de continuar jugando
        _iconMovesBuy->setVisible(false);
        _iconTimeBuy->setVisible(false);
        if(_gameInfo->timeRestriction != NULL)
        {
            _iconTimeBuy->setVisible(true);
        }
        else
        {
            _iconMovesBuy->setVisible(true);
        }
        _buyLabel1->setString(TTLanguageManager::getInstance()->getTag(CONTINUE_PLAYING_1));
        _buyLabel2->setString(TTLanguageManager::getInstance()->getTag(CONTINUE_PLAYING_2));
        std::string _continueIdPrice = "CONTINUE_PLAYING";
        int priceContinue = TTStoreManager::getInstance()->getVirtualItemPrice(_continueIdPrice);
        _buyPrice->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(priceContinue));
        
        // Mostramos los créditos del usuario
        TTUIMap::getInstance()->showUserCredits(_parent,60);
        
        // Efectos del menú
        CallFunc *playEffects = CallFunc::create(std::bind([&](){
            TTAudioManager::getInstance()->playAudio("LEVEL_LOSE");
        }));
        _uiLayer->getParent()->runAction(Sequence::create(_delayEnterMenu,playEffects, NULL));
    }
    
    // Facebook
    showFacebookInfo();
}

void TTGameMenus::showExitMenu(Node* _parent, bool exitGame)
{
    TTUIStackManager::getInstance()->goToNewUI(UI_GAME_MENU_EXIT);
    reset();
    _blockMenus = false;
    _exitGame = exitGame;
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,20);
    _bgUI->setVisible(true);
    
    if(_uiLayer->getParent() != nullptr)
        _uiLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiLayer,21);
    _uiLayer->setVisible(true);
    
    _widgetResume->setVisible(true);
    _exitParent->setVisible(true);
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_GAME_MENUS.ExportJson","panel_enter");
    TTAudioManager::getInstance()->playAudio("PANEL_DOWN");
    
    _levelTitleLabel->setString(TTLanguageManager::getInstance()->getTag(EXIT_TITLE_TAG).c_str());
    _levelTitleLabel->setVisible(true);
    
    if(_exitGame)
    {
        _labelExit1->setString(TTLanguageManager::getInstance()->getTag(EXIT_LEVEL_1_TAG).c_str());
        _labelExit2->setString(TTLanguageManager::getInstance()->getTag(SEEYOUSOON));
        _birdExitGame->setVisible(true);
        _birdExitLevel->setVisible(false);
        _heartExitLevel->setVisible(false);
    }
    else
    {
        _labelExit1->setString(TTLanguageManager::getInstance()->getTag(EXIT_LEVEL_1_TAG).c_str());
        _labelExit2->setString(TTLanguageManager::getInstance()->getTag(EXIT_LEVEL_2_TAG).c_str());
        _birdExitGame->setVisible(false);
        _birdExitLevel->setVisible(true);
        _heartExitLevel->setVisible(true);
    }
}

void TTGameMenus::exitMenu()
{
    TTUIStackManager::getInstance()->closeUI();
    _uiLayer->setVisible(false);
    _widgetResume->setVisible(false);
    _resumeParent->setVisible(false);
    _initParent->setVisible(false);
    _exitParent->setVisible(false);
    _bgUI->setVisible(false);
    _objectivesIconsLose->removeAllChildren();
    TTUIMap::getInstance()->hideUserCredits();
}

void TTGameMenus::pauseUI()
{
    _blockMenus = true;
    _bgUI->setVisible(false);
    
    //guardamos el estado de las monedas, para poder hacer despues el restore correctamente
    _coinsInMenu = (TTUIMap::getInstance()->_widgetMapCoins->getParent() == _uiLayer->getParent());
    TTUIMap::getInstance()->hideUserCredits();
}

void TTGameMenus::restoreUI()
{
    //update de los boosters
    showBoosters();
    
    _blockMenus = false;
    _bgUI->setVisible(true);
    if (_coinsInMenu)
    {
        TTUIMap::getInstance()->showUserCredits(_uiLayer->getParent(),60);
    }
}

void TTGameMenus::onClickBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    // Verificamos el estado del Booster
    int _boosterSubType = ((cocos2d::ui::Widget*)pSender)->getTag();
    std::map<BOOSTERS_TYPES,std::vector<int>>::const_iterator _itrBoosters;
    for(_itrBoosters = _currentGameInfo->_boosters.begin(); _itrBoosters != _currentGameInfo->_boosters.end(); ++_itrBoosters)
    {
        if(_itrBoosters->first != BOARD_BOOSTERS)
            continue;
        
        std::vector<int> _boostersBoard = _itrBoosters->second;
        for(int i = 0; i < _boostersBoard.size(); i++)
        {
            if(_boostersBoard[i] == _boosterSubType)
            {
                BOARD_BOOSTERS_TYPE typeBooster = BOARD_BOOSTERS_TYPE(_boostersBoard[i]);
                TTBoardBoosters* _boardBooster = TTBoostersManager::getInstance()->_boardBoostersInstances.find(typeBooster)->second;
                
                // Verifica si hay boosters de este tipo
                std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"0_"+std::to_string(typeBooster));
                int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
                if(_numBoosters == 0)
                {
                    // Venta de boosters
                    TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
                    TTUIPopup::getInstance()->showPopupBooster(_uiLayer->getParent(),BOARD_BOOSTERS,typeBooster,_boardBooster->_boosterIcon);
                }
            }
        }
    }
}

void TTGameMenus::onClickPlayBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_PLAY");
    
    // Consumimos una vida
    if(TTUserLives::getInstance()->hasLive())
    {
        // Hay vidas para jugar
        ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
        DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
        CallFunc *changeToGameState = CallFunc::create(std::bind([&](){
            exitMenu();
            if(_retryMode)
            {
                sendEvent(EVENT_GAME_RETRY);
            }
            else
            {
                TTAnimalCubes::getInstance()->setState(GAME_STATE_INGAME);
            }
        }));
        _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,changeToGameState,NULL));
        action->play();

        TTAudioManager::getInstance()->playAudio("PANEL_UP");
    }
    else
    {
        // No hay vidas para jugar, mostramos menú de compra de vidas
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
        TTUIPopup::getInstance()->showPopupLives(_uiLayer->getParent());
    }
}

void TTGameMenus::onClickContinueBuyBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_BUY_BOOSTER");
    
    std::string _itemBuyId = "CONTINUE_PLAYING";
    if(TTStoreManager::getInstance()->buyVirtualItem(_itemBuyId))
    {
        // Compra realizada con exito
        TTUIMap::getInstance()->updateUIValues();
        
        TTStoreManager::getInstance()->takeUserVirtualItem("CONTINUE_PLAYING");
        TTGame::getInstance()->reactivateGame();
        
        // Actualizamos la parte visual de la UI
        if(TTGame::getInstance()->_gameConfig->moveRestriction != NULL)
        {
            TTGameUIManager::getInstance()->printMovementsLabel(TTGame::getInstance()->_gameConfig->movesGame,TTGame::getInstance()->_gameConfig->moveRestriction->getLimitMoves());
        }
        
        // Enviamos evento de analitica a Parse
        std::map<std::string,std::string> _dimensions;
        TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_CONTINUE_PLAYING,_dimensions);
        
        // Cerramos el menú
        forceExit();
    }
    else
    {
        // No hay monedas suficientes, mostramos la tienda
        TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
        TTUIShop::getInstance()->showShop(_uiLayer->getParent());
    }
}

void TTGameMenus::forceExit()
{
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *exitMenuFunc = CallFunc::create(std::bind([&](){
        exitMenu();
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,exitMenuFunc,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickRetryBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *retryGame = CallFunc::create(std::bind([&](){
        exitMenu();
        
        // Muestra la UI de inicio de nivel
        TTGameMenus::getInstance()->showInitMenu(_currentGameInfo,_uiLayer->getParent(),true);
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,retryGame,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickContinueBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *initGame = CallFunc::create(std::bind([&](){
        exitMenu();
        TTAnimalCubes::getInstance()->setState(GAME_STATE_MAP);
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,initGame,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickInitExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus || _uiLayer->getParent() == nullptr)
    {
        return;
    }
    
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_enter")->stop();
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *exitMenuFunc = CallFunc::create(std::bind([&](){
        exitMenu();
        
        if(_retryMode)
        {
            TTAnimalCubes::getInstance()->setState(GAME_STATE_MAP);
        }
        else
        {
            sendEvent(EVENT_MAP_RESTORE);
        }
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,exitMenuFunc,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickRetryExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus || _uiLayer->getParent() == nullptr)
        return;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    onClickContinueBtn(pSender,eEventType);
}

void TTGameMenus::onClickExitLevelDoneBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus || _uiLayer->getParent() == nullptr)
        return;
    _blockMenus = true;
    
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *initGame = CallFunc::create(std::bind([&](){
        exitMenu();
        
        if(!_exitGame)
        {
            sendEvent(EVENT_GAME_CONTINUE);
            TTAnimalCubes::getInstance()->setState(GAME_STATE_MAP);
        }
        else
        {
            TTNotificationManager::getInstance()->show();
            Director::getInstance()->end();
            exit(0);
        }
        
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,initGame,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickExitLevelCancelBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    _blockMenus = true;
    
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_GAME_MENUS.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *initGame = CallFunc::create(std::bind([&](){
        exitMenu();
        if(!_exitGame)
        {
            sendEvent(EVENT_GAME_CONTINUE);
        }
    }));
    _uiLayer->getParent()->runAction(Sequence::create(delayExitMenu,initGame,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTGameMenus::onClickConnectFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    TTFacebook::getInstance()->connectUserToFacebook();
}

void TTGameMenus::onClickInviteFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    TTFacebook::getInstance()->showInviteFirendsDialog();
}

void TTGameMenus::onEvent_FacebookConnect(EventCustom* e)
{
    if(TTFacebook::getInstance()->isConnected())
    {
        
        if(_initParent->isVisible())
        {
            _blockMenus = false;
            onClickInitExitBtn(nullptr, ui::Widget::TouchEventType::ENDED);
        }
        
        if(_resumeParent->isVisible())
        {
            _blockMenus = false;
            onClickRetryExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
        }
        
        if(_rankingFacebookNoConnect->isVisible())
        {
            _rankingFacebookNoConnect->setVisible(false);
            showFacebookInfo();
        }
    }
}

void TTGameMenus::onEvent_FacebookRanking(EventCustom* e)
{
    if(TTFacebook::getInstance()->isConnected())
    {
        // Recive el userData del evento
        facebookRankingInfo *rankingInfo = static_cast<facebookRankingInfo*>(e->getUserData());
        bool _hasConnection = rankingInfo->_connection;
        if(_hasConnection)
        {
            // Mostramos el ranking
            CCLOG("Datos recividos de Parse para mostrar el ranking!");
            
            if(rankingInfo->_rankingLevelInfo.size() > 0)
            {
                ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking"))->setVisible(true);
                ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting"))->setVisible(false);
                _rankingFacebookConnect->setVisible(true);
                _rankingFacebookInvite->setVisible(false);

                
                std::vector<facebookFriendScoreInfo> _rankingLevelsInfo = rankingInfo->_rankingLevelInfo;
                for(int i = 0; i < _rankingLevelsInfo.size() && i < 3 ; i++)
                {
                    std::string _rankingId = "ranking_"+std::to_string(i+1);
                    ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("userInfo"))->setVisible(true);
                    ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("inviteBtn"))->setVisible(false);
                    
                    std::string _userFacebookId = _rankingLevelsInfo[i]._name;
                    int _userScore = _rankingLevelsInfo[i]._score;
                    
                    ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId))->setVisible(true);
                    Sprite *_userPhoto = Sprite::createWithTexture(TTFacebook::getInstance()->getTexturePhotoUser(_userFacebookId));
                    _userPhoto->setAnchorPoint(Point(0,0));
                    cocos2d::ui::Widget* _photoUserParent = ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("userInfo")->getChildByName("photoUser"));
                    _userPhoto->setScale(_photoUserParent->getContentSize().height/_userPhoto->getContentSize().height);
                    _photoUserParent->addChild(_userPhoto);
                    
                    std::string _userName = TTFacebook::getInstance()->getNameByFacebookId(_userFacebookId);
                    std::string _userNameCut = _userName.substr(0,12);
                    ((cocos2d::ui::TextBMFont*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("userInfo")->getChildByName("nameUser"))->setString(_userNameCut);
                    ((cocos2d::ui::TextBMFont*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("userInfo")->getChildByName("levelUser"))->setString(std::to_string(_userScore));
                }
                
                for(int i = _rankingLevelsInfo.size(); i < 3; i++)
                {
                    std::string _rankingId = "ranking_"+std::to_string(i+1);
                    ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("userInfo"))->setVisible(false);
                    ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("ranking")->getChildByName(_rankingId)->getChildByName("inviteBtn"))->setVisible(true);
                }
            }
            else
            {
                // No hay usuario para el ranking, ocultamos el ranking
                _rankingFacebookConnect->setVisible(false);
                _rankingFacebookInvite->setVisible(true);
                
                _inviteLabel->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_INVITE_LABEL"));
                _btnInviteLabel->setString(TTLanguageManager::getInstance()->getTag("INVITAR"));
            }
        }
        else
        {
            // Mostramos el fallo de conexión con WIFI
            cocos2d::ui::Widget* _tickFail = ((cocos2d::ui::Widget*)_rankingFacebookConnect->getChildByName("connecting")->getChildByName("Wifi_BackGround")->getChildByName("tick_fail"));
            _tickFail->setVisible(true);
            auto _scaleUp = ScaleTo::create(0.1f,_tickFail->getScale()+0.1f);
            auto _scaleDown = ScaleTo::create(0.1f,_tickFail->getScale());
            _tickFail->runAction(Sequence::create(_scaleUp,_scaleDown,NULL));
        }
    }
}
