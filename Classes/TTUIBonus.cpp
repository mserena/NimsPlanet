//
//  TTUIBonus.cpp
//  animalcubes
//
//  Created by Marc Serena on 31/10/14.
//
//

#include "TTUIBonus.h"
#include "TTLanguageManager.h"
#include "TTGameBonus.h"
#include "TTAudioManager.h"
#include "TTGame.h"

//Singleton
static TTUIBonus *_sharedBonusUI = nullptr;

TTUIBonus* TTUIBonus::getInstance()
{
    if (!_sharedBonusUI)
    {
        _sharedBonusUI = new TTUIBonus();
        _sharedBonusUI->init();
    }
    
    return _sharedBonusUI;
}

void TTUIBonus::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedBonusUI);
}

void TTUIBonus::init()
{
    loadUI();
    
    // Creamos el bg
    Size _screenSize = Director::getInstance()->getWinSize();
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
}

TTUIBonus::~TTUIBonus()
{
    CCLOG("Se destruye la UI del Bonus");
    
    // Eliminamos las animaciones de la UI
    removeAllActionsFromJSON("UI_BONUS.ExportJson");
    
    // Eliminamos la layer contenedora
    _uiBonusLayer->removeAllChildrenWithCleanup(true);
    _uiBonusLayer->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_BONUS0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_BONUS0.png");
}

void TTUIBonus::loadUI()
{
    // Cargamos el XML de la UI Resume
    _uiBonusLayer = Layer::create();
    _uiBonusLayer->retain();
    _widgetBonus = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_BONUS.ExportJson");
    cocos2d::ui::Layout* _layoutShop = dynamic_cast<cocos2d::ui::Layout*>(_widgetBonus);
    _uiBonusLayer->addChild(_layoutShop);
    
    // Escalado Widget
    _widgetBonus->setScale(scale_widget(_widgetBonus,WIDTH_UI_BONUSUI_SPACE,HEIGHT_UI_BONUSUI_SPACE));
    
    // Posicionamiento Widget
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetBonus->setAnchorPoint(Vec2(0.5,0.5));
    _uiBonusLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _uiBonusLayer->setVisible(false);
    
    _titleBonus = (cocos2d::ui::Widget*)_widgetBonus->getChildByName("titleBonus");
    _uiScore = (cocos2d::ui::Widget*)_widgetBonus->getChildByName("uiScore");
    _uiFigures = (cocos2d::ui::Widget*)_widgetBonus->getChildByName("uiFigures");
    
    _figuresLabel = (cocos2d::ui::TextBMFont*)_uiFigures->getChildByName("uiFiguresLabel");
    _scoreBar = (cocos2d::ui::LoadingBar*)_uiScore->getChildByName("scoreBar");
    _scoreLabel = (cocos2d::ui::TextBMFont*)_uiScore->getChildByName("labelScore");
    for(int i = 0; i < 3; i++)
    {
        std::vector<Sprite*> stars;
        char idStarStr[10] = {0};
        sprintf(idStarStr,"star%d",i+1);
        stars.push_back((Sprite*)_uiScore->getChildByName("starsParent")->getChildByName(idStarStr)->getChildByName("star_off"));
        stars.push_back((Sprite*)_uiScore->getChildByName("starsParent")->getChildByName(idStarStr)->getChildByName("star_on"));
        _starsSprites.push_back(stars);
    }
}

void TTUIBonus::unLoadUI()
{
    
}

void TTUIBonus::initBonusUI(Node* _parent)
{
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _parent->addChild(_bgUI,40);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiBonusLayer->getParent() != nullptr)
        _uiBonusLayer->removeFromParentAndCleanup(false);
    _parent->addChild(_uiBonusLayer,43);
    _uiBonusLayer->setVisible(true);
    _widgetBonus->setVisible(true);
    
    Size _screenSize = Director::getInstance()->getWinSize();
    _uiBonusLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    
    _titleBonus->setVisible(false);
    _uiScore->setVisible(false);
    _uiFigures->setVisible(false);
    
    _figuresLabel->setString(std::to_string(TTGameBonus::getInstance()->_numBonusMoves));
    
    // Seteamos el score y posición de las barras
    int _sizeBar = ((Node*)_uiScore->getChildByName("starsParent"))->getBoundingBox().size.width;
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
        Node* star = (Node*)_uiScore->getChildByName("starsParent")->getChildByName(idStarStr);
        float _posXStar = ((TTGame::getInstance()->_gameConfig->scoreStars[i]*_sizeBar)/TTGame::getInstance()->_gameConfig->scoreStars[_starsSprites.size()-1])-star->getBoundingBox().size.width/2;
        star->setPosition(Vec2(_posXStar,star->getPosition().y));
    }
    updateScoreBar();
}

void TTUIBonus::showAnimBonusTitle()
{
    _titleBonus->setVisible(true);
    _uiScore->setVisible(false);
    _uiFigures->setVisible(false);
    
    auto *step2Anim = CallFunc::create(std::bind([&](){
        Size _screenSize = Director::getInstance()->getWinSize();
        auto *moveUI = MoveTo::create(1.0f, Point(_screenSize.width/2,_screenSize.height-_widgetBonus->getBoundingBox().size.height/2));
        auto *moveUIExp = EaseExponentialOut::create(moveUI);
        
        _uiScore->setVisible(true);
        _uiFigures->setVisible(true);
        cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_BONUS.ExportJson","UIEnter");
        
        auto *step3Anim = CallFunc::create(std::bind([&](){
            TTGameBonus::getInstance()->playBonus();
        }));
        
        _uiBonusLayer->runAction(Sequence::create(moveUIExp,step3Anim,NULL));
    }));
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_BONUS.ExportJson","BonusTitleEnter",step2Anim);
    
    TTAudioManager::getInstance()->playAudio("BONUS_ENTER");
    TTAudioManager::getInstance()->playAudio("BONUS_MUSIC");
}

void TTUIBonus::updateUIBonus()
{
    // Update del número de figuras por salir
    _figuresLabel->setString(std::to_string(TTGameBonus::getInstance()->_numBonusMoves-TTGameBonus::getInstance()->_actualMove));
    updateScoreBar();
}

void TTUIBonus::hideBonusUI()
{
    TTAudioManager::getInstance()->playAudio("GAME_MUSIC");
    _uiBonusLayer->setVisible(false);
    _bgUI->setVisible(false);
}

void TTUIBonus::updateScoreBar()
{
    int _actualScore = TTGame::getInstance()->_gameConfig->scoreGame;
    _scoreLabel->setString(TTLanguageManager::getInstance()->convertIntToStringWithDecimalPoint(_actualScore));
    std::vector<int> stars = TTGame::getInstance()->_gameConfig->scoreStars;
    float percentScore = (_actualScore*100)/stars[stars.size()-1];
    if(percentScore > 100)
        _scoreBar->setPercent(100);
    else
        _scoreBar->setPercent(percentScore);
    
    // Gestión de iluminación de estrellas
    int _starsWin = 0;
    for(int i = 0; i < stars.size(); i++)
    {
        if(_actualScore >= stars[i])
        {
            if(_starsSprites[i][1]->isVisible())
                continue;
            
            _starsWin++;
            _starsSprites[i][0]->setVisible(false);
            _starsSprites[i][1]->setVisible(true);
        }
    }
}
