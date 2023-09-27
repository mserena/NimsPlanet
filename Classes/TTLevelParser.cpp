#include "TTLevelParser.h"
#include "TTConfigParser.h"
#include "TTUserInfo.h"

//Singelton
static TTLevelParser *_sharedLevelParser = nullptr;

TTLevelParser* TTLevelParser::getInstance()
{
    if (!_sharedLevelParser)
        _sharedLevelParser = new TTLevelParser();
    
    return _sharedLevelParser;
}

void TTLevelParser::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedLevelParser);
}

TTLevelParser::~TTLevelParser()
{
    
}

gameInfo* TTLevelParser::parseLevel(std::string levelXML, bool basicInfo)
{
    gameInfo* newGameInfo = new gameInfo();
    
    // Abrimos XML nieveles
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(levelXML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result)
    {
        CCLOG("Problema al leer XML (%s) de configuración de niveles",levelXML.c_str());
        abort();
        return newGameInfo;
    }
    
    // Leemos XML niveles
	pugi::xml_node LevelNodes = XMLFile->child("Levels");
	if(!LevelNodes.empty())
	{
        newGameInfo->level = LevelNodes.child("Level").attribute("id").as_int();
        
		for(pugi::xml_node Node = LevelNodes.child("Level"); Node; Node = Node.next_sibling("Level"))
		{
            //Leemos las condiciones del juego
            parseRestrictionAndGoals(Node,newGameInfo);
            parseBoosters(Node,newGameInfo);
            if(!basicInfo)
            {
                parseHowToPages(Node,newGameInfo);
                parseScores(Node,newGameInfo);
                parseBoard(Node,newGameInfo);
            }
       }
	}
    
	return newGameInfo;
}

void TTLevelParser::parseBoard(pugi::xml_node Node, gameInfo* gameConfig)
{
    // Leemos el mapa por filas del XML
    std::vector< std::vector<std::string>  > actualMap;
    for(pugi::xml_node childNode = Node.child("row"); childNode; childNode = childNode.next_sibling("row"))
    {
        const std::string rowDescriptorStr = childNode.attribute("rowDescriptor").value();
        std::vector<std::string> row = tokenizeStringCommaDelimited(rowDescriptorStr);
        actualMap.push_back(row);
    }
    
    // Convertimos el mapa a columnas
    std::vector< std::vector<std::string>  > actualMapColumns;
    for(int i = 0; i < actualMap[0].size(); i++)
    {
        std::vector<std::string> column;
        for( int j = 0; j < actualMap.size(); j++)
            column.push_back(actualMap[j][i]);
        actualMapColumns.push_back(column);
    }

    initBoardInfo boardInfo;
    
    // Mapa del tablero
    boardInfo.boardMap = actualMapColumns;
    
    // Fondo del tablero
    std::string bgStr = Node.child("background").attribute("src").value();
    boardInfo.background = bgStr;
    
    // Propiedades del tablero
    int maxCellsBoard = Node.child("BoardMaxCells").attribute("value").as_int();
    if(maxCellsBoard == 0) maxCellsBoard = 9;
    boardInfo.maxCellsBoard = maxCellsBoard;
    
    // Peso de las figuras
    std::vector<int> _figuresWeight;
    for(pugi::xml_node childNode = Node.child("figureWeight"); childNode; childNode = childNode.next_sibling("figureWeight"))
        _figuresWeight.push_back(childNode.attribute("weight").as_int());
    boardInfo.figuresWeight = _figuresWeight;
    
    gameConfig->boardInit = boardInfo;
}

void TTLevelParser::parseRestrictionAndGoals(pugi::xml_node Node, gameInfo* gameConfig)
{
    // Restrictions
    for(pugi::xml_node childNode = Node.child("restriction"); childNode; childNode = childNode.next_sibling("restriction"))
    {
        switch(childNode.attribute("type").as_int())
        {
            case TIME_RESTRICTION:
            {
                gameConfig->timeRestriction = new TTTimeRestriction(childNode.attribute("value").as_int(),false);
            }
            break;
     
            case MOVEMENTS_RESTRICTION:
            {
                gameConfig->moveRestriction = new TTMovesRestriction(childNode.attribute("value").as_int(),false);
            }
            break;
        }
     }
    
    // Goals
    for(pugi::xml_node childNode = Node.child("goal"); childNode; childNode = childNode.next_sibling("goal"))
    {
        switch(childNode.attribute("type").as_int())
        {
            case SCORE_GOAL:
            {
                gameConfig->goals.push_back(new TTScoreGoal(childNode.attribute("value").as_int(),childNode.attribute("optional").as_bool()));
            }
            break;
            
            case BREAKS_GOAL:
            {
                gameConfig->goals.push_back(new TTBreakGoal(childNode.attribute("value").as_int(),childNode.attribute("optional").as_bool()));
            }
            break;
            case DOWN_OBJECT_GOAL:
            {
                int numObjs = childNode.attribute("value").as_int();
                int initWeight = childNode.attribute("initWeight").as_int();
                int maxWeight = childNode.attribute("maxWeight").as_int();
                int increaseWeight = childNode.attribute("increaseWeight").as_int();
                
                gameConfig->goals.push_back(new TTDownObjectGoal(numObjs,initWeight,maxWeight,increaseWeight,childNode.attribute("optional").as_bool()));
            }
            break;
            case COLLECT_FIGURES_GOAL:
            {
                int figure = childNode.attribute("figure").as_int();
                int num = childNode.attribute("value").as_int();
                bool optional = childNode.attribute("optional").as_bool();
                gameConfig->goals.push_back(new TTCollectFiguresGoal(figure,num,optional));
            }
            break;
            case BLOCK_GOAL:
            {
                int num = childNode.attribute("value").as_int();
                bool optional = childNode.attribute("optional").as_bool();
                gameConfig->goals.push_back(new TTBlockGoal(num,optional));
            }
            break;
            case STONE_GOAL:
            {
                int num = childNode.attribute("value").as_int();
                bool optional = childNode.attribute("optional").as_bool();
                gameConfig->goals.push_back(new TTStoneGoal(num,optional));
            }
            break;
            case NETWORK_GOAL:
            {
                int num = childNode.attribute("value").as_int();
                bool optional = childNode.attribute("optional").as_bool();
                gameConfig->goals.push_back(new TTNetworkGoal(num,optional));
            }
            break;
        }
    }
}

