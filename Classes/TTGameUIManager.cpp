#include "TTGameUIManager.h"
#include "TTLanguageManager.h"
#include "TTUIShop.h"
#include "TTUISettings.h"
#include "TTUserInfo.h"
#include "TTUIPopup.h"
#include "TTUIStackManager.h"
#include "TTStoreManager.h"
#include "TTAudioManager.h"
#include "TTGame.h"

#define NUM_BOOSTERS                    5
#define NUM_OBJECTIVES                  5

//Singleton
static TTGameUIManager *_sharedGameUIManager = nullptr;

TTGameUIManager* TTGameUIManager::getInstance()
{
    if (!_sharedGameUIManager)
    {
        _sharedGameUIManager = new TTGameUIManager();
        _sharedGameUIManager->init();
    }
    
    return _sharedGameUIManager;
}

void TTGameUIManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedGameUIManager);
}

TTGameUIManager::~TTGameUIManager()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    
    CCLOG("Se destruye la UI de juego");
    
    // Eliminamos la layer contenedora
    _uiUpLayer->removeAllChildrenWithCleanup(true);
    _uiUpLayer->removeFromParentAndCleanup(true);
    _uiDownLayer->removeAllChildrenWithCleanup(true);
    _uiDownLayer->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_UP0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_UP0.png");
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_DOWN0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_DOWN0.png");
}

