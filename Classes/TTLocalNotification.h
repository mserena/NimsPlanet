#ifndef animalplanet_TTLocalNotification_h
#define animalplanet_TTLocalNotification_h

#include <string>

class TTLocalNotification{
public:
    static void show(std::string message, int interval, int tag);   //Lanzar notificación local dado el mensaje, los segundos y un identificador.
    static void cancel(int tag);
};

#endif