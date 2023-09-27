#include "TTCellStaticContentNetwork.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTAudioManager.h"
#include "TTParticleManager.h"

TTCellStaticContentNetwork::TTCellStaticContentNetwork(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 cellOwnerId,int hits):
TTCellStaticContent(cellContentParent,_zLayerPosition,cellStaticType,posXCell,posYCell,sizeCell,cellOwnerId),
_cellHits(hits)
{
    _affectAreaCombination = false;
    
    _spriteNetwork = CCSprite::createWithSpriteFrameName("network.png");
    float newScale = _sizeCell/_spriteNetwork->getBoundingBox().size.width;
    _spriteNetwork->setScale(newScale);
    _spriteNetwork->setAnchorPoint(Vec2(0.5,0.5));
    _spriteNetwork->setVisible(false);
    _spriteNetwork->setPosition(Point(_posXCell,_posYCell));
    _parentContent->addChild(_spriteNetwork,1);
    
    _visualHitsToDoBkg = CCSprite::createWithSpriteFrameName("networklabel.png");
    _visualHitsToDoBkg->setScale(newScale*1.3f);
    _visualHitsToDoBkg->setAnchorPoint(Vec2(0.5,0.5));
    _visualHitsToDoBkg->setVisible(false);
    _visualHitsToDoBkg->setPosition(Point(_posXCell+(sizeCell/3.5f),_posYCell-(sizeCell/3.5f)));
    _parentContent->addChild(_visualHitsToDoBkg,2);
    
    _visualHitsToDo = cocos2d::ui::TextBMFont::create("0","comicNums.fnt");
    _visualHitsToDo->setAnchorPoint(Vec2(0.5,0.5));
    float _scaleVisualHits = 0.40*_visualHitsToDoBkg->getContentSize().width*_visualHitsToDoBkg->getScale()/_visualHitsToDo->getContentSize().width;
    _visualHitsToDo->setScale(_scaleVisualHits);
    _visualHitsToDo->setVisible(true);
    _visualHitsToDo->setPosition(Point(_posXCell+(sizeCell/3.4f),_posYCell-(sizeCell/3.3f)));
    _visualHitsToDo->setString(std::to_string(_cellHits));
    _parentContent->addChild(_visualHitsToDo,3);
}

TTCellStaticContentNetwork::~TTCellStaticContentNetwork()
{

}

bool TTCellStaticContentNetwork::isSelectable()
{
    return true;
}

bool TTCellStaticContentNetwork::isFirstSelectable()
{
    return false;
}

bool TTCellStaticContentNetwork::isRefillable()
{
    return false;
}

bool TTCellStaticContentNetwork::isWallContent()
{
    return true;
}

bool TTCellStaticContentNetwork::isStrongWallContent()
{
    return false;
}

void TTCellStaticContentNetwork::initCell()
{
    TTCellStaticContent::initCell();
    _spriteNetwork->setVisible(true);
    _visualHitsToDo->setVisible(true);
    _visualHitsToDoBkg->setVisible(true);
}

void TTCellStaticContentNetwork::createBreakAnimation()
{
    CCLOG("Contenido red eliminado!");
    
    _visualHitsToDo->setVisible(false);
    _visualHitsToDoBkg->setVisible(false);
    
    _spriteNetwork->retain();
    _spriteNetwork->removeFromParentAndCleanup(false);
    _parentContentsCell->addChild(_spriteNetwork,LAYER_ANIMATIONS);
    _spriteNetwork->setPosition(Point(_posXCell,_posYCell));
    _spriteNetwork->release();
    
    auto _scaleUp = ScaleTo::create(0.1f,1.1f);
    auto _scaleDown = ScaleTo::create(0.2f,0);
    auto _removeNetWork = CallFunc::create(std::bind([&](Sprite* spriteNetwork){
        spriteNetwork->removeFromParentAndCleanup(true);
    },_spriteNetwork));
    _spriteNetwork->runAction(Sequence::create(_scaleUp,_scaleDown,_removeNetWork,NULL));
}

void TTCellStaticContentNetwork::setHitsLabel(std::string _hits)
{
    _visualHitsToDo->setString(_hits);
}

int TTCellStaticContentNetwork::hitCell(int _numHits)
{
    for(int i = 0; i < _numHits;i++)
    {
        if(_cellHits == 1)
        {
            createBreakAnimation();
            sendEvent(EVENT_BOARD_NETWORK_BREAK);
            _contentIsDeath = true;
            _cellHits--;
            break;
        }
        _cellHits--;
        _visualHitsToDo->setString(std::to_string(_cellHits));
    }
    
    TTAudioManager::getInstance()->playAudio("NETWORK_BREAK");
    setHitsLabel(std::to_string(_cellHits));
    
    // La red siempre retorna 0 hits, ya que como mucho se elimina
    return 0;
}
