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

#include "WebDialogApple.h"
#import <FacebookSDK/FacebookSDK.h>
#include "Helper.h"

NS_SCREW_IOS_BEGIN

void WebDialogApple::show(WebDialog *dialog) {
#if FB_DEBUG >= 1
    NSLog(@"DialogApple::show\n{\n\taction = %s\n\tparams = %s\n}", dialog->getDialog().c_str(), Value(dialog->getParams()).getDescription().c_str());
#endif
    ValueMap &params = dialog->getParams();
    params["frictionless"] = 1;
    [FBWebDialogs presentDialogModallyWithSession:[FBSession activeSession]
                                           dialog:[NSString stringWithUTF8String:dialog->getDialog().c_str()]
                                       parameters:screw::ios::Helper::valueMap2nsDictionary(params)
                                          handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
#if FB_DEBUG >= 1
                                              NSLog(@"DialogApple::DialogCallback - result = %d, url = %@, error = %@", result, resultURL, error);
#endif
                                              if (dialog->getCallback()) {
                                                  int errorCode = error.code;
                                                  NSMutableDictionary *queries = [NSMutableDictionary dictionary];
                                                  if (resultURL) {
                                                      NSString *queryStr = [resultURL query];
                                                      NSArray *array = [queryStr componentsSeparatedByString:@"&"];
                                                      
                                                      for (NSString *s : array) {
                                                          NSArray *ss = [s componentsSeparatedByString:@"="];
                                                          if (ss.count == 2) {
                                                              NSString *key = [ss[0] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                                                              queries[key] = ss[1];
                                                          }
                                                      }
                                                  }
                                                  ValueMap m(Helper::nsDictionary2ValueMap(queries));
                                                  dialog->getCallback()(errorCode, m);
                                              }
                                          }];
}

NS_SCREW_IOS_END