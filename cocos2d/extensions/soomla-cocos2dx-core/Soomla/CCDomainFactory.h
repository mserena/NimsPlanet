
#ifndef __CCDomainFactory_H_
#define __CCDomainFactory_H_

#include "cocos2d.h"
#include "CCDomain.h"

namespace soomla {
    class CCDomainFactory {
    public:
        static CCDomainFactory* getInstance();
        void registerCreator(const char* key, std::function<soomla::CCDomain *(cocos2d::__Dictionary *)> creator);
        soomla::CCDomain *createWithDictionary(cocos2d::__Dictionary *dict);
        soomla::CCDomain *createWithDictionaryAndType(cocos2d::__Dictionary *dict, const char *type);
    private:
        std::map<std::string, std::function<soomla::CCDomain *(cocos2d::__Dictionary *)>> mCreators;
    };
}


#endif //__CCDomainFactory_H_
