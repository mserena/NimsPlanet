#include "ParseManagerAndroid.h"

#include "platform/android/jni/JniHelper.h"
#include <jni.h>

#include "thegame.h"
#include "TTParseManager.h"

NS_PARSE_JNI_BEGIN  //parse::jni

// Java class
#define  CLASS_NAME "es.fourpoints.parse.ParseManager"

static bool getJNIStaticMethodInfo(cocos2d::JniMethodInfo &methodinfo,
                                   const char *methodName,
                                   const char *paramCode) {
    return cocos2d::JniHelper::getStaticMethodInfo(methodinfo,
                                                   CLASS_NAME,
                                                   methodName,
                                                   paramCode);
}

std::string ParseManagerAndroid::getUserParseIDParse()
{
	CCLOG("ParseManagerAndroid::getUserParseIDParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "getUserParseIDParse", "()Ljava/lang/String;")) {
		return "";
	}

	jstring rest = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID );
	std::string ret = jString2String(methodInfo.env, rest);

	methodInfo.env->DeleteLocalRef(methodInfo.classID);

	return ret;
}

void ParseManagerAndroid::setUserActualLevelParse(int _level)
{
	CCLOG("ParseManagerAndroid::setUserActualLevelParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "setUserActualLevelParse", "(I)V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _level );

	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void ParseManagerAndroid::migrateUserParse(std::vector<levelInfo> _levelsInfo, std::string _facebookId)
{
	CCLOG("ParseManagerAndroid::migrateUserParse");

    cocos2d::JniMethodInfo methodInfo;

    //transformamos el vector de levelinfo en un array de strings
    if (! getJNIStaticMethodInfo(methodInfo, "migrateUserParse", "([Ljava/lang/String;Ljava/lang/String;)V")) {
        return;
    }

	jclass jStringClassID = (jclass) methodInfo.env->FindClass("java/lang/String");

	jobjectArray jitems = (jobjectArray) methodInfo.env->NewObjectArray(_levelsInfo.size(), jStringClassID, methodInfo.env->NewStringUTF(""));
    for(int i = 0; i < _levelsInfo.size(); i++)
    {
		std::stringstream ss;
		ss<<_levelsInfo[i]._levelId<<"|"<<_levelsInfo[i]._newScore<<"|"<<_levelsInfo[i]._stars<<"|"<<_levelsInfo[i]._movements<<"|"<<_levelsInfo[i]._timePlaying;
		jstring s = methodInfo.env->NewStringUTF(ss.str().c_str());
		methodInfo.env->SetObjectArrayElement(jitems, i, s);
		methodInfo.env->DeleteLocalRef(s);
    }

    jstring _facebookIdArg = methodInfo.env->NewStringUTF(_facebookId.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jitems, _facebookIdArg);

    methodInfo.env->DeleteLocalRef(jitems);
    methodInfo.env->DeleteLocalRef(_facebookIdArg);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void ParseManagerAndroid::migrateUserLevelsParse(std::vector<levelInfo> _levelsInfo)
{
	CCLOG("ParseManagerAndroid::migrateUserLevelsParse");

    cocos2d::JniMethodInfo methodInfo;

    //transformamos el vector de levelinfo en un array de strings
    if (! getJNIStaticMethodInfo(methodInfo, "migrateUserLevelsParse", "([Ljava/lang/String;)V")) {
        return;
    }

	jclass jStringClassID = (jclass) methodInfo.env->FindClass("java/lang/String");

	jobjectArray jitems = (jobjectArray) methodInfo.env->NewObjectArray(_levelsInfo.size(), jStringClassID, methodInfo.env->NewStringUTF(""));
    for(int i = 0; i < _levelsInfo.size(); i++)
    {
		std::stringstream ss;
		ss<<_levelsInfo[i]._levelId<<"|"<<_levelsInfo[i]._newScore<<"|"<<_levelsInfo[i]._stars<<"|"<<_levelsInfo[i]._movements<<"|"<<_levelsInfo[i]._timePlaying;
		jstring s = methodInfo.env->NewStringUTF(ss.str().c_str());
		methodInfo.env->SetObjectArrayElement(jitems, i, s);
		methodInfo.env->DeleteLocalRef(s);
    }
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jitems);

    methodInfo.env->DeleteLocalRef(jitems);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}



void ParseManagerAndroid::storeUserLevelInfoParse(int _levelId, int _newScore, int _stars, int _movements, int _timePlaying)
{
	CCLOG("ParseManagerAndroid::storeUserLevelInfoParse");
    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "storeUserLevelInfoParse", "(IIIII)V")) {
        return;
    }
    
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _levelId, _newScore, _stars, _movements, _timePlaying );

    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void ParseManagerAndroid::syncCloudParse()
{
	CCLOG("ParseManagerAndroid::syncCloudParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "syncCloudParse", "()V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void ParseManagerAndroid::syncCloudDataParse()
{
	CCLOG("ParseManagerAndroid::syncCloudDataParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "syncCloudDataParse", "()V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void ParseManagerAndroid::syncCloudDataFacebookParse()
{
	CCLOG("ParseManagerAndroid::syncCloudDataFacebookParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "syncCloudDataFacebookParse", "()V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}


void ParseManagerAndroid::storeUserInfoParse(int _coins, std::string _boardBoostersInfo, std::string _gameBoostersInfo, std::string _appVersion)
{
	CCLOG("ParseManagerAndroid::storeUserInfoParse");
    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "storeUserInfoParse", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
        return;
    }

    jstring _boardBoosterArg = methodInfo.env->NewStringUTF(_boardBoostersInfo.c_str());
    jstring _gameBoosterArg = methodInfo.env->NewStringUTF(_gameBoostersInfo.c_str());
    jstring _appVersionArg = methodInfo.env->NewStringUTF(_appVersion.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _coins, _boardBoosterArg, _gameBoosterArg,  _appVersionArg);

    methodInfo.env->DeleteLocalRef(_boardBoosterArg);
    methodInfo.env->DeleteLocalRef(_gameBoosterArg);
    methodInfo.env->DeleteLocalRef(_appVersionArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}


void ParseManagerAndroid::verifyUserCloudUpdatesParse()
{
	CCLOG("ParseManagerAndroid::verifyUserCloudUpdatesParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "verifyUserCloudUpdatesParse", "()V")) {
        return;
    }

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}


bool ParseManagerAndroid::isUserConnectedToFacebookParse()
{
	CCLOG("ParseManagerAndroid::isUserConnectedToFacebookParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "isUserConnectedToFacebookParse", "()Z")) {
        return false;
    }

    jboolean res = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);

	return (bool)res;
}

/*
 * Desvincula lógicamente el usuario de Facebook
 */
void ParseManagerAndroid::disconnectUserToFacebookParse()
{
	CCLOG("ParseManagerAndroid::disconnectUserToFacebookParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "disconnectUserToFacebookParse", "()V")) {
        return;
    }

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}

void ParseManagerAndroid::connectUserToFacebookParse(std::string _userIdFacebook)
{
	CCLOG("ParseManagerAndroid::connectUserToFacebookParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "connectUserToFacebookParse", "(Ljava/lang/String;)V")) {
        return;
    }

    jstring _userIdFacebookArg = methodInfo.env->NewStringUTF(_userIdFacebook.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _userIdFacebookArg);

    methodInfo.env->DeleteLocalRef(_userIdFacebookArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}


void ParseManagerAndroid::getFriendsHighLevelParse(std::string _friendsIds)
{
	CCLOG("ParseManagerAndroid::getFriendsHighLevelParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "getFriendsHighLevelParse", "(Ljava/lang/String;)V")) {
        return;
    }

    jstring _friendsIdsArg = methodInfo.env->NewStringUTF(_friendsIds.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _friendsIdsArg);

    methodInfo.env->DeleteLocalRef(_friendsIdsArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}

void ParseManagerAndroid::getRankingLevelParse(int _level, std::string _friendsIds, std::string _meId)
{
	CCLOG("ParseManagerAndroid::getRankingLevelParse");

    cocos2d::JniMethodInfo methodInfo;

    if (! getJNIStaticMethodInfo(methodInfo, "getRankingLevelParse", "(ILjava/lang/String;Ljava/lang/String;)V")) {
        return;
    }

    jstring _friendsIdsArg = methodInfo.env->NewStringUTF(_friendsIds.c_str());
    jstring _meIdArg = methodInfo.env->NewStringUTF(_meId.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _level, _friendsIdsArg, _meIdArg);

    methodInfo.env->DeleteLocalRef(_friendsIdsArg);
    methodInfo.env->DeleteLocalRef(_meIdArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}

void ParseManagerAndroid::sendAnalyticEventParse(std::string _eventName, std::map<std::string,std::string> _dimensions)
{
	CCLOG("ParseManagerAndroid::sendAnalyticEventParse");

    cocos2d::JniMethodInfo methodInfo;

    //transformamos el map en un array de strings
    if (! getJNIStaticMethodInfo(methodInfo, "sendAnalyticEventParse", "(Ljava/lang/String;[Ljava/lang/String;)V")) {
        return;
    }

	jclass jStringClassID = (jclass) methodInfo.env->FindClass("java/lang/String");

	jobjectArray jitems = (jobjectArray) methodInfo.env->NewObjectArray(_dimensions.size(), jStringClassID, methodInfo.env->NewStringUTF(""));
	int i = 0;
	for (auto iter = _dimensions.begin(); iter != _dimensions.end(); iter++) {
    	std::string s1 = iter->first+"="+iter->second;
		jstring s = methodInfo.env->NewStringUTF(s1.c_str());
		methodInfo.env->SetObjectArrayElement(jitems, i++, s);
		methodInfo.env->DeleteLocalRef(s);
	}

    jstring _eventNameArg = methodInfo.env->NewStringUTF(_eventName.c_str());

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _eventNameArg, jitems);

    methodInfo.env->DeleteLocalRef(_eventNameArg);
    methodInfo.env->DeleteLocalRef(jitems);

    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}

void ParseManagerAndroid::getServerTime()
{
    CCLOG("ParseManagerAndroid::getServerTime");
    
    cocos2d::JniMethodInfo methodInfo;
    
    if (! getJNIStaticMethodInfo(methodInfo, "getServerTimeParse", "()V")) {
        return;
    }
    
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}


/*
 * Setea las estadísiticas de nivel
 */
void ParseManagerAndroid::setLevelStatisticsParse(int _level, bool _win) {
	CCLOG("ParseManagerAndroid::setLevelStatisticsParse");
	cocos2d::JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "setLevelStatisticsParse", "(IZ)V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _level, _win);

	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

//Utility methods
std::string ParseManagerAndroid::jString2String(JNIEnv *env, jstring jstr) {
	if (jstr!=NULL) {
		const char *cs = env->GetStringUTFChars(jstr, NULL);
		std::string ret(cs);
		env->ReleaseStringUTFChars(jstr, cs);
		return ret;
	}
	return "";
}

std::vector<int> ParseManagerAndroid::jIntArray2IntVector(JNIEnv *env, jintArray array) {
	std::vector<int> ll;
	jsize count = env->GetArrayLength(array);
	jint *oarr = env->GetIntArrayElements(array, NULL);
	for (int i = 0; i < count; i++) {
		ll.push_back((int)oarr[i]);
	}
	env->ReleaseIntArrayElements(array,oarr,JNI_ABORT);
	return ll;
}


NS_PARSE_JNI_END

// native Callbacks
/*
	private static native void nativeParseLinkedToFacebookCallbackDoneAddLevels(int[] _stars);
    private static native void nativeParseLinkedToFacebookCallbackDoneNoChanges();
    private static native void nativeParseLinkedToFacebookAdjustLevelsCallbackDone(int _maxLevel);

	private static native void nativeParseLinkedToFacebookCallbackFail();
	private static native void nativeParseUnLinkedToFacebookCallbackDone();
	private static native void nativeParseUnLinkedToFacebookCallbackFail();
	private static native void nativeParseUpdateFacebookCallbackDone(int[] _stars);
	private static native void nativeGetFriendsHighLevelCallback(String[] _friendsHighLevelInfo);
	private static native void nativeGetRankingLevelCallback(boolean _connection,String[] _rankingLevelInfo);
	private static native void nativeGetUserInfoCallback(boolean connection, int _coins, String _boostersBoard, String _boostersGame);
	private static native void nativeMigrationDoneCallback();
*/

extern "C" {

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseLinkedToFacebookCallbackDoneAddLevels(JNIEnv *env, jclass jclass, jintArray jstars) {
	CCLOG("nativeParseLinkedToFacebookCallbackDoneAddLevels ");

    std::vector<int> _stars = parse::jni::ParseManagerAndroid::jIntArray2IntVector(env, jstars);

	TTParseManager::getInstance()->parseLinkedToFacebookCallbackDoneAddLevels(_stars);

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseLinkedToFacebookAdjustLevelsCallbackDone(JNIEnv *env, jclass jclass,jint jmaxLevel) {
	CCLOG("nativeParseLinkedToFacebookAdjustLevelsCallbackDone ");

	TTParseManager::getInstance()->parseLinkedToFacebookAdjustLevelsCallbackDone((int)jmaxLevel);

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseLinkedToFacebookCallbackDoneNoChanges(JNIEnv *env, jclass jclass) {
	CCLOG("nativeParseLinkedToFacebookCallbackDoneNoChanges ");

	TTParseManager::getInstance()->parseLinkedToFacebookCallbackDoneNoChanges();

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseLinkedToFacebookCallbackFail(JNIEnv *env, jclass jclass) {
	CCLOG("nativeParseLinkedToFacebookCallbackFail ");

	TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseUnLinkedToFacebookCallbackDone(JNIEnv *env, jclass jclass) {
	CCLOG("nativeParseUnLinkedToFacebookCallbackDone ");

	TTParseManager::getInstance()->parseUnLinkedToFacebookCallbackDone();

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseUnLinkedToFacebookCallbackFail(JNIEnv *env, jclass jclass) {
	CCLOG("nativeParseUnLinkedToFacebookCallbackFail ");

	TTParseManager::getInstance()->parseUnLinkedToFacebookCallbackFail();

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeParseUpdateFacebookCallbackDone(JNIEnv *env, jclass jclass, jintArray jstars) {
	CCLOG("nativeParseUpdateFacebookCallbackDone ");

    std::vector<int> _stars = parse::jni::ParseManagerAndroid::jIntArray2IntVector(env, jstars);

    TTParseManager::getInstance()->parseUpdateFacebookCallbackDone(_stars);
}


JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeGetFriendsHighLevelCallback(JNIEnv *env, jclass jclass, jobjectArray jfriendsHighLevelInfo) {
	CCLOG("nativeGetFriendsHighLevelCallback ");

	std::map<std::string,int> _friendsHighLevel;

	jsize count = env->GetArrayLength(jfriendsHighLevelInfo);
	CCLOG("nativeGetFriendsHighLevelCallback total[%d]",(int)count);

	for (int i = 0; i < count; i++) {
		CCLOG("nativeGetFriendsHighLevelCallback iter[%d]",i);
		jstring jstr = (jstring) env->GetObjectArrayElement(jfriendsHighLevelInfo, i);
		const char *cstr = env->GetStringUTFChars(jstr, NULL);
		CCLOG("nativeGetFriendsHighLevelCallback valor[%s]",cstr);

		std::vector<std::string> datos = tokenizeString(cstr,"=");
		_friendsHighLevel.insert(std::make_pair(datos[0], std::stoi(datos[1])));

		env->ReleaseStringUTFChars(jstr, cstr);
	}

	TTParseManager::getInstance()->getFriendsHighLevelCallback(_friendsHighLevel);

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeGetRankingLevelCallback(JNIEnv *env, jclass jclass, jboolean jconnection, jobjectArray jrankingLevelInfo) {
	CCLOG("nativeGetRankingLevelCallback ");

	std::vector<facebookFriendScoreInfo> _rankingLevel;

	jsize count = env->GetArrayLength(jrankingLevelInfo);
	CCLOG("nativeGetRankingLevelCallback total[%d]",(int)count);

	for (int i = 0; i < count; i++) {
		CCLOG("nativeGetRankingLevelCallback iter[%d]",i);
		jstring jstr = (jstring) env->GetObjectArrayElement(jrankingLevelInfo, i);
		const char *cstr = env->GetStringUTFChars(jstr, NULL);
		CCLOG("nativeGetRankingLevelCallback valor[%s]",cstr);

		std::vector<std::string> datos = tokenizeString(cstr,"=");

		facebookFriendScoreInfo _userScore;
		_userScore._name = datos[0];
		_userScore._score = std::stoi(datos[1]);
		_rankingLevel.push_back(_userScore);

		env->ReleaseStringUTFChars(jstr, cstr);
	}

	TTParseManager::getInstance()->getRankingLevelCallback((bool)jconnection, _rankingLevel);

}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeUpdateUserCloudCallback(JNIEnv *env, jclass jclass, jint jcoinsAux) {
	CCLOG("updateUserCloudCallback ");

	TTParseManager::getInstance()->updateUserCloudCallback((int)jcoinsAux);
}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeMigrationDoneCallback(JNIEnv *env, jclass jclass) {
	CCLOG("nativeMigrationDoneCallback ");

    TTParseManager::getInstance()->migrationDoneCallback();
}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeSyncFinishCallback(JNIEnv *env, jclass jclass) {
	CCLOG("nativeSyncFinishCallback ");

    TTParseManager::getInstance()->syncFinishCallback();
}

JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeSyncCloud(JNIEnv *env, jclass jclass) {
	CCLOG("nativeSyncCloud ");

    TTParseManager::getInstance()->syncCloud();
}
    
JNIEXPORT void JNICALL Java_es_fourpoints_parse_ParseManager_nativeGetServerTimeCallback(JNIEnv *env, jclass jclass, jboolean jconnection, jstring jtime) {
    CCLOG("************* nativeGetServerTimeCallback");
    const char *time = env->GetStringUTFChars(jtime,NULL);
    TTParseManager::getInstance()->getServerTimeCallback((bool)jconnection,time);
}
    

} /* extern "C" { */



