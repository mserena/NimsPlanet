#ifndef _PARSE_MANAGER_ANDROID_H_
#define _PARSE_MANAGER_ANDROID_H_

#include "cocos2d.h"
#include <iostream>
#include "TTParseManager.h"

#define NS_PARSE_JNI_BEGIN namespace parse { namespace jni {
#define NS_PARSE_JNI_END } }


NS_PARSE_JNI_BEGIN

class ParseManagerAndroid
{

public:

    /*
     * Retorna el id del objeto de usuario
     */
    static std::string getUserParseIDParse();

    /*
     * Setea el nivel más alto del usuario
     */
    static void setUserActualLevelParse(int _level);

    /*
     * Sincroniza los objetos locales no sincornizados con la nube
     */
    static void syncCloudParse();
    static void syncCloudDataParse();
    static void syncCloudDataFacebookParse();


	/*
	 * Almacena la información del usuario sobre un nivel o la updatea
	 * en caso de que ya existiera y el score nuevo sea mayor.
	 */
	static void storeUserLevelInfoParse(int _levelId, int _newScore, int _stars, int _movements, int _timePlaying);

	/*
	 * Almacena la información del usuario (boosters, versión, monedas, plataforma...)
	 */
	static void storeUserInfoParse(int _coins, std::string _boardBoostersInfo, std::string _gameBoostersInfo, std::string _appVersion);

   /*
     * Función para la migración de datos del usuario
     */
    static void migrateUserParse(std::vector<levelInfo> _levelsInfo, std::string _facebookId);
    static void migrateUserLevelsParse(std::vector<levelInfo> _levelsInfo);

	/*
	 * Obtiene la información del usuario
	 */
	static void verifyUserCloudUpdatesParse();

    /*
     * Vincula lógicamente el usuario a Facebook
     */
    static void connectUserToFacebookParse(std::string _userIdFacebook);

    /*
     * Desvincula lógicamente el usuario de Facebook
     */
    static void disconnectUserToFacebookParse();

	/*
	 * Regresa si el usuario esta vinculado a Facebook
	 */
	static bool isUserConnectedToFacebookParse();

	/*
	 * Obtiene la información de los amigos del usuario de facebook
	 */
	static void getFriendsHighLevelParse(std::string _friendsIds);

	/*
	 * Obtiene el ranking de amigos de un determinado nivel
	 */
	static void getRankingLevelParse(int _level, std::string _friendsIds, std::string _meId);

    /*
     * Envia un evento de analitica a Parse
     */
    static void sendAnalyticEventParse(std::string _eventName, std::map<std::string,std::string> _dimensions);

    /*
     * Setea las estadísiticas de nivel
     */
    static void setLevelStatisticsParse(int _level, bool _win);
    
    /*
     * Regresa el tiempo del servidor
     */
    static void getServerTime();

    //util methods
	static std::string jString2String(JNIEnv *env, jstring jstr);
	static std::vector<int> jIntArray2IntVector(JNIEnv *env, jintArray array);
};

NS_PARSE_JNI_END

#endif /* _PARSE_MANAGER_ANDROID_H_ */
