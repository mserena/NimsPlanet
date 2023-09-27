#ifndef __TTCellStaticContentNetwork_SCENE_H__
#define __TTCellStaticContentNetwork_SCENE_H__

#include "cocos2d.h"
#include "TTCellStaticContent.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocostudio;

class TTCellStaticContentNetwork: public TTCellStaticContent
{

public:

	TTCellStaticContentNetwork(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits);
    ~TTCellStaticContentNetwork();
    
    //Acciones a la celda
    void initCell();
    int hitCell(int _numHits = 1);
    
    //Consultas a la celda
    bool isRefillable();
    bool isSelectable();
    bool isFirstSelectable();
    bool isWallContent();
    bool isStrongWallContent();
    
    void setHitsLabel(std::string _hits);
    
private:
    
    void createBreakAnimation();

public:
    
    int                         _cellHits;
    
private:
    
    Node*                       _parentStone;
    Sprite*                     _spriteNetwork;
    cocos2d::ui::TextBMFont*    _visualHitsToDo;
    Sprite*                     _visualHitsToDoBkg;
    
};

#endif  // __TTCellStaticContentNetwork_SCENE_H__
