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

#ifndef _SCREW_FILEUTILS_H_
#define _SCREW_FILEUTILS_H_

#include "../macros.h"
#include "cocos2d.h"
#include <string>

NS_SCREW_UTILS_BEGIN
using namespace std;

class FileUtils {
public:
	static string getResourcePath(const string &path);
    static string getDocumentPath(const string &path);
    
    /* Create a director in writable directory */
    static bool createDirectory(const string &dirname);
    
    static bool isFileExist(const string &fileOrDirectory);
    
    static bool createFile(const unsigned char *buff, long size, const string &filepath);
    static bool createFile(const char *buff, const string &filepath);
};

NS_SCREW_UTILS_END

#endif /* _SCREW_FILEUTILS_H_ */
