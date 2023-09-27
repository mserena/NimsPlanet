#include "TTCellStaticContentGlue.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"

TTCellStaticContentGlue::TTCellStaticContentGlue(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId, bool _createOnRuntime):
TTCellStaticContent(cellContentParent,_zLayerPosition,cellStaticType,posXCell,posYCell,sizeCell,cellOwnerId)
{
    _affectAreaCombination = true;
    
    _cellGlueSprite = Sprite::createWithSpriteFrameName("pegote_sombra.png");
    float newScale = _sizeCell/_cellGlueSprite->getBoundingBox().size.width;
    _cellGlueSprite->setScale(newScale);
    _cellGlueSprite->setAnchorPoint(Vec2(0.5,0.5));
    _cellGlueSprite->setPosition(Point(_posXCell,_posYCell));
    _cellGlueSprite->setVisible(false);
    _parentContent->addChild(_cellGlueSprite);
    
    if(_createOnRuntime)
    {
        initCell();
        _cellGlueSprite->setScale(0);
        auto _scaleUp = ScaleTo::create(0.2f,newScale+0.1f);
        auto _scaleDown = ScaleTo::create(0.1f,newScale);
        _cellGlueSprite->runAction(Sequence::create(_scaleUp,_scaleDown, NULL));
    }
}

TTCellStaticContentGlue::~TTCellStaticContentGlue()
{

}

bool TTCellStaticContentGlue::isSelectable()
{
    return false;
}

bool TTCellStaticContentGlue::isFirstSelectable()
{
    return false;
}

bool TTCellStaticContentGlue::isRefillable()
{
    return false;
}

bool TTCellStaticContentGlue::isWallContent()
{
    return true;
}

bool TTCellStaticContentGlue::isStrongWallContent()
{
    return false;
}

void TTCellStaticContentGlue::initCell()
{
    TTCellStaticContent::initCell();
    _cellGlueSprite->setVisible(true);
}

void TTCellStaticContentGlue::createBreakAnimation(int idxSprite, float _directionPrefixed)
{
    TTAudioManager::getInstance()->playAudio("GLUE_SOUND");
    
    _cellGlueSprite->retain();
    _cellGlueSprite->removeFromParentAndCleanup(false);
    _parentContentsCell->addChild(_cellGlueSprite,LAYER_ANIMATIONS);
    _cellGlueSprite->setPosition(Point(_posXCell,_posYCell));
    _cellGlueSprite->release();
    
    auto _scaleUp = ScaleTo::create(0.1f,1.1f);
    auto _scaleDown = ScaleTo::create(0.2f,0);
    auto _removeGlue = CallFunc::create(std::bind([&](Sprite* cellGlueSprite){
        cellGlueSprite->removeFromParentAndCleanup(true);
    },_cellGlueSprite));
    _cellGlueSprite->runAction(Sequence::create(_scaleUp,_scaleDown,_removeGlue,NULL));
}

int TTCellStaticContentGlue::hitCell(int _numHits)
{
    createBreakAnimation(1,1);
    _contentIsDeath = true;
    return 0;
}
