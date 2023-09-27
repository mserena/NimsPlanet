#ifndef __TTLanguageManager_SCENE_H__
#define __TTLanguageManager_SCENE_H__

#include "cocos2d.h"
#include "pugixml.hpp"
#include "TTLanguagesDefinition.h"

using namespace cocos2d;

class CC_DLL TTLanguageManager: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTLanguageManager* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();

protected:
    
    TTLanguageManager(){}

public:
    
    virtual ~TTLanguageManager();
    bool init(void);
    
    void setLanguage(LANGUAGES idLanguage);
    std::string getTag(std::string tagId);
    
    std::string getActualLanguage();

    std::string convertIntToStringWithDecimalPoint(int num);
    
public:
    
    LANGUAGES _language;

private:
    
    // tag -> mapa de idiomas, idioma -> texto
    std::map<std::string, std::map<std::string,std::vector<std::string> > > _texts;
    
};

#endif  // __TTLanguageManager_SCENE_H__
