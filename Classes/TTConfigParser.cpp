#include "TTConfigParser.h"
#include "TTUserInfo.h"
#include "TTBoostersManager.h"

#define CONFIG_XML "Config.xml"
#define VERSION_XML "Version.xml"

//Singelton
static TTConfigParser *_sharedConfigParser = nullptr;

TTConfigParser* TTConfigParser::getInstance()
{
    if (!_sharedConfigParser)
    {
        _sharedConfigParser = new TTConfigParser();
        _sharedConfigParser->parseXML();
    }
    return _sharedConfigParser;
}

void TTConfigParser::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedConfigParser);
}

TTConfigParser::~TTConfigParser()
{
    
}

void TTConfigParser::parseXML()
{
    // Abrimos XML de versiones
    CCLOG("Parseando XML de configuración general");
    pugi::xml_document *XMLVersionFile = new pugi::xml_document();
    cocos2d::Data dataVersionXML = FileUtils::getInstance()->getDataFromFile(VERSION_XML);
    pugi::xml_parse_result ResultVersion = XMLVersionFile->load_buffer(dataVersionXML.getBytes(), dataVersionXML.getSize());
    if(!ResultVersion)
    {
        CCLOG("Problema al leer XML de versión de aplicación");
    }
    
    pugi::xml_node ConfigVersionNode = XMLVersionFile->child("Version");
    if(!ConfigVersionNode.empty())
    {
        _idVersionApp = ConfigVersionNode.child("appVersion").attribute("id").as_string();
    }
    
    // Abrimos XML configuracion
    CCLOG("Parseando XML de configuración general");
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(CONFIG_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer XML de configuración general");
        abort();
    }
    
    // Leemos XML de configuracion
    _generalConfig = new configGeneral();
	pugi::xml_node ConfigNode = XMLFile->child("Config");
	if(!ConfigNode.empty())
	{
        // Leemos los recursos generales
        readGeneralResources(ConfigNode.child("generalResources"));
        
        // Leemos los mundos
        readWorlds(ConfigNode.child("Worlds"));
        
        // Leemos los paquetes de caracteres
        readCharacterPacks((ConfigNode.child("CharacterPacks")));
	}
}

void TTConfigParser::readGeneralResources(pugi::xml_node generalResourcesNode)
{
    for(pugi::xml_node Node = generalResourcesNode.child("generalResource"); Node; Node = Node.next_sibling("generalResource"))
    {
        std::string type = Node.attribute("type").as_string();
        std::string src = Node.attribute("src").as_string();

        if(_generalConfig->_generalResources.find(type) == _generalConfig->_generalResources.end())
        {
            // Añadimos tipo y valor
            std::vector<std::string> resourcesVector;
            resourcesVector.push_back(src);
            _generalConfig->_generalResources.insert(std::make_pair(type,resourcesVector));
        }
        else
        {
            // Añadimos solo el valor
            _generalConfig->_generalResources.find(type)->second.push_back(src);
        }
    }
}

void TTConfigParser::readCharacterPacks(pugi::xml_node charactersNode)
{
    for(pugi::xml_node Node = charactersNode.child("CharacterPack"); Node; Node = Node.next_sibling("CharacterPack"))
    {
        characterPack _characterPack;
        
        // Leemos los recursos gráficos
        std::map<std::string,std::vector<std::string> > resources;
        pugi::xml_node resourcesNode = Node.child("CharacterPackGraphics");
        for(pugi::xml_node NodeResources = resourcesNode.child("CharacterPackGraphic"); NodeResources; NodeResources = NodeResources.next_sibling("CharacterPackGraphic"))
        {
            std::string type = NodeResources.attribute("type").as_string();
            std::string src = NodeResources.attribute("src").as_string();
            
            if(resources.find(type) == resources.end())
            {
                // Añadimos tipo y valor
                std::vector<std::string> resourcesVector;
                resourcesVector.push_back(src);
                resources.insert(std::make_pair(type,resourcesVector));
            }
            else
            {
                // Añadimos solo el valor
                resources.find(type)->second.push_back(src);
            }
        }
        _characterPack._graphs = resources;
        
        // Leemos la configuración
        pugi::xml_node ConfigNode = Node.child("CharacterPackConfigs");
        for(pugi::xml_node NodeResourcesConfig = ConfigNode.child("CharacterPackConfig"); NodeResourcesConfig; NodeResourcesConfig = NodeResourcesConfig.next_sibling("CharacterPackConfig"))
        {
            std::string type = NodeResourcesConfig.attribute("type").as_string();
            int value = NodeResourcesConfig.attribute("value").as_int();
            
            if(type == "figures_size_percentage") _characterPack._figuresSizePercentage = value;
        }
        
        //Leemos los sonidos
        std::map<std::string,std::string> sounds;
        pugi::xml_node soundsNode = Node.child("CharacterPackSounds");
        for(pugi::xml_node NodeResources = soundsNode.child("CharacterPackSound"); NodeResources; NodeResources = NodeResources.next_sibling("CharacterPackSound"))
        {
            std::string id = NodeResources.attribute("id").as_string();
            std::string value = NodeResources.attribute("value").as_string();
            sounds.insert(std::make_pair(id,value));
        }
        _characterPack._sounds = sounds;
        
        _generalConfig->_characters.push_back(_characterPack);
    }
}

