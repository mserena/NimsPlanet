#ifndef __TTGoal__
#define __TTGoal__

#include "TTEventsManager.h"

// Tipos de restricciones
typedef enum
{
    SCORE_GOAL,             // Objectivo puntos
    BREAKS_GOAL,            // Objectivo romper
    DOWN_OBJECT_GOAL,       // Objectivo bajar objetos
    COLLECT_FIGURES_GOAL,    // Objectivo coleccionar figuras
    BLOCK_GOAL,             //objetivo collecionar rocas (bloques)
    STONE_GOAL,             //objetivo romper piedras
    NETWORK_GOAL,               //objetivo redes
    NUM_GOALS
}GOAL_TYPES;

class TTGoal {
    
public:

    virtual ~TTGoal(){};
    
    virtual void reset(){};
    
    inline bool isOptional(){
        return _optional;
    };
    
    inline void setOptional(bool opt){
        _optional = opt;
    };

    inline bool isAchieved() {
        return _actualValue>=_goalValue;
    }
    
public:
    bool        _optional;
    GOAL_TYPES  _typeGoal;

    int _actualValue;
    int _goalValue;
};

#endif 