void TTGameUIManager::init()
{
    _blockMenus = false;
    
    // Cargamos el XML de la UI Superior
    _uiUpLayer = Layer::create();
    _uiUpLayer->retain();
    _widgetUp = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_UP.ExportJson");
    cocos2d::ui::Layout* _layoutUp = dynamic_cast<cocos2d::ui::Layout*>(_widgetUp);
    _uiUpLayer->addChild(_layoutUp);
    
    // Escalado Widget UP
    _widgetUp->setScale(scale_widget(_widgetUp,WIDTH_UI_UP_SPACE,HEIGHT_UI_UP_SPACE));
    
    // Seteamos posición
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetUp->setAnchorPoint(Vec2(0.5,0.5));
    _uiUpLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_widgetUp->getBoundingBox().size.height/2)));
    _uiUpLayer->setVisible(false);
    
    // Cargamos el XML de la UI Inferior
    _uiDownLayer = Layer::create();
    _uiDownLayer->retain();
    _widgetDown = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_DOWN.ExportJson");
    cocos2d::ui::Layout* _layoutDown = dynamic_cast<cocos2d::ui::Layout*>(_widgetDown);
    _uiDownLayer->addChild(_layoutDown);
    
    // Escalado Widget DOWN
    _widgetDown->setScale(scale_widget(_widgetDown,WIDTH_UI_DOWN_SPACE,HEIGHT_UI_DOWN_SPACE));

    // Seteamos posición
    _widgetDown->setAnchorPoint(Vec2(0.5,0.5));
    _uiDownLayer->setPosition(Vec2(_screenSize.width/2,(_widgetDown->getBoundingBox().size.height/2)));
    _uiDownLayer->setVisible(false);
    
    _UISpace.x = _widgetUp->getBoundingBox().size.height;
    _UISpace.y = _widgetDown->getBoundingBox().size.height;
    
    // Botones
    _btnMoreRestrictions = (cocos2d::ui::Button*)_widgetUp->getChildByName("restrictions");
    _btnMoreRestrictions->addTouchEventListener(CC_CALLBACK_2(TTGameUIManager::onClickMoreRestrictions, this));
    _btnPause = (cocos2d::ui::Button*)_widgetDown->getChildByName("pause_Btn");
    _btnPause->addTouchEventListener(CC_CALLBACK_2(TTGameUIManager::onClickPause, this));
    
    // Seteamos las referencias a todos los nodos que usaremos
    _restrictionLabel = (cocos2d::ui::TextBMFont*)_widgetUp->getChildByName("restrictions")->getChildByName("labelRestriction");
    _scoreBar = (cocos2d::ui::LoadingBar*)_widgetUp->getChildByName("score")->getChildByName("bar");
    _scoreLabel = (cocos2d::ui::TextBMFont*)_widgetUp->getChildByName("score")->getChildByName("score");
    for(int i = 0; i < 3; i++)
    {
        std::vector<Sprite*> stars;
        char idStarStr[10] = {0};
        sprintf(idStarStr,"star%d",i+1);
        stars.push_back((Sprite*)_widgetUp->getChildByName("score")->getChildByName("starsParent")->getChildByName(idStarStr)->getChildByName("star_off"));
        stars.push_back((Sprite*)_widgetUp->getChildByName("score")->getChildByName("starsParent")->getChildByName(idStarStr)->getChildByName("star_on"));
        _starsSprites.push_back(stars);
    }
    
    //Seteamos los objectivos
    for(int i = 0; i < NUM_OBJECTIVES;i++)
    {
        goalUI _newGoal;
        char strGoal[100] = {0};
        sprintf(strGoal,"objective%d",i);
        cocos2d::ui::Widget* _goal = (cocos2d::ui::Widget*)_widgetUp->getChildByName("objectives")->getChildByName(strGoal);
        _newGoal._obj_off = (cocos2d::ui::Widget*)_goal->getChildByName("obj_off");
        _newGoal._obj_on = (cocos2d::ui::Widget*)_goal->getChildByName("obj_on");
        _newGoal._obj_ok = (cocos2d::ui::Widget*)_goal->getChildByName("obj_ok");
        _newGoal._obj_icon = (Sprite*)_goal->getChildByName("obj_icon");
        _newGoal._tick_ok = (cocos2d::ui::Widget*)_goal->getChildByName("tick_ok");
        _newGoal._label_obj = (cocos2d::ui::TextBMFont*)_goal->getChildByName("obj_label");
        _newGoal._showDecrement = new TTUIShowDecrement(_newGoal._label_obj);
        _goals.push_back(_newGoal);
    }
    
    
    //Seteamos los boosters
    _boostersParent = (cocos2d::ui::Widget*)_widgetDown->getChildByName("boosters");
    for(int i = 0; i < NUM_BOOSTERS; i++)
    {
        boosterUI _newBoosterUI;
        char strBooster[100] = {0};
        sprintf(strBooster,"booster%d",i);
        cocos2d::ui::Widget* _booster = (cocos2d::ui::Widget*)_widgetDown->getChildByName("boosters")->getChildByName(strBooster);
        _newBoosterUI._boosterOff = (cocos2d::ui::Widget*)_booster->getChildByName("booster_off");
        _newBoosterUI._boosterOnParent = (cocos2d::ui::Widget*)_booster->getChildByName("booster_on");
        _newBoosterUI._boosterMore = (cocos2d::ui::Widget*)_booster->getChildByName("booster_on")->getChildByName("moreBooster");
        _newBoosterUI._labelNumBoosters = (cocos2d::ui::TextBMFont*)_booster->getChildByName("booster_on")->getChildByName("infoBooster")->getChildByName("num_label");
        _newBoosterUI._boosterInfo = (cocos2d::ui::TextBMFont*)_booster->getChildByName("booster_on")->getChildByName("infoBooster");
        
        _newBoosterUI._boosterOnParent->reorderChild(_newBoosterUI._boosterMore,3);
        _newBoosterUI._boosterOnParent->reorderChild(_newBoosterUI._boosterInfo,3);
        
        _boosters.push_back(_newBoosterUI);
    }
    
    _boosterApplyUI = (cocos2d::ui::Widget*)_widgetDown->getChildByName("boosterApplyMenu");
    _btnApply = (cocos2d::ui::Button*)_widgetDown->getChildByName("boosterApplyMenu")->getChildByName("applyBtn");
    _btnApply->addTouchEventListener(CC_CALLBACK_2(TTGameUIManager::onClickDoneApplyBoosterBtn, this));
    _btnCancel = (cocos2d::ui::Button*)_widgetDown->getChildByName("boosterApplyMenu")->getChildByName("cancelBtn");
    _btnCancel->addTouchEventListener(CC_CALLBACK_2(TTGameUIManager::onClickCancelApplyBoosterBtn, this));
    _boosterApplyIcon = (cocos2d::ui::Widget*)_widgetDown->getChildByName("boosterApplyMenu")->getChildByName("selectedBooster");
    _boosterApplyUI->setVisible(false);
    
    // Resgistramos eventos
    _classListeners.push_back(registerListenerEvent(EVENT_LANGUAGE_CHANGE,CC_CALLBACK_1(TTGameUIManager::onEvent_languageChange,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_CONTINUE,CC_CALLBACK_1(TTGameUIManager::onEvent_continueGame,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_COLLECT_FIGURE_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_collectFigures,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_BLOCK_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_goalUpdate,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_OBJECT_DOWN_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_goalUpdate,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_BREAKS_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_goalUpdate,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_STONE_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_goalUpdate,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_NETWORK_UI,CC_CALLBACK_1(TTGameUIManager::onEvent_goalUpdate,this)));
    //_classListeners.push_back(registerListenerEvent(EVENT_GAME_PAUSE,CC_CALLBACK_1(TTGameUIManager::onEvent_gamePause,this)));
}

void TTGameUIManager::pauseUI()
{
    _blockMenus = true;
    sendEvent(EVENT_GAME_PAUSE);
}

void TTGameUIManager::restoreUI()
{
    _blockMenus = false;
    
    // Reactivamos tablero en caso necesareo
    if(!_boosterApplyUI->isVisible())
    {
        setBoosters();
        sendEvent(EVENT_GAME_CONTINUE);
    }
}

void TTGameUIManager::setTouchEnabledUI(bool _isEnabled)
{
    _btnPause->setTouchEnabled(_isEnabled);
    _btnMoreRestrictions->setTouchEnabled(_isEnabled);
    
    for(int i = 0; i < _boosters.size(); i++)
    {
        _boosters[i]._boosterOnParent->setTouchEnabled(_isEnabled);
    }
}

void TTGameUIManager::resetPrinter()
{
    for(int i = 0; i < _goals.size(); i++)
    {
        _goals[i]._obj_off->setVisible(true);
        _goals[i]._obj_on->setVisible(false);
        _goals[i]._obj_ok->setVisible(false);
        _goals[i]._obj_icon->setVisible(false);
        _goals[i]._obj_icon->removeAllChildrenWithCleanup(true);
        _goals[i]._tick_ok->setVisible(false);
        
        cocos2d::Color3B _colorWite = cocos2d::Color3B(255,255,255);
        _goals[i]._label_obj->setColor(_colorWite);
        _goals[i]._label_obj->setVisible(false);
        _goals[i]._showDecrement->reset();
    }
}

void TTGameUIManager::showPrinter(gameInfo* gameInfo, Node* _parent)
{
    TTUIStackManager::getInstance()->goToNewUI(UI_INGAME);
    
    resetPrinter();
    _gameInfo = gameInfo;
    _actualParent = _parent;
    
    // Seteamos el padre de la UI
    if(_uiUpLayer->getParent() != nullptr)
        _uiUpLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiUpLayer);
    if(_uiDownLayer->getParent() != nullptr)
        _uiDownLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiDownLayer);
    _uiUpLayer->setVisible(true);
    _uiDownLayer->setVisible(true);
    
    // Seteamos los movimientos iniciales, en caso de que los haya
    if(_gameInfo->moveRestriction != NULL)
    {
        char strMovements[10] = {0};
        sprintf(strMovements,"%d",_gameInfo->moveRestriction->getLimitMoves());
        _restrictionLabel->setString(strMovements);
    }
    
    // Seteamos el tiempo, en caso de que lo haya
    if(_gameInfo->timeRestriction != NULL)
    {
        int timeInit = _gameInfo->timeRestriction->_limitTime;
        _restrictionLabel->setString(secondsToStringTime(timeInit));
    }
    
    // Seteamos el score y posición de las barras
    int _sizeBar = ((Node*)_widgetUp->getChildByName("score")->getChildByName("starsParent"))->getBoundingBox().size.width;
    _scoreBar->setVisible(true);
    _scoreBar->setPercent(0);
    _scoreLabel->setString("0");
    for(int i = 0; i < _starsSprites.size(); i++)
    {
        _starsSprites[i][0]->setVisible(true);
        _starsSprites[i][1]->setVisible(false);
        
        if(i == _starsSprites.size()-1)
            break;
        
        char idStarStr[10] = {0};
        sprintf(idStarStr,"star%d",i+1);
        Node* star = (Node*)_widgetUp->getChildByName("score")->getChildByName("starsParent")->getChildByName(idStarStr);
        float _posXStar = ((gameInfo->scoreStars[i]*_sizeBar)/gameInfo->scoreStars[_starsSprites.size()-1])-star->getBoundingBox().size.width/2;
        star->setPosition(Vec2(_posXStar,star->getPosition().y));
    }
    
    // Seteamos los goal
    if(_gameInfo->goals.size() > NUM_OBJECTIVES)
    {
        CCLOG("Hay más objectivos definidos de los que se pueden mostrar en la UI");
        abort();
    }
    
    for(int i = 0; i < _gameInfo->goals.size(); i++)
    {
        _goals[i]._obj_off->setVisible(false);
        _goals[i]._obj_on->setVisible(true);
        _goals[i]._label_obj->setVisible(true);
        
        switch(_gameInfo->goals[i]->_typeGoal)
        {
            case SCORE_GOAL:
            {
                Sprite* _icon = Sprite::createWithSpriteFrameName("objetivo_puntos.png");
                _icon->setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                std::vector<int> stars = _gameInfo->scoreStars;
                int numStars = 0;
                for(int j = 0; j < stars.size(); j++)
                {
                    if(_gameInfo->goals[i]->_goalValue >= stars[j])
                    {
                        numStars++;
                    }
                }
                _goals[i]._label_obj->setString(std::to_string(numStars));
            }
            break;
            case BREAKS_GOAL:
            {
                Sprite* _icon = Sprite::createWithSpriteFrameName("objetivo_barro.png");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));
            }
            break;
            case DOWN_OBJECT_GOAL:
            {
                int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                char objectStr[100] = {0};
                sprintf(objectStr,"objetivo_bajable_world%d.png",_actualWorld+1);
                Sprite* _icon = Sprite::createWithSpriteFrameName(objectStr);
                _icon->setName("iconGoal");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));
            }
            break;
            case COLLECT_FIGURES_GOAL:
            {
                int _figureToCollect = (static_cast<TTCollectFiguresGoal*>(_gameInfo->goals[i]))->_figureToCollect;
                char idFigureStr[100] = {0};
                int _actualWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
                sprintf(idFigureStr,"objetivo_figura%d_world%d.png",_figureToCollect+1,_actualWorld+1);
                Sprite* _icon = Sprite::createWithSpriteFrameName(idFigureStr);
                _icon->setName("iconGoal");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));
            }
            break;
            case BLOCK_GOAL:
            {
                Sprite* _icon = Sprite::createWithSpriteFrameName("objetivo_madera.png");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));
            }
            break;
            case STONE_GOAL:
            {
                Sprite* _icon = Sprite::createWithSpriteFrameName("objetivo_piedra.png");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));

            }
            break;
            case NETWORK_GOAL:
            {
                Sprite* _icon = Sprite::createWithSpriteFrameName("objetivo_red.png");
                _icon->cocos2d::Node::setAnchorPoint(Point(0,0));
                _icon->setScale(_goals[i]._obj_icon->getBoundingBox().size.width/_icon->getBoundingBox().size.width);
                _goals[i]._obj_icon->addChild(_icon);
                _goals[i]._obj_icon->setVisible(true);
                
                _goals[i]._label_obj->setString(std::to_string(_gameInfo->goals[i]->_goalValue));
            }
            break;
            default:
            break;
        }
    }
    
    //Seteamos los booster
    setBoosters();
    
    //Desactivamos la UI de uso de booster
    _btnPause->setVisible(true);
    _boostersParent->setVisible(true);
    _boosterApplyUI->setVisible(false);
}

