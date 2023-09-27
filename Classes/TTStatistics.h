//
//  TTStatistics.h
//  animalcubes
//
//  Created by Albert Aymerich on 05/09/14.
//
//

#ifndef __animalcubes_TTStatistics_h__
#define __animalcubes_TTStatistics_h__

#include "thegame.h"
#include "TTGameInfo.h"
#include "network/HttpClient.h"

#define PRINT_STATISTICS false
#define SEND_DATA false
#define STATISTICS_SERVER_URL "http://httpbin.org/post"


void mapToJson( std::stringstream &ss, const std::string name, std::map<int,int> &data);
void mapVectorToJson( std::stringstream &ss, const std::string name, std::map<int,std::vector<int>> &data);
void vectorToJson( std::stringstream &ss, const std::string name, std::vector<int> &data);


// Clase singleton
class TTStatistics {
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTStatistics* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTStatistics(){}
    
public:
    
    ~TTStatistics();
    void init();
    
    void setDeviceID(std::string devideID);
    void setGameInfo(gameInfo* gameInfo);
    
private:
    
    //events
    void onEvent_GameFinishPlayScored(EventCustom* e);
    void onEvent_GameFinish(EventCustom* e);
    void onEvent_CreateObject(EventCustom* e);
    void onEvent_ObjectDown(EventCustom* e);
    
    std::string createJson();
    void printData(std::string datos);
    
    //network methods
    void sendData(std::string datos);
    
    void onHttpRequestCompleted(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
    
private:
 
    EventListener* _evPlayListener;
    EventListener* _evFinishListener;
    EventListener* _evObjectDownListener;
    
    //properties
 
    std::string _deviceID;
    
    gameInfo* _gameInfo;
    
    bool _gameAborted;
    
    int _numStars;

    std::map<int,int> _collectedFigures; // typeFigure -> #collected (removed from board)
    std::map<int,int> _totalFigures;  //typefigure -> #total of figures created
    
    std::map<int,int> _numMovStars;  //#start->#movement
    std::map<int,std::vector<int>> _numMovObjectAppeared;  //#typeobject -> vector of #movement
    std::map<int,std::vector<int>> _numMovObjectCollected;  //#typeobject -> vector of #movement
    
    //number of 3,4,5,6,7,8,9,10,...combinations
    std::map<int,int> _numCombinations;  //#combination -> accumulated value of that type
    
    
public:
    
    //properties
    
};


#endif //__animalcubes_TTStatistics_h__
