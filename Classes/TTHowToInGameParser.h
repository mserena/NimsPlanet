//
//  TTHowToInGameParser.h
//  animalplanet
//
//  Created by Marc Serena on 18/3/15.
//
//

#ifndef __animalplanet__TTHowToInGameParser__
#define __animalplanet__TTHowToInGameParser__

#include "thegame.h"
#include "cocos2d.h"
#include "pugixml.hpp"

using namespace cocos2d;

typedef struct cellIdStruct
{
    int _col;
    int _row;
}cellIdStruct;

typedef struct stepTutorial
{
    std::vector<cellIdStruct>   _cellsNoAllowedVisible;
    std::vector<cellIdStruct>   _cellsAllowed;
    std::string                 _textId;
}stepTutorial;


class CC_DLL TTHowToInGameParser : public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTHowToInGameParser* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTHowToInGameParser(){}
    
public:
    
    ~TTHowToInGameParser();
    void parseXML();
    
    std::map<int, std::vector<stepTutorial>> _mapTutorialInGame;
    
};

#endif /* defined(__animalplanet__TTHowToInGameParser__) */
