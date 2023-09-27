/****************************************************************************
 Copyright (c) hiepndhut@gmail.com
 Copyright (c) 2014 No PowerUp Games
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef _SCREW_APPREQUESTS_H_
#define _SCREW_APPREQUESTS_H_

#include <iostream>
#include <vector>
#include "../macros.h"
#include "../data/Data.h"
#include "GraphObject.h"
#include "WebDialog.h"
#include "Request.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_SCREW_FACEBOOK;

using namespace std;

NS_SCREW_FACEBOOK_BEGIN

extern const string FacebookRequestsDidFetchNotification;

/* App Requests Manager
 *
 *
 */
class AppRequests {
    
public:
    static AppRequests *getInstance();
    
    Vector<GraphRequest *> getRequests();
    Vector<GraphRequest *> getRequests(int type);
    GraphRequest *getRequest(const string &rid);
    
    void clearRequest(GraphRequest *request);
    void clearRequest(const string &rid);
    
    void fetchAppRequests(const ApprequestsRequestCallback &callback = nullptr);
    
    void purgeData();
    
private:
    AppRequests();
    virtual ~AppRequests();
    
    void didFetchAppRequests(const Vector<GraphRequest *> &request);
    
    static AppRequests *_instance;
    data::Data *_data;
};

NS_SCREW_FACEBOOK_END

#endif /* _SCREW_APPREQUESTS_H_ */
