#include "TTCellStaticContent.h"
#include "TTEventsManager.h"
#include "TTCell.h"

TTCellStaticContent::TTCellStaticContent(cocos2d::Node* parentCell,int _zLayerPosition,CONTENT_TYPES cellStaticType,int posXCell,int posYCell,int sizeCell,Vec2 ownerCellId):
TTCellContent(cellStaticType),
_posXCell(posXCell),
_posYCell(posYCell),
_sizeCell(sizeCell),
_emptyContent(true),
_cellStaticType(cellStaticType),
_ownerCellId(ownerCellId),
_cellStaticState(STATIC_CONTENT_STATE_NOTHING)
{
    _parentContentsCell = parentCell;
    _parentContent = Node::create();
    _parentContent->setVisible(true);
    _parentContentsCell->addChild(_parentContent,_zLayerPosition);
}

TTCellStaticContent::~TTCellStaticContent()
{
    _parentContent->removeFromParentAndCleanup(true);
}

void TTCellStaticContent::initCell()
{

}

int TTCellStaticContent::getZContent()
{
    return _parentContent->getZOrder();
}

bool TTCellStaticContent::isEmptyCell()
{
    /*if(_cellStaticType == EMPTY_STATIC_CONTENT)
        return true;*/
    return false;
}

bool TTCellStaticContent::isMovableContent()
{
    return false;
}
