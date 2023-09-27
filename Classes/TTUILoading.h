#ifndef __TTUILoading_SCENE_H__
#define __TTUILoading_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"


using namespace cocos2d;
using namespace cocostudio;

#define WIDTH_UI_LOADING_SPACE             80
#define HEIGHT_UI_LOADING_SPACE            40

class CC_DLL TTUILoading: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUILoading* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUILoading(){}
    
public:
    
    void init();
    ~TTUILoading();
    void showLoading(Node* _parent);
    
public:
    
    cocos2d::ui::Widget* _widgetLoading;
    cocos2d::ui::TextBMFont* _loadingTextLabel;
    
    Node*           _loadingParent;
    Sprite*         _bgLoading;
    Sprite*         _logoBgLoading;
    
};

#endif  // __TTUILoading_SCENE_H__
