//
//  TTHowToInGameManager.cpp
//  animalplanet
//
//  Created by Marc Serena on 18/3/15.
//
//

#include "TTHowToInGameManager.h"
#include "thegame.h"
#include "TTGameUIManager.h"

//Singelton
static TTHowToInGameManager *_sharedHowToInGameManager = nullptr;

TTHowToInGameManager* TTHowToInGameManager::getInstance()
{
    if (!_sharedHowToInGameManager)
    {
        _sharedHowToInGameManager = new TTHowToInGameManager();
    }
    
    return _sharedHowToInGameManager;
}

void TTHowToInGameManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedHowToInGameManager);
}

TTHowToInGameManager::~TTHowToInGameManager()
{

}

void TTHowToInGameManager::init(cocos2d::Layer* parentLayer)
{
    _isTutorialActive = false;
    _parentLayer = parentLayer;
    
    CCLOG("Creando manager de tutoriales In Game");

    // Parsea el XML del tutorial
    TTHowToInGameParser::getInstance()->parseXML();
    
    // creamos el BG de fondo del tutorial
    Size _screenSize = Director::getInstance()->getWinSize();
    _bgTutorial = Sprite::create("pixelBg.png");
    _bgTutorial->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgTutorial->setScale(_screenSize.width,_screenSize.height);
    _bgTutorial->setVisible(false);
    _parentLayer->addChild(_bgTutorial,1);
    
    _handTutorial = Sprite::createWithSpriteFrameName("mano_derecha.png");
    _handTutorial->setScale(scale_sprite(_handTutorial,25,25));
    _handTutorial->setAnchorPoint(Point(0.80f,0));
    _handTutorial->setVisible(false);
    _parentLayer->addChild(_handTutorial,4);
    
    _textBg = Sprite::createWithSpriteFrameName("bocablo.png");
    _textBg->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_textBg->getContentSize().height/2)));
    _textBg->setScale(scale_sprite(_textBg,85,25));
    _textBg->setVisible(false);
    _parentLayer->addChild(_textBg,4);
    
    _birdAnimation = Sprite::create();
    _birdAnimation->setVisible(false);
    _birdAnimation->setAnchorPoint(Point(0.1f,0.15f));
    _textBg->addChild(_birdAnimation);
    Animate* animNode = Animate::create(AnimationCache::getInstance()->getAnimation("bird_tutorial_inGame"));
    Sequence* _animSequence = Sequence::create(animNode,DelayTime::create(0.2),NULL);
    _birdAnimation->runAction(RepeatForever::create(_animSequence));
    
    _howtoInGameText = Label::createWithBMFont("markerfeltwide.fnt","");
    _howtoInGameText->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
    _howtoInGameText->setVisible(false);
    _howtoInGameText->setPosition(Point(_textBg->getContentSize().width-(_textBg->getContentSize().width/3),_textBg->getContentSize().height/2));
    _textBg->addChild(_howtoInGameText);
}

bool TTHowToInGameManager::hasTutorial(int _levelId)
{
    if(TTHowToInGameParser::getInstance()->_mapTutorialInGame.find(_levelId) != TTHowToInGameParser::getInstance()->_mapTutorialInGame.end())
    {
        return true;
    }
    return false;
}

void TTHowToInGameManager::showTutorial(int _levelId)
{
    if(hasTutorial(_levelId))
    {
        TTGameUIManager::getInstance()->setTouchEnabledUI(false);
        _isTutorialActive = true;
        _actualStepsTutorial = TTHowToInGameParser::getInstance()->_mapTutorialInGame.find(_levelId)->second;
        _actualIDStepTutorial = 0;
        showStepTutorial();
    }
}

