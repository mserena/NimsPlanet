#import "ParseManager_iOS.h"


#import <Parse/Parse.h>

#define MAX_OBJECTS_TO_SEND 20


//funciones fuera de la clase

void storeUserLevelsInfoParsePartitioned(NSArray* _levelsInfo, int _indexInicial)
{
    NSMutableArray *_levelsInfoToSend = [NSMutableArray array];
    for(int i = _indexInicial; i < _indexInicial+MAX_OBJECTS_TO_SEND && i < _levelsInfo.count; i++)
    {
        [_levelsInfoToSend addObject:_levelsInfo[i]];
    }
    
    //Intentamos un guardado de objetos
    [PFObject saveAllInBackground:_levelsInfoToSend block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            //Marcamos el objeto como no sincronizado con el cloud
            [PFObject unpinAllInBackground:_levelsInfoToSend withName:@"NoSyncCloud"];
            if(_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.count)
            {
                storeUserLevelsInfoParsePartitioned(_levelsInfo,_indexInicial+MAX_OBJECTS_TO_SEND);
            }
            else
            {
                ParseManager_iOS::syncCloudDataFacebookParse();
            }
        }
        else
        {
            TTParseManager::getInstance()->syncFinishCallback();
        }
    }];
}

void saveObjectsToAnonymousUserPartitioned(NSArray* _levelsInfo, int _indexInicial)
{
    NSMutableArray *_levelsInfoToSend = [NSMutableArray array];
    for(int i = _indexInicial; i < _indexInicial+MAX_OBJECTS_TO_SEND && i < _levelsInfo.count; i++)
    {
        [_levelsInfoToSend addObject:_levelsInfo[i]];
    }
    
    //Intentamos un guardado de objetos
    [PFObject saveAllInBackground:_levelsInfoToSend block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            //Marcamos el objeto como no sincronizado con el cloud
            if(_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.count)
            {
                saveObjectsToAnonymousUserPartitioned(_levelsInfo,_indexInicial+MAX_OBJECTS_TO_SEND);
            }
            else
            {
                // Actualizamos el nivel actual del usuario
                PFUser *user = [PFUser currentUser];
                int highLevel = [[user objectForKey:@"highLevel"] intValue] + _levelsInfo.count;
                NSNumber *actualHighLevel = [NSNumber numberWithInt:highLevel];
                [user setObject:actualHighLevel forKey:@"highLevel"];
                [user saveInBackground];
                
                std::vector<int> _stars;
                for(PFObject *objUA in _levelsInfo)
                {
                    _stars.push_back([[objUA objectForKey:@"stars"] intValue]);
                }
                
                // Callback de conexión realizada
                TTParseManager::getInstance()->parseUpdateFacebookCallbackDone(_stars);
                TTParseManager::getInstance()->syncFinishCallback();
            }
        }
        else
        {
            TTParseManager::getInstance()->syncFinishCallback();
        }
    }];
}

void saveObjectsToFacebookUserPartitioned(NSArray* _levelsInfo, int _indexInicial)
{
    NSMutableArray *_levelsInfoToSend = [NSMutableArray array];
    for(int i = _indexInicial; i < _indexInicial+MAX_OBJECTS_TO_SEND && i < _levelsInfo.count; i++)
    {
        [_levelsInfoToSend addObject:_levelsInfo[i]];
    }
    
    //Intentamos un guardado de objetos
    [PFObject saveAllInBackground:_levelsInfoToSend block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            //Marcamos el objeto como no sincronizado con el cloud
            if(_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.count)
            {
                saveObjectsToFacebookUserPartitioned(_levelsInfo,_indexInicial+MAX_OBJECTS_TO_SEND);
            }
            else
            {
                // Callback de conexión realizada
                TTParseManager::getInstance()->parseLinkedToFacebookCallbackDoneNoChanges();
            }
        }
        else
        {
            TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
        }
    }];
}

