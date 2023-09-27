

#ifndef __CCError_H_
#define __CCError_H_

#include "cocos2d.h"

namespace soomla {

    class CCError : public cocos2d::Ref {
    public:
        static CCError *createWithObject(cocos2d::Ref *obj);
        const char *getInfo() {return mInfo.c_str();};
    private:
		/**
		   Extra information about what went wrong
		 */
        std::string mInfo;

        CCError():
            mInfo("") {}

        bool init(std::string &errorInfo);
    };
};


#endif //__CCError_H_
