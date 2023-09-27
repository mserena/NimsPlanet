//
//  TTAudioManager.cpp
//  animalplanet
//
//  Created by Marc Serena on 03/11/14.
//
//

#include "TTAudioManager.h"
#include "TTUserInfo.h"
#include "TTConfigParser.h"

#define FADEOUT_DEFAULT_TIME      0.8
#define FADEIN_DEFAULT_TIME       0.2

//Singelton
static TTAudioManager *_sharedAudioManager = nullptr;

TTAudioManager* TTAudioManager::getInstance()
{
    if (!_sharedAudioManager)
    {
        _sharedAudioManager = new TTAudioManager();
        _sharedAudioManager->init();
    }
    
    return _sharedAudioManager;
}

void TTAudioManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedAudioManager);
}

TTAudioManager::~TTAudioManager()
{
    
}

bool TTAudioManager::init(void)
{
    
    // Consultamos las preferencias
    _musicLevel = TTUserInfo::getInstance()->_userData->_musicLevel;
    _effectsLevel = TTUserInfo::getInstance()->_userData->_effectsLevel;
    
    // Según el SO obtenemos la extensión
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        _extensionBgFiles = ".m4a";
        _extensionEffectFiles = ".mp3";
    #else
        _extensionBgFiles = ".ogg";
        _extensionEffectFiles = ".ogg";
    #endif
    
    // Preload de la música del juego
    for(auto outer_iter=_soundMusic.begin(); outer_iter!=_soundMusic.end(); ++outer_iter)
    {
        std::string fileName = outer_iter->second;
        fileName += _extensionBgFiles;
        SimpleAudioEngine::getInstance()->preloadBackgroundMusic(fileName.c_str());
    }
    
    // Preload de los efectos del juego
    for(auto outer_iter=_soundEffects.begin(); outer_iter!=_soundEffects.end(); ++outer_iter)
    {
        std::string fileName = outer_iter->second;
        fileName += _extensionEffectFiles;
        SimpleAudioEngine::getInstance()->preloadEffect(fileName.c_str());
    }
    
    // Seteamos los volumenes
    this->changeVolumeMusic(_musicLevel);
    this->changeVolumeFx(_effectsLevel);
    
    return true;
}

void TTAudioManager::loadActualWorldEffects()
{
    
    int _charactersPack = TTConfigParser::getInstance()->_generalConfig->_worlds[TTUserInfo::getInstance()->_userData->_selectedWorld]._charactersPack;
    std::map<std::string,std::string> _charactersSounds = TTConfigParser::getInstance()->_generalConfig->_characters[_charactersPack]._sounds;
    for(auto iterator = _charactersSounds.begin(); iterator != _charactersSounds.end(); iterator++)
    {
        std::string fileName = iterator->second;
        fileName += _extensionEffectFiles;
        SimpleAudioEngine::getInstance()->preloadEffect(fileName.c_str());
        
        _soundEffectsWorld.insert(std::make_pair(iterator->first,iterator->second));
    }
}

void TTAudioManager::unLoadActualWorldEffects()
{
    
    std::map<std::string,std::string>::iterator _soundEffectsWorldItr;
    for(_soundEffectsWorldItr = _soundEffectsWorld.begin(); _soundEffectsWorldItr != _soundEffectsWorld.end(); ++_soundEffectsWorldItr)
    {
        std::string fileName = _soundEffectsWorldItr->second;
        fileName += _extensionEffectFiles;
        SimpleAudioEngine::getInstance()->unloadEffect(fileName.c_str());
    }
    _soundEffectsWorld.clear();
}

