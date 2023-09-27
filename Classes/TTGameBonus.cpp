#include "TTGameBonus.h"
#include "TTBoard.h"
#include "TTLanguageManager.h"
#include "TTParticleManager.h"
#include "TTUIBonus.h"
#include "TTAudioManager.h"
#include "TTGame.h"
#include "TTConfigParser.h"
#include "TTUserInfo.h"

#define NUM_FIGURES_RAINBOW 6

//Singleton
static TTGameBonus *_sharedGameBonus = nullptr;

TTGameBonus* TTGameBonus::getInstance()
{
    if (!_sharedGameBonus)
    {
        _sharedGameBonus = new TTGameBonus();
        _sharedGameBonus->init();
    }
    
    return _sharedGameBonus;
}

void TTGameBonus::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedGameBonus);
}

void TTGameBonus::init()
{
    _isInBonus = false;
    _screenSize = Director::getInstance()->getWinSize();
    
    // Registramos el touch para el Bonus
    auto _touchListener = EventListenerTouchAllAtOnce::create();
    _touchListener->onTouchesBegan = CC_CALLBACK_2(TTGameBonus::onTouchesBegan, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_touchListener,1);
}

TTGameBonus::~TTGameBonus()
{

}

void TTGameBonus::initGameBonus(Node* _parent, int _gameBonusMoves)
{
    //parar el juego
    TTGameUIManager::getInstance()->pauseUI();
    
    _isInBonus = true;
    _controlFigures = _gameBonusMoves;
    _numBonusMoves = _gameBonusMoves;
    _actualParent = _parent;
    _actualMove = 0;
    
    TTUIBonus::getInstance()->initBonusUI(_parent);
    
    // Rellenamos el vector de colores
    _colourFigures.push_back(Color3B(255,104,232)); //Rosa
    _colourFigures.push_back(Color3B(255,0,0));     //Rojo
    _colourFigures.push_back(Color3B(255,246,0));   //Amarillo
    _colourFigures.push_back(Color3B(73,154,255));  //Azul
    _colourFigures.push_back(Color3B(75,219,19));   //Verde
    _colourFigures.push_back(Color3B(255,162,0));   //NAranja
    
    //Creamos los sprites de los animales del mundo actual
    float _figureSize = _screenSize.width/(NUM_FIGURES_RAINBOW+4);

    for(int i = 0; i < NUM_FIGURES_RAINBOW; i++)
    {
        std::string _figureName = "figura"+std::to_string(i+1)+".png";
        Sprite* _figure = Sprite::createWithSpriteFrameName(_figureName);
        int _selectedWorld = TTUserInfo::getInstance()->_userData->_selectedWorld;
        int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[_selectedWorld]._charactersPack;
        float _percentage = TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._figuresSizePercentage;
        float _figureScale = _figureSize/_figure->getBoundingBox().size.width;
        _figureScale = _figureScale * _percentage/100 + 0.1;
        
        _figure->setScale(_figureScale);
        _figure->setPosition(Point(((_figureSize*2)+(_figure->getBoundingBox().size.width/2))+_figureSize*i,-_figure->getBoundingBox().size.height/2));
        _parent->addChild(_figure,42);
        _figuresRainbow.push_back(_figure);
        
        auto streak = MotionStreak::create(1.8f,2.0f,_figureSize,_colourFigures[i],"streak.png");
        _parent->addChild(streak,41);
        _motionSteaks.push_back(streak);
        
        // Acciones
        auto _move = MoveTo::create(1.5f,Point(_figure->getPosition().x,_screenSize.height+(_figure->getBoundingBox().size.height/2)));
        _figure->runAction(_move);
    }

    TTAudioManager::getInstance()->playAudio("BONUS_RAINBOW_UP");
    
    // Acciones
    auto _delayShow = DelayTime::create(2.5f);
    auto _finishShow = CallFunc::create(std::bind([&](){
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTGameBonus::update),this);
        
        for(int i = 0; i < _figuresRainbow.size(); i++)
        {
            _figuresRainbow[i]->removeFromParentAndCleanup(true);
            _motionSteaks[i]->removeFromParentAndCleanup(true);
        }
        _figuresRainbow.clear();
        _motionSteaks.clear();
        _colourFigures.clear();
        
        TTUIBonus::getInstance()->showAnimBonusTitle();
    }));
    _parent->runAction(Sequence::create(_delayShow,_finishShow,NULL));
    
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTGameBonus::update),this,0,false);
}

