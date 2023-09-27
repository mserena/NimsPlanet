#ifndef __TTUserInfo_SCENE_H__
#define __TTUserInfo_SCENE_H__

#include "cocos2d.h"
#include "thegame.h"

using namespace cocos2d;

#define USER_HAS_DATA               "_UserHasData"
#define USER_WORLD                  "_UserWorld"
#define USER_LEVEL                  "_UserLevel"
#define USER_LEVEL_STARS            "_UserLevelStars"
#define USER_BOOSTERS_BOARD         "_UserBoostersBoard"
#define USER_BOOSTERS_GAME          "_UserBoostersGame"
#define USER_SETTINGS_MUSIC         "_UserSettingsMusic"
#define USER_SETTINGS_EFFECTS       "_UserSettingsEffects"
#define USER_SETTINGS_LANGUAGE      "_UserSettingsLanguage"
#define USER_SETTINGS_LIKE_FACEBOOK "_UserSettingsLikeFacebook"
#define USER_PARSE_MIGRATION_DONE   "_UserParseMigrationDone"

typedef struct userActualProgress
{
    int _actualLevel;
    int _actualWorld;
};

typedef struct facebookInfo
{
    std::string Name;
    int         level;
}facebookInfo;

typedef struct userData
{
    // Nivel actual seleccionado
    int _selectedWorld;
    int _selectedLevel;
    
    // Settings
    int     _musicLevel;
    int     _effectsLevel;
    int     _language;
    bool    _likeOnFacebook;
    
    //Información de Parse
    bool            _parseMigrationDone;
    bool            _facebookMigrationDone;
    // Niveles
    std::vector<int>   _levelStars;
    
}userData;

typedef struct userDataVolatile
{
    bool newLevelOpen;
}userDataVolatile;

// Clase singleton
class CC_DLL TTUserInfo: public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUserInfo* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUserInfo(){}
    
public:
    
    void init();
    ~TTUserInfo();
    
    void saveData();
    void readData();
    
    userActualProgress getActualUserProgress();
    
private:
    
    void testDataLoad();

public:
    
    UserDefault         *def;
    userData            *_userData;
    userDataVolatile    _userDataVolatile;
    
};

#endif  // __TTUserInfo_SCENE_H__
