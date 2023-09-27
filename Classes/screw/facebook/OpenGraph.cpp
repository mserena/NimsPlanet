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

#include "OpenGraph.h"

NS_SCREW_FACEBOOK_BEGIN

void OpenGraphAction::setTags(const vector<string> &tags) {
    ValueVector vv;
    for (auto i : tags) {
        vv.push_back(Value(i));
    }
    
    set("tags", Value(vv));
}

vector<string> OpenGraphAction::getTags() {
    vector<string> ll;
    Value &v = get("tags");
    if (v.getType() == Value::Type::VECTOR) {
        for (auto i : v.asValueVector()) {
            ll.push_back(i.asString());
        }
    }
    return ll;
}

void ShareDialogParams::setFriends(const list<string> &friends) {
    ValueVector vv;
    for (auto i : friends) {
        vv.push_back(Value(i));
    }
    
    set("friends", Value(vv));
}

list<string> ShareDialogParams::getFriends() {
    list<string> ll;
    Value &v = get("friends");
    if (v.getType() == Value::Type::VECTOR) {
        for (auto i : v.asValueVector()) {
            ll.push_back(i.asString());
        }
    }
    return ll;
}

NS_SCREW_FACEBOOK_END
