#ifndef TTMapLayer_H_
#define TTMapLayer_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "TTMapManager.h"

using namespace cocos2d;
using namespace cocostudio;

class TTMapLayer : public cocos2d::Layer
{
    
public:
    
    TTMapLayer();
	~TTMapLayer();
    void update( float dt );
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    virtual void onExit() override;
    
    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event);
    
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
private:
    
    EventListenerTouchAllAtOnce *_touchListener;
    
    TTMapManager*    _mapMgr;
    
};

#endif // TTMapLayer_H_
