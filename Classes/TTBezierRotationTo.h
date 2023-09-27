//
//  BezierRotationTo.h
//  animalcubes
//
//  Created by Marc Serena on 03/10/14.
//
//

#ifndef __animalcubes__BezierRotationTo__
#define __animalcubes__BezierRotationTo__

#include "cocos2d.h"

using namespace cocos2d;

class TTBezierRotationTo: public BezierTo
{
public:
    
    static BezierTo* create(float t,ccBezierConfig k);
    virtual void update(float t);
};

#endif /* defined(__animalcubes__BezierRotationTo__) */
