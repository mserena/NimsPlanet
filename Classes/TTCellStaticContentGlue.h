#ifndef __TTCellStaticContentGlue_SCENE_H__
#define __TTCellStaticContentGlue_SCENE_H__

#include "cocos2d.h"
#include "TTCellStaticContent.h"

using namespace cocos2d;

class TTCellStaticContentGlue: public TTCellStaticContent
{

public:

	TTCellStaticContentGlue(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId, bool _createOnRuntime = false);
    ~TTCellStaticContentGlue();
    
    //Acciones a la celda
    void initCell();
    int hitCell(int _numHits = 1);
    
    //Consultas a la celda
    bool isRefillable();
    bool isSelectable();
    bool isFirstSelectable();
    bool isWallContent();
    bool isStrongWallContent();
    
private:
    
    void createBreakAnimation(int idxSprite, float _directionPrefixed = 0);
    
private:
    
    Sprite*                     _cellGlueSprite;

};

#endif  // __TTCellStaticContentGlue_SCENE_H__
