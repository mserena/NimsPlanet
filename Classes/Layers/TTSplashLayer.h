#ifndef TTSplashLayer_H_
#define TTSplashLayer_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

class TTSplashLayer : public cocos2d::Layer
{
    
public:
    
    TTSplashLayer();
	~TTSplashLayer();
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    virtual void onExit() override;
    virtual void showSplashScreen();
    virtual void showLoading();

private:
    
    Sprite*                     _bgSplash;
    Sprite*                     _logoSplash;
    Sprite*                     _bgSplashAndroid;
    Sprite*                     _logoSplashAndroid;
    cocos2d::ui::TextBMFont*    _labelSplash;
};

#endif // TTSplashLayer_H_
