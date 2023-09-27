 //
//  TTFacebook.cpp
//  animalplanet
//
//  Created by Marc Serena on 25/11/14.
//
//

#include "TTFacebook.h"
#include "TTUIMap.h"
#include "TTEventsManager.h"
#include "TTUserInfo.h"
#include "TTLanguageManager.h"
#include "TTUserLives.h"
#include "TTUIStackManager.h"
#include "TTUIPopupSmart.h"
#include "TTConfigParser.h"
#include "TTParseManager.h"
#include "TTAnimalCubes.h"
#include "TTMapStopperManager.h"

using namespace screw::facebook;

//Singelton
static TTFacebook *_sharedFacebookManager = nullptr;

TTFacebook* TTFacebook::getInstance()
{
    if (!_sharedFacebookManager)
    {
        _sharedFacebookManager = new TTFacebook();
        _sharedFacebookManager->init();
    }
    
    return _sharedFacebookManager;
}

void TTFacebook::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedFacebookManager);
}

TTFacebook::~TTFacebook()
{
    removeSafeListenerEvent(_classListeners);
}

bool TTFacebook::init(void)
{
    // Inicializamos las variables
    _isUpdatingInfo = false;
    _updatedMeInfo = false;
    _updatedFriendsInfo = false;
    _userFacebookConnection = false;
    _userGetScore = false;
    
    // Inicializa la libreria de facebook
    Session::start();
    
    // Seteamos los listeners
    //SESSION
    EventListenerCustom *listenerSession= EventListenerCustom::create( FacebookLoginStatusChangedNotification, [=](EventCustom *event)
    {
        this->onLoginStatusNotification();
    });

    //DETALLE DE USUARIO
    EventListenerCustom *listenerMe = EventListenerCustom::create( FacebookUserDetailDidFetchNotification, [=](EventCustom *event)
    {
        this->onUserDetailsNotification();
    });
                                                                  
    //AMIGOS
    EventListenerCustom *listenerFriends = EventListenerCustom::create( FacebookFriendsDidFetchNotification, [=](EventCustom *event)
    {
        this->onUserFriendsNotification();
    });

    //SCORES
    EventListenerCustom *listenerScores= EventListenerCustom::create( FacebookScoresDidFetchNotification, [=](EventCustom *event)
    {
        this->onScoresNotification();
    });
    
    //REQUESTS
    EventListenerCustom *listenerRequests = EventListenerCustom::create(FacebookRequestsDidFetchNotification, [=](EventCustom *event)
    {
        this->onRequestNotification();
    });
    
    //PHOTOLOADER
    // Seteamos el callback para el PhotoLoader
    EventListenerCustom *listenerPhoto = EventListenerCustom::create( PhotoLoaderLoadedNotification, [=](EventCustom *event)
    {
        PhotoLoaderEvent *_event = static_cast<PhotoLoaderEvent*>(event);
        this->onPhotoLoadedNotification(_event->getUid());
    });
    
    _classListeners.push_back(registerListenerEvent(EVENT_FACEBOOK_CONNECTION,CC_CALLBACK_1(TTFacebook::onEvent_FacebookConnect,this)));
    
    //añadir listeners
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerSession,Director::getInstance()->getRunningScene());
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerMe,Director::getInstance()->getRunningScene());
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerFriends,Director::getInstance()->getRunningScene());
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerPhoto,Director::getInstance()->getRunningScene());
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRequests,Director::getInstance()->getRunningScene());
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerScores,Director::getInstance()->getRunningScene());
    
    //iniciar conexión (si habia un token, ya hace el open)
    Facebook::getInstance()->start();

    return true;
}

