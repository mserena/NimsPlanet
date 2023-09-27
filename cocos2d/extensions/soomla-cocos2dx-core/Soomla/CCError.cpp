

#include "CCError.h"

USING_NS_CC;

namespace soomla {
    #define CANNOT_EXTRACT "Cannot extract error data"
    #define JSON_ERROR_INFO "errorInfo"

    #define TAG "CCError"

    CCError *CCError::createWithObject(cocos2d::Ref *obj) {
        if (obj == nullptr) {
            return nullptr;
        }

        __Dictionary *dict = static_cast<__Dictionary *>(obj);
        if (dict->objectForKey(JSON_ERROR_INFO) == nullptr) {
            return nullptr;
        }

        std::string errorInfo;
        __String *errorInfoStr = dynamic_cast<__String *>(dict->objectForKey(JSON_ERROR_INFO));
        if (errorInfoStr) {
            errorInfo = errorInfoStr->getCString();
        } else {
            errorInfo = CANNOT_EXTRACT;
        }

        CCError *ret = new CCError();
        if (ret->init(errorInfo)) {
            ret->autorelease();
        } else {
            CC_SAFE_DELETE(ret);
        }
        return ret;
    }

    bool CCError::init(std::string &errorInfo) {
        mInfo = errorInfo;

        return true;
    }
}