void TTGameBonus::update(float t)
{
    for(int i = 0; i < _figuresRainbow.size(); i++)
    {
        _motionSteaks[i]->setPosition(_figuresRainbow[i]->getPosition());
    }
}

void TTGameBonus::playBonus()
{
    // Creamos todas las figuras del bonus
    float _figureSize = _screenSize.width/(NUM_FIGURES_RAINBOW+1 );
    for(int i = 0; i < _numBonusMoves; i++)
    {
        int _randFigure = rand()%NUM_FIGURES_RAINBOW;
        std::string _figureName;
        if(rand()%2 == 0)
        {
            _figureName = "figura"+std::to_string(_randFigure+1)+".png";
        }
        else
        {
            _figureName = "figura"+std::to_string(_randFigure+1)+"_anim_susto_1.png";
        }
        
        Sprite* _figure = Sprite::createWithSpriteFrameName(_figureName);
        float _figureScale = _figureSize/_figure->getBoundingBox().size.width;
        _figure->setScale(_figureScale);
        _figure->setVisible(false);
        _actualParent->addChild(_figure,43);
        _figuresBonus.push_back(_figure);
    }
    
    // Lanzamos las figuras aleatóriamente
    launchFigure();
}

void TTGameBonus::launchFigure()
{
    int _limitY = _screenSize.height - (_screenSize.height/3);
    int _limitLaunchY = _screenSize.height/3;
    int _limitMinJump = _screenSize.height/2;
    Sprite* _figureToLaunch = _figuresBonus[_actualMove];
    
    // Decide el punto de inicio y final
    Point _initPosition;
    Point _finishPosition;
    if(rand()%2 == 0)
    {
        // Izquierda
        _initPosition.x = -_figureToLaunch->getBoundingBox().size.width;
        _finishPosition.x = _screenSize.width+_figureToLaunch->getBoundingBox().size.width;
    }
    else
    {
        // Derecha
        _initPosition.x = _screenSize.width+_figureToLaunch->getBoundingBox().size.width;
        _finishPosition.x = -_figureToLaunch->getBoundingBox().size.width;
    }
    _initPosition.y = rand()%_limitLaunchY;
    _finishPosition.y = rand()%_limitLaunchY;
    
    // Decide si hay rotación
    if(rand()%2 == 0)
    {
        //Figura con rotación
        auto _rotateFigure = RotateBy::create(0.5f,360);
        _figureToLaunch->runAction(RepeatForever::create(_rotateFigure));
    }
    
    _figureToLaunch->setVisible(true);
    _figureToLaunch->setPosition(_initPosition);
    float _randTimeJump = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1.5f)))+1.0f;
    int _jumpHeight = _limitMinJump+rand()%(_limitY-_limitMinJump);
    auto _launchMove = JumpTo::create(_randTimeJump,_finishPosition,_jumpHeight-_initPosition.y,1);
    auto _onFigureFinish = CallFunc::create(std::bind([&](Sprite* _figure){
        if(_figure->isVisible())
        {
            float _x = _figure->getPosition().x;
            if (_x < 0)
            {
                _x = 0;
            }
            if (_x >= _screenSize.width )
            {
                _x = _screenSize.width;
            }
            // La figura no ha sido cazada
            showScoreFigure(Point(_figure->getPosition().x,_figure->getPosition().y),false);
        }
        
        _controlFigures--;
        if(_controlFigures == 0)
        {
            finishBonus();
        }
    },_figureToLaunch));
    _figureToLaunch->runAction(Sequence::create(_launchMove,_onFigureFinish,NULL));
    
    _actualMove++;
    
    TTUIBonus::getInstance()->updateUIBonus();
    
    if(_actualMove != _numBonusMoves)
    {
        float _randTime = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1.0f)))+0.1f;
        auto _launchOtherFigure = CallFunc::create(CC_CALLBACK_0(TTGameBonus::launchFigure,this));
        _actualParent->runAction(Sequence::create(DelayTime::create(_randTime),_launchOtherFigure, NULL));
    }
}