void saveObjectsToAnonymousUserFromFBPartitioned(NSArray* _levelsInfo, int _indexInicial)
{
    NSMutableArray *_levelsInfoToSend = [NSMutableArray array];
    for(int i = _indexInicial; i < _indexInicial+MAX_OBJECTS_TO_SEND && i < _levelsInfo.count; i++)
    {
        [_levelsInfoToSend addObject:_levelsInfo[i]];
    }
    
    //Intentamos un guardado de objetos
    [PFObject saveAllInBackground:_levelsInfoToSend block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            //Marcamos el objeto como no sincronizado con el cloud
            if(_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.count)
            {
                saveObjectsToAnonymousUserFromFBPartitioned(_levelsInfo,_indexInicial+MAX_OBJECTS_TO_SEND);
            }
            else
            {
                // Actualizamos el nivel actual del usuario
                PFUser *user = [PFUser currentUser];
                int highLevel = [[user objectForKey:@"highLevel"] intValue] + _levelsInfo.count;
                NSNumber *actualHighLevel = [NSNumber numberWithInt:highLevel];
                [user setObject:actualHighLevel forKey:@"highLevel"];
                [user saveInBackground];
                
                std::vector<int> _stars;
                for(PFObject *objUA in _levelsInfo)
                {
                    _stars.push_back([[objUA objectForKey:@"stars"] intValue]);
                }
                
                // Callback de conexión realizada
                TTParseManager::getInstance()->parseLinkedToFacebookCallbackDoneAddLevels(_stars);
            }
        }
        else
        {
            TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
        }
    }];
}

void deleteAllFromLevelsInfoPartitioned(NSArray* _levelsInfo, int _indexInicial, int _facebookHighLevel)
{
    NSMutableArray *_levelsInfoToDelete = [NSMutableArray array];
    for(int i = _indexInicial; i < _indexInicial+MAX_OBJECTS_TO_SEND && i < _levelsInfo.count; i++)
    {
        [_levelsInfoToDelete addObject:_levelsInfo[i]];
    }
    
    //Intentamos un guardado de objetos
    [PFObject deleteAllInBackground:_levelsInfoToDelete block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            //Marcamos el objeto como no sincronizado con el cloud
            if(_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.count)
            {
                deleteAllFromLevelsInfoPartitioned(_levelsInfo,_indexInicial+MAX_OBJECTS_TO_SEND,_facebookHighLevel);
            }
            else
            {
                PFUser *user = [PFUser currentUser];
                NSNumber *actualHighLevel = [NSNumber numberWithInt:_facebookHighLevel];
                [user setObject:actualHighLevel forKey:@"highLevel"];
                [user saveInBackground];
                
                TTParseManager::getInstance()->parseLinkedToFacebookAdjustLevelsCallbackDone(_facebookHighLevel);
            }
        }
        else
        {
            TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
        }
    }];
}


std::string ParseManager_iOS::getUserParseIDParse()
{
    PFUser *user = [PFUser currentUser];
    NSString* idUser = [user objectForKey:@"username"];
    if(idUser == nil)
    {
        return "";
    }
    else
    {
        return [idUser UTF8String];
    }
}

//metodos de la clase ParseManager_iOS

void ParseManager_iOS::setUserActualLevelParse(int _level)
{
    CCLOG("ParseManager_iOS::setUserActualLevelParse");
    // Lo guardamos como nivel más alto del usuario
    NSNumber *levelIdNS = [NSNumber numberWithInt:_level];
    [[PFUser currentUser] setObject:levelIdNS forKey:@"highLevel"];
    [[PFUser currentUser] saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error)
    {
        if(!error && succeeded && isUserConnectedToFacebookParse())
        {
            NSString *userFacebookIdNS = [[PFUser currentUser] objectForKey:@"facebookId"];
            PFQuery *getUF = [PFQuery queryWithClassName:@"UserFacebook"];
            [getUF whereKey:@"idFacebook" equalTo:userFacebookIdNS];
            [getUF getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error)
            {
                if(!error && object != nil)
                {
                    object[@"highLevel"] = [[PFUser currentUser] objectForKey:@"highLevel"];
                    [object saveInBackground];
                }
            }];
        }
    }];
}

void ParseManager_iOS::migrateUserLevelsParse(std::vector<levelInfo> _levelsInfo)
{
    CCLOG("ParseManager_iOS::migrateUserLevelsParse");
    // Creamos el array de objetos
    NSMutableArray *objects = [NSMutableArray array];
    for(int i = 0; i < _levelsInfo.size(); i++)
    {
        NSNumber *levelIdNS = [NSNumber numberWithInt:_levelsInfo[i]._levelId];
        NSNumber *scoreNS = [NSNumber numberWithInt:_levelsInfo[i]._newScore];
        NSNumber *starsNS = [NSNumber numberWithInt:_levelsInfo[i]._stars];
        NSNumber *movementsNS = [NSNumber numberWithInt:_levelsInfo[i]._movements];
        NSNumber *timeNS = [NSNumber numberWithInt:_levelsInfo[i]._timePlaying];
        PFUser *user = [PFUser currentUser];
        
        PFObject *levelInfoObj = [PFObject objectWithClassName:@"LevelsInfo"];
        levelInfoObj[@"user"] = user;
        levelInfoObj[@"level"] = levelIdNS;
        levelInfoObj[@"score"] = scoreNS;
        levelInfoObj[@"stars"] = starsNS;
        levelInfoObj[@"movements"] = movementsNS;
        levelInfoObj[@"timePlaying"] = timeNS;
        
        [objects addObject:levelInfoObj];
    }
    
    [PFObject pinAllInBackground:objects withName:@"NoSyncCloud" block:^(BOOL succeeded, NSError *error) {
        if(!error)
        {
            TTParseManager::getInstance()->syncCloud();
            //Llamamos al callback de la sincronización
            TTParseManager::getInstance()->migrationDoneCallback();
        }
    }];
}

