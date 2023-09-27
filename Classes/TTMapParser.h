#ifndef __TTMapParser_SCENE_H__
#define __TTMapParser_SCENE_H__

#include "thegame.h"
#include "cocos2d.h"
#include "pugixml.hpp"

typedef struct mapAnimation
{
    int                 _animType;
    std::string         _animResource;
    std::vector<int>    _animPosition;
    
    int               _timeRepeat;
}mapAnimation;

typedef struct mapInfo
{
    int _mapPieces;
    std::string _mapPrefix;
    std::string _mapSuffix;
    
    Size _mapDesignResolution;
    std::vector<std::vector<int> >  _cellsPositions;
    std::vector<int>                _mapStoppers;
    std::vector<mapAnimation>       _mapAnimations;
}mapInfo;

using namespace cocos2d;

class CC_DLL TTMapParser: public Ref
{

public:
    
    // Retorna la instancia Singleton de la clase
    static TTMapParser* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTMapParser(){}
    
public:
    
    ~TTMapParser();
    void parseXML();
    
    std::vector<int> getActiveMapStoppers();
    
private:
    
    void readMapPieces(pugi::xml_node mapPiecesNode);
    void readMapStoppers(pugi::xml_node mapStoppersNode);
    void readMapCells(pugi::xml_node mapCellsNode);
    void readMapAnimations(pugi::xml_node mapAnimationsNode);
    
public:
    
    mapInfo* _mapInfo;
};

#endif  // __TTMapParser_SCENE_H__