void TTLevelParser::parseHowToPages(pugi::xml_node Node, gameInfo* gameConfig)
{
    // HowToPages
    const std::string pagesStr = Node.child("HowToPlay").attribute("pages").value();
    gameConfig->howToPages = (tokenizeStringCommaDelimitedToInt(pagesStr));
}

void TTLevelParser::parseScores(pugi::xml_node Node, gameInfo* gameConfig)
{
    gameConfig->_scoreDetahFigure = Node.child("scoreDeathFigure").attribute("value").as_int();
    gameConfig->_scoreDetahCombo = Node.child("scoreDeathCombo").attribute("value").as_int();
    gameConfig->_scoreDetahComboColor = Node.child("scoreDeathComboColor").attribute("value").as_int();
    gameConfig->_scoreOvni = Node.child("scoreOvni").attribute("value").as_int();
    gameConfig->_scoreCreateCombo = Node.child("scoreCreateCombo").attribute("value").as_int();
    gameConfig->_scoreCreateComboColor = Node.child("scoreCreateComboColor").attribute("value").as_int();
    gameConfig->_scoreBonusCatch = Node.child("scoreBonusCatch").attribute("value").as_int();
    gameConfig->_scoreBonusNoCatch = Node.child("scoreBonusNoCatch").attribute("value").as_int();
    
    // Score Podium
    const std::string scoreStarsStr = Node.child("stars").attribute("scores").value();
    gameConfig->scoreStars = (tokenizeStringCommaDelimitedToInt(scoreStarsStr));
}

void TTLevelParser::parseBoosters(pugi::xml_node Node, gameInfo* gameConfig)
{
    // Board Boosters
    pugi::xml_node boardBoostersNode = Node.child("boardBoosters");
    bool _hasBoardBoostersDefinition = !boardBoostersNode.empty();
    for(pugi::xml_node childNode = boardBoostersNode.child("boardBooster"); childNode; childNode = childNode.next_sibling("boardBooster"))
    {
        // Busca si existe el key para boosters de tablero
        std::map<BOOSTERS_TYPES,std::vector<int>>::iterator _boostersItr = gameConfig->_boosters.find(BOARD_BOOSTERS);
        if(_boostersItr == gameConfig->_boosters.end())
        {
            std::vector<int> _boostersIDs;
            _boostersIDs.push_back(childNode.attribute("type").as_int());
            gameConfig->_boosters.insert(std::make_pair(BOARD_BOOSTERS,_boostersIDs));
        }
        else
        {
            _boostersItr->second.push_back(childNode.attribute("type").as_int());
        }
    }
    
    //Game Boosters
    //Level Boosters
    pugi::xml_node levelBoostersNode = Node.child("levelBoosters");
    bool _hasLevelBoostersDefinition = !levelBoostersNode.empty();
    for(pugi::xml_node childNode = levelBoostersNode.child("levelBooster"); childNode; childNode = childNode.next_sibling("levelBooster"))
    {
        // Busca si existe el key para boosters de tablero
        std::map<BOOSTERS_TYPES,std::vector<int>>::iterator _boostersItr = gameConfig->_boosters.find(GAME_BOOSTERS);
        if(_boostersItr == gameConfig->_boosters.end())
        {
            std::vector<int> _boostersIDs;
            _boostersIDs.push_back(childNode.attribute("type").as_int());
            gameConfig->_boosters.insert(std::make_pair(GAME_BOOSTERS,_boostersIDs));
        }
        else
        {
            _boostersItr->second.push_back(childNode.attribute("type").as_int());
        }
    }

    // Si no se han definido boosters en el XML del juego, cogemos los generales
    configWorld* world = &TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld];
    if (!_hasBoardBoostersDefinition)
    {
        gameConfig->_boosters[BOARD_BOOSTERS] = world->_boosters[BOARD_BOOSTERS];
    }
    if (!_hasLevelBoostersDefinition)
    {
        gameConfig->_boosters[GAME_BOOSTERS] = world->_boosters[GAME_BOOSTERS];
    }
}