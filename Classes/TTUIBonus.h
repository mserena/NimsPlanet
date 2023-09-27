//
//  TTUIBonus.h
//  animalcubes
//
//  Created by Marc Serena on 31/10/14.
//
//

#ifndef __animalcubes__TTUIBonus__
#define __animalcubes__TTUIBonus__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_BONUSUI_SPACE             60
#define HEIGHT_UI_BONUSUI_SPACE            50

class CC_DLL TTUIBonus: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIBonus* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTUIBonus(){}
    
public:
    
    void init();
    
    void loadUI();
    void unLoadUI();
    
    ~TTUIBonus();
    void initBonusUI(Node* _parent);
    void showAnimBonusTitle();
    void hideBonusUI();
    void updateUIBonus();
    void updateScoreBar();
    
public:
    
    cocos2d::Layer*         _uiBonusLayer;
    cocos2d::ui::Widget*    _widgetBonus;
    Sprite*                 _bgUI;
    
    cocos2d::ui::LoadingBar*                _scoreBar;
    cocos2d::ui::TextBMFont*                _scoreLabel;
    std::vector<std::vector<Sprite*> >      _starsSprites;
    
    cocos2d::ui::Widget*    _titleBonus;
    cocos2d::ui::Widget*    _uiScore;
    cocos2d::ui::Widget*    _uiFigures;
    
    cocos2d::ui::TextBMFont*    _figuresLabel;
};

#endif /* defined(__animalcubes__TTUIBonus__) */