void ParseManager_iOS::migrateUserParse(std::vector<levelInfo> _levelsInfo, std::string _facebookId)
{
    CCLOG("ParseManager_iOS::migrateUserParse");

    // Seteamos el highLevel de los usuarios
    setUserActualLevelParse(_levelsInfo.size()-1);
    
    if(_facebookId != "")
    {
        PFUser *user = [PFUser currentUser];
        NSString *userFacebookIdNS = [NSString stringWithCString:_facebookId.c_str() encoding:[NSString defaultCStringEncoding]];
        [user setObject:userFacebookIdNS forKey:@"facebookId"];
        
        PFObject *newUserFacebook = [PFObject objectWithClassName:@"UserFacebook"];
        newUserFacebook[@"idFacebook"] = userFacebookIdNS;
        newUserFacebook[@"highLevel"] = [user objectForKey:@"highLevel"];
        [newUserFacebook pinInBackgroundWithName:@"UserFacebookNoSync" block:^(BOOL succeeded, NSError *error) {
            if(!error)
            {
                migrateUserLevelsParse(_levelsInfo);
            }
        }];
    }
    else
    {
        migrateUserLevelsParse(_levelsInfo);
    }
}

void ParseManager_iOS::storeUserLevelInfoParse(int _levelId, int _newScore, int _stars, int _movements, int _timePlaying)
{
    CCLOG("ParseManager_iOS::storeUserLevelInfoParse");
    
    NSNumber *levelIdNS = [NSNumber numberWithInt:_levelId];
    NSNumber *scoreNS = [NSNumber numberWithInt:_newScore];
    NSNumber *starsNS = [NSNumber numberWithInt:_stars];
    NSNumber *movementsNS = [NSNumber numberWithInt:_movements];
    NSNumber *timeNS = [NSNumber numberWithInt:_timePlaying];
    PFUser *user = [PFUser currentUser];
    
    PFObject *levelsInfoObj = [PFObject objectWithClassName:@"LevelsInfo"];
    levelsInfoObj[@"user"] = user;
    levelsInfoObj[@"level"] = levelIdNS;
    levelsInfoObj[@"score"] = scoreNS;
    levelsInfoObj[@"stars"] = starsNS;
    levelsInfoObj[@"movements"] = movementsNS;
    levelsInfoObj[@"timePlaying"] = timeNS;
	
    //no guardar siempre el actual level, ya que en una repeticion de nivel se guradaria como actua level uno equivocado
	//ya se controla desde la aplicacion 
	//setUserActualLevelParse(_levelId);
    
    //Marcamos el objeto como no sincronizado con el cloud e intentamos sincronizarlo
    [levelsInfoObj pinInBackgroundWithName:@"NoSyncCloud" block:^(BOOL succeeded, NSError *error)
    {
        [levelsInfoObj saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error)
         {
             if(!error && succeeded)
             {
                 [levelsInfoObj unpinInBackgroundWithName:@"NoSyncCloud"];
             }
         }];
    }];
}

