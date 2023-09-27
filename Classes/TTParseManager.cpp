//
//  ParseManager.cpp
//  animalplanet
//
//  Created by Marc Serena on 18/2/15.
//
//

#include "TTParseManager.h"
#include "TTConfigParser.h"
#include "TTStoreManager.h"
#include "TTBoostersManager.h"
#include "TTFacebook.h"
#include "TTEventsManager.h"
#include "TTUserInfo.h"
#include "TTUIStackManager.h"
#include "TTAnimalCubes.h"
#include "TTMapManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ParseManager_iOS.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
 #include "../../proj.android/jni/parse/ParseManagerAndroid.h"
#endif

//Singleton
static TTParseManager *_sharedParseManager = nullptr;

TTParseManager* TTParseManager::getInstance()
{
    if (!_sharedParseManager)
    {
        _sharedParseManager = new TTParseManager();
        _sharedParseManager->init();
    }
    
    return _sharedParseManager;
}

void TTParseManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedParseManager);
}

TTParseManager::~TTParseManager()
{
    
}

void TTParseManager::init()
{
    _syncronizing = false;
}

void TTParseManager::connectUserToFacebook(std::string _facebookId)
{
	if(this->isUserConnectedToFacebook())
	{
		return;
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::connectUserToFacebookParse(_facebookId);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::connectUserToFacebookParse(_facebookId);
#endif
}

void TTParseManager::disconnectUserToFacebook()
{
    TTUIStackManager::getInstance()->showUIConnecting(TTAnimalCubes::getInstance()->getActualLayer());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::disconnectUserToFacebookParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::disconnectUserToFacebookParse();
#endif
}

void TTParseManager::syncCloud()
{
    if(_syncronizing)
        return;
    
    _syncronizing = true;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::syncCloudParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::syncCloudParse();
#endif
}

void TTParseManager::setUserActualLevel(int _level)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::setUserActualLevelParse(_level);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::setUserActualLevelParse(_level);
#endif
}

void TTParseManager::storeUserLevelInfo(int _levelId, int _newScore, int _stars, int _movements, int _timePlaying)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::storeUserLevelInfoParse(_levelId,_newScore,_stars,_movements,_timePlaying);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::storeUserLevelInfoParse(_levelId,_newScore,_stars,_movements,_timePlaying);
#endif
}

bool TTParseManager::isUserConnectedToFacebook()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return ParseManager_iOS::isUserConnectedToFacebookParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    bool ret =  parse::jni::ParseManagerAndroid::isUserConnectedToFacebookParse();
    return ret;
#endif
    return false; //por defecto
}

void TTParseManager::storeUserInfo()
{
    // Obetenemos datos del usuario
    std::string _appVersion = TTConfigParser::getInstance()->_idVersionApp;
    int _coins = TTStoreManager::getInstance()->getUserBalance();
    
    // Coidifcamos la información de los boosters de tablero y juego
    std::string _boardBoostersInfo = "";
    std::map<BOARD_BOOSTERS_TYPE,TTBoardBoosters*>  _boardBoostersInstances = TTBoostersManager::getInstance()->_boardBoostersInstances;
    std::map<BOARD_BOOSTERS_TYPE,TTBoardBoosters*>::iterator _boardBoostersInstancesItr;
    for(_boardBoostersInstancesItr = _boardBoostersInstances.begin(); _boardBoostersInstancesItr != _boardBoostersInstances.end(); ++_boardBoostersInstancesItr)
    {
        TTBoardBoosters* _boardBooster = _boardBoostersInstancesItr->second;
        std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"0_"+std::to_string(_boardBooster->_boosterSubType));
        int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
        _boardBoostersInfo += std::to_string(_numBoosters);
        if(_boardBooster != _boardBoostersInstances.rbegin()->second)
        {
            _boardBoostersInfo += ",";
        }
    }
    
    std::string _gameBoostersInfo = "";
    std::map<GAME_BOOSTERS_TYPE,TTGameBoosters*>  _gameBoostersInstances = TTBoostersManager::getInstance()->_gameBoostersInstances;
    std::map<GAME_BOOSTERS_TYPE,TTGameBoosters*>::iterator _gameBoostersInstancesItr;
    for(_gameBoostersInstancesItr = _gameBoostersInstances.begin(); _gameBoostersInstancesItr != _gameBoostersInstances.end(); ++_gameBoostersInstancesItr)
    {
        TTGameBoosters* _gameBooster = _gameBoostersInstancesItr->second;
        std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"1_"+std::to_string(_gameBooster->_boosterSubType));
        int _numBoosters = TTStoreManager::getInstance()->getNumberOfUserVirtualItems(_boosterItemId);
        _gameBoostersInfo += std::to_string(_numBoosters);
        if(_gameBooster != _gameBoostersInstances.rbegin()->second)
        {
            _gameBoostersInfo += ",";
        }
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::storeUserInfoParse(_coins,_boardBoostersInfo,_gameBoostersInfo,_appVersion);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::storeUserInfoParse(_coins,_boardBoostersInfo,_gameBoostersInfo,_appVersion);
#endif
}