void TTGameUIManager::setBoosters()
{
    //Reseteamos los boosters
    TTBoostersManager::getInstance()->resetBoosters();
    
    for(int i = 0; i < _boosters.size(); i++)
    {
        if(_boosters[i]._boosterOnParent->getChildByName("icon") != nullptr)
            _boosters[i]._boosterOnParent->getChildByName("icon")->removeFromParentAndCleanup(true);
    }
    
    int _activeBoosters = 0;
    std::map<BOOSTERS_TYPES,std::vector<int>>::const_iterator _itrBoosters;
    for(_itrBoosters = _gameInfo->_boosters.begin(); _itrBoosters != _gameInfo->_boosters.end(); ++_itrBoosters)
    {
        if(_itrBoosters->first != GAME_BOOSTERS)
            continue;
        
        std::vector<int> _boostersGame = _itrBoosters->second;
        for(int i = 0; i < _boostersGame.size(); i++)
        {
            GAME_BOOSTERS_TYPE typeBooster = GAME_BOOSTERS_TYPE(_boostersGame[i]);
            TTGameBoosters* _gameBooster = TTBoostersManager::getInstance()->_gameBoostersInstances.find(typeBooster)->second;
            
            Sprite* _icon = Sprite::createWithSpriteFrameName(_gameBooster->_boosterIcon);
            _icon->setName("icon");
            _icon->setPosition(Point(_boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.width/2,_boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.height/2));
            _icon->setScale((_boosters[_activeBoosters]._boosterOnParent->getBoundingBox().size.width/_icon->getBoundingBox().size.width)*1.3f);
            _boosters[_activeBoosters]._boosterOnParent->addChild(_icon,0);
            
            _boosters[_activeBoosters]._boosterOnParent->setTag(_gameBooster->_boosterSubType);
            _boosters[_activeBoosters]._boosterOnParent->addTouchEventListener(CC_CALLBACK_2(TTGameUIManager::onClickBoosterBtn, this));
            
            std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"1_"+std::to_string(typeBooster));
            int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
            if(_numBoosters == 0)
            {
                _boosters[_activeBoosters]._boosterMore->setVisible(true);
                _boosters[_activeBoosters]._boosterInfo->setVisible(false);
            }
            else
            {
                _boosters[_activeBoosters]._boosterMore->setVisible(false);
                _boosters[_activeBoosters]._boosterInfo->setVisible(true);
                _boosters[_activeBoosters]._labelNumBoosters->setString(std::to_string(_numBoosters));
            }
            
            _boosters[_activeBoosters]._boosterOnParent->setVisible(true);
            _boosters[_activeBoosters]._boosterOff->setVisible(false);
            
            _activeBoosters++;
        }
    }
    
    for(int i = _activeBoosters; i < NUM_BOOSTERS; i++)
    {
        _boosters[i]._boosterOnParent->setVisible(false);
        _boosters[i]._boosterOff->setVisible(true);
    }
}