void TTGameBonus::finishBonus()
{
    _isInBonus = false;
    
    TTUIBonus::getInstance()->hideBonusUI();
    
    for(int i = 0; i < _figuresBonus.size(); i++)
    {
        _figuresBonus[i]->removeFromParentAndCleanup(true);
    }
    _figuresBonus.clear();
    
    TTBoard::getInstance()->finishBoard();
}

void TTGameBonus::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event)
{
    for(int i = 0; i < _figuresBonus.size(); i++)
    {
        if(_figuresBonus[i]->getBoundingBox().containsPoint(touches[0]->getLocation()) && _figuresBonus[i]->isVisible())
        {
            // Una figura ha sido tocada
            _figuresBonus[i]->setVisible(false);
            
            // Mostramos los puntos
            showScoreFigure(_figuresBonus[i]->getPosition(),true);
            
            // Cargamos las partículas
            ValueMap _catchParticlesDic = TTParticleManager::getInstance()->getParticle("animal_cazado");
            ParticleSystemQuad* _cachtPartices = ParticleSystemQuad::create(_catchParticlesDic);
            _cachtPartices->setPosition(_figuresBonus[i]->getPosition());
            _cachtPartices->setScale(_figuresBonus[i]->getScale());
            _cachtPartices->resetSystem();
            _cachtPartices->setAutoRemoveOnFinish(true);
            _actualParent->addChild(_cachtPartices,42);
            
            // Sonido
            TTAudioManager::getInstance()->playAudio("BONUS_FIGURE_CATCH");
            
            // Update de la barra
            TTUIBonus::getInstance()->updateUIBonus();
        }
    }
}

void TTGameBonus::showScoreFigure(Point posToShow, bool catchFigure)
{
    // Creamos el label para el score de tablero
    auto _scoreBoardLabel  = Label::createWithBMFont("markerfeltScore.fnt","0");

    _actualParent->addChild(_scoreBoardLabel,50);
    
    Size _screensize = Director::getInstance()->getWinSize();
    float _scale = (5*_screensize.width/100)/_scoreBoardLabel->getContentSize().width;
    _scoreBoardLabel->setScale(_scale);

    float _x = posToShow.x;
    if (_x < 0)
    {
        _x = 0;
    }
    if (_x >= _screenSize.width )
    {
        _x = _screenSize.width;
    }

    Point _pos = Point(_x,posToShow.y);
    
    int scoreWined = (catchFigure==true) ? TTGame::getInstance()->_gameConfig->_scoreBonusCatch : TTGame::getInstance()->_gameConfig->_scoreBonusNoCatch;
     _scoreBoardLabel->setString(std::to_string(scoreWined));
    _scoreBoardLabel->setPosition(_pos);
 
    TTGame::getInstance()->_gameConfig->scoreGame += scoreWined;

    float _incrementX = (_x==0) ? _scoreBoardLabel->getBoundingBox().size.width : -_scoreBoardLabel->getBoundingBox().size.width ;
    if (catchFigure)
    {
        _incrementX = 0;
    }
    // Seteamos la acción de movimiento
    float _time = 0.6f;
    auto moveLabel = MoveTo::create(_time,Point(_scoreBoardLabel->getPosition().x + _incrementX, _scoreBoardLabel->getPosition().y+_scoreBoardLabel->getBoundingBox().size.height*2));
    auto fadeOut = FadeTo::create(_time,0);
    auto _cleanLabel = RemoveSelf::create();
    _scoreBoardLabel->runAction(Sequence::create(Spawn::create(moveLabel,fadeOut, NULL),_cleanLabel,NULL));
}
