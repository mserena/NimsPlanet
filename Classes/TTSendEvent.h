//
//  TTSendEvent.h
//  animalcubes
//
//  An action to send an event
//  Created by Albert Aymerich on 11/10/14.
//
//

#ifndef __animalcubes__TTSendEvent__
#define __animalcubes__TTSendEvent__

#include "cocos2d.h"

using namespace cocos2d;

/**
 @brief Sends an event
 */
class TTSendEvent : public ActionInstant
{
public:
	/** create the action */
	static TTSendEvent * create(int eventId,void *optionalUserData = nullptr);
    
	//
    // Override
    //
	virtual void update(float time) override;
	virtual TTSendEvent* clone() const override;
	virtual TTSendEvent* reverse() const override;
    
protected:
    TTSendEvent() : _optionalUserData(nullptr){}
    virtual ~TTSendEvent(){}
    
	/** init the action */
	bool init(int eventId,void *optionalUserData );
    
protected:
	int _eventId;
    void* _optionalUserData;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(TTSendEvent);
};


#endif /* defined(__animalcubes__TTSendEvent__) */
