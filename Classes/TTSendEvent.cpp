//
//  TTSendEvent.cpp
//  animalcubes
//
//  Created by Albert Aymerich on 11/10/14.
//
//

#include "TTSendEvent.h"
#include "TTEventsManager.h"


TTSendEvent * TTSendEvent::create(int eventId,void *optionalUserData /*= nullptr*/)
{
	TTSendEvent *ret = new TTSendEvent();
    
	if (ret && ret->init(eventId,optionalUserData)) {
		ret->autorelease();
	}
	return ret;
}

bool TTSendEvent::init(int eventId,void *optionalUserData ) {
    _eventId = eventId;
    _optionalUserData = optionalUserData;
	return true;
}

void TTSendEvent::update(float time) {
    sendEvent(_eventId,_optionalUserData);
}

TTSendEvent *TTSendEvent::reverse() const
{
	// no reverse here, just return a clone
    return this->clone();
}

TTSendEvent * TTSendEvent::clone() const
{
	// no copy constructor
	auto a = new TTSendEvent();
	a->init(_eventId,_optionalUserData);
	a->autorelease();
	return a;
}