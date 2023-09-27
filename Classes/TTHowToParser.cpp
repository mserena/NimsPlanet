//
//  TTHowToParser.cpp
//  animalplanet
//
//  Created by Luis Prieto on 11/11/14.
//
//

#include "TTHowToParser.h"

#define HOWTO_XML "howTo.xml"
#define LANGUAGES_XML "Languages.xml"

//Singelton
static TTHowToParser *_sharedHowToParser = nullptr;

TTHowToParser* TTHowToParser::getInstance()
{
    if (!_sharedHowToParser)
    {
        _sharedHowToParser = new TTHowToParser();
        _sharedHowToParser->parseXML();
    }
    
    return _sharedHowToParser;
}

void TTHowToParser::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedHowToParser);
}

TTHowToParser::~TTHowToParser()
{
    
}

void TTHowToParser::parseXML()
{
    CCLOG("Parseando XML How To");

    
    // Abrimos XML HowTo
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(HOWTO_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer el XML HowTo");
        abort();
    }
    
    // Leemos XML HowTo
    pugi::xml_node howToNode = XMLFile->child("howTo");
    if(!howToNode.empty())
    {
        for(pugi::xml_node tutorialNode = howToNode.child("tutorial"); tutorialNode; tutorialNode = tutorialNode.next_sibling("tutorial"))
        {
            structTutorial tutorial;
            tutorial._id = tutorialNode.attribute("id").as_string();
            tutorial._text = tutorialNode.child("text").attribute("id").as_string();
            tutorial._img = tutorialNode.child("img").attribute("src").as_string();
            _tutorials.push_back(tutorial);
        }
    }

}

std::string TTHowToParser::getTextFromTutorial(std::string tutorial)
{
    std::string text = "";
    
    for(int i = 0; i < _tutorials.size(); i++)
    {
        if(_tutorials[i]._id == tutorial)
        {
            text = _tutorials[i]._text;
            break;
        }
    }
    
    return text;
}

std::string TTHowToParser::getImgFromTutorial(std::string tutorial)
{
    std::string img = "";
    
    for(int i = 0; i < _tutorials.size(); i++)
    {
        if(_tutorials[i]._id == tutorial)
        {
            img = _tutorials[i]._img;
            break;
        }
    }
    
    return img;
}
