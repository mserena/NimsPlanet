//
//  TTFacebook.h
//  animalplanet
//
//  Created by Marc Serena on 25/11/14.
//
//

#ifndef __animalplanet__TTFacebook__
#define __animalplanet__TTFacebook__

#include "thegame.h"
#include "screw/screw.h"

typedef enum recuestHeartTypes
{
    _RequestTypeSendHeart = 1,
    _RequestTypeAskforHeart,
}recuestHeartTypes;

using namespace cocos2d;

//evento de amigos a invitar
typedef struct FBFriendsInviteEvent {
    Vector<GraphUser*> friendsToInvite;
} FBFriendsInviteEvent;


class CC_DLL TTFacebook: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTFacebook* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTFacebook(){}
    
public:
    
    virtual ~TTFacebook();
    bool init(void);
    
    void disconnectFromFacebook();
    void connectUserToFacebook();
    
    void updateFacebookInfo();
    void updateAppRequests();
    void facebookInfoUpdated();
    void updateAllUserPhotos();
    
    std::string getStringFriends();
    std::string getMeId();
    std::string getNameByFacebookId(std::string _idUser);
    
    Texture2D* getTexturePhotoMe();
    std::string getUserFacebookId();
    
    Texture2D* getTexturePhotoUser(std::string idUser);
    
    void sendOrAskHeart(std::string _userIds, recuestHeartTypes _type);
    void acceptAllUserRequest();
    void showInviteFirendsDialog();
    
    bool isConnected();
    
    std::vector<GraphUser*> getRanking();
    
    //events from screw
    void onLoginStatusNotification();
    void onUserDetailsNotification();
    void onUserFriendsNotification();
    void onPhotoLoadedNotification(string _uid);
    void onRequestNotification();
    void onScoresNotification();
    
    void userConecctionAlerted();
    bool isUserConnection();
    
    void onEvent_FacebookConnect(EventCustom* e);

public:
    
    std::map<std::string,int> _friendsHighLevelInfo;
    
private:
    
    bool    _isUpdatingInfo;
    bool    _updatedMeInfo;
    bool    _userGetScore;
    bool    _updatedFriendsInfo;
    int     _usersToUpdatePhoto;
    bool    _userFacebookConnection;
    
    std::vector<EventListener*> _classListeners;
    
};

#endif /* defined(__animalplanet__TTFacebook__) */