void TTGameUIManager::finishPrinter()
{
    _uiUpLayer->setVisible(false);
    _uiDownLayer->setVisible(false);
    
    TTUIStackManager::getInstance()->closeUI();
}

void TTGameUIManager::printTimeLabel(int remainTime)
{
    _restrictionLabel->setString(secondsToStringTime(remainTime));
    if (remainTime!=_remainTime && remainTime<10)
    {
        auto _scaleOut = ScaleBy::create(0.1f, 1.5);
        auto _scaleIn = ScaleBy::create(0.3f, 1/1.5);
        _restrictionLabel->runAction(Sequence::create(_scaleOut,_scaleIn, NULL));
    }
    _remainTime=remainTime;
}

void TTGameUIManager::printScoreLabel(int score)
{
    _scoreLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(score));
    
    std::vector<int> stars = _gameInfo->scoreStars;
    float percentScore = (score*100)/stars[stars.size()-1];
    if(percentScore > 100)
        _scoreBar->setPercent(100);
    else
        _scoreBar->setPercent(percentScore);
    
    // Gestión de iluminación de estrellas
    int _starsWin = 0;
    for(int i = 0; i < stars.size(); i++)
    {
        if(score >= stars[i])
        {
            if(_starsSprites[i][1]->isVisible())
                continue;
            
            _starsWin++;
            _starsSprites[i][0]->setVisible(false);
            _starsSprites[i][1]->setVisible(true);
        }
    }
    
    //Actualiza el goal si debe hacerlo
    if(_starsWin == 0)
        return;
    
    for(int i = 0; i < _gameInfo->goals.size(); i++)
    {
        if(_gameInfo->goals[i]->_typeGoal == SCORE_GOAL)
        {
            int _actualGoalStars = std::stoi(_goals[i]._label_obj->getString());
            
            if(_actualGoalStars <= _starsWin)
            {
                // Poner el goal a finalizado
                showGoalComplete(_goals[i]);
                _goals[i]._showDecrement->addNewDecrement(0);
            }
            else
            {
                // Animación de decremento de goal
                showGoalStep(_goals[i]);
                _goals[i]._showDecrement->addNewDecrement(_actualGoalStars-_starsWin);
            }
        }
    }
}

