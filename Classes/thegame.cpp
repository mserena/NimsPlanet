//
//  thegame.cpp
//  animalcubes
//
//  Created by Albert Aymerich on 05/09/14.
//
//

#include "thegame.h" 

using namespace cocos2d;
using namespace cocostudio;

//SCALE function
float scale_widget (cocos2d::ui::Widget* widget, int width_space, int height_space )
{
    Size _screenSize = Director::getInstance()->getWinSize();
    Size _spaceForWidget = Size((width_space*_screenSize.width)/100,(height_space*_screenSize.height)/100);
    Size _widgetSize = widget->getBoundingBox().size;
    float _scaleWidth = _spaceForWidget.width/_widgetSize.width;
    float _scaleHeight = _spaceForWidget.height/_widgetSize.height;
    if(_scaleWidth < _scaleHeight) {
        return _scaleWidth;
    }
    return _scaleHeight;
    
}

float scale_sprite (Sprite* sprite, int width_space, int height_space )
{
    Size _screenSize = Director::getInstance()->getWinSize();
    Size _spaceForSprite = Size((width_space*_screenSize.width)/100,(height_space*_screenSize.height)/100);
    Size _spriteSize = sprite->getBoundingBox().size;
    float _scaleWidth = _spaceForSprite.width/_spriteSize.width;
    float _scaleHeight = _spaceForSprite.height/_spriteSize.height;
    if(_scaleWidth < _scaleHeight) {
        return _scaleWidth;
    }
    return _scaleHeight;
}

float scale_label_by_parent (Label* label, int width_space, int height_space )
{
    Size _parentSize = label->getParent()->getContentSize(); //Director::getInstance()->getWinSize();
    Size _spaceForLabel = Size((width_space*_parentSize.width)/100,(height_space*_parentSize.height)/100);
    Size _labelSize = label->getBoundingBox().size;
    float _scaleWidth = _spaceForLabel.width/_labelSize.width;
    float _scaleHeight = _spaceForLabel.height/_labelSize.height;
    if(_scaleWidth < _scaleHeight) {
        return _scaleWidth;
    }
    return _scaleHeight;
}

//TOKENIZER generic function
std::vector<std::string> tokenizeString(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> tokens;
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    
    while (std::string::npos != pos || std::string::npos != lastPos)
    {  // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return tokens;
}

//TOKENIZER by comma
std::vector<std::string> tokenizeStringCommaDelimited(const std::string& str)
{
    return tokenizeString(str, ", ");
}

//TOKENIZER  a string delimitted by comma to vector of integers
std::vector<int> tokenizeStringCommaDelimitedToInt(const std::string& str)
{
    std::vector<std::string> vals = tokenizeStringCommaDelimited(str);
    std::vector<int> res;
    std::transform(vals.begin(), vals.end(), std::back_inserter(res),
                   [](const std::string& str) { return std::stoi(str); });
    return res;
}

//TOKENIZER  a string delimitted by comma to vector of long
std::vector<long> tokenizeStringCommaDelimitedToLong(const std::string& str)
{
    std::vector<std::string> vals = tokenizeStringCommaDelimited(str);
    std::vector<long> res;
    std::transform(vals.begin(), vals.end(), std::back_inserter(res),
                   [](const std::string& str) { return std::stol(str); });
    return res;
}

//seconds to string time mm:ss
std::string secondsToStringTime(int time)
{
    int seconds = time%60;
    int minutes = (time/60)%60;
    char strMin[10] = {0};
    char strSec[10] = {0};
    
    sprintf(strMin,"%d:",minutes);
    if(seconds < 10)
        sprintf(strSec,"0%d",seconds);
    else
        sprintf(strSec,"%d",seconds);
    strcat(strMin,strSec);
    
    return strMin;
}

void removeAllActionsFromJSON(const char* jsonName)
{
    std::unordered_map<std::string, cocos2d::Vector<ActionObject*>> *_actionDic = &cocostudio::ActionManagerEx::getInstance()->_actionDic;
    std::unordered_map<std::string, cocos2d::Vector<ActionObject*>>::iterator iter;
    for (iter = _actionDic->begin(); iter != _actionDic->end(); iter++)
    {
        std::string _actualJsonName = iter->first;
        if(_actualJsonName != jsonName)
            continue;
        
        cocos2d::Vector<ActionObject*> objList = iter->second;
        int listCount = objList.size();
        for (int i = 0; i < listCount; i++)
        {
            ActionObject* action = objList.at(i);
            if (action != nullptr)
            {
                action->stop();
            }
        }
        objList.clear();
        _actionDic->erase(iter);
        
        return;
    }
}


std::string generateRanomAlphanumeric(int lenght)
{
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    
    char _alphanumStr[lenght];
    for (int i = 0; i < 20; ++i)
    {
        _alphanumStr[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return _alphanumStr;
}

timeStruct parseTimeToTimeStruct(long long int _timeToParse)
{
    time_t _seconds = _timeToParse/1000;
    struct tm * _timeinfo;
    _timeinfo = localtime(&_seconds);
    
    timeStruct _timeStruct;
    _timeStruct._month = _timeinfo->tm_mon;
    _timeStruct._day = _timeinfo->tm_mday;
    _timeStruct._hour = _timeinfo->tm_hour;
    _timeStruct._min = _timeinfo->tm_min;
    _timeStruct._seconds = _timeinfo->tm_sec;
    
    return _timeStruct;
}

