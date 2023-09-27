//
//  TTStatistics.cpp
//  animalcubes
//
//  Created by Albert Aymerich on 05/09/14.
//
//

#include "TTStatistics.h"
#include "TTGame.h"
#include "network/Httpclient.h"

//helper functions
void mapToJson( std::stringstream &ss, const std::string name, std::map<int,int> &data)
{
    //map
    ss<<"\""<<name<<"\": {";
    auto final_iter = --data.end();
    for (auto it=data.begin(); it!=data.end(); ++it) {
        ss<<"\""<< it->first <<"\":"<<it->second;
        if (it!=final_iter) ss<<",";
    }
    ss<<"}";
}

void mapVectorToJson( std::stringstream &ss, const std::string name, std::map<int,std::vector<int>> &data)
{
    //map
    ss<<"\""<<name<<"\": {";
    auto final_iter = --data.end();
    for (auto it=data.begin(); it!=data.end(); ++it) {
        vectorToJson(ss, std::to_string(it->first), it->second);
        if (it!=final_iter) ss<<",";
    }
    ss<<"}";
}

void vectorToJson( std::stringstream &ss, const std::string name, std::vector<int> &data)
{
    ss<<"\""<< name <<"\": [";
    auto final_vect = --data.end();
    for(auto it2 = data.begin(); it2!=data.end(); ++it2) {
        ss<<*it2;
        if (it2!=final_vect) ss<<",";
    }
    ss<<"]";
}



//Singleton
static TTStatistics* _sharedStatistics = nullptr;

TTStatistics* TTStatistics::getInstance()
{
    if (!_sharedStatistics)
    {
        _sharedStatistics = new TTStatistics();
        _sharedStatistics->_evPlayListener = nullptr;
        _sharedStatistics->_evFinishListener = nullptr;
    }
    
    return _sharedStatistics;
}

void TTStatistics::destroyInstance()
{
    if(_sharedStatistics) {
        delete(_sharedStatistics);
        _sharedStatistics = nullptr;
    }
}

void TTStatistics::init()
{
    CCLOG("Initializing game statistics");
    // Registro de recepcion de eventos
    
    //para evitar posibles dobles registros
    removeSafeListenerEvent(_evPlayListener);
    removeSafeListenerEvent(_evFinishListener);
    removeSafeListenerEvent(_evObjectDownListener);
    _evPlayListener = registerListenerEvent(EVENT_GAME_FINISH_PLAY_SCORED,CC_CALLBACK_1(TTStatistics::onEvent_GameFinishPlayScored,this));
    _evFinishListener = registerListenerEvent(EVENT_GAME_FINISH,CC_CALLBACK_1(TTStatistics::onEvent_GameFinish,this));
    _evObjectDownListener = registerListenerEvent(EVENT_OBJECT_DOWN,CC_CALLBACK_1(TTStatistics::onEvent_ObjectDown,this));
 
    registerListenerEvent(EVENT_CREATE_OBJECT ,CC_CALLBACK_1(TTStatistics::onEvent_CreateObject,this));


    _gameInfo = nullptr;
    _gameAborted = false;
    _numStars=0;
    _collectedFigures.clear();
    _totalFigures.clear();
    _numMovStars.clear();
    _numMovObjectAppeared.clear();
    _numMovObjectCollected.clear();
    _numCombinations.clear();
 
}

TTStatistics::~TTStatistics()
{
    removeSafeListenerEvent(_evPlayListener);
    removeSafeListenerEvent(_evFinishListener);
    removeSafeListenerEvent(_evObjectDownListener);
    _evPlayListener = nullptr;
    _evFinishListener = nullptr;
    _evObjectDownListener = nullptr;

    removeListenerEvent(EVENT_CREATE_OBJECT);
}

void TTStatistics::setGameInfo(gameInfo *gameInfo)
{
    this->_gameInfo = gameInfo;
}

void TTStatistics::setDeviceID(std::string devideID)
{
    this->_deviceID = devideID;
}

void TTStatistics::onEvent_GameFinishPlayScored(EventCustom* e)
{
    // Recibir el userData del evento
    finishPlayInfo *finishPlay = static_cast<finishPlayInfo*>(e->getUserData());
    
    //numero de la combinacion de la jugada
    _numCombinations[finishPlay->selectionLength]++;
    
    //figuras recogidas en la jugada
    for(auto it=finishPlay->combinationTypes.begin(); it!=finishPlay->combinationTypes.end(); ++it)
    {
        _collectedFigures[it->first]+=it->second;
    }
    
    //control de las stars
    if (_numStars<3) {
        int estrellasActuales = _gameInfo->getScoreStars();
        if (estrellasActuales > _numStars) {
            for(int i=_numStars;i<estrellasActuales;i++) {
                _numMovStars[i+1] = _gameInfo->movesGame;
            }
        }
        _numStars = estrellasActuales;
    }
    
    printData(createJson());
}

void TTStatistics::onEvent_ObjectDown(EventCustom* e)
{
    ObjectDataEvent *data = static_cast<ObjectDataEvent*>(e->getUserData());
    int _objeto = data->typeObject*100+data->subTypeObject;
    std::vector<int> _movs =_numMovObjectCollected[_objeto];
    _movs.push_back(_gameInfo->movesGame);
}

void TTStatistics::onEvent_GameFinish(EventCustom* e)
{
    if (SEND_DATA) {
        std::string toSend = createJson();
        printData(toSend);
        //TODO: poner la URL correcta del servidor
        sendData(toSend);
    }

}

