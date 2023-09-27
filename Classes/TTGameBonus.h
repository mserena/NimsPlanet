#ifndef __TTGameBonus_SCENE_H__
#define __TTGameBonus_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "TTGameInfo.h"


using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_BONUS_SPACE             30
#define HEIGHT_UI_BONUS_SPACE            30

class CC_DLL TTGameBonus: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTGameBonus* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTGameBonus(){}
    
public:
    
    void init();
    ~TTGameBonus();
    void update(float t);

    void initGameBonus(Node* _parent, int _gameBonusMoves);
    
    void playBonus();
    void finishBonus();
    
    void launchFigure();
    void showScoreFigure(Point posToShow, bool catchFigure);
    
    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    
public:
    
    std::vector<Color3B>        _colourFigures;
    std::vector<Sprite*>        _figuresRainbow;
    std::vector<Sprite*>        _figuresBonus;
    std::vector<MotionStreak*>  _motionSteaks;
    
    Size                    _screenSize;
    Node*                   _actualParent;
    
    int                     _numBonusMoves;
    int                     _actualMove;
    int                     _controlFigures;
    
    bool                    _isInBonus;
    
};

#endif  // __TTGameBonus_SCENE_H__