void TTGameUIManager::showGoalStep(goalUI _goal)
{
    auto _scaleUp = ScaleTo::create(0.2f,_goal._obj_on->getScale()+0.2f);
    auto _scaleDown = ScaleTo::create(0.2f,_goal._obj_on->getScale());
    _goal._obj_on->runAction(Sequence::create(_scaleUp,_scaleDown, NULL));
}

void TTGameUIManager::showGoalComplete(goalUI _goal)
{
    cocos2d::Color3B _colorGreen = cocos2d::Color3B(156,255,0);
    _goal._label_obj->setColor(_colorGreen);
    _goal._obj_ok->setVisible(true);
    
    _goal._tick_ok->setVisible(true);
    auto _scaleTickUp = ScaleTo::create(0.2f,_goal._tick_ok->getScale()+0.5f);
    auto _scaleTickDown = ScaleTo::create(0.2f,_goal._tick_ok->getScale());
    _goal._tick_ok->runAction(Sequence::create(_scaleTickUp,_scaleTickDown, NULL));
    
    auto _scaleUp = ScaleTo::create(0.2f,_goal._obj_on->getScale()+0.2f);
    auto _scaleDown = ScaleTo::create(0.2f,_goal._obj_on->getScale());
    _goal._obj_ok->runAction(Sequence::create(_scaleUp,_scaleDown, NULL));
    
    // Sonido de objetivo conseguido
    TTAudioManager::getInstance()->playAudio("GOAL_WIN");
}

