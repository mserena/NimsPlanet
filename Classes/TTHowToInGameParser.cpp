//
//  TTHowToInGameParser.cpp
//  animalplanet
//
//  Created by Marc Serena on 18/3/15.
//
//

#include "TTHowToInGameParser.h"

#define HOWTO_INGAME_XML "howToInGame.xml"

//Singelton
static TTHowToInGameParser *_sharedHowToInGameParser = nullptr;

TTHowToInGameParser* TTHowToInGameParser::getInstance()
{
    if (!_sharedHowToInGameParser)
    {
        _sharedHowToInGameParser = new TTHowToInGameParser();
    }
    
    return _sharedHowToInGameParser;
}

void TTHowToInGameParser::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedHowToInGameParser);
}

TTHowToInGameParser::~TTHowToInGameParser()
{
    
}

void TTHowToInGameParser::parseXML()
{
    CCLOG("Parseando XML HowToInGame");
    
    // Abrimos XML HowTo
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(HOWTO_INGAME_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer el XML HowToInGame");
        abort();
    }
    
    // Leemos XML HowTo
    pugi::xml_node howToNode = XMLFile->child("howToInGame");
    if(!howToNode.empty())
    {
        for(pugi::xml_node tutorialNode = howToNode.child("tutorial"); tutorialNode; tutorialNode = tutorialNode.next_sibling("tutorial"))
        {
            // Obtenemos el id del nivel del tutorial
            int _levelTutorial = tutorialNode.attribute("levelId").as_int();
            
            std::vector<stepTutorial> _stepsTutorial;
            // Leemos todos los steps del tutorial
            pugi::xml_node _stepsTutorialNode = tutorialNode.child("tutorialSteps");
            for(pugi::xml_node tutorialStep = _stepsTutorialNode.child("tutorialStep"); tutorialStep; tutorialStep = tutorialStep.next_sibling("tutorialStep"))
            {
                // Leemos las celdas permitidas
                std::vector<cellIdStruct> _cellsAllowedStep;
                pugi::xml_node _stepsTutorialCellsAllowedNode = tutorialStep.child("cellsAllowed");
                for(pugi::xml_node cellAllowed = _stepsTutorialCellsAllowedNode.child("cellAllowed"); cellAllowed; cellAllowed = cellAllowed.next_sibling("cellAllowed"))
                {
                    std::string _cellIdStr = cellAllowed.attribute("id").as_string();
                    std::vector<int> _cellIdInt = tokenizeStringCommaDelimitedToInt(_cellIdStr);
                    
                    cellIdStruct _cellId;
                    _cellId._col = _cellIdInt[0];
                    _cellId._row = _cellIdInt[1];
                    
                    _cellsAllowedStep.push_back(_cellId);
                }
                
                //Leemos las celdas no permitidas visibles
                std::vector<cellIdStruct> _cellsNoAllowedVisibleStep;
                pugi::xml_node _stepsTutorialCellsNoAllowedNode = tutorialStep.child("cellsVisibleNoAllowed");
                for(pugi::xml_node cellNoAllowed = _stepsTutorialCellsNoAllowedNode.child("cellVisibleNoAllowed"); cellNoAllowed; cellNoAllowed = cellNoAllowed.next_sibling("cellVisibleNoAllowed"))
                {
                    std::string _cellIdStr = cellNoAllowed.attribute("id").as_string();
                    std::vector<int> _cellIdInt = tokenizeStringCommaDelimitedToInt(_cellIdStr);
                    
                    cellIdStruct _cellId;
                    _cellId._col = _cellIdInt[0];
                    _cellId._row = _cellIdInt[1];
                    
                    _cellsNoAllowedVisibleStep.push_back(_cellId);
                }
                
                stepTutorial _step;
                _step._cellsAllowed = _cellsAllowedStep;
                _step._cellsNoAllowedVisible = _cellsNoAllowedVisibleStep;
                
                // Leemos el identificador del texto
                std::string _textIdStep = tutorialStep.child("text").attribute("id").as_string();
                _step._textId = _textIdStep;
                
                _stepsTutorial.push_back(_step);
            }
            
            _mapTutorialInGame.insert(std::make_pair(_levelTutorial,_stepsTutorial));
        }
    }
}