void TTParseManager::verifyUserCloudUpdates()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::verifyUserCloudUpdatesParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::verifyUserCloudUpdatesParse();
#endif
}

void TTParseManager::getFriendsHighLevel(std::string _friendsIds)
{
	if (_friendsIds.size()==0) {
		//no se tienen amigos, se envia directamente el callback de amigos
		std::map<std::string,int> _friendsHighLevelInfo;
		TTParseManager::getInstance()->getFriendsHighLevelCallback(_friendsHighLevelInfo);
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::getFriendsHighLevelParse(_friendsIds);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::getFriendsHighLevelParse(_friendsIds);
#endif
}

void TTParseManager::getRankingLevel(int _level, std::string _friendsIds, std::string _meId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::getRankingLevelParse(_level,_friendsIds, _meId);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::getRankingLevelParse(_level,_friendsIds, _meId);
#endif
}

void TTParseManager::sendAnalyticEvent(std::string _eventName, std::map<std::string,std::string> _dimensions)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::sendAnalyticEventParse(_eventName,_dimensions);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::sendAnalyticEventParse(_eventName,_dimensions);
#endif
}

void TTParseManager::migrateUser(std::vector<levelInfo> _levelsInfo, std::string _userFacebookId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::migrateUserParse(_levelsInfo,_userFacebookId);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::migrateUserParse(_levelsInfo,_userFacebookId);
#endif
}

std::string TTParseManager::getUserParseID()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return ParseManager_iOS::getUserParseIDParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return parse::jni::ParseManagerAndroid::getUserParseIDParse();
#endif
}

void TTParseManager::parseUpdateFacebookCallbackDone(std::vector<int> _stars)
{
    
    CCLOG("parseUpdateFacebookCallbackDone -> El vector de estrellas tiene %d elementos, se intenta agregar %d elemtos nuevos",TTUserInfo::getInstance()->_userData->_levelStars.size(),_stars.size());
    // Tenemos información del cloud de niveles, actualizamos los datos locales
    for(int i = 0; i < _stars.size(); i++)
    {
        int _mapPieces = TTMapParser::getInstance()->_mapInfo->_cellsPositions.size();
        if(TTUserInfo::getInstance()->_userData->_levelStars.size() > _mapPieces)
        {
            //No hacemos nada, hay un error
        }
        else
        {
            TTUserInfo::getInstance()->_userData->_levelStars.push_back(_stars[i]);
        }
    }
    TTUserInfo::getInstance()->saveData();
    
    // Informamos de nuevos datos de usuario recividos
    sendEvent(EVENT_USER_INFO_UPDATED);
}


void TTParseManager::parseLinkedToFacebookCallbackDoneAddLevels(std::vector<int> _stars)
{
    CCLOG("parseLinkedToFacebookCallbackDoneAddLevels -> El vector de estrellas tiene %d elementos, se intenta agregar %d elemtos nuevos",TTUserInfo::getInstance()->_userData->_levelStars.size(),_stars.size());
    // Tenemos información del cloud de niveles, actualizamos los datos locales
    for(int i = 0; i < _stars.size(); i++)
    {
        int _mapPieces = TTMapParser::getInstance()->_mapInfo->_cellsPositions.size();
        if(TTUserInfo::getInstance()->_userData->_levelStars.size() > _mapPieces)
        {
            //No hacemos nada, hay un error
        }
        else
        {
            TTUserInfo::getInstance()->_userData->_levelStars.push_back(_stars[i]);
        }
    }
    TTUserInfo::getInstance()->saveData();
    
    // Informamos de conexión completada
    TTUIStackManager::getInstance()->hideUIConnecting();
    sendEvent(EVENT_FACEBOOK_CONNECTION);
    sendEvent(EVENT_USER_INFO_UPDATED);
}

void TTParseManager::parseLinkedToFacebookCallbackDoneNoChanges()
{
    // Informamos de conexión completada
    TTUIStackManager::getInstance()->hideUIConnecting();
    sendEvent(EVENT_FACEBOOK_CONNECTION);
    sendEvent(EVENT_USER_INFO_UPDATED);
}

void TTParseManager::parseLinkedToFacebookCallbackFail()
{
    // No se ha podido conectar a Facebook por error en Parse
    TTFacebook::getInstance()->disconnectFromFacebook();
    TTUIStackManager::getInstance()->hideUIConnecting(true);
}

void TTParseManager::parseUnLinkedToFacebookCallbackDone()
{
    // Informamos de desconexión completada
    TTFacebook::getInstance()->disconnectFromFacebook();
    TTUIStackManager::getInstance()->hideUIConnecting();
}