void TTHowToInGameManager::showStepTutorial()
{
    _bgTutorial->setVisible(true);
    
    // Mostramos la veladura en todas las casillas del tablero menos las permitidas
    std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
    std::vector<TTCell*>  _cellsAllowedStep = getActualAllowedCells();
    for(int i = 0; i < _cellsBoard.size(); i++)
    {
        for(int j = 0; j < _cellsBoard[i].size(); j++)
        {
            bool _isCellAllowed = false;
            for(int w = 0; w < _cellsAllowedStep.size(); w++)
            {
                if(_cellsAllowedStep[w] == _cellsBoard[i][j])
                {
                    _isCellAllowed = true;
                }
            }
            
            if(!_isCellAllowed && _cellsBoard[i][j]->_cellTypeID != CELL_EMPTY && _cellsBoard[i][j]->_cellTypeID != CELL_WITH_EXTERNAL_RESOURCE)
            {
                _cellsBoard[i][j]->_cellTutorialGlaze->setVisible(true);
            }
        }
    }

    std::vector<TTCell*>  _cellsNoAllowedVisibleStep = getActualNoAllowedVisibleCells();
    for(int i = 0; i < _cellsBoard.size(); i++)
    {
        for(int j = 0; j < _cellsBoard[i].size(); j++)
        {
            for(int w = 0; w < _cellsNoAllowedVisibleStep.size(); w++)
            {
                if(_cellsNoAllowedVisibleStep[w] == _cellsBoard[i][j])
                {
                    _cellsBoard[i][j]->_cellTutorialGlaze->setVisible(false);
                }
            }
        }
    }

    
    // Mostramos la animación del dedo
    _handTutorial->setVisible(true);
    _handTutorial->setOpacity(255);
    Vector<cocos2d::FiniteTimeAction *> _pathHand;
    for(int i = 0; i < _cellsAllowedStep.size(); i++)
    {
        if(i == 0) _handTutorial->setPosition(_parentLayer->convertToWorldSpace(TTBoard::getInstance()->_parentBoard->convertToWorldSpace(Point(_cellsAllowedStep[i]->_posXCell,_cellsAllowedStep[i]->_posYCell))));
        auto _move = MoveTo::create(0.3f, _parentLayer->convertToWorldSpace(TTBoard::getInstance()->_parentBoard->convertToWorldSpace(Point(_cellsAllowedStep[i]->_posXCell,_cellsAllowedStep[i]->_posYCell))));
        _pathHand.insert(i, _move);
    }
    auto _handMoveSequence = Sequence::create(_pathHand);
    auto _resituateHand = CallFunc::create(std::bind([&](std::vector<TTCell*>  _cellsAllowedStep){
        _handTutorial->setPosition(_parentLayer->convertToWorldSpace(TTBoard::getInstance()->_parentBoard->convertToWorldSpace(Point(_cellsAllowedStep[0]->_posXCell,_cellsAllowedStep[0]->_posYCell))));
    },_cellsAllowedStep));
    auto _fadeOffHand = FadeTo::create(0.2f,0);
    auto _fadeOnHand = FadeTo::create(0.2f,255);
    _handTutorial->runAction(RepeatForever::create(Sequence::create(_handMoveSequence,DelayTime::create(0.3f),_fadeOffHand,_resituateHand,DelayTime::create(1.0f),_fadeOnHand,NULL)));
    
    
    // Mostramos el texto y imágen del tutorial
    _textBg->setVisible(true);
    _birdAnimation->setVisible(true);
    
    _howtoInGameText->setScale(1);
    _howtoInGameText->setString(TTLanguageManager::getInstance()->getTag(_actualStepsTutorial[_actualIDStepTutorial]._textId));
    _howtoInGameText->setScale(scale_label_by_parent(_howtoInGameText,60,85));
    _howtoInGameText->setVisible(true);
}

void TTHowToInGameManager::hideStepTutorial()
{
    _bgTutorial->setVisible(false);
    
    // Ocultamos la veladura en todas las casillas
    std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
    for(int i = 0; i < _cellsBoard.size(); i++)
    {
        for(int j = 0; j < _cellsBoard[i].size(); j++)
        {
            _cellsBoard[i][j]->_cellTutorialGlaze->setVisible(false);
        }
    }
    
    // Ocultamos la animación del dedo
    _handTutorial->setVisible(false);
    _handTutorial->stopAllActions();
    
    // Ocultamos el texto y imágen del tutorial
    _textBg->setVisible(false);
    _birdAnimation->setVisible(false);
    _howtoInGameText->setVisible(false);
}

void TTHowToInGameManager::nextStepTutorial()
{
    if(_actualIDStepTutorial == _actualStepsTutorial.size()-1)
    {
        // Se han acabado los steps de tutorial
        _isTutorialActive = false;
        TTBoard::getInstance()->launchShowHelpCombination();
        TTGameUIManager::getInstance()->setTouchEnabledUI(true);
    }
    else
    {
        _actualIDStepTutorial++;
        showStepTutorial();
    }
}

std::vector<TTCell*> TTHowToInGameManager::getActualAllowedCells()
{
    std::vector<TTCell*> _allowedCells;
    std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
    for(int w = 0; w < _actualStepsTutorial[_actualIDStepTutorial]._cellsAllowed.size(); w++)
    {
        _allowedCells.push_back(_cellsBoard[_actualStepsTutorial[_actualIDStepTutorial]._cellsAllowed[w]._col][_actualStepsTutorial[_actualIDStepTutorial]._cellsAllowed[w]._row]);
    }
    return _allowedCells;
}

std::vector<TTCell*> TTHowToInGameManager::getActualNoAllowedVisibleCells()
{
    std::vector<TTCell*> _noAllowedVisibleCells;
    std::vector<std::vector<TTCell*> > _cellsBoard = TTBoard::getInstance()->_Cells;
    for(int w = 0; w < _actualStepsTutorial[_actualIDStepTutorial]._cellsNoAllowedVisible.size(); w++)
    {
        _noAllowedVisibleCells.push_back(_cellsBoard[_actualStepsTutorial[_actualIDStepTutorial]._cellsNoAllowedVisible[w]._col][_actualStepsTutorial[_actualIDStepTutorial]._cellsNoAllowedVisible[w]._row]);
    }
    return _noAllowedVisibleCells;
}

int TTHowToInGameManager::getTotalCellsToSelectActualStep()
{
    return _actualStepsTutorial[_actualIDStepTutorial]._cellsAllowed.size();
}
