
#ifndef __CCServiceManager_H_
#define __CCServiceManager_H_

#include "cocos2d.h"
#include "CCCoreService.h"

namespace soomla {
    class CCServiceManager: cocos2d::Ref {
    public:
        static CCServiceManager *getInstance();
        bool init();
        void setCommonParams(cocos2d::__Dictionary *commonParams);
    };
}

#endif //__CCServiceManager_H_