void TTGameUIManager::printMovementsLabel(int actualMovements, int totalMovements)
{
    char strMovements[10] = {0};
    sprintf(strMovements,"%d",totalMovements-actualMovements);
    _restrictionLabel->setString(strMovements);
}

void TTGameUIManager::onClickMoreRestrictions(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (_blockMenus)
        return;
    
    if(TTBoard::getInstance()->_isUpdatingBoard || TTBoard::getInstance()->_stepsManager->isRunningSteps() || TTGame::getInstance()->_paused || TTGame::getInstance()->isFinished())
        return;
    
    // Envia evento de pausa y bloquea los menus
    sendEvent(EVENT_GAME_PAUSE);
    _blockMenus = true;
    
    //Mostramos el popup de la reestricción actual
    if(_gameInfo->moveRestriction != NULL)
    {
        std::string _boosterIcon = TTBoostersManager::getInstance()->_boardBoostersInstances.find(BOARD_BOOSTER_MORE_MOVEMENTS_5)->second->_boosterIcon;
        TTUIPopup::getInstance()->showPopupBooster(_actualParent,0,0,_boosterIcon);
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
    }
    else if(_gameInfo->timeRestriction != NULL)
    {
        std::string _boosterIcon = TTBoostersManager::getInstance()->_boardBoostersInstances.find(BOARD_BOOSTER_MORE_TIME_15)->second->_boosterIcon;
        TTUIPopup::getInstance()->showPopupBooster(_actualParent,0,1,_boosterIcon);
        TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
    }
    else
    {
        CCLOG("Se intenta comprar una reestricción que no existe");
        TTUIStackManager::getInstance()->goToNewUI(UI_SHOP);
        TTUIShop::getInstance()->showShop(_actualParent);
    }
}

