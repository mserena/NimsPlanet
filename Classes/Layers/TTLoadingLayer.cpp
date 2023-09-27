#include "TTLoadingLayer.h"
#include "TTUILoading.h"

TTLoadingLayer::TTLoadingLayer()
{
    
}

TTLoadingLayer::~TTLoadingLayer()
{
    CCLOG("Se destruye la escena de loading.");
}

void TTLoadingLayer::onEnter()
{
    Layer::onEnter();
    log("Scene TTLoadingLayer onEnter");
}

void TTLoadingLayer::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    log("Scene TTLoadingLayer onEnterTransitionDidFinish");
    
    TTUILoading::getInstance()->showLoading(this);
}

void TTLoadingLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    log("Scene TTLoadingLayer onExitTransitionDidStart");
}

void TTLoadingLayer::onExit()
{
    Layer::onExit();
    log("Scene TTLoadingLayer onExit");
}