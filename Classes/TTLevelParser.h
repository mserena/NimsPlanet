#ifndef __TTLevelParser_SCENE_H__
#define __TTLevelParser_SCENE_H__

#include "thegame.h"
#include "pugixml.hpp"

#include "TTGameInfo.h"

#include "TTRestriction.h"
#include "TTTimeRestriction.h"
#include "TTMovesRestriction.h"

#include "TTGoal.h"
#include "TTScoreGoal.h"
#include "TTBlockGoal.h"
#include "TTStoneGoal.h"
#include "TTNetworkGoal.h"

using namespace cocos2d;

class CC_DLL TTLevelParser: public Ref
{

public:
    
    // Retorna la instancia Singleton de la clase
    static TTLevelParser* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:

	TTLevelParser(){}
    
public:
    
    ~TTLevelParser();
    gameInfo* parseLevel(std::string levelXML, bool basicInfo = false);
    
private:
    
    void parseBoard(pugi::xml_node Node, gameInfo* gameConfig);
    void parseRestrictionAndGoals(pugi::xml_node Node, gameInfo* gameConfig);
    void parseScores(pugi::xml_node Node, gameInfo* gameConfig);
    void parseBoosters(pugi::xml_node Node, gameInfo* gameConfig);
    void parseHowToPages(pugi::xml_node Node, gameInfo* gameConfig);
};

#endif  // __TTLevelParser_SCENE_H__
