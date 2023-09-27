#include "TTCellStaticContentStone.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"
#include "TTParticleManager.h"

TTCellStaticContentStone::TTCellStaticContentStone(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits):
TTCellStaticContent(cellContentParent,_zLayerPosition,cellStaticType,posXCell,posYCell,sizeCell,cellOwnerId),
_cellHits(hits)
{
    _affectAreaCombination = true;
    
    _parentStone = Node::create();
    if(_cellHits > MAX_HITS_STONE) _cellHits = MAX_HITS_STONE;
    
    // Cargamos las maderas iniciales
    for(int i = 0; i < _cellHits; i++)
    {
        char strStone[100] = {0};
        sprintf(strStone,"stone_%d.png",i+1);
        Sprite* _stoneSprite = CCSprite::createWithSpriteFrameName(strStone);
        float newScale = _sizeCell/_stoneSprite->getBoundingBox().size.width;
        _stoneSprite->setScale(newScale);
        _stoneSprite->setAnchorPoint(Vec2(0.5,0.5));
        _stoneSprite->setPosition(Point(_posXCell,_posYCell));
        _stoneSprite->setVisible(false);
        _parentStone->addChild(_stoneSprite,i);
        _cellStoneSprites.push_back(_stoneSprite);
    }
    _parentContent->addChild(_parentStone);
}

TTCellStaticContentStone::~TTCellStaticContentStone()
{

}

bool TTCellStaticContentStone::isSelectable()
{
    return false;
}

bool TTCellStaticContentStone::isFirstSelectable()
{
    return false;
}

bool TTCellStaticContentStone::isRefillable()
{
    return false;
}

bool TTCellStaticContentStone::isWallContent()
{
    return true;
}

bool TTCellStaticContentStone::isStrongWallContent()
{
    return true;
}

void TTCellStaticContentStone::initCell()
{
    TTCellStaticContent::initCell();
    
    for(int i = 0; i < _cellStoneSprites.size(); i++)
        _cellStoneSprites[i]->setVisible(true);
}

void TTCellStaticContentStone::createBreakAnimation(int idxSprite)
{
    // Eliminamos el sprite
    _cellStoneSprites[idxSprite]->setVisible(false);
    _cellStoneSprites[idxSprite]->removeFromParent();
    
    ValueMap _stoneParticles = TTParticleManager::getInstance()->getParticle("stone_particles");
    ParticleSystemQuad* _stones = ParticleSystemQuad::create(_stoneParticles);
    _stones->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    _stones->setAutoRemoveOnFinish(true);
    Size _screenSize = Director::getInstance()->getWinSize();
    float _scaleFire = 1.5*_sizeCell/100;
    _stones->setScale(_scaleFire);
    _stones->setPosition(Point(_posXCell,_posYCell));
    _parentContentsCell->addChild(_stones,LAYER_ANIMATIONS);
    _stones->resetSystem();
}

int TTCellStaticContentStone::hitCell(int _numHits)
{
    for(int i = 0; i < _numHits;i++)
    {
        if(_cellHits == 1)
        {
            createBreakAnimation(0);
            sendEvent(EVENT_BOARD_STONE_BREAK);
            _contentIsDeath = true;
            break;
        }
        else
        {
            createBreakAnimation(_cellHits-1);
        }
        
        _cellHits--;
    }
    
    return 0;
}
