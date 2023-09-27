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

//  Created by Igor Yegoroff on 5/16/13.

#ifndef __CCVirtualItem_H__
#define __CCVirtualItem_H__

#include "cocos2d.h"
#include "CCDomain.h"
#include "CCStoreConsts.h"
#include "CCSoomlaMacros.h"
#include "CCSoomlaEntity.h"

namespace soomla {
	/** 
     @class CCVirtualItem
     @brief A virtual item.

     This is the parent class of all virtual items in the application.
     Almost every entity in your virtual economy will be a virtual item. There 
     are many types of virtual items - each one of the various types extends
     `CCVirtualItem` and adds its own behavior on top of it.
	*/    
    class CCVirtualItem : public CCSoomlaEntity {
    public:

        cocos2d::__String *getItemId();

        virtual void save();
    };
    
}

#endif /* defined(__CCVirtualItem_H__) */
