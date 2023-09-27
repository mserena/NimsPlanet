//
//  TTBoostersManager.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoostersManager.h"
#include "TTGame.h"
#include "TTGameUIManager.h"

//Singelton
static TTBoostersManager *_sharedBoostersManagerParser = nullptr;

TTBoostersManager* TTBoostersManager::getInstance()
{
    if (!_sharedBoostersManagerParser)
    {
        _sharedBoostersManagerParser = new TTBoostersManager();
        _sharedBoostersManagerParser->loadBoosters();
    }
    
    return _sharedBoostersManagerParser;
}

void TTBoostersManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedBoostersManagerParser);
}

TTBoostersManager::~TTBoostersManager()
{
    
}

void TTBoostersManager::loadBoosters()
{
    // Creamos la instancia de todos los boosters de tablero
    TTBoardBoosterMoreMoves* _boosterMoreMoves = new TTBoardBoosterMoreMoves();
    _boardBoostersInstances.insert(std::make_pair(_boosterMoreMoves->_boosterSubType,_boosterMoreMoves));

    TTBoardBoosterMoreTime* _boosterMoreTime = new TTBoardBoosterMoreTime();
    _boardBoostersInstances.insert(std::make_pair(_boosterMoreTime->_boosterSubType,_boosterMoreTime));
    
    TTBoardBoosterCombos* _boosterCombos = new TTBoardBoosterCombos();
    _boardBoostersInstances.insert(std::make_pair(_boosterCombos->_boosterSubType,_boosterCombos));
    
    TTBoardBoosterComboColor* _boosterComboColor = new TTBoardBoosterComboColor();
    _boardBoostersInstances.insert(std::make_pair(_boosterComboColor->_boosterSubType,_boosterComboColor));
    
    // Creamos la instancia de todos los boosters de juego
    TTGameBoosterComboColor* _boosterGameComboColor = new TTGameBoosterComboColor();
    _gameBoostersInstances.insert(std::make_pair(_boosterGameComboColor->_boosterSubType,_boosterGameComboColor));
    
    TTGameBoosterHammer* _boosterGameHammer = new TTGameBoosterHammer();
    _gameBoostersInstances.insert(std::make_pair(_boosterGameHammer->_boosterSubType,_boosterGameHammer));
    
    TTGameBoosterPick* _boosterGamePick = new TTGameBoosterPick();
    _gameBoostersInstances.insert(std::make_pair(_boosterGamePick->_boosterSubType,_boosterGamePick));
    
    TTGameBoosterBoom* _boosterGameBoom = new TTGameBoosterBoom();
    _gameBoostersInstances.insert(std::make_pair(_boosterGameBoom->_boosterSubType,_boosterGameBoom));

    TTGameBoosterRocket* _boosterGameRocket = new TTGameBoosterRocket();
    _gameBoostersInstances.insert(std::make_pair(_boosterGameRocket->_boosterSubType,_boosterGameRocket));

}

void TTBoostersManager::verifyBoostersSelection(Point _pointTouch)
{
    for(auto _booster = _gameBoostersInstances.begin(); _booster != _gameBoostersInstances.end(); ++_booster)
    {
        if(_booster->second->_activeSelection)
        {
            _booster->second->moveApplyPosition(_pointTouch);
            return;
        }
    }
}

void TTBoostersManager::releaseBoostersSelection()
{
    
    for(auto _booster = _gameBoostersInstances.begin(); _booster != _gameBoostersInstances.end(); ++_booster)
    {
        if(_booster->second->_activeSelection)
        {
            TTAudioManager::getInstance()->playAudio("SELECT_BOOSTER_APPLY");
            return;
        }
    }
}

void TTBoostersManager::applyActiveGameBoosters()
{
    for(auto _booster = _gameBoostersInstances.begin(); _booster != _gameBoostersInstances.end(); ++_booster)
    {
        if(_booster->second->_activeSelection)
        {
            switch(_booster->second->_boosterSubType)
            {
                case GAME_BOOSTER_COMBO_COLOR:
                {
                    TTGameBoosterComboColor* _comboColorBooster = static_cast<TTGameBoosterComboColor*>(_booster->second);
                    _comboColorBooster->aplyGameBooster();
                    return;
                }
                break;
                case GAME_BOOSTER_HAMMER:
                {
                    TTGameBoosterHammer* _hammerBooster = static_cast<TTGameBoosterHammer*>(_booster->second);
                    _hammerBooster->aplyGameBooster();
                    return;
                }
                break;
                case GAME_BOOSTER_PICK:
                {
                    TTGameBoosterPick* _pickBooster = static_cast<TTGameBoosterPick*>(_booster->second);
                    _pickBooster->aplyGameBooster();
                    return;
                }
                break;
                case GAME_BOOSTER_BOOM:
                {
                    TTGameBoosterBoom* _boomBooster = static_cast<TTGameBoosterBoom*>(_booster->second);
                    _boomBooster->aplyGameBooster();
                    return;
                }
                case GAME_BOOSTER_ROCKET:
                {
                    TTGameBoosterRocket* _rocketBooster = static_cast<TTGameBoosterRocket*>(_booster->second);
                    _rocketBooster->aplyGameBooster();
                    return;
                }
                break;
                default:
                break;
            }
        }
    }
}

