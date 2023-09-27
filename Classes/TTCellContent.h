//
//  TTCellContent.h
//  animalplanet
//
//  Created by Marc Serena on 21/1/15.
//
//

#ifndef __animalplanet__TTCellContent__
#define __animalplanet__TTCellContent__

#include "cocos2d.h"

using namespace cocos2d;

#define FIGURE_Z_LAYER 500

// Definición de los tipos de contenido
typedef enum
{
    // Contenidos dinámicos
    FIGURE_DYNAMIC_CONTENT,                // Figuras
    COMBO_VERTICAL_DYNAMIC_CONTENT,        // Combos vertical
    COMBO_HORIZONTAL_DYNAMIC_CONTENT,      // Combos horizontal
    COMBO_AREA_DYNAMIC_CONTENT,            // Combos area
    COMBO_COLOR_DYNAMIC_CONTENT,           // Combo de color
    DOWN_OBJECT_DYNAMIC_CONTENT,           // Objeto bajable
    
    // Hasta aqui contenidos dinámicos
    NUM_DYNAMIC_CONTENTS,
    
    // Contenidos estáticos
    MUD_STATIC_CONTENT,                    // Celda con barro
    STONE_STATIC_CONTENT,                  // Celda con piedra
    WOOD_STATIC_CONTENT,                   // Celda con madera
    NET_STATIC_CONTENT,                    // Celda con red
    GLUE_STATIC_CONTENT,                   // Celda con pegamento
    
    // Hasta aqui contenidos estáticos
    NUM_STATIC_CONTENTS,
    
}CONTENT_TYPES;

class TTCellContent
{
    
public:
    
    TTCellContent(CONTENT_TYPES contentLayerType);
    virtual ~TTCellContent();
    
    //Acciones a la celda
    virtual void initCell(){};
    virtual void selectedCell() = 0;
    virtual void unselectedCell() = 0;
    virtual int hitCell(int _numHits = 1) = 0;
    virtual void launchContentAnimation(){};
    
    //Consultas a la celda
    virtual bool isRefillable() = 0;
    virtual bool isSelectable() = 0;
    virtual bool isFirstSelectable() = 0;
    virtual bool isWallContent() = 0;           //Las celdas no pueden pasar por encima
    virtual bool isStrongWallContent() = 0;     //Los combos no pueden pasar por encima
    virtual bool isMovableContent() = 0;
    bool isComboContent();
    virtual int getZContent() = 0;
    
public:
    
    cocos2d::Node*    _parentContentsCell;
    cocos2d::Node*    _parentContent;
    
    CONTENT_TYPES   _contentLayerType;
    
    bool            _affectAreaCombination;     // Indica si el contenido es afectado por una combinación de area
    
    bool            _isUpdatingContent;         // Indica si el contenido se esta actualizando (animaciones, destrucciones, FX...)
    bool            _contentIsDeath;            // Indica el estado del contenido, si se ha eliminado
    bool            _contentIsBlocked;          // Indica que el contenido esta bloqueado y no puede aplicarse ningún hit a el, pero si a las capas inferiores
    
};


#endif /* defined(__animalplanet__TTCellContent__) */
