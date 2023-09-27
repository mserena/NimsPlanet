//
//  TTHowToInGameManager.h
//  animalplanet
//
//  Created by Marc Serena on 18/3/15.
//
//

#ifndef __animalplanet__TTHowToInGameManager__
#define __animalplanet__TTHowToInGameManager__

#include "cocos2d.h"
#include "TTHowToInGameParser.h"
#include "TTBoard.h"

using namespace cocos2d;

class CC_DLL TTHowToInGameManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTHowToInGameManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTHowToInGameManager(){}
    
public:
    
    virtual ~TTHowToInGameManager();
    void init(cocos2d::Layer* parentLayer);
    
    bool hasTutorial(int _levelId);
    void showTutorial(int _levelId);
    void showStepTutorial();
    void hideStepTutorial();
    void nextStepTutorial();
    std::vector<TTCell*> getActualAllowedCells();
    std::vector<TTCell*> getActualNoAllowedVisibleCells();
    int getTotalCellsToSelectActualStep();

public:
    
    bool                        _isTutorialActive;
    int                         _actualIDStepTutorial;
    std::vector<stepTutorial>   _actualStepsTutorial;
    
private:
    
    cocos2d::Layer* _parentLayer;
    Sprite*         _bgTutorial;
    Sprite*         _handTutorial;
    Sprite*         _textBg;
    Sprite*         _birdAnimation;
    Label*          _howtoInGameText;
    
};

#endif /* defined(__animalplanet__TTHowToInGameManager__) */
