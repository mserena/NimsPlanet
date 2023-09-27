/*
 Copyright (C) 2012-2014 Soomla Inc.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "CCKeyValueStorage.h"
#include "CCCoreService.h"

namespace soomla {
    static CCKeyValueStorage *s_SharedKeyValueStorage = NULL;
    
    CCKeyValueStorage *CCKeyValueStorage::getInstance() {
        if (!s_SharedKeyValueStorage) {
            s_SharedKeyValueStorage = new CCKeyValueStorage();
            s_SharedKeyValueStorage->retain();
        }
        
        return s_SharedKeyValueStorage;
    }

    const char *CCKeyValueStorage::getValue(const char *key) const {
        return CCCoreService::getInstance()->kvStorageGetValue(key);
    }

    void CCKeyValueStorage::setValue(const char *key, const char *val) {
        return CCCoreService::getInstance()->kvStorageSetValue(key, val);
    }

    void CCKeyValueStorage::deleteKeyValue(const char *key) {
        CCCoreService::getInstance()->kvStorageDeleteKeyValue(key);
    }

    void CCKeyValueStorage::purge() {
        CCCoreService::getInstance()->kvStoragePurge();
    }
}