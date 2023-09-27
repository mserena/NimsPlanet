#include "TTParticleManager.h"
#include "TTEventsManager.h"

#define LANGUAGES_XML "Languages.xml"

//Singelton
static TTParticleManager *_sharedParticleManager = nullptr;

TTParticleManager* TTParticleManager::getInstance()
{
    if (!_sharedParticleManager)
    {
        _sharedParticleManager = new TTParticleManager();
        _sharedParticleManager->init();
    }
    
    return _sharedParticleManager;
}

void TTParticleManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedParticleManager);
}

TTParticleManager::~TTParticleManager()
{
    
}

bool TTParticleManager::init(void)
{
    return true;
}

void TTParticleManager::loadParticle(std::string idParticle)
{
    // Verifica que la partícula no este cargada
    if(_particles.find(idParticle) != _particles.end())
        return;
    
    char strParticles[100] = {0};
    sprintf(strParticles,"%s.plist",idParticle.c_str());
    auto valuemap = FileUtils::getInstance()->getValueMapFromFile(strParticles);
    
    _particles.insert(std::make_pair(idParticle,valuemap));
}

ValueMap TTParticleManager::getParticle(std::string idParticle)
{
    if(_particles.find(idParticle) != _particles.end())
    {
        return _particles.find(idParticle)->second;
    }
    else
    {
        CCLOG("Intentas recuperar particulas que no han sido cargadas");
        abort();
    }
}

