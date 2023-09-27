//
//  TTAudioManager.h
//  animalplanet
//
//  Created by Marc Serena on 03/11/14.
//
//

#ifndef __animalplanet__TTAudioManager__
#define __animalplanet__TTAudioManager__

#include "cocos2d.h"
#include "TTAudioDefines.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define MAX_VOLUME_EFFECTS    1.0
#define MAX_VOLUME_BACKGROUND 0.2

#define MUSIC_VOLUME()      (_musicLevel*MAX_VOLUME_BACKGROUND/100)
#define EFFECTS_VOLUME()    (_effectsLevel*MAX_VOLUME_EFFECTS/100)

class CC_DLL TTAudioManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTAudioManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTAudioManager(){}
    
public:
    
    virtual ~TTAudioManager();
    bool init(void);
    
    void loadActualWorldEffects();
    void unLoadActualWorldEffects();
    
    void playAudio(std::string tag, bool loop = false);
    void stopAudio(std::string tag);
    
    void changeVolumeMusic(int newVolumePercent);
    void changeVolumeFx(int newVolumePercent);
    
private:

    void setMusic(bool _activated);
    void setEffects(bool _activated);

    void changeBackgroundMusicWithFadeEffect(std::string tag, float _timeToChange);
    
    void fadeOutBackgroundMusic(float _timeFadeOut);
    void fadeOutUpdate(float dt);

    void fadeInBackgroundMusic(float _timeFadeIn);
    void fadeInUpdate(float dt);
    
private:
    
    std::string _actualMusic;
    
    bool _musicOn;
    bool _effectsOn;
    int _musicLevel;
    int _effectsLevel;
    
    float _fadeOut;
    float _fadeIn;
    
    std::string _extensionBgFiles;
    std::string _extensionEffectFiles;
    
    std::map<std::string,std::string> _soundEffectsWorld;
    
};

#endif /* defined(__animalplanet__TTAudioManager__) */
