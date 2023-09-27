//
//  TTUIShows.cpp
//  animalcubes
//
//  Created by Marc Serena on 09/10/14.
//
//

#include "TTUIShows.h"

/*
 * Show decrement
 */
TTUIShowDecrement::TTUIShowDecrement(cocos2d::ui::TextBMFont* label):
_finalValue(0),
_actualValue(0),
_label(label)
{
    
}

TTUIShowDecrement::~TTUIShowDecrement()
{
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this);
}

void TTUIShowDecrement::reset()
{
    _finalValue = 0;
    _actualValue = 0;
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this);
}

void TTUIShowDecrement::addNewDecrement(int finalValue)
{
    // Eliminamos la función del update
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this);
    
    _finalValue = finalValue;
    if (_finalValue<0)
    {
        _finalValue = 0;
    }
    _actualValue = std::stoi(_label->getString());
    
    if(_actualValue == _finalValue)
    {
        return;
    }
    
    //Agregamos la función al update
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this,0.1f,false);
}

void TTUIShowDecrement::decrementUpdate(float _time)
{
    if(_label == nullptr)
    {
        // Si ya no existe el label, eliminamos el update
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this);
    }
    
    _actualValue--;
    if (_actualValue>=0)
    {
        _label->setString(std::to_string(_actualValue));
    }
    
    if(_actualValue == _finalValue)
    {
        //Se ha terminado el decremento
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTUIShowDecrement::decrementUpdate),this);
    }
}

