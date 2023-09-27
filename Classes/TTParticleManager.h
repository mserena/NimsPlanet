#ifndef __TTParticleManager_SCENE_H__
#define __TTParticleManager_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;

class CC_DLL TTParticleManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTParticleManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();

protected:
    
    TTParticleManager(){}

public:
    
    virtual ~TTParticleManager();
    bool init(void);
    
    void loadParticle(std::string idParticle);
    ValueMap getParticle(std::string idParticle);
    
private:
    
    std::map<std::string,ValueMap> _particles;

};

#endif  // __TTParticleManager_SCENE_H__