bool TTFacebook::isConnected()
{
    bool _screwConnected = Session::getActiveSession()!=nullptr && Session::getActiveSession()->isOpened();
    bool _parseConnected = TTParseManager::getInstance()->isUserConnectedToFacebook();

    CCLOG("FB TTFacebook::isConnected Screw[%d] Parse[%d]",_screwConnected, _parseConnected);

    if(_screwConnected && _parseConnected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TTFacebook::disconnectFromFacebook()
{
    // Cerramos la sesión de usuario
    Session::getActiveSession()->close();
}

void TTFacebook::connectUserToFacebook()
{
    if(Session::getActiveSession()!=nullptr && Session::getActiveSession()->isOpened())
    {
        Session::getActiveSession()->close();
    }
    
    // Abrimos la sessión de Facebook
    _userFacebookConnection = true;
    Session::getActiveSession()->open(true, {"user_friends"},
                                      DefaultAudience::PUBLIC,
                                      LoginBehavior::WITH_FALLBACK_TO_WEBVIEW);
}

void TTFacebook::updateFacebookInfo()
{
    CCLOG("CALLING updateFacebookInfo");
    // Actualizamos los datos de usuarios de Facebook
    _isUpdatingInfo = true;
    Facebook::getInstance()->fetchUserDetails(nullptr);
    Facebook::getInstance()->fetchFriends(nullptr);
    AppRequests::getInstance()->fetchAppRequests(nullptr);
}

void TTFacebook::updateAppRequests()
{
    AppRequests::getInstance()->fetchAppRequests(nullptr);
}

std::string TTFacebook::getStringFriends()
{
    std::string _friendsIds;
    Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
    for (auto f : friends)
    {
        _friendsIds += f->getId().c_str();
        if(friends.getIndex(f) != friends.size()-1)
        {
            _friendsIds += ",";
        }
    }
    return _friendsIds;
}

std::string TTFacebook::getMeId()
{
    std::string _me = "";
    if(Facebook::getInstance()->getUser() != nullptr) _me = Facebook::getInstance()->getUser()->getId();
    return _me;
}

std::string TTFacebook::getNameByFacebookId(std::string _idUser)
{
    if(_idUser == Facebook::getInstance()->getUser()->getId())
    {
        return Facebook::getInstance()->getUser()->getName();
    }
    
    Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
    for (auto f : friends)
    {
        if(f->getId() == _idUser)
        {
            return f->getName();
        }
    }
    
    return "";
}

void TTFacebook::facebookInfoUpdated()
{
    _updatedFriendsInfo = false;
    _updatedMeInfo = false;
    
    //Buscamos los scores
    Facebook::getInstance()->fetchScores(nullptr);
    
    // Descargamos la información de niveles de los usuarios
    TTParseManager::getInstance()->getFriendsHighLevel(getStringFriends());
    
    // Printamos la información de facebook actualizada
    CCLOG("Información de facebook actualizada");
    GraphUser *user = Facebook::getInstance()->getUser();
    if (user)
    {
        CCLOG("Usuario activo %s -> Nombre: %s",user->getId().c_str(),user->getName().c_str());
        Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
        for (auto f : friends)
        {
            CCLOG("Usuario amigo %s -> Nombre: %s",f->getId().c_str(),f->getName().c_str());
        }
        
        // Updateamos las fotos de los usuarios
        updateAllUserPhotos();
        
        if(_userFacebookConnection)
        {
            // Realizamos la conexión de usuario a nivel de Parse
            TTParseManager::getInstance()->connectUserToFacebook(user->getId().c_str());
        }
    }
    else
    {
        _isUpdatingInfo=false;
    }
}


void TTFacebook::updateAllUserPhotos()
{
    GraphUser *user = Facebook::getInstance()->getUser();
    if (user==nullptr)
    {
        //si hemos llegado aqui y no hay info usuario, el proceso de actualizacion ha acabado
        _isUpdatingInfo=false;
        return;
    }
    
    // Download de todas las fotos
    Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
    _usersToUpdatePhoto = (int)friends.size()+1;
    PhotoLoader::getInstance()->download(user->getId());
    for (auto f : friends)
    {
        std::string _idUser = f->getId();
        PhotoLoader::getInstance()->download(_idUser);
    }
}

Texture2D* TTFacebook::getTexturePhotoUser(std::string idUser)
{
    if (!PhotoLoader::getInstance()->isPhotoExist(idUser))
    {
        //si no existe la foto que se pide, se pide el download si estamos conectados a FB
        if (isConnected())
        {
            PhotoLoader::getInstance()->download(idUser);
        }
    }
    Texture2D *texture = PhotoLoader::getInstance()->loadTexture(idUser);
    return texture;
}

Texture2D* TTFacebook::getTexturePhotoMe()
{
    GraphUser *user = Facebook::getInstance()->getUser();
    return this->getTexturePhotoUser(user ? user->getId() : "000");
}

std::string TTFacebook::getUserFacebookId()
{
    GraphUser *user = Facebook::getInstance()->getUser();
    return user->getId();
}

void TTFacebook::sendOrAskHeart(std::string _userIds, recuestHeartTypes _type)
{
    RequestDialogBuilder *builder = new RequestDialogBuilder();
    builder->setTo(_userIds);
    builder->setType(_type);
    if(_type == _RequestTypeSendHeart)
    {
        builder->setTitle(TTLanguageManager::getInstance()->getTag(SEND_HEARTS));
        builder->setMessage(TTLanguageManager::getInstance()->getTag(SEND_HEARTS_DESC));
    }
    else
    {
        builder->setTitle(TTLanguageManager::getInstance()->getTag(ASK_FOR_HEARTS));
        builder->setMessage(TTLanguageManager::getInstance()->getTag(ASK_FOR_HEARTS_DESC));
    }
    
    builder->setCallback([](int error, const string &requestId, const list<string> &recipients)
    {
        if (requestId.length())
        {
            // Request succeeded
            CCLOG("Se han enviado los corazones a los usuarios indicados");
            sendEvent(EVENT_FACEBOOK_REQUEST_SEND);
        }
        else
        {
            TTUIStackManager::getInstance()->goToNewUI(UI_POPUP_SMART);
            TTUIPopupSmart::getInstance()->showPopupState(Director::getInstance()->getRunningScene(),SMART_POPUP_FACEBOOK_STATE,false);
        }
    });
    
    builder->build()->show();
    builder->release();
}

#pragma mark - screw event callbacks

void TTFacebook::onLoginStatusNotification()
{
    sendEvent(EVENT_FACEBOOK_CONNECTION_REFRESH_BUTTON);

    CCLOG("FB onLoginStatusNotification Session opened[%d] UpdatingInfo[%d]",Session::getActiveSession()->isOpened(),_isUpdatingInfo);
    // Si la sesión esta abierta buscamos actualizaciones en los datos de facebook
    if (Session::getActiveSession()->isOpened())
    {
        if(isUserConnection()) TTUIStackManager::getInstance()->showUIConnecting(Director::getInstance()->getRunningScene());
        
        if (!_isUpdatingInfo)
        {
            updateFacebookInfo();
        }
    }
    else
    {
        TTUIStackManager::getInstance()->hideUIConnecting(isUserConnection());
        
        sendEvent(EVENT_FACEBOOK_CONNECTION);
        _updatedFriendsInfo=false;
        _updatedMeInfo=false;
        _isUpdatingInfo=false;
    }
}

void TTFacebook::onUserDetailsNotification()
{
    CCLOG("FB onUserDetailsNotification");
    GraphUser *user = Facebook::getInstance()->getUser();
    _updatedMeInfo = true;
    if (user)
    {
        if(_updatedFriendsInfo)
        {
            facebookInfoUpdated();
        }
    }
}

void TTFacebook::onUserFriendsNotification()
{
    CCLOG("FB onUserFriendsNotification");
    _updatedFriendsInfo = true;
    if(_updatedMeInfo)
    {
        facebookInfoUpdated();
    }    
}

void TTFacebook::onRequestNotification()
{
    CCLOG("FB onRequestNotification");

    Vector<GraphRequest *> requests = AppRequests::getInstance()->getRequests();
    for (GraphRequest *r : requests)
    {
        CCLOG("REQUEST Type = %d, From -> Name =  %s  FacebookId = %s",r->getType(),r->getFrom()->getName().c_str(),r->getFrom()->getId().c_str());
    }
}

void TTFacebook::onPhotoLoadedNotification(string _uid)
{
    CCLOG("FB onPhotoLoadedNotification [%s]", _uid.c_str());
    //envio de evento con informacion de actualizacion
    FBDataEvent dataEvent;
    dataEvent.userId = _uid;
    sendEvent(EVENT_FACEBOOK_PHOTO,&dataEvent);
    sendEvent(EVENT_FACEBOOK_USERS_INFO_UPDATED);
    
    _usersToUpdatePhoto--;
    if(_usersToUpdatePhoto == 0)
    {
        // Toda la información de facebook ha sido actualizada
        _isUpdatingInfo = false;
    }
    
}

void TTFacebook::onScoresNotification()
{
    CCLOG("FB onScoresNotification");
    _userGetScore = true;
}


void TTFacebook::acceptAllUserRequest()
{
    Vector<GraphRequest *> requests = AppRequests::getInstance()->getRequests();
    for (GraphRequest *r : requests)
    {
        CCLOG("ACCEPT REQUEST Type = %d, From -> Name =  %s  FacebookId = %s",r->getType(),r->getFrom()->getName().c_str(),r->getFrom()->getId().c_str());
        
        if(r->getType() == _RequestTypeSendHeart)
        {
            // Nos han dado una vida
            TTUserLives::getInstance()->addOneLive();
        }
        else if(r->getType() == _RequestTypeAskforHeart)
        {
            // Enviamos una vida al usuario
            sendOrAskHeart(r->getFrom()->getId(),_RequestTypeSendHeart);
        }
        AppRequests::getInstance()->clearRequest(r->getId());
    }
}

void TTFacebook::showInviteFirendsDialog()
{
    RequestDialogBuilder *builder = new RequestDialogBuilder();
    builder->setTitle(TTLanguageManager::getInstance()->getTag(FACEBOOK_INVITE_FRIENDS_TITLE));
    builder->setMessage(TTLanguageManager::getInstance()->getTag(FACEBOOK_INVITE_FRIENDS_TEXT));
    builder->setFilters("app_non_users");
    
    builder->setCallback([](int error, const string &requestId, const list<string> &recipients){
        if (requestId.length()) {
            // Request succeeded verificamos cuantos usuarios han sido invitados
            int _numInvitedFriends = recipients.size();
            if(TTMapStopperManager::getInstance()->isStopperActive())
            {
                TTMapStopperManager::getInstance()->userInvitedFriends(_numInvitedFriends);
            }
            else
            {
                TTMapStopperManager::destroyInstance();
            }
        }
    });
    
    builder->build()->show();
    builder->release();
    
    // Enviamos evento de analitica a Parse
    std::map<std::string,std::string> _dimensions;
    TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_INVITE_FRIENDS,_dimensions);
}

std::vector<GraphUser*> TTFacebook::getRanking()
{
    std::vector<GraphUser*> _usersRanking;
    Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
    
    if(Facebook::getInstance()->getUser())
    {
        friends.pushBack(Facebook::getInstance()->getUser());
    }
    
    for(int i = 0; i < 3; i++)
    {
        GraphUser* _bestUser = nullptr;
        int _scoreAux = 0;
        for (auto f : friends)
        {
            if(f->getScore() >= _scoreAux)
            {
                _bestUser = f;
                _scoreAux = f->getScore();
            }
        }

        if(_bestUser != nullptr && _bestUser->getScore() > 0)
        {
            _usersRanking.push_back(_bestUser);
            friends.eraseObject(_bestUser);
        }
    }
    
    return _usersRanking;
}

void TTFacebook::userConecctionAlerted()
{
    _userFacebookConnection = false;
}

bool TTFacebook::isUserConnection()
{
    return _userFacebookConnection;
}

void TTFacebook::onEvent_FacebookConnect(EventCustom* e)
{

}
