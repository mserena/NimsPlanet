#ifndef __TTConfigParser_SCENE_H__
#define __TTConfigParser_SCENE_H__

#include "cocos2d.h"
#include "pugixml.hpp"
#include "TTBoostersManager.h"

using namespace cocos2d;

typedef struct configWorld
{
    std::map<std::string,std::vector<std::string> > _resources;
    std::vector<std::string> _levels;
    
    // Boosters
    std::map<BOOSTERS_TYPES,std::vector<int>> _boosters;
    
    // Characters
    int _charactersPack;
    
    // Scores geneeric data
    int _scoreDetahFigure;
    int _scoreDetahCombo;
    int _scoreDetahComboColor;
    int _scoreOvni;
    int _scoreCreateCombo;
    int _scoreCreateComboColor;
    int _scoreBonusCatch;
    int _scoreBonusNoCatch;
}configWorld;

typedef struct characterPack
{
    //Graphics
    std::map<std::string,std::vector<std::string> > _graphs;
    
    //Config
    int _figuresSizePercentage;
    
    //Sounds
    std::map<std::string,std::string > _sounds;
}characterPack;

typedef struct configGeneral
{
    std::map<std::string,std::vector<std::string> > _generalResources;
    std::vector<configWorld> _worlds;
    std::vector<characterPack> _characters;
}configGeneral;

class CC_DLL TTConfigParser: public Ref
{

public:
    
    // Retorna la instancia Singleton de la clase
    static TTConfigParser* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTConfigParser(){}
    
public:
    
    ~TTConfigParser();
    void parseXML();
    
    std::string getLevelStr(int level);
    
private:
    
    void readGeneralResources(pugi::xml_node generalResourcesNode);
    void readWorlds(pugi::xml_node worldsNode);
    void readCharacterPacks(pugi::xml_node charactersNode);
    
public:
    
    std::string         _idVersionApp;
    configGeneral*      _generalConfig;
    
};

#endif  // __TTConfigParser_SCENE_H__