void TTGameUIManager::onClickPause(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (_blockMenus)
        return;
    
    if(TTBoard::getInstance()->_isUpdatingBoard || TTBoard::getInstance()->_stepsManager->isRunningSteps() || TTGame::getInstance()->_paused || TTGame::getInstance()->isFinished())
        return;
    
    // Envia evento de pausa y bloquea los menus
    sendEvent(EVENT_GAME_PAUSE);
    _blockMenus = true;
    
    TTUIStackManager::getInstance()->goToNewUI(UI_SETTINGS);
    TTUISettings::getInstance()->showPause(_actualParent);
}

void TTGameUIManager::onClickCancelApplyBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    //Reseteamos los boosters
    setBoosters();
    
    //Desactivamos la UI de uso de booster
    _btnPause->setVisible(true);
    _boostersParent->setVisible(true);
    _boosterApplyUI->setVisible(false);
    
    TTBoard::getInstance()->_parentHelpBooster->setVisible(false);
    
    sendEvent(EVENT_GAME_CONTINUE);
}

void TTGameUIManager::activateBoosterUI(int _boosterSubType)
{
    std::map<BOOSTERS_TYPES,std::vector<int>>::const_iterator _itrBoosters;
    for(_itrBoosters = _gameInfo->_boosters.begin(); _itrBoosters != _gameInfo->_boosters.end(); ++_itrBoosters)
    {
        if(_itrBoosters->first != GAME_BOOSTERS)
            continue;
        
        std::vector<int> _boostersGame = _itrBoosters->second;
        for(int i = 0; i < _boostersGame.size(); i++)
        {
            if(_boostersGame[i] == _boosterSubType)
            {
                GAME_BOOSTERS_TYPE typeBooster = GAME_BOOSTERS_TYPE(_boostersGame[i]);
                TTGameBoosters* _gameBooster = TTBoostersManager::getInstance()->_gameBoostersInstances.find(typeBooster)->second;
                
                // Verifica si hay boosters de este tipo
                std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"1_"+std::to_string(typeBooster));
                int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
                if(_numBoosters == 0)
                {
                    // Venta de boosters
                    TTUIStackManager::getInstance()->goToNewUI(UI_POPUP);
                    TTUIPopup::getInstance()->showPopupBooster(_uiDownLayer->getParent(),GAME_BOOSTERS,typeBooster,_gameBooster->_boosterIcon);
                }
                else
                {
                    //Activamos la UI de uso de booster
                    _btnPause->setVisible(false);
                    _boostersParent->setVisible(false);
                    _boosterApplyUI->setVisible(true);
                    _boosterApplyIcon->removeAllChildrenWithCleanup(true);
                    Sprite* _applyIcon = Sprite::createWithSpriteFrameName(_gameBooster->_boosterIcon);
                    _applyIcon->setPosition(Point(_boosterApplyIcon->getBoundingBox().size.width/2,_boosterApplyIcon->getBoundingBox().size.height/2));
                    _applyIcon->setScale((_boosterApplyIcon->getBoundingBox().size.width/_applyIcon->getBoundingBox().size.width)*1.3f);
                    _boosterApplyIcon->addChild(_applyIcon);
                    
                    //Damos paso a la selección en tablero
                    _gameBooster->showApplyPosition();
                }
            }
        }
    }
}

