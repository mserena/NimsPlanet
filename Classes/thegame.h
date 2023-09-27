//
//  thegame.h
//  animalcubes
//
//  Created by Albert Aymerich on 05/09/14.
//  to include instead of cocos2d.h
//  a place where to put global macros, functions, ...
//

#ifndef animalcubes_thegame_h
#define animalcubes_thegame_h

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "cocostudio/CocoStudio.h"

// Custom Actions
#include "TTAction.h"

#ifndef COCOS2D_DEBUG  //en modo release, nunca esta definido
    #define TEST_GAME 1
#else 
    #define TEST_GAME 1
#endif


//random function
struct RNG {
    int operator() (int n) {
        return std::rand() / (1.0 + RAND_MAX) * n;
    }
};

// struc para el tiempo
typedef struct timeStruct
{
    int _month;
    int _day;
    int _hour;
    int _min;
    int _seconds;
}timeStruct;

//SCALE function
float scale_widget (cocos2d::ui::Widget* widget, int width_space, int height_space );
float scale_sprite (Sprite* sprite, int width_space, int height_space );
float scale_label_by_parent (Label* label, int width_space, int height_space );

//TOKENIZER generic function
std::vector<std::string> tokenizeString(const std::string& str, const std::string& delimiters);

//TOKENIZER by comma
std::vector<std::string> tokenizeStringCommaDelimited(const std::string& str);

//TOKENIZER  a string delimitted by comma to vector of integers
std::vector<int> tokenizeStringCommaDelimitedToInt(const std::string& str);

//TOKENIZER  a string delimitted by comma to vector of long
std::vector<long> tokenizeStringCommaDelimitedToLong(const std::string& str);

//seconds to string time mm:ss
std::string secondsToStringTime(int time);

//Elimina todas las acciones de cocostudio::ActionManagerEx presentes en un JSON
void removeAllActionsFromJSON(const char* jsonName);

//Genera una cadena alfanumérica random
std::string generateRanomAlphanumeric(int lenght);

// Operaciones de tiempo
timeStruct parseTimeToTimeStruct(long long int _timeToParse);

#endif
