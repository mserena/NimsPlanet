//
//  TTNotificationManager.h
//  animalplanet
//
//  Created by Luis Prieto on 06/02/15.
//
//

#ifndef animalplanet_TTNotificationManager_h
#define animalplanet_TTNotificationManager_h

#include "cocos2d.h"

//ID para cada notificación local
#define ID_NOTIFICATION_FULLLIVES           1
#define ID_NOTIFICATION_MISSYOU             2

//Parámetros del juego
#define MAX_GOALS                   5
#define SECONDS_TO_REGENERATE       1800    //30min
#define DAYS_NOTIFICATION_MISSYOU   5      //Lanzamos notificación tras 5 días sin entrar en el juego.

using namespace cocos2d;

class CC_DLL TTNotificationManager: public Ref{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTNotificationManager* getInstance(void);
    // Destruye la instancia Singleton
    static void destroyInstance();
    
    static void show();   //Lanzar notificación local dado el mensaje, los segundos y un identificador.
    static void cancel();    //Borra todas las notificaciones locales.
    
protected:
    TTNotificationManager(){}
};

#endif
