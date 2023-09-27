#ifndef __TTCellStaticContentWood_SCENE_H__
#define __TTCellStaticContentWood_SCENE_H__

#include "cocos2d.h"
#include "TTCellStaticContent.h"

#define MAX_HITS_WOOD    3

using namespace cocos2d;

class TTCellStaticContentWood: public TTCellStaticContent
{

public:

	TTCellStaticContentWood(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits);
    ~TTCellStaticContentWood();
    
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
    
    Node*                       _parentWood;
    std::vector<Sprite*>        _cellWoodSprites;
    int                         _cellHits;

};

#endif  // __TTCellStaticContentWood_SCENE_H__
