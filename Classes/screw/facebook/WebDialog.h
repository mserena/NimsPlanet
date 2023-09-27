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

#ifndef _SCREW_WEBDIALOG_H_
#define _SCREW_WEBDIALOG_H_

#include <iostream>
#include "../macros.h"
#include "cocos2d.h"
#include "GraphObject.h"

USING_NS_CC;
using namespace std;

NS_SCREW_FACEBOOK_BEGIN

typedef std::function<void(int error, ValueMap &values)> WebDialogCallback;
typedef std::function<void(int error, const string &requestId, const list<string> &recipients)> RequestDialogCallback;
typedef std::function<void(int error, const string &feedId)> FeedDialogCallback;

class WebDialog;

class WebDialogImpl {
public:
    virtual ~WebDialogImpl(){}
    virtual void show(WebDialog *dialog) = 0;
};

class WebDialog : public Object {
    
public:
    WebDialog();
    ~WebDialog();
    WebDialog(const string &dialog, const ValueMap &params, const WebDialogCallback &callback);
    static WebDialog *create(const string &dialog, const ValueMap &params, const WebDialogCallback &callback);
	
    const WebDialogCallback &getCallback() const;
    const string &getDialog() const;
    ValueMap &getParams();
    
	void setCallback(const WebDialogCallback &callback);
	void setDialog(const string &dialog);
	void setParams(const ValueMap &params);

    void show();
    
private:
    string _dialog;
    ValueMap _params;
    WebDialogCallback _callback;
    
    WebDialogImpl *_impl;
    
    /* Dialogs in progress */
    static Vector<WebDialog *> _dialogs;
};

/*
 * @doc https://developers.facebook.com/docs/games/requests
 */
class RequestDialogBuilder : public Object {
public:
    
    RequestDialogBuilder *setMessage(const string &message);
    RequestDialogBuilder *setFilters(const string &filters);
    RequestDialogBuilder *setTitle(const string &title);
    RequestDialogBuilder *setTo(const string &uid);
    RequestDialogBuilder *setTo(const list<string> &uids);
    RequestDialogBuilder *setType(int type);
    RequestDialogBuilder *setData(const string &key, const string &value);
    RequestDialogBuilder *setActionType(const string &actionType);
    RequestDialogBuilder *setObjectId(const string &objectId);
    RequestDialogBuilder *setCallback(const RequestDialogCallback &callback);
    
    WebDialog *build();
    
private:
    ValueMap _params;
    ValueMap _data;
    RequestDialogCallback _callback;
};

class FeedDialogBuilder : public Object {
public:
    
    FeedDialogBuilder *setName(const string &name);
    FeedDialogBuilder *setLink(const string &link);
    FeedDialogBuilder *setCaption(const string &cation);
    FeedDialogBuilder *setDescription(const string &description);
    FeedDialogBuilder *setTo(const string &uid);
    FeedDialogBuilder *setCallback(const FeedDialogCallback &callback);
    
    WebDialog *build();
    
private:
    ValueMap _params;
    FeedDialogCallback _callback;
};


NS_SCREW_FACEBOOK_END

#endif /* _SCREW_WEBDIALOG_H_ */
