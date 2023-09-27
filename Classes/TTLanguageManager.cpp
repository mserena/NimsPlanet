#include "TTLanguageManager.h"
#include "TTEventsManager.h"
#include "TTUserInfo.h"

#define LANGUAGES_XML "Languages.xml"

//Singelton
static TTLanguageManager *_sharedLanguageManager = nullptr;

TTLanguageManager* TTLanguageManager::getInstance()
{
    if (!_sharedLanguageManager)
    {
        _sharedLanguageManager = new TTLanguageManager();
        _sharedLanguageManager->init();
    }
    
    return _sharedLanguageManager;
}

void TTLanguageManager::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedLanguageManager);
}

TTLanguageManager::~TTLanguageManager()
{
    
}

bool TTLanguageManager::init(void)
{
    
    if(TTUserInfo::getInstance()->_userData->_language == -1)
    {
        //Seteamos idioma según dispositivo
        if(Application::getInstance()->getCurrentLanguage() == LanguageType::SPANISH)
        {
            _language = SPANISH;
            TTUserInfo::getInstance()->_userData->_language = SPANISH;
        }
        else if(Application::getInstance()->getCurrentLanguage() == LanguageType::FRENCH)
        {
            _language = FRENCH;
            TTUserInfo::getInstance()->_userData->_language = FRENCH;
        }
        else if(Application::getInstance()->getCurrentLanguage() == LanguageType::GERMAN)
        {
            _language = GERMAN;
            TTUserInfo::getInstance()->_userData->_language = GERMAN;
        }
        else if(Application::getInstance()->getCurrentLanguage() == LanguageType::PORTUGUESE)
        {
            _language = PORTUGUESE;
            TTUserInfo::getInstance()->_userData->_language = PORTUGUESE;
        }
        else if(Application::getInstance()->getCurrentLanguage() == LanguageType::ITALIAN)
        {
            _language = ITALIAN;
            TTUserInfo::getInstance()->_userData->_language = ITALIAN;
        }
        else
        {
            _language = ENGLISH;
            TTUserInfo::getInstance()->_userData->_language = ENGLISH;
        }
        TTUserInfo::getInstance()->saveData();
    }
    else
    {
        // Seteamos idioma según el user Data
        if(TTUserInfo::getInstance()->_userData->_language == SPANISH)
        {
            _language = SPANISH;
        }
        else if(TTUserInfo::getInstance()->_userData->_language == FRENCH)
        {
            _language = FRENCH;
        }
        else if(TTUserInfo::getInstance()->_userData->_language == GERMAN)
        {
            _language = GERMAN;
        }
        else if(TTUserInfo::getInstance()->_userData->_language == PORTUGUESE)
        {
            _language = PORTUGUESE;
        }
        else if(TTUserInfo::getInstance()->_userData->_language == ITALIAN)
        {
            _language = ITALIAN;
        }
        else
        {
            _language = ENGLISH;
        }
    }
    
    // Abrimos XML idiomas
    pugi::xml_document *XMLFile = new pugi::xml_document();
    cocos2d::Data dataXML = FileUtils::getInstance()->getDataFromFile(LANGUAGES_XML);
    pugi::xml_parse_result Result = XMLFile->load_buffer(dataXML.getBytes(), dataXML.getSize());
    if(!Result){
        CCLOG("Problema al leer XML de idiomas");
        return false;
    }
    
    // Leemos XML idiomas
	pugi::xml_node TextNodes = XMLFile->child("Texts");
	if(!TextNodes.empty())
	{
		for(pugi::xml_node Node = TextNodes.child("Text"); Node; Node = Node.next_sibling("Text"))
		{
            std::string idTag = Node.attribute("id").as_string();
            std::string idTagStr;
            idTagStr.assign(idTag.begin(),idTag.end());
            std::map<std::string, std::vector<std::string> > _languagesTags;
            for(pugi::xml_node childNode = Node.child("Language"); childNode; childNode = childNode.next_sibling("Language"))
            {
                std::string idLanguage = childNode.attribute("id").as_string();
                std::vector<std::string> lines;
                for(pugi::xml_node childNodeLines = childNode.child("line"); childNodeLines; childNodeLines = childNodeLines.next_sibling("line"))
                {
                    std::string text = childNodeLines.attribute("text").as_string();
                    lines.push_back(text);
                }
                _languagesTags.insert(std::make_pair(idLanguage,lines));
            }
            _texts.insert(std::make_pair(idTagStr,_languagesTags));
        }
	}
    
    return true;
}

void TTLanguageManager::setLanguage(LANGUAGES idLanguage)
{
    TTUserInfo::getInstance()->_userData->_language = idLanguage;
    TTUserInfo::getInstance()->saveData();
    
    _language = idLanguage;
    sendEvent(EVENT_LANGUAGE_CHANGE);
}

std::string TTLanguageManager::getActualLanguage()
{
	std::string _actualLanguage = _languages.find(_language)->second;
	return _actualLanguage;
}

std::string TTLanguageManager::getTag(std::string tagId)
{
    std::string _actualLanguage = _languages.find(_language)->second;
    if(_texts.find(tagId) == _texts.end())
    {
        return "NULL TAG";
    }

    std::vector<std::string> tag = _texts.find(tagId)->second.find(_actualLanguage)->second;
    if(tag.size() == 1)
    {
        return tag[0];
    }
    else
    {
        std::string tagMount = "";
        for(int i = 0; i < tag.size(); i++)
        {
            tagMount += tag[i];
            if(i != (tag.size()-1)) tagMount += '\n';
        }
        return tagMount;
    }
}

std::string TTLanguageManager::convertIntToStringWithDecimalPoint(int num)
{
    std::string numWithCommas = std::to_string(num);
    int insertPosition = numWithCommas.length() - 3;
    while (insertPosition > 0) {
        numWithCommas.insert(insertPosition, getTag(SEPARATOR_MIL));
        insertPosition-=3;
    }
    return numWithCommas;
}

