//
//  TTBoosterDefines.h
//  animalcubes
//
//  Created by Marc Serena on 14/10/14.
//
//

#ifndef animalcubes_TTBoosterDefines_h
#define animalcubes_TTBoosterDefines_h

// Tipos de booster
typedef enum
{
    BOARD_BOOSTERS,
    GAME_BOOSTERS,
}BOOSTERS_TYPES;

// Tipos de booster de tablero
typedef enum
{
    BOARD_BOOSTER_MORE_MOVEMENTS_5, // Booster que aumenta la restricción de moviemintos
    BOARD_BOOSTER_MORE_TIME_15,     // Booster que aumenta la restricción de tiempo
    BOARD_BOOSTER_COMBOS,           // Booster que coloca aleatoriamente combos en el tablero
    BOARD_BOOSTER_COMBO_COLOR,      // Booster que coloca un combo de color aleatoriamente en el tablero
}BOARD_BOOSTERS_TYPE;

// Tipos de booster de juego
typedef enum
{
    GAME_BOOSTER_HAMMER,            // Booster martillo, un toque en una casilla
    GAME_BOOSTER_PICK,              // Booster pico, elimina una casilla
    GAME_BOOSTER_BOOM,              // Booster bomba, un toque en un area 3x3
    GAME_BOOSTER_ROCKET,            // Booster cohete, un toque en una linea
    GAME_BOOSTER_COMBO_COLOR,       // Booster combo de color, coloca un combo de color en una casilla
}GAME_BOOSTERS_TYPE;

#endif