void TTConfigParser::readWorlds(pugi::xml_node worldsNode)
{
    for(pugi::xml_node Node = worldsNode.child("World"); Node; Node = Node.next_sibling("World"))
    {
        configWorld _configWorld;

        std::vector<std::string> levels;
        pugi::xml_node levelsNode = Node.child("levels");
        for(pugi::xml_node NodeLevel = levelsNode.child("level"); NodeLevel; NodeLevel = NodeLevel.next_sibling("level"))
        {
            std::string srcLevel = NodeLevel.attribute("src").as_string();
            levels.push_back(srcLevel);
        }
        
        std::map<std::string,std::vector<std::string> > resoruces;
        pugi::xml_node resourcesNode = Node.child("worldResources");
        for(pugi::xml_node NodeResources = resourcesNode.child("worldResource"); NodeResources; NodeResources = NodeResources.next_sibling("worldResource"))
        {
            std::string type = NodeResources.attribute("type").as_string();
            std::string src = NodeResources.attribute("src").as_string();
            
            if(resoruces.find(type) == resoruces.end())
            {
                // Añadimos tipo y valor
                std::vector<std::string> resourcesVector;
                resourcesVector.push_back(src);
                resoruces.insert(std::make_pair(type,resourcesVector));
            }
            else
            {
                // Añadimos solo el valor
                resoruces.find(type)->second.push_back(src);
            }
        }
        
        int charactersPack = Node.attribute("CharacterPackId").as_int();
        _configWorld._charactersPack = charactersPack;
        
        //scores
        pugi::xml_node scoresNode = Node.child("scores");
        _configWorld._scoreDetahFigure = scoresNode.child("scoreDeathFigure").attribute("value").as_int();
        _configWorld._scoreDetahCombo = scoresNode.child("scoreDeathCombo").attribute("value").as_int();
        _configWorld._scoreDetahComboColor = scoresNode.child("scoreDeathComboColor").attribute("value").as_int();
        _configWorld._scoreOvni = scoresNode.child("scoreOvni").attribute("value").as_int();
        _configWorld._scoreCreateCombo = scoresNode.child("scoreCreateCombo").attribute("value").as_int();
        _configWorld._scoreCreateComboColor = scoresNode.child("scoreCreateComboColor").attribute("value").as_int();
        _configWorld._scoreBonusCatch = scoresNode.child("scoreBonusCatch").attribute("value").as_int();
        _configWorld._scoreBonusNoCatch = scoresNode.child("scoreBonusNoCatch").attribute("value").as_int();
        
        // Board boosters
        pugi::xml_node boardBoostersNode = Node.child("boardBoosters");
        for(pugi::xml_node childNode = boardBoostersNode.child("boardBooster"); childNode; childNode = childNode.next_sibling("boardBooster"))
        {
            // Busca si existe el key para boosters de tablero
            std::map<BOOSTERS_TYPES,std::vector<int>>::iterator _boostersItr = _configWorld._boosters.find(BOARD_BOOSTERS);
            if(_boostersItr == _configWorld._boosters.end())
            {
                std::vector<int> _boostersIDs;
                _boostersIDs.push_back(childNode.attribute("type").as_int());
                _configWorld._boosters.insert(std::make_pair(BOARD_BOOSTERS,_boostersIDs));
            }
            else
            {
                _boostersItr->second.push_back(childNode.attribute("type").as_int());
            }
        }
        
        //Level Boosters
        pugi::xml_node levelBoostersNode = Node.child("levelBoosters");
        for(pugi::xml_node childNode = levelBoostersNode.child("levelBooster"); childNode; childNode = childNode.next_sibling("levelBooster"))
        {
            // Busca si existe el key para boosters de tablero
            std::map<BOOSTERS_TYPES,std::vector<int>>::iterator _boostersItr = _configWorld._boosters.find(GAME_BOOSTERS);
            if(_boostersItr == _configWorld._boosters.end())
            {
                std::vector<int> _boostersIDs;
                _boostersIDs.push_back(childNode.attribute("type").as_int());
                _configWorld._boosters.insert(std::make_pair(GAME_BOOSTERS,_boostersIDs));
            }
            else
            {
                _boostersItr->second.push_back(childNode.attribute("type").as_int());
            }
        }
        
        _configWorld._levels = levels;
        _configWorld._resources = resoruces;
        _generalConfig->_worlds.push_back(_configWorld);
    }
}

std::string TTConfigParser::getLevelStr(int level)
{
    // Buscamos en que mundo esta el nivel
    int levelsCount = 0;
    for(int i = 0; i < _generalConfig->_worlds.size(); i++)
    {
        levelsCount += _generalConfig->_worlds[i]._levels.size();
        if(level <= levelsCount-1)
        {
            // Marcamos el nivel actual como seleccionado
            TTUserInfo::getInstance()->_userData->_selectedLevel = level;
            TTUserInfo::getInstance()->_userData->_selectedWorld = i;
            
            int levelWorld = level;
            if(i!=0)
            {
                levelWorld = level-_generalConfig->_worlds[i-1]._levels.size();
            }
            
            std::string _levelStr = _generalConfig->_worlds[i]._levels[levelWorld];
            return _levelStr;
        }
    }
    return "";
}
