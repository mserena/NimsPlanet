#ifndef TTMainMenuLayer_H_
#define TTMainMenuLayer_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

class TTMainMenuLayer : public cocos2d::Layer
{
    
public:
    
    TTMainMenuLayer();
	~TTMainMenuLayer();
    void update( float dt );
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    virtual void onExit() override;

private:
    
    void showMainMenu();
    void loadResources();
    void resourcesLoadedControl();
    
    void onTouchPlayBtn(Ref *sender,ui::Widget::TouchEventType touchEvent);
    void onTouchFacebookBtn(Ref *sender,ui::Widget::TouchEventType touchEvent);
    void onTouchFacebookLikeBtn(Ref *sender,ui::Widget::TouchEventType touchEvent);
    
    void onFacebookConnection(EventCustom* event);
    
private:
    int         _resourcesToLoad;
    
    Sprite*     _bgMainMenu;
    Sprite*     _logoMainMenu;
    Sprite*     _animalsMainMenu;
    
    cocos2d::ui::Button*        _btnPlay;
    cocos2d::ui::TextBMFont*    _playLabel;
    cocos2d::ui::Button*        _btnFacebook;
    cocos2d::ui::Button*        _btnFacebookLike;
    Sprite*                     _coinsForFacebookLike;
    cocos2d::ui::TextBMFont*    _facebookLabel;
    
};

#endif // TTMainMenuLayer_H_
