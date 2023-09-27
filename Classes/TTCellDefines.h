#ifndef __TTCellDefines_SCENE_H__
#define __TTCellDefines_SCENE_H__

//Definicion de las capas de una celda
#define STATIC_CONTENT_LAYER    0
#define DYNAMIC_CONTENT_LAYER   4

// Layers de la celda
typedef enum
{
    CELL_BACKGROUND_LAYER,
    CELL_CONTENT_LAYER,
    CELL_MARK_LAYER,
    CELL_PREMARK_LAYER,
}CELL_LAYERS;

typedef enum
{
    CELL_SPRITE_DYNAMIC_LAYER,
    CELL_ARROW_DYNAMIC_LAYER,
}CELL_DYNAMIC_LAYERS;

#define LAYER_ANIMATIONS 1000
#define LAYER_TUTORIAL_GLAZE 2000

//Definicion de los path de las celdas
typedef enum
{
    CELL_PATH_N,
    CELL_PATH_NE,
    CELL_PATH_E,
    CELL_PATH_SE,
    CELL_PATH_S,
    CELL_PATH_WS,
    CELL_PATH_W,
    CELL_PATH_NW,
    NUM_CELL_PATHS
}CELL_PATHS;

// Timepo de caida maximo (Toda la columna)
#define FALL_TIME   0.5

// Tiempo de mover las celdas en un mix de tablero
#define MOVE_MIX_TIME   0.5

// Tamaño de la figura dentro de la celda
//#define FIGURE_SIZE_PERCENTAGE              95
#define BACKGROUND_SIZE_PERCENTAGE          99
#define SELECTION_MARK_SIZE_PERCENTAGE      110

// Definición de los identificadores de las figuras 
typedef enum
{
    FIGURE_1,
    FIGURE_2,
    FIGURE_3,
    FIGURE_4,
    FIGURE_5,
    FIGURE_6,
    FIGURE_COLOR,
}CELL_FIGURES;

#define NUM_CELL_NORMAL_FIGURES 6

// Acciones de celdas
typedef enum
{
    
}CELL_ACTIONS;

#endif  // __TTCellDefines_SCENE_H__
