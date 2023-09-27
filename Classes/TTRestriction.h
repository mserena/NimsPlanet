#ifndef __TTRestriction__
#define __TTRestriction__

#include "TTEventsManager.h"

// Tipos de restricciones
typedef enum
{
    TIME_RESTRICTION,       // Restriccion de tiempo
    MOVEMENTS_RESTRICTION   // Restriccion de movimientos
}RESTRICTION_TYPES;

class TTRestriction {
    
public:
    
    virtual ~TTRestriction(){};

    virtual void reset(){};
    
    inline bool allowsFinish()
    {
        return _allowFinish;
    };
    
    inline void setAllowFinish(bool fin)
    {
        _allowFinish = fin;
    };

public:
    
    RESTRICTION_TYPES _typeRestriction;

protected:
    
    bool _allowFinish;
    
};


#endif 
