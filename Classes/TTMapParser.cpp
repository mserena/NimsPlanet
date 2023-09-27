#include "TTMapParser.h"
#include "TTMapStopperManager.h"
#include "TTUserInfo.h"

#define CONFIG_XML "Map.xml"

//Singelton
static TTMapParser *_sharedMapParser = nullptr;

TTMapParser* TTMapParser::getInstance()
{
    if (!_sharedMapParser)
    {
        _sharedMapParser = new TTMapParser();
        _sharedMapParser->parseXML();
    }
    
    return _sharedMapParser;
}

void TTMapParser::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedMapParser);
}

TTMapParser::~TTMapParser()
{
    
}

void TTMapParser::parseXML()
{
    CCLOG("Parseando XML del mapa");
    
    // Abrimos XML configuracion
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(CONFIG_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer el XML del mapa");
        abort();
    }
    
    // Leemos XML idiomas
    _mapInfo = new mapInfo();
	pugi::xml_node ConfigNode = XMLFile->child("mapConfig");
	if(!ConfigNode.empty())
	{
        // Leemos la resolución de diseño
        std::vector<int> _resolution = tokenizeStringCommaDelimitedToInt(ConfigNode.child("mapDesignResolution").attribute("resolution").as_string());
        _mapInfo->_mapDesignResolution = Size(_resolution[0],_resolution[1]);
        
        // Leemos los fragmenos del mapa
        readMapPieces(ConfigNode.child("mapPieces"));
        // Leemos los stoppers de mapa
        readMapStoppers(ConfigNode.child("mapStopper"));
        // Leemos las posiciones de las casillas
        readMapCells(ConfigNode.child("mapLevels"));
        // Leemos las animaciones del mapa
        readMapAnimations(ConfigNode.child("mapAnimations"));
	}
}

void TTMapParser::readMapStoppers(pugi::xml_node mapStoppersNode)
{
    for(pugi::xml_node Node = mapStoppersNode.child("stopper"); Node; Node = Node.next_sibling("stopper"))
    {
        _mapInfo->_mapStoppers.push_back(Node.attribute("level").as_int());
    }
}

void TTMapParser::readMapPieces(pugi::xml_node mapPiecesNode)
{
    _mapInfo->_mapPieces = mapPiecesNode.attribute("numPieces").as_int();
    _mapInfo->_mapPrefix = mapPiecesNode.attribute("prefixName").as_string();
    _mapInfo->_mapSuffix = mapPiecesNode.attribute("suffixName").as_string();
}

void TTMapParser::readMapCells(pugi::xml_node mapCellsNode)
{
    for(pugi::xml_node Node = mapCellsNode.child("level"); Node; Node = Node.next_sibling("level"))
    {
        std::vector<int> position = tokenizeStringCommaDelimitedToInt(Node.attribute("position").as_string());
        _mapInfo->_cellsPositions.push_back(position);
    }
}

void TTMapParser::readMapAnimations(pugi::xml_node mapAnimationsNode)
{
    for(pugi::xml_node Node = mapAnimationsNode.child("mapAnimation"); Node; Node = Node.next_sibling("mapAnimation"))
    {
        mapAnimation _anim;
        _anim._animType = Node.attribute("type").as_int();
        _anim._animResource = Node.attribute("resource").as_string();
        _anim._animPosition = tokenizeStringCommaDelimitedToInt(Node.attribute("position").as_string());
        _anim._timeRepeat = Node.attribute("timeRepeat").as_int();
        _mapInfo->_mapAnimations.push_back(_anim);
    }
}

std::vector<int> TTMapParser::getActiveMapStoppers()
{
    std::vector<int> _stoppersDefined = _mapInfo->_mapStoppers;
    std::vector<int> _stoppersUnloked = TTMapStopperManager::getInstance()->getStoppersUnlokedInfo();
    TTMapStopperManager::destroyInstance();
    
    for(int i = 0; i < _stoppersDefined.size(); i++)
    {
        if(std::find(_stoppersUnloked.begin(), _stoppersUnloked.end(), _stoppersDefined[i]) != _stoppersUnloked.end()
           || _stoppersDefined[i] < TTUserInfo::getInstance()->getActualUserProgress()._actualLevel)
        {
            _stoppersDefined.erase(_stoppersDefined.begin()+i);
            i--;
        }
    }
    
    return _stoppersDefined;
}