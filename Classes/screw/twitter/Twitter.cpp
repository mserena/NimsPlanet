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

#include "Twitter.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "TwitterApple.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "jni/screw/twitter/TwitterAndroid.h"
#endif

NS_SCREW_TWITTER_BEGIN

void Twitter::tweet(const string &message, const string &imagePath, const TwitterTweetCallback &callback) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    screw::ios::TwitterApple::tweet(message, imagePath, callback);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    screw::jni::TwitterAndroid::tweet(message, imagePath, callback);
#endif
}

NS_SCREW_TWITTER_END
