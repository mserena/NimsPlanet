//
//  ParseManager.h
//  animalplanet
//
//  Created by Marc Serena on 18/2/15.
//
//

#ifndef __animalplanet__TTParseManager__
#define __animalplanet__TTParseManager__

#include "cocos2d.h"
USING_NS_CC;

typedef struct facebookFriendScoreInfo
{
    std::string _name;
    int         _score;
}facebookFriendScoreInfo;

typedef struct facebookRankingInfo
{
    bool _connection;
    std::vector<facebookFriendScoreInfo> _rankingLevelInfo;
}facebookRankingInfo;

typedef struct levelInfo
{
    int _levelId;
    int _newScore;
    int _stars;
    int _movements;
    int _timePlaying;
}levelInfo;

// Define de los eventos de analitica
#define ANALYTICS_LEVEL_INIT "User_Init_Level"
#define ANALYTICS_TUTORIAL_MENU "User_Enter_Tutorial_Menu"
#define ANALYTICS_SETTINGS_MENU "User_Enter_Settings_Menu"
#define ANALYTICS_LANGUAGE_MENU "User_Set_Language"
#define ANALYTICS_MUSIC_OFF "User_Set_Music_Off"
#define ANALYTICS_FX_OFF "User_Set_Fx_Off"
#define ANALYTICS_SHOP_MENU "User_Enter_Shop_Menu"
#define ANALYTICS_BUY_COINS "User_Buy_Coins"
#define ANALYTICS_LIVES_MENU "User_Enter_Lives_Menu"
#define ANALYTICS_CONTINUE_PLAYING "User_Continue_Playing"
#define ANALYTICS_GIVE_LIVES_MENU "User_Enter_Facebook_Give_Lives"
#define ANALYTICS_ASK_LIVES_MENU "User_Enter_Facebook_Ask_Lives"
#define ANALYTICS_MAIL_MENU "User_Enter_Facebook_Mail"
#define ANALYTICS_INVITE_FRIENDS "User_Enter_Facebook_Invite_Friends"
#define ANALYTICS_LIKE_FACEBOOK "User_Facebook_Like_Button"
#define ANALYTICS_BOOSTER_BOARD_PURCHASE "User_Purchase_Booster_Board"
#define ANALYTICS_BOOSTER_INGAME_PURCHASE "User_Purchase_Booster_Ingame"
#define ANALYTICS_LIVES_PURCHASE "User_Purchase_Lives"
#define ANALYTICS_MIGRATION "User_Migration"

class TTParseManager: public Ref
{
    
public:
    
    // Retorna la instancia Singleton de la clase
    static TTParseManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTParseManager(){};
    
public:
    
    void init();
    ~TTParseManager();
    
    void syncCloud();
    
    void storeUserLevelInfo(int _levelId, int _newScore, int _stars, int _movements, int _timePlaying);
    void storeUserInfo();
    void verifyUserCloudUpdates();
    
    void connectUserToFacebook(std::string _facebookId);
    void disconnectUserToFacebook();
    
    std::string getUserParseID();
    bool isUserConnectedToFacebook();
    void getFriendsHighLevel(std::string _friendsIds);
    void getRankingLevel(int _level, std::string _friendsIds, std::string _meId);
    void setUserActualLevel(int _level);
    void migrateUser(std::vector<levelInfo> _levelsInfo, std::string _userFacebookId = "");
    
    void sendAnalyticEvent(std::string _eventName, std::map<std::string,std::string> _dimensions);
    
    void setLevelStatistics(int _level, bool _win);
    void getServerTime();
    
    // Callbacks
    void parseUpdateFacebookCallbackDone(std::vector<int> _stars);
    void parseLinkedToFacebookCallbackDoneAddLevels(std::vector<int> _stars);
    void parseLinkedToFacebookCallbackDoneNoChanges();
    void parseLinkedToFacebookAdjustLevelsCallbackDone(int _maxLevel);
    void parseLinkedToFacebookCallbackFail();
    void parseUnLinkedToFacebookCallbackDone();
    void parseUnLinkedToFacebookCallbackFail();
    
    void getFriendsHighLevelCallback(std::map<std::string,int> _friendsHighLevelInfo);
    void getRankingLevelCallback(bool _connection, std::vector<facebookFriendScoreInfo> _rankingLevel);
    void updateUserCloudCallback(int _coinsAux);
    void migrationDoneCallback();
    
    void syncFinishCallback();
    
    void getServerTimeCallback(bool _error,std::string _serverTime);
    
private:
    
    bool _syncronizing;
};

#endif /* defined(__animalplanet__TTParseManager__) */
