//
//  TTHowToParser.h
//  animalplanet
//
//  Created by Luis Prieto on 11/11/14.
//
//

#ifndef animalplanet_TTHowToParser_h
#define animalplanet_TTHowToParser_h

#include "thegame.h"
#include "cocos2d.h"
#include "pugixml.hpp"

using namespace cocos2d;

typedef struct structTutorial
{
    std::string _id;
    std::string _text;
    std::string _img;
}structTutorial;


class CC_DLL TTHowToParser : public Ref
{
public:
    // Retorna la instancia Singleton de la clase
    static TTHowToParser* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTHowToParser(){}
    
public:
    
    ~TTHowToParser();
    void parseXML();
    std::string getTextFromTutorial(std::string tutorial);
    std::string getImgFromTutorial(std::string tutorial);
    
    std::vector<structTutorial> _tutorials;
    
};

#endif
