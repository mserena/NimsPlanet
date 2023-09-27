#ifndef TTLoadingLayer_H_
#define TTLoadingLayer_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

class TTLoadingLayer : public cocos2d::Layer
{
    
public:
    
    TTLoadingLayer();
	~TTLoadingLayer();
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    virtual void onExit() override;

};

#endif // TTLoadingLayer_H_
