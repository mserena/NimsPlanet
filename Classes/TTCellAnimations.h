#ifndef __TTCellAnimations_SCENE_H__
#define __TTCellAnimations_SCENE_H__

// Ids de las animaciones standby
typedef enum
{
    ANIM_STANDBY_EYES,    // Animación parpadeo figuras
    ANIM_STANDBY_TURN,    // Animación giro de figuras
    ANIM_STANBY_COMPRESS,
    NUM_CELL_ANIMATIONS_STANDBY
}CELL_ANIMATIONS_STANDBY_ID;

// Mapa de animaciones standby
const std::map<CELL_ANIMATIONS_STANDBY_ID, std::string> _cellAnimationsInfo{{ANIM_STANDBY_EYES,"anim_ojos"},
                                                                            {ANIM_STANDBY_TURN,"anim_giro"},
                                                                            {ANIM_STANBY_COMPRESS,"anim_encogs"}
                                                                            };

#define ANIM_SELECTION_SCARY            "anim_asustado"
#define ANIM_SELECTION_REBOUND          "anim_rebote"
#define ANIM_SELECTION_JUMP             "anim_salto"
#define ANIM_SELECTION_TURN             "anim_giro"

// Animaciones stanby
#define ANIM_STANDBY_MIN_TIME   1
#define AMIM_STANDBY_MAX_TIME   2

#define ANIM_STANDBY_MIN_FIGURES   3
#define AMIM_STANDBY_MAX_FIGURES   8

// Animaciones help
#define ANIM_HELP_TIME   5


#endif  // __TTCellAnimations_SCENE_H__
