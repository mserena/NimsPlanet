#ifndef __TTAnimalCubes_SCENE_H__
#define __TTAnimalCubes_SCENE_H__

#include "cocos2d.h"
#include "TTGameManager.h"

using namespace cocos2d;

typedef enum
{
    GAME_STATE_SPLASH,
    GAME_STATE_MAINMENU,
    GAME_STATE_MAP,
    GAME_STATE_INGAME
}GAME_STATE;

// Clase singleton
class CC_DLL TTAnimalCubes: public Ref
{

public:

    // Retorna la instancia Singleton de la clase
    static TTAnimalCubes* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTAnimalCubes(){}

public:
    
    void init();
    ~TTAnimalCubes();
    void update( float dt);
    void loadAll();
    
    void setState(GAME_STATE gameState);
    Layer* getActualLayer();
    
private:
    
    //events
    void onEvent_onLoadingScene(EventCustom* e);
    void onEvent_onGameLoaded(EventCustom* e);
    void onEvent_onMapLoaded(EventCustom* e);
    void onEvent_onMainMenuLoaded(EventCustom* e);
    
private:

    Layer* _mapLayer;
    Layer* _loadingLayer;
    Layer* _gameLayer;
    Layer* _mainMenuLayer;
    Layer* _splashLayer;
    
    std::vector<EventListener*> _classListeners;
    
public:

    Scene*          _generalScene;
    GAME_STATE      _gameState;
};

#endif  // __TTAnimalCubes_SCENE_H__