void ParseManager_iOS::syncCloudDataFacebookParse()
{
    CCLOG("ParseManager_iOS::syncCloudDataFacebookParse");
    
    // Intenta sincronizar la clase de niveles de cloud a local
    // Si el usuario no esta conectado a Facebook, nunca hay que sincronizar de Cloud
    if(isUserConnectedToFacebookParse())
    {
        // Sincronizamos los datos con el usuario de Facebook
        PFUser *user = [PFUser currentUser];
        NSString *userFacebookIdNS = [user objectForKey:@"facebookId"];
        PFQuery *getUFLevels = [PFQuery queryWithClassName:@"LevelsInfoFacebook"];
        [getUFLevels whereKey:@"idFacebook" equalTo:userFacebookIdNS];
        [getUFLevels whereKey:@"level" greaterThan:[user objectForKey:@"highLevel"]] ;
        [getUFLevels orderByAscending:@"level"];
        getUFLevels.limit = 1000;
        [getUFLevels findObjectsInBackgroundWithBlock:^(NSArray *objectsUF, NSError *error)
         {
             if(!error && objectsUF.count > 0)
             {
                 //Gana Facebook, solo actualizamos los objetos restantes
                 NSMutableArray *objectsToAnonymousUser = [NSMutableArray array];
                 for(PFObject *objUF in objectsUF)
                 {
                     PFObject *newObjUA = [PFObject objectWithClassName:@"LevelsInfo"];
                     newObjUA[@"user"] = user;
                     newObjUA[@"level"] = [objUF objectForKey:@"level"];
                     newObjUA[@"score"] = [objUF objectForKey:@"score"];
                     newObjUA[@"stars"] = [objUF objectForKey:@"stars"];
                     newObjUA[@"movements"] = [objUF objectForKey:@"movements"];
                     newObjUA[@"timePlaying"] = [objUF objectForKey:@"timePlaying"];
                     [objectsToAnonymousUser addObject:newObjUA];
                 }
                 
                 saveObjectsToAnonymousUserPartitioned(objectsToAnonymousUser,0);
             }
             /*else if(!error && objectsUF.count == 0)
             {
                 // Verificamos que no haya más niveles en local
                 PFQuery *queryClodFU = [PFQuery queryWithClassName:@"UserFacebook"];
                 [queryClodFU whereKey:@"idFacebook" equalTo:[user objectForKey:@"facebookId"]];
                 [queryClodFU getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error)
                 {
                    if(!error && object != nil)
                    {
                        __block int highLevelFacebook = [[object objectForKey:@"highLevel"] intValue];
                        int highLevelUser = [[user objectForKey:@"highLevel"] intValue];
                        if(highLevelUser > highLevelFacebook)
                        {
                            PFQuery *getUserLevels = [PFQuery queryWithClassName:@"LevelsInfo"];
                            [getUserLevels whereKey:@"user" equalTo:user];
                            [getUserLevels whereKey:@"level" greaterThan:[NSNumber numberWithInt:highLevelFacebook]];
                            getUserLevels.limit = 1000;
                            [getUserLevels findObjectsInBackgroundWithBlock:^(NSArray *objectsToDelete, NSError *error)
                             {
                                 if(!error)
                                 {
                                     [PFObject deleteAllInBackground:objectsToDelete block:^(BOOL succeeded, NSError *error) {
                                         if(!error && succeeded)
                                         {
                                             // Actualizamos el nivel actual del usuario
                                             NSNumber *actualHighLevel = [NSNumber numberWithInt:highLevelFacebook];
                                             [user setObject:actualHighLevel forKey:@"highLevel"];
                                             [user saveInBackground];
                                             
                                             // Callback para actualizar datos locales del usuario
                                             TTParseManager::getInstance()->parseLinkedToFacebookAdjustLevelsCallbackDone(highLevelFacebook);
                                             TTParseManager::getInstance()->syncFinishCallback();
                                         }
                                         else
                                         {
                                             TTParseManager::getInstance()->syncFinishCallback();
                                         }
                                     }];
                                 }
                                 else
                                 {
                                     TTParseManager::getInstance()->syncFinishCallback();
                                 }
                             }];
                        }
                        else
                        {
                            TTParseManager::getInstance()->syncFinishCallback();
                        }
                    }
                    else
                    {
                        TTParseManager::getInstance()->syncFinishCallback();
                    }
                 }];

             }*/
             else
             {
                 TTParseManager::getInstance()->syncFinishCallback();
             }
         }];
    }else{
        TTParseManager::getInstance()->syncFinishCallback();
    }
}

void ParseManager_iOS::syncCloudDataParse()
{
    CCLOG("ParseManager_iOS::syncCloudDataParse");
    
    // Intena sincronizar la clase de niveles de local a cloud
    PFQuery *queryLocalToCloud = [PFQuery queryWithClassName:@"LevelsInfo"];
    [queryLocalToCloud fromPinWithName:@"NoSyncCloud"];
    [queryLocalToCloud findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error)
     {
         if(!error)
         {
             CCLOG("Se han encontrado %d objetos de nivel no sincronizados...intentando sincronizarlos...",objects.count);
             
             if(objects.count != 0)
             {
                 //Intentamos store de todos los objetos
                 storeUserLevelsInfoParsePartitioned(objects,0);
             }
             else
             {
                 syncCloudDataFacebookParse();
             }
         }
         else
         {
             TTParseManager::getInstance()->syncFinishCallback();
         }
     }];
}