void TTGameUIManager::onClickBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    if(TTBoard::getInstance()->_isUpdatingBoard || TTBoard::getInstance()->_stepsManager->isRunningSteps() || TTGame::getInstance()->_paused || TTGame::getInstance()->isFinished())
        return;
    
    // Verificamos el estado del Booster
    int _boosterSubType = ((cocos2d::ui::Widget*)pSender)->getTag();
    activateBoosterUI(_boosterSubType);
}

void TTGameUIManager::onClickDoneApplyBoosterBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType != ui::Widget::TouchEventType::ENDED || _blockMenus)
        return;
    
    TTBoostersManager::getInstance()->applyActiveGameBoosters();
    
    // Actualizamos los boosters que nos quedan
    setBoosters();
    
    //Desactivamos la UI de uso de booster
    _btnPause->setVisible(true);
    _boostersParent->setVisible(true);
    _boosterApplyUI->setVisible(false);
    
    TTBoard::getInstance()->applyBooster();
}

void TTGameUIManager::onEvent_collectFigures(EventCustom* e)
{
    CollectFiguresDataEvent* data = static_cast<CollectFiguresDataEvent*>(e->getUserData());
    
    for(int i = 0; i < _gameInfo->goals.size(); i++)
    {
        if(_gameInfo->goals[i]->_typeGoal == COLLECT_FIGURES_GOAL)
        {
            
            if(data->figure != (static_cast<TTCollectFiguresGoal*>(_gameInfo->goals[i]))->_figureToCollect)
                continue;
            
            int actualCollections = data->currentValue;
            int _actualGoalCollections = data->goalValue;
            if(_actualGoalCollections <= actualCollections)
            {
                // Poner el goal a finalizado solo  laprimera vez que se ha obtenido el goal
                if (data->recentAchieved)
                {
                    showGoalComplete(_goals[i]);
                    _goals[i]._showDecrement->addNewDecrement(0);
                }
            }
            else
            {
                // Animación decremento goal
                showGoalStep(_goals[i]);
                _goals[i]._showDecrement->addNewDecrement(_actualGoalCollections-actualCollections);
            }
        }
    }
}

void TTGameUIManager::onEvent_goalUpdate(EventCustom* e)
{
    GenericGoalDataEvent* data = static_cast<GenericGoalDataEvent*>(e->getUserData());
    
    for(int i = 0; i < _gameInfo->goals.size(); i++)
    {
        if(_gameInfo->goals[i]->_typeGoal == data->typeGoal)
        {
            int _actualValue = data->currentValue;
            int _goalValue = data->goalValue;
            if(_gameInfo->goals[i]->isAchieved())
            {
                // Poner el goal a finalizado solo la primera vez que es conseguido
                if (data->recentAchieved)
                {
                    showGoalComplete(_goals[i]);
                    _goals[i]._showDecrement->addNewDecrement(0);
                }
            }
            else
            {
                // Animación decremento goal
                showGoalStep(_goals[i]);
                _goals[i]._showDecrement->addNewDecrement(_goalValue-_actualValue);
            }
        }
    }
    
}

void TTGameUIManager::onEvent_languageChange(EventCustom* e)
{

}

void TTGameUIManager::onEvent_continueGame(EventCustom* e)
{
    // Retornamos de la pausa
    _blockMenus = false;
}