
#include "CCDomain.h"
#include "CCDomainHelper.h"
#include "CCCoreConsts.h"

cocos2d::__Array *soomla::CCDomain::getDomainsFromDictArray(cocos2d::__Array *dictArray) {
    return CCDomainHelper::getInstance()->getDomainsFromDictArray(dictArray);
}

cocos2d::__Array *soomla::CCDomain::getDictArrayFromDomains(cocos2d::__Array *domainArray) {
    return CCDomainHelper::getInstance()->getDictArrayFromDomains(domainArray);
}

cocos2d::__Dictionary *soomla::CCDomain::putTypeData(cocos2d::__Dictionary *dict, const char *type) {
    dict->setObject(cocos2d::__String::create(type), CCCoreConsts::JSON_JSON_TYPE);
    return dict;
}
