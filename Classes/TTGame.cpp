#include "TTGame.h"
#include "TTBoard.h"

//Singelton
static TTGame *_sharedGame = nullptr;

TTGame* TTGame::getInstance()
{
    if (!_sharedGame)
    {
        _sharedGame = new TTGame();
        _sharedGame->init();
    }
    
    return _sharedGame;
}

void TTGame::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedGame);
}

void TTGame::init()
{
    _gameConfig = new gameInfo();
    
    // Registro de recepcion de eventos
    _classListeners.push_back(registerListenerEvent(EVENT_RESTRICTION_FINISH_TIME,CC_CALLBACK_1(TTGame::onEvent_RestrictionTimeFinish,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_FINISH_PLAY,CC_CALLBACK_1(TTGame::onEvent_FinishWinPlay,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_READY_TO_PLAY,CC_CALLBACK_1(TTGame::onEvent_ReadyToPlay,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_RESTRICTION_FINISH_MOVES,CC_CALLBACK_1(TTGame::onEvent_RestrictionMovesFinish,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_PAUSE,CC_CALLBACK_1(TTGame::onEvent_GamePause,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GAME_CONTINUE,CC_CALLBACK_1(TTGame::onEvent_GameContinue,this)));
    _classListeners.push_back(registerListenerEvent(EVENT_GOAL_ACHIEVED,CC_CALLBACK_1(TTGame::onEvent_GoalAchieved,this)));
}

TTGame::~TTGame()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    
    delete(_gameConfig);
}

void TTGame::initGame()
{
    // Inicializamos variables
    _gameConfig->winGame = false;
    _gameConfig->scoreGame = 0;
    _gameConfig->movesGame = 0;
    _gameConfig->currentTime = 0.0;
    _finishedGame = false;
    _paused = false;
}

void TTGame::reset()
{
    delete(_gameConfig);
    _gameConfig = new gameInfo();
}

void TTGame::reactivateGame()
{
    CCLOG("Reactivamos juego");
    
    _gameConfig->winGame = false;
    _finishedGame = false;
    _paused = false;
    if(_gameConfig->timeRestriction != NULL)
    {
        _gameConfig->timeRestriction->addExtraTime(15);
    }
    else if (_gameConfig->moveRestriction != NULL)
    {
        _gameConfig->moveRestriction->addExtraMoves(5);
    }
    
    sendEvent(EVENT_GAME_CONTINUEBUY);
}

void TTGame::update(float dt)
{
    if (!_finishedGame && !_paused) {
        _gameConfig->currentTime+=dt;
    }
    // Update de las restricciones si es necesareo
    if(_gameConfig->timeRestriction != NULL)
    {
        _gameConfig->timeRestriction->update(dt);
        int timeToFinish = _gameConfig->timeRestriction->getTimeToFinish();
        TTGameUIManager::getInstance()->printTimeLabel(timeToFinish);
    }
}

void TTGame::finishGame()
{
    _finishedGame = true;
    _paused = false;
}

bool TTGame::isFinished()
{
    return _finishedGame;
}

void TTGame::onEvent_GamePause(cocos2d::EventCustom *e)
{
    _paused = true;
}

void TTGame::onEvent_GameContinue(cocos2d::EventCustom *e)
{
    _paused = false;
}

void TTGame::onEvent_RestrictionTimeFinish(cocos2d::EventCustom *e)
{
    _gameConfig->winGame = false;
    finishGame();
    sendEvent(EVENT_GAME_FINISH);
}

void TTGame::onEvent_RestrictionMovesFinish(cocos2d::EventCustom *e)
{
    _gameConfig->winGame = false;
    finishGame();
}

void TTGame::onEvent_GoalAchieved(EventCustom* e)
{
    //Poner esto en un goalManager ??
    CCLOG("Goal achieved!!");
    //validar que se haya conseguido el juego con todos los goals
    bool allGoalsAchieved = true;
    bool someGoalIsOptional = false;
    for(int i = 0; i < _gameConfig->goals.size() && allGoalsAchieved; i++)
    {
        TTGoal* goal = _gameConfig->goals[i];
        allGoalsAchieved = allGoalsAchieved && goal->isAchieved();
        if (allGoalsAchieved)
        {
            someGoalIsOptional = someGoalIsOptional || goal->isOptional();
        }
    }
    if (allGoalsAchieved)
    {
        _gameConfig->winGame = true;
        if (!someGoalIsOptional)
        {
            finishGame();
        }
    }
}

void TTGame::onEvent_ReadyToPlay(cocos2d::EventCustom *e)
{
    if(_finishedGame)
    {
        //Verifica si el juego ha terminado
        sendEvent(EVENT_GAME_FINISH);
        return;
    }
}

void TTGame::onEvent_FinishWinPlay(cocos2d::EventCustom *e)
{
    // Recive el userData del evento
    finishPlayInfo *finishPlay = static_cast<finishPlayInfo*>(e->getUserData());

    // Incrementamos los movimientos si no se trata de un movimiento de booster
    if (finishPlay->boosterPlay==-1)
    {
        _gameConfig->movesGame++;
        if(_gameConfig->moveRestriction != NULL)
        {
            _gameConfig->moveRestriction->doMove();
            TTGameUIManager::getInstance()->printMovementsLabel(_gameConfig->movesGame,_gameConfig->moveRestriction->getLimitMoves());
        }
    }
    
    int newScoreWin = calcScoreWined(*finishPlay);

    _gameConfig->scoreGame += newScoreWin;
    TTGameUIManager::getInstance()->printScoreLabel(_gameConfig->scoreGame);
    
    
    //acumular figuras recogidas en la jugada
    std::map<int,int> _collectedFigures = finishPlay->combinationTypes;

    for(int i = 0; i < _gameConfig->goals.size(); i++)
    {
        switch(_gameConfig->goals[i]->_typeGoal)
        {
            case SCORE_GOAL:
            {
                static_cast<TTScoreGoal*>(_gameConfig->goals[i])->scoreWin(newScoreWin);
            }
            break;
            case COLLECT_FIGURES_GOAL:
            {
                 for (auto it=_collectedFigures.begin(); it!=_collectedFigures.end(); ++it)
                 {
                    static_cast<TTCollectFiguresGoal*>(_gameConfig->goals[i])->newCollect(it->first,it->second);
                 }
            }
            break;
            default:
            break;
        }
    }
    //envio de evento justo despues de tener todos los datos de score calculados
    sendEvent(EVENT_GAME_FINISH_PLAY_SCORED,finishPlay);
    
    //control de final de juego
    if(_gameConfig->moveRestriction != NULL)
    {
        if(_gameConfig->moveRestriction->_finishedMoves)
        {
            finishGame();
            sendEvent(EVENT_GAME_FINISH);
        }
    }

}

int TTGame::calcScoreWined(finishPlayInfo _playInfo)
{
    int combinationLenght = _playInfo.combinationLength;
    int combosInCombination = _playInfo.combosInCombination;
    int combosColorInCombination = _playInfo.combosColorInCombination;
    int selectionLenght = _playInfo.selectionLength;
    int newScoreWin = 0;
    
    // Verifica las muertes
    newScoreWin += (combinationLenght-combosInCombination-combosColorInCombination)*_gameConfig->_scoreDetahFigure;
    newScoreWin += (combosInCombination)*_gameConfig->_scoreDetahCombo;
    newScoreWin += (combosColorInCombination)*_gameConfig->_scoreDetahComboColor;
    
    if(_playInfo.isOvni)
        newScoreWin += _gameConfig->_scoreOvni;
    
    // Verifica las creaciones
    if(selectionLenght >= MIN_SELECT_CELLS_TO_CREATE_COMBO && selectionLenght < MIN_SELECT_CELLS_TO_CREATE_COMBO_COLOR)
    {
        newScoreWin += _gameConfig->_scoreCreateCombo;
    }
    else if(selectionLenght >= MIN_SELECT_CELLS_TO_CREATE_COMBO_COLOR)
    {
        newScoreWin += _gameConfig->_scoreCreateComboColor;
    }
    
    return newScoreWin;
}
