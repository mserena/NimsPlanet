

#ifndef __CCDomain_H_
#define __CCDomain_H_

#include "cocos2d.h"

namespace soomla {
    class CCDomain: public cocos2d::Ref {
    public:
        virtual bool initWithDictionary(cocos2d::__Dictionary *dict) = 0;
        virtual cocos2d::__Dictionary *toDictionary() = 0;
        virtual ~CCDomain() {};
    protected:
        cocos2d::__Array *getDomainsFromDictArray(cocos2d::__Array *dictArray);
        cocos2d::__Array *getDictArrayFromDomains(cocos2d::__Array *domainArray);
        cocos2d::__Dictionary *putTypeData(cocos2d::__Dictionary *dict, const char* type);
    };
}

#endif //__CCDomain_H_
