//
//  BezierRotationTo.cpp
//  animalcubes
//
//  Created by Marc Serena on 03/10/14.
//
//

#include "TTBezierRotationTo.h"

BezierTo* TTBezierRotationTo::create(float t,ccBezierConfig k)
{
    TTBezierRotationTo* _bezierRotation = new TTBezierRotationTo;
    _bezierRotation->CCBezierTo::initWithDuration(t,k);
    return _bezierRotation;
}

void TTBezierRotationTo::update(float t)
{
    Point oldpos = getTarget()->getPosition();
    BezierTo::update(t);
    Point newpos = getTarget()->getPosition();
    float angle = atan2(-newpos.y + oldpos.y, newpos.x - oldpos.x) * 180 / M_PI;
    getTarget()->setRotation(angle+180);
}