void ParseManager_iOS::syncCloudParse()
{
    CCLOG("ParseManager_iOS::syncCloudParse");
    // Guardamos los datos de usuario
    [[PFUser currentUser] saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
        
        if(!error)
        {
            // Miramos si hay que sincronizar usuario de Facebook
            PFQuery *queryLocalFU = [PFQuery queryWithClassName:@"UserFacebook"];
            [queryLocalFU fromPinWithName:@"UserFacebookNoSync"];
            [queryLocalFU getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error) {
                if([error code] == kPFErrorObjectNotFound)
                {
                    syncCloudDataParse();
                }
                else if(!error && object != nil)
                {
                    //Creamos el usuario de Facebook si no existe
                    PFQuery *queryClodFU = [PFQuery queryWithClassName:@"UserFacebook"];
                    [queryClodFU whereKey:@"idFacebook" equalTo:[object objectForKey:@"idFacebook"]];
                    [queryClodFU getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error) {
                        if([error code] == kPFErrorObjectNotFound)
                        {
                            [object setObject:[[PFUser currentUser] objectForKey:@"highLevel"] forKey:@"highLevel"];
                            [object saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
                                if(!error && succeeded)
                                {
                                    [PFObject unpinAllObjectsInBackgroundWithName:@"UserFacebookNoSync"];
                                    syncCloudDataParse();
                                }
                                else
                                {
                                    TTParseManager::getInstance()->syncFinishCallback();
                                }
                            }];
                        }
                        else if(!error)
                        {
                            [PFObject unpinAllObjectsInBackgroundWithName:@"UserFacebookNoSync"];
                            syncCloudDataParse();
                        }
                        else
                        {
                            TTParseManager::getInstance()->syncFinishCallback();
                        }
                    }];
                }
                else
                {
                    TTParseManager::getInstance()->syncFinishCallback();
                }
            }];
        }
        else
        {
            TTParseManager::getInstance()->syncFinishCallback();
        }
    }];
}

void ParseManager_iOS::storeUserInfoParse(int _coins, std::string _boardBoostersInfo, std::string _gameBoostersInfo, std::string _appVersion)
{
    CCLOG("ParseManager_iOS::storeUserInfoParse");
    
    NSString *platformNS = @"iOS";
    NSString *boardBoostersInfoNS = [NSString stringWithCString:_boardBoostersInfo.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *gameBoostersInfoNS = [NSString stringWithCString:_gameBoostersInfo.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *appVersionNS = [NSString stringWithCString:_appVersion.c_str() encoding:[NSString defaultCStringEncoding]];
    NSNumber *coinsNS = [NSNumber numberWithInt:_coins];
    
    [[PFUser currentUser] setObject:platformNS forKey:@"platform"];
    [[PFUser currentUser] setObject:boardBoostersInfoNS forKey:@"boostersBoardInfo"];
    [[PFUser currentUser] setObject:gameBoostersInfoNS forKey:@"boostersGameInfo"];
    [[PFUser currentUser] setObject:appVersionNS forKey:@"appVersion"];
    [[PFUser currentUser] setObject:coinsNS forKey:@"coins"];
    [[PFUser currentUser] saveInBackground];
}

void ParseManager_iOS::verifyUserCloudUpdatesParse()
{
    CCLOG("ParseManager_iOS::verifyUserCloudUpdatesParse");
    PFUser *user = [PFUser currentUser];
    NSString* _nameUser = [user objectForKey:@"username"];
    if(_nameUser == nil)
    {
        //No se ha creado el usuario aún
        return;
    }
    
    PFQuery *query= [PFUser query];
    [query whereKey:@"username" equalTo:_nameUser];
    [query getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error)
    {
        if(!error)
        {
            int coinsAux = [[object objectForKey:@"coinsAux"] intValue];
            if(coinsAux != 0)
            {
                [[PFUser currentUser] setObject:@0 forKey:@"coinsAux"];
                [[PFUser currentUser] saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
                    if(!error && succeeded)
                    {
                        TTParseManager::getInstance()->updateUserCloudCallback(coinsAux);
                    }
                }];
            }
        }
    }];
}

bool ParseManager_iOS::isUserConnectedToFacebookParse()
{
    CCLOG("ParseManager_iOS::isUserConnectedToFacebookParse");

    PFUser *user = [PFUser currentUser];
    NSString *facebookIdUser = [user objectForKey:@"facebookId"];
    if(facebookIdUser == nil || [facebookIdUser  isEqual: @""])
    {
        return false;
    }
    return true;
}