void TTAudioManager::playAudio(std::string tag, bool loop)
{
    //caso inicial
    if (tag.length()==0)
    {
        return;
    }
    
    if(_soundMusic.find(tag) != _soundMusic.end())
    {
        _actualMusic = tag;
        if(!_musicOn)
            return;
        
        if(SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        {
            changeBackgroundMusicWithFadeEffect(tag,1.0f);
        }
        else
        {
            SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(MUSIC_VOLUME());
            std::string _musicFile = _soundMusic.find(tag)->second.c_str()+_extensionBgFiles;
            SimpleAudioEngine::getInstance()->playBackgroundMusic(_musicFile.c_str(),loop);
        }
    }
    else if(_soundEffects.find(tag) != _soundEffects.end())
    {
        if(!_effectsOn)
            return;
        
        std::string _musicFile = _soundEffects.find(tag)->second.c_str()+_extensionEffectFiles;
        SimpleAudioEngine::getInstance()->playEffect(_musicFile.c_str(),loop);
    }
    else if(_soundEffectsWorld.find(tag) != _soundEffectsWorld.end())
    {
        if(!_effectsOn)
            return;
        
        std::string _musicFile = _soundEffectsWorld.find(tag)->second.c_str()+_extensionEffectFiles;
        SimpleAudioEngine::getInstance()->playEffect(_musicFile.c_str());
    }
    else
    {
        CCLOG("Intentas reproducir un tag de audio que no existe %s",tag.c_str());
        abort();
    }
}

void TTAudioManager::stopAudio(std::string tag)
{
    if(_soundMusic.find(tag) != _soundMusic.end())
    {
        if(SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        {
            fadeOutBackgroundMusic(FADEOUT_DEFAULT_TIME);
        }
    }
    else if(_soundEffects.find(tag) != _soundEffects.end())
    {
        SimpleAudioEngine::getInstance()->stopAllEffects();
    }
    else
    {
        CCLOG("Intentas reproducir un tag de audio que no existe %s",tag.c_str());
        abort();
    }
}

void TTAudioManager::changeBackgroundMusicWithFadeEffect(std::string tag, float _timeToChange)
{
    if(_soundMusic.find(tag) != _soundMusic.end())
    {
        std::string _musicFile = _soundMusic.find(tag)->second.c_str()+_extensionBgFiles;
        
        // Hacemos FadeOut de la música anterior
        fadeOutBackgroundMusic(_timeToChange/2);
        
        auto _fadeInFunction = CallFunc::create(std::bind([&](std::string _musicFile, float _timeToChange){
            // Hacemos FadeIn de la nueva música
            SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            SimpleAudioEngine::getInstance()->playBackgroundMusic(_musicFile.c_str(),true);
            fadeInBackgroundMusic(_timeToChange/2);
        },_musicFile,_timeToChange));
        
        Director::getInstance()->getRunningScene()->runAction(Sequence::create(DelayTime::create(_timeToChange/2),_fadeInFunction,NULL));
    }
    else
    {
        CCLOG("Intentas reproducir un tag de audio que no existe %s",tag.c_str());
        abort();
    }
}

void TTAudioManager::fadeOutBackgroundMusic(float _timeFadeOut)
{
    float bgVol = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    float callsToUpdate = _timeFadeOut/0.1f;
    _fadeOut = bgVol/callsToUpdate;
    
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTAudioManager::fadeOutUpdate),this,0.1f,false);
}

void TTAudioManager::fadeOutUpdate(float dt)
{
    float bgVol = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgVol-_fadeOut);
    
    if(bgVol-_fadeOut <= 0)
    {
        // Se ha finalizado el fadeOut
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTAudioManager::fadeOutUpdate),this);
    }
}

void TTAudioManager::fadeInBackgroundMusic(float _timeFadeIn)
{
    Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTAudioManager::fadeOutUpdate),this);
    
    float callsToUpdate = _timeFadeIn/0.1f;
    _fadeIn = MUSIC_VOLUME()/callsToUpdate;
    
    Director::getInstance()->getScheduler()->schedule(SEL_SCHEDULE(&TTAudioManager::fadeInUpdate),this,0.1f,false);
}

void TTAudioManager::fadeInUpdate(float dt)
{
    float bgVol = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgVol+_fadeIn);
    
    if(bgVol+_fadeIn >= MUSIC_VOLUME() )
    {
        // Se ha finalizado el fadeIn
        SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(MUSIC_VOLUME());
        Director::getInstance()->getScheduler()->unschedule(SEL_SCHEDULE(&TTAudioManager::fadeInUpdate),this);
    }
}

void TTAudioManager::setMusic(bool _activated)
{
    if (_activated == _musicOn)
    {
        return;
    }
    _musicOn = _activated;
    
    if(!_activated)
    {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }
    else
    {
        playAudio(_actualMusic,true);
    }
}

void TTAudioManager::setEffects(bool _activated)
{
    _effectsOn = _activated;
}

void TTAudioManager::changeVolumeMusic(int newVolumePercent)
{
    _musicLevel = newVolumePercent;
    TTUserInfo::getInstance()->_userData->_musicLevel = _musicLevel;
    TTUserInfo::getInstance()->saveData();
    
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(MUSIC_VOLUME());

    this->setMusic(_musicLevel>0);
}

void TTAudioManager::changeVolumeFx(int newVolumePercent)
{
    _effectsLevel=newVolumePercent;
    TTUserInfo::getInstance()->_userData->_effectsLevel = _effectsLevel;
    TTUserInfo::getInstance()->saveData();

    SimpleAudioEngine::getInstance()->setEffectsVolume(EFFECTS_VOLUME());

    this->setEffects(_effectsLevel>0);
}
