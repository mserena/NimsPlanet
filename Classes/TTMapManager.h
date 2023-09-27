#ifndef __TTMapManager_SCENE_H__
#define __TTMapManager_SCENE_H__

#include "thegame.h"
#include "TTGameInfo.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIWidget.h"
#include "TTMapDefines.h"

#include "TTMapAnimation.h"
#include "TTMapAnimationRotation.h"
#include "TTMapAnimationAnimator.h"
#include "TTMapAnimationStaticSprite.h"
#include "TTMapAnimationParticles.h"

#define STEP_TIME     1.0f


using namespace cocos2d;

typedef struct mapCell
{
    Node* _parentCell;
    
    std::vector<Sprite*> _stars;
    Sprite* _cellOn;
    Sprite* _cellOff;
    LabelBMFont* _fntLevel;
    
    bool _hasStopper;
    Sprite* _stopperOn;
    Sprite* _stopperOff;
}mapCell;

typedef struct userMap
{
    Node*       _parent;
    Sprite*     _frame;
    Sprite*     _photo;
}userMap;

class TTMapManager: public Ref
{
    
public:
    
	TTMapManager(cocos2d::Layer* parentLayer);
    ~TTMapManager();
    bool init(void);
    void update( float dt);
 
    void loadMapPieces();
    void loadMapAssets();
    void loadMapAnimations();
    
    void resourceMapPieceLoaded(Texture2D* texture, int _mapId);
    void assetsMapLoaded(Texture2D* texture);
    void controlResoucesLoaded();

    void setVisibleMap();
    
    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);

private:
    
    void createUsersUI();
    void createMapCells();
    
    void updateUsersMultiPhotos(float dt);

    void animateActiveCell();
    void reloadMapInfo();
    void verifyActualLevelToInit(int _actualLevel);
    
    bool verifyButtons(Vec2 point, EventTouch::EventCode _typeTouch);
    bool blockDisplacement(float _verticalDisp);
    
    void resituateMapForUserPosition();
    
    void onEvent_unlockActualLevel(EventCustom* e);
    void onEvent_PauseMap(EventCustom* e);
    void onEvent_ReturnToMap(EventCustom* e);
    void onEvent_AnimMapLoaded(EventCustom* e);
    void onEvent_userFriendsUpdated(EventCustom* e);
    void onEvent_ChangeLanguage(EventCustom* e);
    void onEvent_userInfoUpdated(EventCustom* e);

public:

    cocos2d::Layer*         _parentLayer;
    gameInfo*               _gameBasicConfig;
    bool                    _pauseMap;
    bool                    _isWalking;
    bool                    _waitForUpdateMap;
    
private:
    
    Layer*                  _mapLayer;
    Node*                   _mapNode;
    int                     _totalResourcesToLoad;
    
    Size                    _mapSize;
    float                   _mapScale;
    std::vector<Sprite*>    _mapPieces;
    std::vector<mapCell*>   _mapCells;
    Sprite*                 _halo1;
    Sprite*                 _halo2;
    
    std::vector<TTMapAnimation*>   _mapAnimations;
    
    SCROLL_MOVE_DIRECTION   _moveDirection;
    float                   _totalDisp;
    float                   _timeDisp;
    bool                    _onMove;
    
    bool                    _blockDownDisp;
    bool                    _blockUpDisp;
    int                     _actualLevelBtnSelected;
    
    std::vector<EventListener*> _classListeners;
    
    //cartel madera
    Label*  _textWinLevel;
    
    // Fotografias facebook
    userMap*                                _meUserMap;
    std::map<int,userMap*>                  _actualFriendsUsersMap;
    std::map<int,std::vector<std::string>>  _multiFriendsInfo;
};

#endif  // __TTMapManager_SCENE_H__
