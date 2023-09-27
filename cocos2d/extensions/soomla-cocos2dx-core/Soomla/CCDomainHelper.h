

#ifndef __CCDomainHelper_H_
#define __CCDomainHelper_H_

#include "cocos2d.h"
#include "CCDomain.h"

namespace soomla {
    class CCDomainHelper {
    public:
        static CCDomainHelper* getInstance();
        cocos2d::__Array *getDomainsFromDictArray(cocos2d::__Array *dictArray, const char *type = nullptr);
        cocos2d::__Array *getDictArrayFromDomains(cocos2d::__Array *domainArray);
    };
}


#endif //__CCDomainHelper_H_
