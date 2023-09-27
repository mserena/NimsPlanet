#include "TTCellStaticContentWood.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"

TTCellStaticContentWood::TTCellStaticContentWood(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits):
TTCellStaticContent(cellContentParent,_zLayerPosition,cellStaticType,posXCell,posYCell,sizeCell,cellOwnerId),
_cellHits(hits)
{
    _affectAreaCombination = true;
    
    _parentWood = Node::create();
    if(_cellHits > MAX_HITS_WOOD) _cellHits = MAX_HITS_WOOD;
    
    // Cargamos las maderas iniciales
    for(int i = 0; i <= _cellHits; i++)
    {
        char strWood[100] = {0};
        sprintf(strWood,"wood_%d.png",i+1);
        Sprite* _woodSprite = CCSprite::createWithSpriteFrameName(strWood);
        float newScale = _sizeCell/_woodSprite->getBoundingBox().size.width;
        _woodSprite->setScale(newScale);
        _woodSprite->setAnchorPoint(Vec2(0.5,0.5));
        _woodSprite->setPosition(Point(_posXCell,_posYCell));
        _woodSprite->setVisible(false);
        _parentWood->addChild(_woodSprite,i);
        _cellWoodSprites.push_back(_woodSprite);
    }
    _parentContent->addChild(_parentWood);
}

TTCellStaticContentWood::~TTCellStaticContentWood()
{

}

bool TTCellStaticContentWood::isSelectable()
{
    return false;
}

bool TTCellStaticContentWood::isFirstSelectable()
{
    return false;
}

bool TTCellStaticContentWood::isRefillable()
{
    return false;
}

bool TTCellStaticContentWood::isWallContent()
{
    return true;
}

bool TTCellStaticContentWood::isStrongWallContent()
{
    return false;
}

void TTCellStaticContentWood::initCell()
{
    TTCellStaticContent::initCell();
    
    for(int i = 0; i < _cellWoodSprites.size(); i++)
        _cellWoodSprites[i]->setVisible(true);
}

void TTCellStaticContentWood::createBreakAnimation(int idxSprite, float _directionPrefixed)
{
    // Movimiento
    float _direction = 1;
    if(_directionPrefixed == 0)
    {
        int _randDir = rand()%2;
        if(_randDir == 0)
            _direction = -1;
    }
    else
    {
        _direction = _directionPrefixed;
    }
    
    Point _startPoint = Vec2(_posXCell,_posYCell);
    Point _finalPoint = Point(_startPoint.x+ ((_direction)*(_sizeCell*2)),_startPoint.y-(_sizeCell*2));
    ccBezierConfig bezierConfg;
    bezierConfg.controlPoint_1 = Point(_startPoint.x+((_direction)*(_sizeCell/2)),_startPoint.y+_sizeCell);
    bezierConfg.controlPoint_2 = Point(_startPoint.x+((_direction)*((_sizeCell/2)*3)),_startPoint.y+_sizeCell);
    bezierConfg.endPosition = _finalPoint;
    BezierTo* _bezierMove = BezierTo::create(0.5f, bezierConfg);
    
    // Alpha y rotación
    FadeTo* _fadeWood = FadeTo::create(0.5f,0);
    RotateBy* _rotateWood = RotateBy::create(0.5f,360);
    
    _cellWoodSprites[idxSprite]->retain();
    _cellWoodSprites[idxSprite]->removeFromParent();
    _parentContentsCell->addChild(_cellWoodSprites[idxSprite],LAYER_ANIMATIONS);
    _cellWoodSprites[idxSprite]->setPosition(Point(_posXCell,_posYCell));
    _cellWoodSprites[idxSprite]->release();
    _cellWoodSprites[idxSprite]->runAction(Spawn::create(_bezierMove,Sequence::create(DelayTime::create(0.2f),_fadeWood,NULL),_rotateWood,NULL));
}

int TTCellStaticContentWood::hitCell(int _numHits)
{
    for(int i = 0; i < _numHits;i++)
    {
        if(_cellHits == 1)
        {
            createBreakAnimation(1,1);
            createBreakAnimation(0,-1);
            sendEvent(EVENT_BOARD_WOOD_BREAK);
            _contentIsDeath = true;
            break;
        }
        else
        {
            createBreakAnimation(_cellHits);
        }
        _cellHits--;
    }
    return 0;
}