void ParseManager_iOS::disconnectUserToFacebookParse()
{
    CCLOG("ParseManager_iOS::disconnectUserToFacebookParse");

    PFUser *user = [PFUser currentUser];
    [user setObject:@"" forKey:@"facebookId"];
    [user saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error)
     {
         if(!error && succeeded)
         {
             // Callback de desconexión a Facebook
             TTParseManager::getInstance()->parseUnLinkedToFacebookCallbackDone();
         }
         else
         {
             // Callback de error de desconexión a Facebook
             TTParseManager::getInstance()->parseUnLinkedToFacebookCallbackFail();
         }
     }];
}

void ParseManager_iOS::connectUserToFacebookParse(std::string _userIdFacebook)
{
    CCLOG("ParseManager_iOS::connectUserToFacebookParse");
    
    // Primero verificamos que no existan objetos sin sincronizar
    PFQuery *queryLocalNoSyncObjs = [PFQuery queryWithClassName:@"LevelsInfo"];
    [queryLocalNoSyncObjs fromPinWithName:@"NoSyncCloud"];
    [queryLocalNoSyncObjs findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error)
     {
         if(!error && objects.count > 0)
         {
             CCLOG("Se han encontrado %d objetos de nivel no sincronizados...no podemos conectar a Facebook...",objects.count);
             // Sincronizamos
             TTParseManager::getInstance()->syncCloud();
             // Callback de error de conexión a Facebook
             TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
         }
         else if(!error && objects.count == 0)
         {
             // Verificamos si existe el usuario de Facebook
             NSString *userFacebookIdNS = [NSString stringWithCString:_userIdFacebook.c_str() encoding:[NSString defaultCStringEncoding]];
             PFQuery *queryUserFacebook = [PFQuery queryWithClassName:@"UserFacebook"];
             [queryUserFacebook whereKey:@"idFacebook" equalTo:userFacebookIdNS];
             [queryUserFacebook getFirstObjectInBackgroundWithBlock:^(PFObject *object, NSError *error)
              {
                  if([error code] == kPFErrorObjectNotFound)
                  {
                      // No existe el usuario, creamos el registo para el usuario de Facebook y sincronizamos
                      PFUser *user = [PFUser currentUser];
                      [user setObject:userFacebookIdNS forKey:@"facebookId"];
                      [user saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
                          if (!error && succeeded)
                          {
                              //crear el usuario de FB
                              PFObject *newUserFacebook = [PFObject objectWithClassName:@"UserFacebook"];
                              newUserFacebook[@"idFacebook"] = userFacebookIdNS;
                              newUserFacebook[@"highLevel"] = [user objectForKey:@"highLevel"];
                              [newUserFacebook saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error)
                               {
                                   if(!error && succeeded)
                                   {
                                       // Sincronizamos el nuevo usuario de Facebook creado copiando todos los datos del usuario anonimo del dispositivo
                                         PFQuery *getUserLevels = [PFQuery queryWithClassName:@"LevelsInfo"];
                                        [getUserLevels whereKey:@"user" equalTo:user];
                                        [getUserLevels orderByAscending:@"level"];
                                        getUserLevels.limit = 1000;
                                        [getUserLevels findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error)
                                         {
                                             if (!error)
                                             {
                                                 NSMutableArray *objectsToFacebookUser = [NSMutableArray array];
                                                 for (PFObject *object in objects)
                                                 {
                                                     PFObject *objToFacebook = [PFObject objectWithClassName:@"LevelsInfoFacebook"];
                                                     objToFacebook[@"idFacebook"] = userFacebookIdNS;
                                                     objToFacebook[@"level"] = [object objectForKey:@"level"];
                                                     objToFacebook[@"score"] = [object objectForKey:@"score"];
                                                     objToFacebook[@"stars"] = [object objectForKey:@"stars"];
                                                     objToFacebook[@"movements"] = [object objectForKey:@"movements"];
                                                     objToFacebook[@"timePlaying"] = [object objectForKey:@"timePlaying"];
                                                     [objectsToFacebookUser addObject:objToFacebook];
                                                 }
                                                 saveObjectsToFacebookUserPartitioned(objectsToFacebookUser,0);
                                             }
                                             else
                                             {
                                                 // Callback de error de conexión a Facebook
                                                 TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                                             }
                                         }];
                                   }
                                   else
                                   {
                                       //limpiamos el usuario anonimo del facebook, porque no se ha podido crear el de FB
                                       [user setObject:@"" forKey:@"facebookId"];
                                       [user saveInBackground];
                                       TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                                  }
                               }];
                          }
                          else
                          {
                              // Callback de error de conexión a Facebook
                              TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                          }
                      }];
                      
                  }
                  else if(!error)
                  {
                      PFUser *user = [PFUser currentUser];
                      [user setObject:userFacebookIdNS forKey:@"facebookId"];
                      [user saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
                          if(!error && succeeded)
                          {
                              PFUser *user = [PFUser currentUser];
                              int _facebookHighLevel = [[object objectForKey:@"highLevel"] intValue];
                              int _userHighLevel = [[user objectForKey:@"highLevel"] intValue];
                              
                              if(_facebookHighLevel > _userHighLevel)
                              {
                                  // Tenemos que añadir datos al usuario anónimo
                                  PFQuery *getUFLevels = [PFQuery queryWithClassName:@"LevelsInfoFacebook"];
                                  [getUFLevels whereKey:@"idFacebook" equalTo:userFacebookIdNS];
                                  [getUFLevels whereKey:@"level" greaterThan:[user objectForKey:@"highLevel"]] ;
                                  [getUFLevels orderByAscending:@"level"];
                                  getUFLevels.limit = 1000;
                                  [getUFLevels findObjectsInBackgroundWithBlock:^(NSArray *objectsUF, NSError *error)
                                   {
                                       if(!error && objectsUF.count > 0)
                                       {
                                           //Gana Facebook, solo actualizamos los objetos restantes
                                           NSMutableArray *objectsToAnonymousUser = [NSMutableArray array];
                                           for(PFObject *objUF in objectsUF)
                                           {
                                               PFObject *newObjUA = [PFObject objectWithClassName:@"LevelsInfo"];
                                               newObjUA[@"user"] = user;
                                               newObjUA[@"level"] = [objUF objectForKey:@"level"];
                                               newObjUA[@"score"] = [objUF objectForKey:@"score"];
                                               newObjUA[@"stars"] = [objUF objectForKey:@"stars"];
                                               newObjUA[@"movements"] = [objUF objectForKey:@"movements"];
                                               newObjUA[@"timePlaying"] = [objUF objectForKey:@"timePlaying"];
                                               [objectsToAnonymousUser addObject:newObjUA];
                                           }
                                           saveObjectsToAnonymousUserFromFBPartitioned(objectsToAnonymousUser,0);
                                        }
                                       else
                                       {
                                           // Callback de error de conexión a Facebook
                                           TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                                       }
                                   }];
                                  
                              }
                              else if(_facebookHighLevel < _userHighLevel)
                              {
                                  PFQuery *getUserLevels = [PFQuery queryWithClassName:@"LevelsInfo"];
                                  [getUserLevels whereKey:@"user" equalTo:user];
                                  [getUserLevels whereKey:@"level" greaterThan:[NSNumber numberWithInt:_facebookHighLevel]];
                                  getUserLevels.limit = 1000;
                                  [getUserLevels findObjectsInBackgroundWithBlock:^(NSArray *objectsToDelete, NSError *error)
                                   {
                                       if(!error)
                                       {
                                           deleteAllFromLevelsInfoPartitioned(objectsToDelete, 0, _facebookHighLevel);
                                        }
                                       else
                                       {
                                           TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                                       }
                                   }];

                              }
                              else
                              {
                                  // Callback de conexión realizada
                                  TTParseManager::getInstance()->parseLinkedToFacebookCallbackDoneNoChanges();
                              }
                          }
                          else
                          {
                              // Callback de error de conexión a Facebook
                              TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                          }
                      }];
                      
                  }
                  else
                  {
                      // Callback de error de conexión a Facebook
                      TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
                  }
                  
              }];
         }
         else
         {
             // Callback de error de conexión a Facebook
             TTParseManager::getInstance()->parseLinkedToFacebookCallbackFail();
         }
     }];
}

