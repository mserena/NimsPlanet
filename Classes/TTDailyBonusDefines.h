//
//  TTDailyBonusDefines.h
//  animalplanet
//
//  Created by Marc Serena on 9/4/15.
//
//

#ifndef animalplanet_TTDailyBonusDefines_h
#define animalplanet_TTDailyBonusDefines_h

enum MACHINE_STATE
{
    MACHINE_STATE_STANBY,
    MACHINE_STATE_SPIN,
    MACHINE_STATE_WIN
};

enum COMBINATION_TYPE
{
    COMBINATION_TYPE_NO_PRIZE,
    COMBINATION_TYPE_MONEYS,
    COMBINATION_TYPE_HAMMER,
    COMBINATION_TYPE_PEAK,
    COMBINATION_TYPE_BOMB,
    COMBINATION_TYPE_ROCKET,
    COMBINATION_TYPE_COMBO_COLOR
};

enum FIGURE_SLOT
{
    FIGURE_SLOT_COIN,
    FIGURE_SLOT_HAMMER,
    FIGURE_SLOT_PEAK,
    FIGURE_SLOT_BOMB,
    FIGURE_SLOT_ROCKET,
    FIGURE_SLOT_COMBO_COLOR,
    NUM_FIGURES_SLOT
};

typedef struct slotInfo
{
    int         _idFigureSlot;
    
    Node*       _slotParentNode;
    Sprite*     _spriteFigureSlot;
}slotInfo;

typedef struct rollerInfo
{
    Node*                   _parentRoller;
    std::vector<slotInfo>   _rollerSlots;
    bool                    _isRolling;
    bool                    _isFinished;
}rollerInfo;

typedef struct timeInfo
{
    bool _waitForTime;
    bool _timeRecived;
    
    bool _error;
    timeStruct _timeServer;
}timeInfo;

typedef struct combinationInfo
{
    COMBINATION_TYPE            _actualCombinationType;
    std::vector<int>            _actualCombinationIds;
}combinationInfo;

//Definicion de los sprites de las figuras de slot
const std::map<int, std::string> _figureSlotSpritesMap{ {FIGURE_SLOT_COIN, "figure_slot_monedas.png"},
                                                        {FIGURE_SLOT_HAMMER, "figure_slot_martillo.png"},
                                                        {FIGURE_SLOT_PEAK, "figure_slot_pico.png"},
                                                        {FIGURE_SLOT_BOMB, "figure_slot_bomba.png"},
                                                        {FIGURE_SLOT_ROCKET, "figure_slot_cohete.png"},
                                                        {FIGURE_SLOT_COMBO_COLOR, "figure_slot_combo_color.png"}};

#define TIME_ROLLING        1
#define TIME_ONE_ROLL       0.8
#define TIME_ROLL_STOP      0.3

#define DEFAULT_DAILY_FREE_SPINS    3

// User daily bonus data
#define USER_DATA_DAILY_BONUS_LAST_DAY          "_UserDailyBonusLastDay"
#define USER_DATA_DAILY_BONUS_LAST_MONTH         "_UserDailyBonusLastMonth"
#define USER_DATA_DAILY_BONUS_FREE_SPINS        "_UserDailyBonusFreeSpins"
#define USER_DATA_DAILY_BONUS_HAS_DATA          "_UserDailyBonusHasData"

typedef struct userDailyBonusInfo
{
    int _lastDayPlayed;
    int _lastMonthPlayed;
    int _freeSpins;
}userDailyBonusInfo;

#endif
