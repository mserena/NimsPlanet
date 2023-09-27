#ifndef __TTCellStaticContentStone_SCENE_H__
#define __TTCellStaticContentStone_SCENE_H__

#include "cocos2d.h"
#include "TTCellStaticContent.h"

#define MAX_HITS_STONE    2

using namespace cocos2d;

class TTCellStaticContentStone: public TTCellStaticContent
{

public:

	TTCellStaticContentStone(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits);
    ~TTCellStaticContentStone();
    
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
    
    void createBreakAnimation(int idxSprite);
    
private:    
    Node*                       _parentStone;
    std::vector<Sprite*>        _cellStoneSprites;
    int                         _cellHits;

};

#endif  // __TTCellStaticContentStone_SCENE_H__