void TTStatistics::onEvent_CreateObject(EventCustom* e)
{
    // Recibir el userData del evento
    ObjectDataEvent *data = static_cast<ObjectDataEvent*>(e->getUserData());
    int value = data->typeObject*100+data->subTypeObject;
    if (data->dynamic) {
        value += 2 * (10000);
    } else {
        value += 1 * (10000);
    }
    if (value==20100) {
        ;
    }
    _totalFigures[value]++;
    //caso de objetos bajables
    if (data->typeObject==DOWN_OBJECT_DYNAMIC_CONTENT)
    {
        int _objeto = data->typeObject*100+data->subTypeObject;
        std::vector<int> _movs =_numMovObjectAppeared[_objeto];
        _movs.push_back(_gameInfo->movesGame);
    }
}


std::string TTStatistics::createJson()
{
    std::stringstream ss;
    ss<<"{";
    
    ss<<"\"deviceID\":\""<<_deviceID<<"\",";
    
    ss<<"\"level\":"<<_gameInfo->level<<",";
    
    //TODO: configuration level
    ss<<"\"config\":{ ";
    
    vectorToJson(ss, "figuresWeight", _gameInfo->boardInit.figuresWeight);
    ss<<" ,";
    vectorToJson(ss, "scoreStars", _gameInfo->scoreStars);
    ss<<" ,";
    
    ss<<"\"restrictions\":{ ";
        if (_gameInfo->timeRestriction!=NULL)
        {
            ss<<"\"time\":"<<_gameInfo->timeRestriction->_limitTime;
        }
        if (_gameInfo->moveRestriction!=NULL)
        {
            if (_gameInfo->timeRestriction!=NULL) {
                ss<<" ,";
            }
            ss<<"\"moves\":"<<_gameInfo->moveRestriction->_limitMoves;
        }
    ss<<" },";

    ss<<"\"goals\":[ ";
    
    for(int i=0; i<_gameInfo->goals.size();i++)
    {
        TTGoal* _goal = _gameInfo->goals[i];
        ss<<"{\"type\":"<<_goal->_typeGoal<<",";
        ss<<"\"value\":"<<_goal->_goalValue<<",";
        ss<<"\"optional\":"<<_goal->_optional;
        switch(_goal->_typeGoal)
        {
            case SCORE_GOAL:
                break;
            case BREAKS_GOAL:
                break;
            case DOWN_OBJECT_GOAL:
            {
                TTDownObjectGoal* downGoal = static_cast<TTDownObjectGoal*>(_goal);
                ss<<",\"initWeight\":"<< downGoal->_initWeight;
                ss<<",\"maxWeight\":"<< downGoal->_maxWeight;
                ss<<",\"increaseWeight\":"<< downGoal->_increaseWeight;
                break;
            }
            case COLLECT_FIGURES_GOAL:
            {
                ss<<",\"figure\":"<< static_cast<TTCollectFiguresGoal*>(_goal)->_figureToCollect;
                break;
            }
            default:
                break;
        }
        ss<<"}";
        if (i<_gameInfo->goals.size()-1) ss<<",";
    }
    ss<<" ]";
    
    
    ss<<" },";
    
    ss<<"\"points\":"<<_gameInfo->scoreGame<<",";
    
    ss<<"\"movements\":"<<_gameInfo->movesGame<<",";
    
    ss<<"\"totalTime\":"<< static_cast<int>(_gameInfo->currentTime) <<",";
    
    ss<<"\"gameWin\":"<<_gameInfo->winGame<<",";
    
    ss<<"\"gameAborted\":"<<_gameAborted<<",";
    
    //collected figures
    mapToJson(ss, "collectedFigures", _collectedFigures);
    ss<<" ,";
    
    //total figures
    mapToJson(ss, "totalFigures", _totalFigures);
    ss<<" ,";
    
    //stars
    ss<<"\"stars\":"<<_numStars<<",";
    
    //#movements in stars
    mapToJson(ss, "starMovements", _numMovStars);
    ss<<" ,";
    
    //#movement of object appear
    mapVectorToJson(ss, "objectAppearMovements", _numMovObjectAppeared);
    ss<<" ,";
    
    //#movement of collected objects
    mapVectorToJson(ss, "objectCollectMovements", _numMovObjectCollected);
    ss<<" ,";
    
    //combinations
    mapToJson(ss, "numCombinations", _numCombinations);
    
    ss<<"}";
    
    return ss.str();
    
}

void TTStatistics::printData(std::string datos)
{
    if (PRINT_STATISTICS) {
        CCLOG("STATISTICS DATA:%s", datos.c_str());
    }
    
}

#pragma mark - NETWORK

void TTStatistics::sendData(std::string datos)
{

    if (SEND_DATA==false){
        return;
    }
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(STATISTICS_SERVER_URL);
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback( CC_CALLBACK_2(TTStatistics::onHttpRequestCompleted, this) );
    
    
    std::vector<std::string> headers;
    headers.push_back("Content-Type: application/json; charset=utf-8");
    request->setHeaders(headers);
    
    // write the post data
    const char* postData = datos.c_str();
    request->setRequestData(postData, strlen(postData));
    
    request->setTag("POST Statistics");
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}

void TTStatistics::onHttpRequestCompleted(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response)
{
    if (!response)
    {
        return;
    }
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLOG("%s completed", response->getHttpRequest()->getTag());
    }

    // Check the HTTP Status Code
    int statusCode = response->getResponseCode();
    CCLOG("HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    
    // A connection failure
    if (!response->isSucceed())
    {
        CCLOG("response failed");
        CCLOG("error buffer: %s", response->getErrorBuffer());
        return;
    }

    // The data will be placed in the buffer.
    std::vector<char> * buffer = response->getResponseData();
    std::string s2(buffer->begin(), buffer->end());
    
    CCLOG("RESPUESTA STATISTICS:%s",s2.c_str());
    
  
}