void ParseManager_iOS::getFriendsHighLevelParse(std::string _friendsIds)
{
    CCLOG("ParseManager_iOS::getFriendsHighLevelParse");

    NSString *friendsIdsNS = [NSString stringWithCString:_friendsIds.c_str() encoding:[NSString defaultCStringEncoding]];
    NSArray *friendsIdsArray = [friendsIdsNS componentsSeparatedByString:@","];
    
    PFQuery *usersFriends = [PFQuery queryWithClassName:@"UserFacebook"];
    [usersFriends whereKey:@"idFacebook" containedIn:friendsIdsArray];
    [usersFriends selectKeys:@[@"idFacebook", @"highLevel"]];
    usersFriends.limit = 1000;
    [usersFriends findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error)
    {
        if (!error)
        {
            CCLOG("Successfully retrieved %d friends from Parse",objects.count);
            std::map<std::string,int> _friendsHighLevel;
            for (PFObject *object in objects)
            {
                NSString *idFriend = [object objectForKey:@"idFacebook"];
                int highLevel = [[object objectForKey:@"highLevel"] intValue];
                _friendsHighLevel.insert(std::make_pair([idFriend UTF8String],highLevel));
                
            }
            TTParseManager::getInstance()->getFriendsHighLevelCallback(_friendsHighLevel);
        }
        else
        {
            // Error en la consulta
        }
    }];
    
}

