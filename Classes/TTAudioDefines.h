//
//  TTAudioDefines.h
//  animalplanet
//
//  Created by Marc Serena on 03/11/14.
//
//

#ifndef animalplanet_TTAudioDefines_h
#define animalplanet_TTAudioDefines_h

//Music
const std::map<std::string,std::string> _soundMusic =
    {{"GAME_MUSIC","Music_loop_game"},
    {"MAP_MUSIC","Music_road_loop"},
    {"BONUS_MUSIC","bonus_loop"},
    {"DAILY_BONUS_MUSIC","slot_music_loop"}
    };

//Effects
const std::map<std::string,std::string> _soundEffects =
    {//{"COMBO_SELECT","Angry_Animal"},                          Sonido no usado
    {"COMBOCOLOR_SELECT","Rainbow_Select"},
    //{"COMBOCOLOR_SELECT_SECOND","Rainbow_Second_Select"},     Sonido no usado
    {"COMBOCOLOR_UNSELECT","Rainbow_UnSelect"},
    {"GENERIC_UNSELECT","UnDo"},
        
    {"COMBO_SOUND_SIMPLY","Explosion_1"},
    {"COMBO_SOUND_COMPLEX","Explosion_2"},
    {"COMBO_COLOUR_SOUND","Colour_combo_trail_effect"},
        
    //{"DOWN_LINE","Line_animals_GoDown"},                      Sonido no usado
    {"DOWN_LINE_HOLES","Multi_Line_animals_GoDown"},
    {"DEATH_ANIMALS","Destroy_animal_sellection"},
    {"HELP_BOARD","Blue_selection"},
    {"UFO_ABDUCTION","UFO_Abduction"},
    {"MUD_BREAK","Mud_Sand_break"},
    {"WOOD_BREAK","Wood_tables_destruction"},
    {"GOAL_WIN","Goal_Wined"},
    {"STONE_BREAK","stone_breaking"},
    {"NETWORK_BREAK","fence_disappear"},
    {"GLUE_SOUND","put_glue"},
        
    {"NOTE1","Note_1"},
    {"NOTE2","Note_2"},
    {"NOTE3","Note_3"},
    {"NOTE4","Note_4"},
    {"NOTE5","Note_5"},
    {"NOTE6","Note_6"},
    {"NOTE7","Note_7"},
    {"NOTE8","Note_8"},
    {"NOTE9","Note_9"},
    {"NOTE10","Note_10"},
    {"NOTE11","Note_11"},
        
    {"WALK","Walk"},
    {"FARM_MAP","barn_animals"},
    {"RABBIT_MAP","carrot_rabbit"},
    {"OVNI_MAP","ovni_damaged"},

    {"LEVEL_PARTY","level_exceeded"},
    {"LEVEL_WIN","win_game"},
    {"LEVEL_LOSE","lose_leave_game"},
        
    {"PANEL_DOWN","Panel_down"},
    {"PANEL_UP","Panel_up"},
    {"BUTTON_PLAY","Button_play_panel"},
    {"WIN_STAR","Win_Star"},
    {"BUTTON_LANG_SELECT","Button_Language_Selection"},
    {"BUTTON_LANG","Button_Languages"},
    {"BUTTON_ON","Button_ON"},
    {"BUTTON_OFF","Button_Off"},
    {"BUTTON_SELECT_STAGE","Button_Select_stage_road"},
    {"BUTTON_EXIT","Button_X"},
    {"PANEL_BUY_DOWN","Panel_Down_Buy_Zone"},
    {"PANEL_BUY_UP","Panel_Up_Buy_Zone"},
    {"PANEL_POPUP_IN","Pannel_Booster_Appear"},
    {"PANEL_POPUP_OUT","Pannel_Booster_Disappear"},
    {"BUTTON_BUY_BOOSTER","Buy_booster"},
    {"BUTTON_CLOSE_SHOP","Button_Close_Buy_Zone"},
    {"BUTTON_BUY_ITEM","Button_inside_Down_Buy_Zone"},
    {"COIN_INCREMENT","Coin_Increment_Buy_Zone"},
        
    {"SELECT_BOOSTER_APPLY","select_figure_for_booster"},
    {"HAMMER_BOOSTER","Hammer_action"},
    {"PICK_BOOSTER","Peak_Action"},
    {"COMBOCOLOR_BOOSTER","Raimbow_Put_Over_Animal"},
    {"ROCKET_BOOSTER","Rocket_action"},
    {"BOOM_BOOSTER_COUNT","Bomb_action_tic_tac"},
    {"BOOM_BOOSTER_EXPLOSION","Expliosion_by_bomb_action_tic_tac"},
        
    {"BONUS_FIGURE_CATCH","Bonus_Explode_Selection"},
    {"BONUS_RAINBOW_UP","Rainbow_Up_Appear_Action"},
    {"BONUS_ENTER","Intro_Bonus"},
        
    {"MIX_BOARD","re-combination_figures"},
    {"MIX_BOARD_ENTER","down_exit_panel_no_combinations"},
    {"MIX_BOARD_EXIT","down_panel_no_combinations"},
        
    {"START_SLOT_MACHINE","start_slot_machine"},
    {"SLOT_LOOP","roller_loop"},
    {"SLOT_STOP_1","stop1"},
    {"SLOT_STOP_2","stop2"},
    {"SLOT_STOP_3","stop3"},
    {"SLOT_WIN","slot_prize"},
        
    {"STOPPER_UNLOCK","ah_choir"}
    };

#endif
