#ifndef __TTEventsName_SCENE_H__
#define __TTEventsName_SCENE_H__

#include <map>
#include <string>

typedef enum
{
    //Eventos de tiempo
    EVENT_TIME_SERVER,
    
    //Eventos de transicion de escenas
    EVENT_SCENE_ON_LOADING,
    EVENT_MAIN_MENU_LOADED,
    
    //Eventos del mapa
    EVENT_MAP_LOADED,
    EVENT_MAP_PAUSE,
    EVENT_MAP_RESTORE,
    EVENT_MAP_ANIM_LOADED,
    EVENT_USER_INFO_UPDATED,
    EVENT_MAP_LEVEL_UNLOCK,
    
    //Eventos de Facebook
    EVENT_FACEBOOK_USERS_INFO_UPDATED,
    EVENT_FACEBOOK_CONNECTION,
    EVENT_FACEBOOK_REQUEST_SEND,
    EVENT_FACEBOOK_PHOTO,
    EVENT_FACEBOOK_FRIENDS_INVITE,
    EVENT_FACEBOOK_RANKING,
	EVENT_FACEBOOK_CONNECTION_REFRESH_BUTTON,
    
    //Eventos de juego
    EVENT_GAME_LOADED,
    EVENT_GAME_FINISH_PLAY,
    EVENT_GAME_FINISH_PLAY_SCORED,
    EVENT_GAME_READY_TO_PLAY,
    EVENT_GAME_FINISH,
    EVENT_GAME_CONTINUEBUY,
    EVENT_GAME_FINISHED_BOARD,
    EVENT_GAME_RETRY,
    EVENT_GAME_PAUSE,
    EVENT_GAME_CONTINUE,
    
    //Eventos del tablero
    EVENT_BOARD_CELL_DEATH,
    EVENT_BOARD_CELL_POSITIONED,
    EVENT_BOARD_WOOD_BREAK,
    EVENT_BOARD_STONE_BREAK,
    EVENT_BOARD_NETWORK_BREAK,
    EVENT_BOARD_KAMEHAMEHA_FINISHED,
    EVENT_BOARD_STEP_FINISHED,
    EVENT_BOARD_ALL_STEPS_FINISHED,
    
    //Eventos de idiomas
    EVENT_LANGUAGE_CHANGE,
    
    //Eventos de restricciones
    EVENT_RESTRICTION_FINISH_TIME,
    EVENT_RESTRICTION_FINISH_MOVES,
    
    //Eventos de objetivos
    EVENT_GOAL_ACHIEVED,
    EVENT_GOAL_NEW_BREAK,
    EVENT_OBJECT_MOVE,
    EVENT_OBJECT_DOWN,
    EVENT_OBJECT_DOWN_UI,
    EVENT_COLLECT_FIGURE_UI,
    EVENT_BLOCK_UI,
    EVENT_BREAKS_UI,
    EVENT_STONE_UI,
    EVENT_NETWORK_UI,

    //eventos de tienda
    EVENT_COINS_BALANCE_UPDATED,
    EVENT_PURCHASE_STORE_ERROR,
    EVENT_MARKET_ITEMS_REFRESHED,
    
    //eventos asociados a estadisticas
    EVENT_CREATE_OBJECT
}EVENT_CODES;

//datos de evento EVENT_CREATE_OBJECT
typedef struct ObjectDataEvent{
    bool dynamic;
    int typeObject;
    int subTypeObject;
    
} ObjectDataEvent;

//datos de evento de EVENT_BLOCK_BREAK
//datos de evento de EVENT_OBJECT_DOWN_UI
typedef struct GenericGoalDataEvent{
    int currentValue;
    int goalValue;
    int typeGoal;
    bool recentAchieved;
} GenericGoalDataEvent;

//datos de evento de EVENT_COLLECT_FIGURE_UI
typedef struct CollectFiguresDataEvent{
    int currentValue;
    int goalValue;
    int figure;
    bool recentAchieved;
} CollectFiguresDataEvent;

//datos para los eventos de compra con los stores 
typedef struct MarketDataEvent  {
    bool cancelled;
    bool success;
} MarketDataEvent;

//datos para eventos de actualizacion de usuarios de FB
typedef struct FBDataEvent {
    std::string userId;
} FBDataEvent;

typedef struct TimeServer {
    bool  _error;
    long long int   _date;
}TimeServer;

#endif  // __TTEventsName_SCENE_H__