void ParseManager_iOS::getRankingLevelParse(int _level, std::string _friendsIds, std::string _meId)
{
    CCLOG("ParseManager_iOS::getRankingLevelParse");

    NSNumber *levelNS = [NSNumber numberWithInt:_level];
    NSString *friendsIdsNS = [NSString stringWithCString:_friendsIds.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *meIdNS = [NSString stringWithCString:_meId.c_str() encoding:[NSString defaultCStringEncoding]];
    friendsIdsNS = [friendsIdsNS stringByAppendingString:@","];
    friendsIdsNS = [friendsIdsNS stringByAppendingString:meIdNS];
    NSArray *friendsIdsArray = [friendsIdsNS componentsSeparatedByString:@","];
    
    PFQuery *usersFriends = [PFQuery queryWithClassName:@"LevelsInfoFacebook"];
    [usersFriends whereKey:@"idFacebook" containedIn:friendsIdsArray];
    [usersFriends whereKey:@"level" equalTo:levelNS];
    [usersFriends orderByDescending:@"score"];
    usersFriends.limit = 4;
    [usersFriends findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error)
    {
        if (!error)
        {
            std::vector<facebookFriendScoreInfo> _rankingLevel;
            for (PFObject *object in objects)
            {
                NSString *facebookId = [object objectForKey:@"idFacebook"];
                int level = [[object objectForKey:@"level"] intValue];
                int score = [[object objectForKey:@"score"] intValue];
                CCLOG("Ranking -> Usuario %s nivel = %d score = %d",[facebookId UTF8String],level,score);
                
                facebookFriendScoreInfo _userScore;
                _userScore._name = [facebookId UTF8String];
                _userScore._score = score;
                _rankingLevel.push_back(_userScore);
            }
            TTParseManager::getInstance()->getRankingLevelCallback(true,_rankingLevel);
        }
        else
        {
            //Error en la consulta
            std::vector<facebookFriendScoreInfo> _rankingLevel;
            TTParseManager::getInstance()->getRankingLevelCallback(false,_rankingLevel);
        }
    }];
}

void ParseManager_iOS::sendAnalyticEventParse(std::string _eventName, std::map<std::string,std::string> _dimensions)
{
    CCLOG("ParseManager_iOS::sendAnalyticEventParse");

    NSMutableDictionary *dimensions = [NSMutableDictionary dictionary];
    std::map<std::string,std::string>::iterator _dimensionsItr;
    for(_dimensionsItr = _dimensions.begin(); _dimensionsItr != _dimensions.end(); ++_dimensionsItr)
    {
        NSString *key = [NSString stringWithCString:(*_dimensionsItr).first.c_str() encoding:[NSString defaultCStringEncoding]];
        NSString *value = [NSString stringWithCString:(*_dimensionsItr).second.c_str() encoding:[NSString defaultCStringEncoding]];
        
        [dimensions setObject:value forKey:key];
    }
    
    NSString *eventName = [NSString stringWithCString:_eventName.c_str() encoding:[NSString defaultCStringEncoding]];
    [PFAnalytics trackEvent:eventName dimensions:dimensions];
}

void ParseManager_iOS::setLevelStatisticsParse(int _level, bool _win)
{
    [PFCloud callFunctionInBackground:@"levelStatistics" withParameters:@{@"idLevel": [NSNumber numberWithInt:_level],@"levelWin": [NSNumber numberWithBool:_win]}];
}

void ParseManager_iOS::getServerTimeParse()
{
    CCLOG("Current System Time Zone %s",[[[NSTimeZone systemTimeZone] name] UTF8String]);
    [PFCloud callFunctionInBackground:@"getServerTime" withParameters:@{@"timeZone":[[NSTimeZone systemTimeZone] name]} block:^(NSString *object, NSError *error)
    {
        if(!error)
        {
            // Enviamos callback de respuesta con el string de la hora
            TTParseManager::getInstance()->getServerTimeCallback(false,[object UTF8String]);
        }
        else
        {
            // Enviamos callback de error al intentar obtener la hora
            TTParseManager::getInstance()->getServerTimeCallback(true,"");
        }
    }];
}
