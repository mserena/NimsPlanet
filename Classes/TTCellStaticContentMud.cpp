#include "TTCellStaticContentMud.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"

TTCellStaticContentMud::TTCellStaticContentMud(cocos2d::Node* parentBoard,int _zLayerPosition,CONTENT_TYPES cellStaticType,int posXCell,int posYCell,int sizeCell,Vec2 cellOwnerId,int hits):
TTCellStaticContent(parentBoard,_zLayerPosition,cellStaticType,posXCell,posYCell,sizeCell,cellOwnerId),
_cellHits(hits)
{
    _affectAreaCombination = false;
  
    for(int i = 0; i < _cellHits; i++)
    {
        char strMud[100] = {0};
        sprintf(strMud,"mud%d.png",i+1);
        Sprite* _mudSprite = CCSprite::createWithSpriteFrameName(strMud);
        float newScale = _sizeCell/_mudSprite->getBoundingBox().size.width;
        _mudSprite->setPosition(Point(_posXCell,_posYCell));
        _mudSprite->setScale(newScale);
        _mudSprite->setVisible(false);
        _parentContent->addChild(_mudSprite);
        _cellMud.push_back(_mudSprite);
    }
}

TTCellStaticContentMud::~TTCellStaticContentMud()
{
}

bool TTCellStaticContentMud::isRefillable()
{
    return true;
}

bool TTCellStaticContentMud::isSelectable()
{
    return false;
}

bool TTCellStaticContentMud::isFirstSelectable()
{
    return false;
}

bool TTCellStaticContentMud::isWallContent()
{
    return false;
}

bool TTCellStaticContentMud::isStrongWallContent()
{
    return false;
}

void TTCellStaticContentMud::initCell()
{
    TTCellStaticContent::initCell();
    
    for(int i = 0; i < _cellMud.size(); i++)
        _cellMud[i]->setVisible(true);
}

int TTCellStaticContentMud::hitCell(int _numHits)
{
    int _hitsToDo = _numHits;
    for(int i = 0; i < _numHits; i++)
    {
        _hitsToDo--;
        if(_cellHits > 0)
        {
            _cellHits--;
            createBreakAnimation();
            if(_cellHits == 0)
            {
                sendEvent(EVENT_GOAL_NEW_BREAK);
                _contentIsDeath = true;
                break;
            }
        }
    }
    return _hitsToDo;
}

void TTCellStaticContentMud::createBreakAnimation()
{
    auto _moveMud = MoveTo::create(0.5f,Point(_posXCell,_posYCell-_sizeCell*3));
    
    // Alpha y rotación
    FadeTo* _fadeMud= FadeTo::create(0.5f,0);
    RotateBy* _rotateMud = RotateBy::create(0.5f,360);
    
     _cellMud[_cellHits]->retain();
     _cellMud[_cellHits]->removeFromParentAndCleanup(false);
     _parentContentsCell->addChild(_cellMud[_cellHits],LAYER_ANIMATIONS);
     _cellMud[_cellHits]->setPosition(Point(_posXCell,_posYCell));
     _cellMud[_cellHits]->release();
     _cellMud[_cellHits]->runAction(Spawn::create(_moveMud,Sequence::create(DelayTime::create(0.2f),_fadeMud,NULL),_rotateMud,NULL));
}

void TTCellStaticContentMud::preSelectCell()
{
    /*if(_cellHits==0) return;
    _cellPreSelectedMark->setVisible(true);*/
}

void TTCellStaticContentMud::unPreSelectCell()
{
    /*if(_cellHits==0) return;
    _cellPreSelectedMark->setVisible(false);*/
}