void TTBoostersManager::resetBoosters()
{
    for(auto _booster = _gameBoostersInstances.begin(); _booster != _gameBoostersInstances.end(); ++_booster)
    {
        _booster->second->resetBooster();
    }
}

TTBooster* TTBoostersManager::getBoosterById(std::string boosterId)
{
    TTBooster* _booster = nullptr;
    
    // Extraemos el tipo y subtipo del booster
    std::stringstream stringId(boosterId);
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(stringId, segment, '_')) seglist.push_back(segment);
    
    int typeBooster = std::atoi(seglist[seglist.size()-2].c_str());
    int subTypeBooster = std::atoi(seglist[seglist.size()-1].c_str());
    
    // Buscamos el booster
    if(typeBooster == BOARD_BOOSTERS)
    {
        if(_boardBoostersInstances.find(BOARD_BOOSTERS_TYPE(subTypeBooster)) != _boardBoostersInstances.end())
        {
            _booster = _boardBoostersInstances.find(BOARD_BOOSTERS_TYPE(subTypeBooster))->second;
        }
    }
    else if(typeBooster == GAME_BOOSTERS)
    {
        if(_gameBoostersInstances.find(GAME_BOOSTERS_TYPE(subTypeBooster)) != _gameBoostersInstances.end())
        {
            _booster = _gameBoostersInstances.find(GAME_BOOSTERS_TYPE(subTypeBooster))->second;
        }
    }
    
    return _booster;
}

void TTBoostersManager::applyBoosterById(std::string boosterId)
{
    TTBooster* _booster = getBoosterById(boosterId);
    
    switch(_booster->_boosterType)
    {
        case BOARD_BOOSTERS:
        {
            (static_cast<TTBoardBoosters*>(_booster))->_boosterActivated = true;
            applyActiveBoardBoosters();
        }
        break;
        case GAME_BOOSTERS:
        {
            (static_cast<TTGameBoosters*>(_booster))->_boosterActivated = true;
            TTGameUIManager::getInstance()->activateBoosterUI((static_cast<TTGameBoosters*>(_booster))->_boosterSubType);
        }
        break;
    }
}

void TTBoostersManager::activateBoosterById(std::string boosterId)
{
    TTBooster* _booster = getBoosterById(boosterId);
    
    switch(_booster->_boosterType)
    {
        case BOARD_BOOSTERS:
        {
            (static_cast<TTBoardBoosters*>(_booster))->_boosterActivated = true;
        }
        break;
        case GAME_BOOSTERS:
        {
            (static_cast<TTGameBoosters*>(_booster))->_boosterActivated = true;
        }
        break;
    }
}

void TTBoostersManager::applyActiveBoardBoosters()
{
    for(auto _booster = _boardBoostersInstances.begin(); _booster != _boardBoostersInstances.end(); ++_booster)
    {
        if(_booster->second->_boosterActivated && _booster->second->canBeApplied(TTGame::getInstance()->_gameConfig))
        {
            switch(_booster->second->_boosterSubType)
            {
                case BOARD_BOOSTER_COMBOS:
                {
                    (static_cast<TTBoardBoosterCombos*>(_booster->second))->aplyBoardBooster(TTBoard::getInstance()->getAllNormalCells());
                }
                break;
                case BOARD_BOOSTER_COMBO_COLOR:
                {
                    (static_cast<TTBoardBoosterComboColor*>(_booster->second))->aplyBoardBooster(TTBoard::getInstance()->getAllNormalCells());
                }
                break;
                case BOARD_BOOSTER_MORE_MOVEMENTS_5:
                {
                    (static_cast<TTBoardBoosterMoreMoves*>(_booster->second))->aplyBoardBooster(TTGame::getInstance()->_gameConfig);
                    
                    // Actualizamos la parte visual de la UI
                    TTGameUIManager::getInstance()->printMovementsLabel(TTGame::getInstance()->_gameConfig->movesGame,TTGame::getInstance()->_gameConfig->moveRestriction->getLimitMoves());
                }
                break;
                case BOARD_BOOSTER_MORE_TIME_15:
                {
                    (static_cast<TTBoardBoosterMoreTime*>(_booster->second))->aplyBoardBooster(TTGame::getInstance()->_gameConfig);
                    
                    // Actualizamos la parte visual de la UI
                    int timeToFinish = TTGame::getInstance()->_gameConfig->timeRestriction->getTimeToFinish();
                    TTGameUIManager::getInstance()->printTimeLabel(timeToFinish);
                }
                break;
                default:
                break;
            }
        }
    }
}
