#include "TTUserInfo.h"
#include "TTStoreManager.h"
#include "TTConfigParser.h"
#include "TTParseManager.h"
#include "TTFacebook.h"
#include "TTMapManager.h"

//Singleton
static TTUserInfo *_sharedUserInfo = nullptr;

TTUserInfo* TTUserInfo::getInstance()
{
    if (!_sharedUserInfo)
    {
        _sharedUserInfo = new TTUserInfo();
        _sharedUserInfo->init();
    }
    
    return _sharedUserInfo;
}

void TTUserInfo::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedUserInfo);
}

void TTUserInfo::init()
{
    _userData = new userData();
    
    // Inicializamos los datos volátiles
    _userDataVolatile.newLevelOpen = false;
    
    // Recuperamos la estructura guardada y si no existe la creamos
    def = UserDefault::getInstance();
    
    if(def->getBoolForKey(USER_HAS_DATA,false))
    {
        CCLOG("Leemos los datos de usuario.");
        readData();

#if TEST_GAME
        testDataLoad();
#endif
        
        //Portección para control del vector de estrellas
        int _mapPieces = TTMapParser::getInstance()->_mapInfo->_cellsPositions.size();
        if(_userData->_levelStars.size() > _mapPieces)
        {
            TTUserInfo::getInstance()->_userData->_levelStars.erase(TTUserInfo::getInstance()->_userData->_levelStars.begin()+_mapPieces,TTUserInfo::getInstance()->_userData->_levelStars.end());
            TTUserInfo::getInstance()->saveData();
        }
        
    }
    else
    {
        //No hay datos de usuario, es una instalación nueva.
        _userData->_effectsLevel = 100;
        _userData->_musicLevel = 100;
        _userData->_language = -1;
        _userData->_likeOnFacebook = false;
        _userData->_parseMigrationDone = true;
        saveData();
        
        TTParseManager::getInstance()->setUserActualLevel(-1);
        
        //Entregamos el premio de primer uso de la aplicación
        TTStoreManager::getInstance()->addUserInitGameCoins();
    }
}

TTUserInfo::~TTUserInfo()
{
    delete(_userData);
}

userActualProgress TTUserInfo::getActualUserProgress()
{
    int _actualLevel = _userData->_levelStars.size();
    int _actualWorld = 0;
    for(int i = 0; i < TTConfigParser::getInstance()->_generalConfig->_worlds.size(); i++)
    {
        int _levelsInWorld = TTConfigParser::getInstance()->_generalConfig->_worlds[i]._levels.size();
        if(_actualLevel < _levelsInWorld)
        {
            _actualWorld = i;
            break;
        }
    }

    // Verificamos que no pase de niveles del mundo
    int _totalWorldLevels = 0;
    for(int i = 0; i < TTConfigParser::getInstance()->_generalConfig->_worlds.size(); i++)
    {
        _totalWorldLevels += TTConfigParser::getInstance()->_generalConfig->_worlds[i]._levels.size();
    }
    
    if(_actualLevel >= (_totalWorldLevels-1))
    {
        _actualLevel = _totalWorldLevels-1;
    }
    
    userActualProgress _userProgress;
    _userProgress._actualLevel = _actualLevel;
    _userProgress._actualWorld = _actualWorld;
    return _userProgress;
}

void TTUserInfo::saveData()
{
    def->setBoolForKey(USER_HAS_DATA,true);
    def->setIntegerForKey(USER_SETTINGS_MUSIC,_userData->_musicLevel);
    def->setIntegerForKey(USER_SETTINGS_EFFECTS,_userData->_effectsLevel);
    def->setIntegerForKey(USER_SETTINGS_LANGUAGE,_userData->_language);
    def->setBoolForKey(USER_SETTINGS_LIKE_FACEBOOK,_userData->_likeOnFacebook);
    def->setBoolForKey(USER_PARSE_MIGRATION_DONE,_userData->_parseMigrationDone);
    
    int _mapPieces = TTMapParser::getInstance()->_mapInfo->_cellsPositions.size();
    if(_userData->_levelStars.size() > _mapPieces)
    {
        TTUserInfo::getInstance()->_userData->_levelStars.erase(TTUserInfo::getInstance()->_userData->_levelStars.begin()+_mapPieces,TTUserInfo::getInstance()->_userData->_levelStars.end());
    }
    std::string strLevelStars = "";
    for(int i = 0; i < _userData->_levelStars.size();i++)
    {
        char strStars[100] = {0};
        sprintf(strStars,"%d,",_userData->_levelStars[i]);
        strLevelStars += strStars;
    }
    def->setStringForKey(USER_LEVEL_STARS,strLevelStars);
    
    def->flush();
}

void TTUserInfo::readData()
{
    _userData->_levelStars = tokenizeStringCommaDelimitedToInt(def->getStringForKey(USER_LEVEL_STARS));
    _userData->_musicLevel = def->getIntegerForKey(USER_SETTINGS_MUSIC);
    _userData->_effectsLevel = def->getIntegerForKey(USER_SETTINGS_EFFECTS);
    _userData->_language = def->getIntegerForKey(USER_SETTINGS_LANGUAGE);
    _userData->_likeOnFacebook = def->getBoolForKey(USER_SETTINGS_LIKE_FACEBOOK);
    _userData->_parseMigrationDone = def->getBoolForKey(USER_PARSE_MIGRATION_DONE);
    
    _userData->_levelStars = tokenizeStringCommaDelimitedToInt(def->getStringForKey(USER_LEVEL_STARS));
}

void TTUserInfo::testDataLoad()
{
    /*int _levelsToOpenTest = 26;
    _userData->_levelStars.clear();
    for(int i = 0; i < _levelsToOpenTest; i++)
    {
        _userData->_levelStars.push_back(3);
    }*/
}