void TTParseManager::parseUnLinkedToFacebookCallbackFail()
{
    // No se ha podido conectar a Facebook por error en Parse
    TTUIStackManager::getInstance()->hideUIConnecting(true);
}

void TTParseManager::getFriendsHighLevelCallback(std::map<std::string,int> _friendsHighLevelInfo)
{
    // Ya tenemos los datos de los usuarios de Facebook
    TTFacebook::getInstance()->_friendsHighLevelInfo = _friendsHighLevelInfo;
    sendEvent(EVENT_FACEBOOK_USERS_INFO_UPDATED);
}

void TTParseManager::getRankingLevelCallback(bool _connection, std::vector<facebookFriendScoreInfo> _rankingLevel)
{
	//controlar mi puntuacion actual directamente en el vector
	if (_connection) {
		int puntActual = TTGame::getInstance()->_gameConfig->scoreGame;
		//solo controlarlo en el ranking de finalizacion de nivel, con nivel ganado
		if (puntActual>0 && TTGame::getInstance()->_gameConfig->winGame)
		{
			std::string  _me = TTFacebook::getInstance()->getMeId();

			//Iterar sobre el vector, si mi puntacion actual es mayor que alguno, insertarlo antes,
			//si soy yo mismo, actualizarlo
			for(auto it = _rankingLevel.begin(); it!=_rankingLevel.end(); ++it)
			{
				if (puntActual > it->_score)
				{
					if (it->_name == _me)
					{
						//actualizar
						it->_score = puntActual;
					} else
					{
						//insertar
						facebookFriendScoreInfo _meScore;
						_meScore._name = _me;
						_meScore._score = puntActual;
						_rankingLevel.insert(it, _meScore);
					}
					break;
				}
			}

			//volver a iterar, si estoy repetido, eliminar el segundo elemento
			bool yaExisto = false;
			for(auto it = _rankingLevel.begin(); it!=_rankingLevel.end(); ++it)
			{
				if (it->_name == _me)
				{
					if (yaExisto)
					{
						//eliminarlo
						_rankingLevel.erase(it);
						break;
					}
					yaExisto = true;
				}
			}
			//caso especial de si el vector está vacío (soy el primero en jugarlo)
			if (_rankingLevel.size()==0)
			{
				//insertar
				facebookFriendScoreInfo _meScore;
				_meScore._name = _me;
				_meScore._score = puntActual;
				_rankingLevel.push_back(_meScore);
			}
		}
	}
    facebookRankingInfo info;
    info._connection = _connection;
    info._rankingLevelInfo = _rankingLevel;
    sendEvent(EVENT_FACEBOOK_RANKING,&info);
}

void TTParseManager::setLevelStatistics(int _level, bool _win)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::setLevelStatisticsParse(_level,_win);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::setLevelStatisticsParse(_level,_win);
#endif
}

void TTParseManager::getServerTime()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ParseManager_iOS::getServerTimeParse();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    parse::jni::ParseManagerAndroid::getServerTime();
#endif
}

void TTParseManager::updateUserCloudCallback(int _coinsAux)
{
    if(_coinsAux != 0)
    {
        TTStoreManager::getInstance()->syncWithParseInfo(_coinsAux);
    }
}

void TTParseManager::migrationDoneCallback()
{
    TTUserInfo::getInstance()->_userData->_parseMigrationDone = true;
    TTUserInfo::getInstance()->saveData();
}

void TTParseManager::syncFinishCallback()
{
    _syncronizing = false;
}

void TTParseManager::parseLinkedToFacebookAdjustLevelsCallbackDone(int _maxLevel)
{
    int _maxLevelLogic = _maxLevel+1;
    if(TTUserInfo::getInstance()->_userData->_levelStars.size() < _maxLevelLogic) return;
    TTUserInfo::getInstance()->_userData->_levelStars.erase(TTUserInfo::getInstance()->_userData->_levelStars.begin()+_maxLevelLogic,TTUserInfo::getInstance()->_userData->_levelStars.end());
    TTUserInfo::getInstance()->saveData();
    
    TTUIStackManager::getInstance()->hideUIConnecting();
    sendEvent(EVENT_FACEBOOK_CONNECTION);
    sendEvent(EVENT_USER_INFO_UPDATED);
}

void TTParseManager::getServerTimeCallback(bool _error, std::string _serverTime)
{
    std::string _m = _serverTime;
    CCLOG("TTParseManager::getServerTimeCallback :::::::::: Time = %s, error = %d",_serverTime.c_str(),_error);
    
    TimeServer dataEvent;
    dataEvent._error = _error;
    dataEvent._date = std::atoll(_serverTime.c_str());
    sendEvent(EVENT_TIME_SERVER,&dataEvent);
}
