#ifndef __TTCellStaticContentMud_SCENE_H__
#define __TTCellStaticContentMud_SCENE_H__

#include "cocos2d.h"
#include "TTCellStaticContent.h"

using namespace cocos2d;

class TTCellStaticContentMud: public TTCellStaticContent
{

public:

	TTCellStaticContentMud(cocos2d::Node* parentBoard,int _zLayerPosition,CONTENT_TYPES cellStaticType,int posXCell,int posYCell,int sizeCell,Vec2 cellOwnerId,int hits);
    ~TTCellStaticContentMud();
    
    //Acciones a la celda
    void initCell();
    void preSelectCell();
    void unPreSelectCell();
    int hitCell(int _numHits = 1);
    
    //Consultas a la celda
    bool isRefillable();
    bool isSelectable();
    bool isFirstSelectable();
    bool isWallContent();
    bool isStrongWallContent();
    
    void createBreakAnimation();
    
private:
    
    std::vector<Sprite*>       _cellMud;
    int                        _cellHits;

};

#endif  // __TTCellStaticContentMud_SCENE_H__
