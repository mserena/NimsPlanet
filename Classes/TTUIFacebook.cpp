#include "TTUIFacebook.h"
#include "TTLanguageManager.h"
#include "TTEventsManager.h"
#include "TTAnimalCubes.h"
#include "TTUIStackManager.h"
#include "TTAudioManager.h"
#include "TTUserInfo.h"
#include "TTUIPopup.h"
#include "TTUIMap.h"
#include "TTParseManager.h"

#define TABLE_VISIBLE_CELLS 5

/*
 *
 * Clase TTUIFriendCell
 *
 */

TTUIFriendCell::TTUIFriendCell()
{
    _uiCellLayer = Layer::create();
    _uiCellLayer->setAnchorPoint(Point(0.5,0.5));
    _widgetCell = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_FACEBOOK_CELL.ExportJson");
    cocos2d::ui::Layout* _layoutCell = dynamic_cast<cocos2d::ui::Layout*>(_widgetCell);
    _uiCellLayer->addChild(_layoutCell);
    
    // Escalamos el widget
    _widgetCell->setScale(0.90f,0.80f);
    _widgetCell->setTouchEnabled(false);
    
    // Cargamos los recursos
    _usrName = (cocos2d::ui::TextBMFont*)_widgetCell->getChildByName("userBtn")->getChildByName("userName");
    _tickOn = (cocos2d::ui::Widget*)_widgetCell->getChildByName("userBtn")->getChildByName("tickOff")->getChildByName("tickOn");
    _tickOn->setVisible(false);
    _userPhotoParent = (cocos2d::ui::ImageView*)_widgetCell->getChildByName("userBtn")->getChildByName("userImg");
    _userPhoto = nullptr;
    
    this->addChild(_uiCellLayer);
    
    float _cellWidth = (90*TTUIFacebook::getInstance()->_tableSize.width)/100;
    float _spaceWidth = TTUIFacebook::getInstance()->_tableSize.width-_cellWidth;
    _widgetCell->setPosition(Point(_spaceWidth/6,0));
}

void TTUIFriendCell::update(GraphUser *user, bool selectedCell)
{
    _user = user;
    //update photo
    this->updatePhoto(user->getId());
    
    std::string _userName = user->getFirstName()+" "+user->getLastName();
    std::string _userNameCut = _userName.substr(0,15);
    
    _usrName->setString(_userNameCut);
    
    _tickOn->setVisible(selectedCell);
}

void TTUIFriendCell::updatePhoto(std::string _uid)
{
    //remove previous userphoto
    if (_userPhoto!=nullptr)
    {
        _userPhoto->removeFromParent();
    }
    // User photo
    _userPhoto = Sprite::createWithTexture(TTFacebook::getInstance()->getTexturePhotoUser(_uid));
    _userPhoto->setAnchorPoint(Point(0,0));
    _userPhoto->setScale(_userPhotoParent->getContentSize().height/_userPhoto->getContentSize().height);
    _userPhotoParent->addChild(_userPhoto);
}



/*
 *
 * Clase TTUIFriendsTable
 *
 */
TTUIFriendsTable::TTUIFriendsTable()
{
    _table = TableView::create(this,TTUIFacebook::getInstance()->_tableSize);
    _table->setDirection(ScrollView::Direction::VERTICAL);
    _table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _table->setDelegate(this);
    this->addChild(_table);

    TTEventsManager::getInstance()->addListenerEvent("TTUIFacebookFriends", EVENT_FACEBOOK_PHOTO, CC_CALLBACK_1(TTUIFriendsTable::onEvent_userPhotoUpdated,this));

    // Fondo de espacio para la tabla
    /*Sprite* _bgTable = Sprite::create();
    _bgTable->setColor(Color3B::BLUE);
    _bgTable->setOpacity(50);
    _bgTable->setTextureRect(Rect(0,0,TTUIFacebook::getInstance()->_tableSize.width,TTUIFacebook::getInstance()->_tableSize.height));
    _bgTable->setAnchorPoint(Point(0,0));
    this->addChild(_bgTable);*/
}

void TTUIFriendsTable::onEvent_userPhotoUpdated(EventCustom* e)
{
    FBDataEvent *dataEvent = static_cast<FBDataEvent*>(e->getUserData());
    for(int i = 0; i < _users.size(); i++)
    {
        TTUIFriendCell *cell = (TTUIFriendCell *) _table->cellAtIndex(i);
        if (cell && cell->_user->getId() == dataEvent->userId)
        {
            cell->updatePhoto(dataEvent->userId);
        }
    }
}
void TTUIFriendsTable::onExit()
{
    TTEventsManager::getInstance()->removeSafeListenerEvents("TTUIFacebookFriends");
    _table->removeFromParentAndCleanup(true);
    Node::onExit();
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}

Size TTUIFriendsTable::cellSizeForTable(TableView *table)
{
    float _cellWidth = (90*TTUIFacebook::getInstance()->_tableSize.width)/100;
    float _cellHeight = (90*(TTUIFacebook::getInstance()->_tableSize.height/TABLE_VISIBLE_CELLS))/100;
    return Size(_cellWidth,_cellHeight);
}

TableViewCell *TTUIFriendsTable::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TTUIFriendCell* cell = (TTUIFriendCell *) table->dequeueCell();
    GraphUser* user = _users.at(idx);
    if (!cell)
    {
        cell = new TTUIFriendCell();
        cell->autorelease();
    }
    
    bool _markCell = false;
    for(int i = 0; i < TTUIFacebook::getInstance()->_selectedUsers.size(); i++)
    {
        if(TTUIFacebook::getInstance()->_selectedUsers[i]->getId() == user->getId())
        {
            _markCell = true;
        }
    }
    cell->update(user,_markCell);
    
    return cell;
}

ssize_t TTUIFriendsTable::numberOfCellsInTableView(TableView *table)
{
    return _users.size();
}

void TTUIFriendsTable::markAllCells(bool mark)
{
    for (int i = 0; i < _users.size(); i++)
    {
        TTUIFriendCell *cell = (TTUIFriendCell*) _table->cellAtIndex(i);
        if(cell)
        {
            cell->_tickOn->setVisible(mark);
        }
    }
}

void TTUIFriendsTable::tableCellTouched(TableView *table, TableViewCell *cell)
{
    GraphUser *user = ((TTUIFriendCell *)cell)->_user;
    bool _unselected = false;
    for(int i = 0; i < TTUIFacebook::getInstance()->_selectedUsers.size(); i++)
    {
        if(TTUIFacebook::getInstance()->_selectedUsers[i]->getId() == user->getId())
        {
            TTUIFacebook::getInstance()->_selectedUsers.erase(TTUIFacebook::getInstance()->_selectedUsers.begin()+i);
            _unselected = true;
            break;
        }
    }
    
    // Seteamos el tick de selección
    ((TTUIFriendCell *)cell)->_tickOn->setVisible(!((TTUIFriendCell *)cell)->_tickOn->isVisible());
    
    if(!_unselected)
    {
        auto _scaleUp = ScaleTo::create(0.1f,((TTUIFriendCell *)cell)->_tickOn->getScale()+0.1f);
        auto _scaleDown = ScaleTo::create(0.1f,((TTUIFriendCell *)cell)->_tickOn->getScale());
        ((TTUIFriendCell *)cell)->_tickOn->runAction(Sequence::create(_scaleUp,_scaleDown,NULL));
        
        TTUIFacebook::getInstance()->_selectedUsers.push_back(user);
    }
}

void TTUIFriendsTable::reload()
{
    _users.clear();
    Vector<GraphUser *> friends = Facebook::getInstance()->getFriends();
    for (auto f : friends)
    {
        _users.pushBack(f);
    }
    _table->reloadData();
}

/*
 *
 * Clase TTUIMailCell
 *
 */

TTUIMailCell::TTUIMailCell()
{
    _uiCellLayer = Layer::create();
    _uiCellLayer->setAnchorPoint(Point(0.5,0.5));
    _widgetCell = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_FACEBOOK_CELL_MAIL.ExportJson");
    cocos2d::ui::Layout* _layoutCell = dynamic_cast<cocos2d::ui::Layout*>(_widgetCell);
    _uiCellLayer->addChild(_layoutCell);
    
    // Escalamos el widget
    _widgetCell->setScale(0.90f,0.80f);
    _widgetCell->setTouchEnabled(false);
    _widgetCell->setVisible(true);
    
    _userPhotoParent = (cocos2d::ui::ImageView*)_widgetCell->getChildByName("userBtn")->getChildByName("userImg");
    _userPhoto =  nullptr;
    _imgReciveHeart = (cocos2d::ui::ImageView*)_widgetCell->getChildByName("userBtn")->getChildByName("iconMail")->getChildByName("imgHeartRecive");
    _imgSendHeart = (cocos2d::ui::ImageView*)_widgetCell->getChildByName("userBtn")->getChildByName("iconMail")->getChildByName("imgHeartSend");
    _imgReciveHeart->setVisible(false);
    _imgSendHeart->setVisible(false);
    
    _referenceText = (cocos2d::ui::Text*)_widgetCell->getChildByName("userBtn")->getChildByName("textMail");
    _referenceText->setVisible(false);
    _messageMail = Label::createWithBMFont("markerfeltwide.fnt","");
    _messageMail->setPosition(_referenceText->getPosition());
    _referenceText->getParent()->addChild(_messageMail);
    _messageMail->setAlignment(TextHAlignment::LEFT,TextVAlignment::CENTER);
    
    this->addChild(_uiCellLayer);
    float _cellWidth = (90*TTUIFacebook::getInstance()->_tableSize.width)/100;
    float _spaceWidth = TTUIFacebook::getInstance()->_tableSize.width-_cellWidth;
    _widgetCell->setPosition(Point(_spaceWidth/6,0));
}

void TTUIMailCell::update(GraphRequest *request)
{
    _request = request;
    
    //update photo
    this->updatePhoto(_request->getFrom()->getId());
    
    // Según el tipo de request...
    if(_request->getType() == _RequestTypeSendHeart)
    {
        _imgReciveHeart->setVisible(true);
        _imgSendHeart->setVisible(false);
        std::string _userName = _request->getFrom()->getName();
        std::string _userNameCut = _userName.substr(0,15);
        _messageMail->setString(_userNameCut+". "+TTLanguageManager::getInstance()->getTag(FACEBOOK_RECIVE_HEART_MAIL_TEXT));
    }
    else if(_request->getType() == _RequestTypeAskforHeart)
    {
        _imgSendHeart->setVisible(true);
        _imgReciveHeart->setVisible(false);
        std::string _userName = _request->getFrom()->getName();
        std::string _userNameCut = _userName.substr(0,15);
        _messageMail->setString(_userNameCut+". "+TTLanguageManager::getInstance()->getTag(FACEBOOK_ASK_HEART_MAIL_TEXT));
    }
}

void TTUIMailCell::updatePhoto(std::string _uid)
{
    //remove previous userphoto
    if (_userPhoto!=nullptr)
    {
        _userPhoto->removeFromParent();
    }
    // User photo
    _userPhoto = Sprite::createWithTexture(TTFacebook::getInstance()->getTexturePhotoUser(_uid));
    _userPhoto->setAnchorPoint(Point(0,0));
    _userPhoto->setScale(_userPhotoParent->getContentSize().height/_userPhoto->getContentSize().height);
    _userPhotoParent->addChild(_userPhoto);
}

/*
 *
 * Clase TTUIMailTable
 *
 */
TTUIMailTable::TTUIMailTable()
{
    _table = TableView::create(this,TTUIFacebook::getInstance()->_tableSize);
    _table->setDirection(ScrollView::Direction::VERTICAL);
    _table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    _table->setDelegate(this);
    this->addChild(_table);
    
    TTEventsManager::getInstance()->addListenerEvent("TTUIFacebookMail", EVENT_FACEBOOK_PHOTO, CC_CALLBACK_1(TTUIMailTable::onEvent_userPhotoUpdated,this));

    // Fondo de espacio para la tabla
    /*Sprite* _bgTable = Sprite::create();
    _bgTable->setColor(Color3B::BLUE);
    _bgTable->setOpacity(50);
    _bgTable->setTextureRect(Rect(0,0,TTUIFacebook::getInstance()->_tableSize.width,TTUIFacebook::getInstance()->_tableSize.height));
    _bgTable->setAnchorPoint(Point(0,0));
    this->addChild(_bgTable);*/    
}

void TTUIMailTable::onEvent_userPhotoUpdated(EventCustom* e)
{
    FBDataEvent *dataEvent = static_cast<FBDataEvent*>(e->getUserData());
    for(int i = 0; i < _request.size(); i++)
    {
        TTUIMailCell *cell = (TTUIMailCell *) _table->cellAtIndex(i);
        if (cell && cell->_request->getFrom()->getId() == dataEvent->userId)
        {
            cell->updatePhoto(dataEvent->userId);
        }
    }
}

void TTUIMailTable::onExit()
{
    TTEventsManager::getInstance()->removeSafeListenerEvents("TTUIFacebookMail");
    _table->removeFromParentAndCleanup(true);
    Node::onExit();
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}


Size TTUIMailTable::cellSizeForTable(TableView *table)
{
    float _cellWidth = (90*TTUIFacebook::getInstance()->_tableSize.width)/100;
    float _cellHeight = (90*(TTUIFacebook::getInstance()->_tableSize.height/TABLE_VISIBLE_CELLS))/100;
    return Size(_cellWidth,_cellHeight);
}

TableViewCell *TTUIMailTable::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TTUIMailCell* cell = (TTUIMailCell *) table->dequeueCell();
    GraphRequest* request = _request.at(idx);
    if (!cell)
    {
        cell = new TTUIMailCell();
        cell->autorelease();
    }
    
    cell->update(request);
    return cell;
}

ssize_t TTUIMailTable::numberOfCellsInTableView(TableView *table)
{
    return _request.size();
}

void TTUIMailTable::tableCellTouched(TableView *table, TableViewCell *cell)
{
    
}

void TTUIMailTable::reload()
{
    _request.clear();
    Vector<GraphRequest *> requests = AppRequests::getInstance()->getRequests();
    for (GraphRequest *r : requests)
    {
       //guardar solo las de tipo recuestHeartTypes
        if (r->getType() == _RequestTypeAskforHeart || r->getType() == _RequestTypeSendHeart)
        {
            _request.pushBack(r);
        }
    }
    _table->reloadData();
}


/*
 *
 * Clase TTUIFacebook
 *
 */

//Singleton
static TTUIFacebook *_sharedSettingsUI = nullptr;

TTUIFacebook* TTUIFacebook::getInstance()
{
    if (!_sharedSettingsUI)
    {
        _sharedSettingsUI = new TTUIFacebook();
    }
    
    return _sharedSettingsUI;
}

void TTUIFacebook::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedSettingsUI);
}

TTUIFacebook::~TTUIFacebook()
{
    CCLOG("Se destruye la UI de facebook");
    
    // Eliminamos las acciones
    removeAllActionsFromJSON("UI_FACEBOOK.ExportJson");
    
    // Eliminamos la layer contenedora
    _uiFacebookLayer->removeAllChildrenWithCleanup(true);
    _uiFacebookLayer->removeFromParentAndCleanup(true);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UI_FACEBOOK0.plist");
    Director::getInstance()->getTextureCache()->removeTextureForKey("UI_FACEBOOK0.png");
    
    //Desregistrar listeners de eventos
    TTEventsManager::getInstance()->removeSafeListenerEvents("TTUIFacebook");
}

void TTUIFacebook::preloadResources()
{
    Director::getInstance()->getTextureCache()->addImageAsync("UI_FACEBOOK0.png",CC_CALLBACK_1(TTUIFacebook::resourceLoaded, this));
}

void TTUIFacebook::resourceLoaded(Texture2D* texture)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UI_FACEBOOK0.plist","UI_FACEBOOK0.png");
    init();
    TTUIStackManager::getInstance()->hideUILoading();
    showPage();
}

void TTUIFacebook::init()
{
    _blockMenus = false;
    
    // Registramos los eventos
    TTEventsManager::getInstance()->addListenerEvent("TTUIFacebook", EVENT_FACEBOOK_REQUEST_SEND,CC_CALLBACK_1(TTUIFacebook::onEvent_FacebookSendRequest,this));
    TTEventsManager::getInstance()->addListenerEvent("TTUIFacebook", EVENT_FACEBOOK_CONNECTION,CC_CALLBACK_1(TTUIFacebook::onEvent_FacebookConnect,this));
    
    // Cargamos el XML de la UI Facebook
    _uiFacebookLayer = Layer::create();
    _uiFacebookLayer->retain();
    _widgetFacebook = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_FACEBOOK.ExportJson");
    cocos2d::ui::Layout* _layoutFacebook = dynamic_cast<cocos2d::ui::Layout*>(_widgetFacebook);
    _uiFacebookLayer->addChild(_layoutFacebook,30);
    
    // Escalado Widget
    _widgetFacebook->setScale(scale_widget(_widgetFacebook,WIDTH_UI_FACEBOOK_SPACE,HEIGHT_UI_FACEBOOK_SPACE));
    
    // Posicionamiento Widget
    Size _screenSize = Director::getInstance()->getWinSize();
    _widgetFacebook->setAnchorPoint(Vec2(0.5,0.5));
    _uiFacebookLayer->setPosition(Vec2(_screenSize.width/2,_screenSize.height-(_widgetFacebook->getBoundingBox().size.height/2)));
    _uiFacebookLayer->setVisible(false);
    
    _facebookTitle = (cocos2d::ui::TextBMFont*)_widgetFacebook->getChildByName("facebookBg")->getChildByName("titleMenu");
    _btnExit = (cocos2d::ui::Button*)_widgetFacebook->getChildByName("facebookBg")->getChildByName("exitBtn");
    _btnExit->addTouchEventListener(CC_CALLBACK_2(TTUIFacebook::onClickExitBtn,this));
    
    // Pantallas de facebook
    _facebookGeneralParent = (cocos2d::ui::Widget*)_widgetFacebook->getChildByName("facebookBg")->getChildByName("ui_general");
    _connectBtn = (cocos2d::ui::Button*)_facebookGeneralParent->getChildByName("conBtn");
    _connectBtnLabel = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("conBtn")->getChildByName("label");
    _connectBtn->addTouchEventListener(CC_CALLBACK_2(TTUIFacebook::onClickConnectFacebookBtn, this));
    _generalText1 = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("label1");
    _generalText2 = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("label2");
    _generalText3 = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("label3");
    _generalText4 = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("label4");
    _generalBtnText = (cocos2d::ui::TextBMFont*)_facebookGeneralParent->getChildByName("conBtn")->getChildByName("label");
    _generalText1->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_GENERAL_TEXT1"));
    _generalText2->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_GENERAL_TEXT2"));
    _generalText3->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_GENERAL_TEXT3"));
    _generalText4->setString(TTLanguageManager::getInstance()->getTag("FACEBOOK_GENERAL_TEXT4"));
    _generalBtnText->setString(TTLanguageManager::getInstance()->getTag("CONNECT"));
    
    _facebookMailParent = (cocos2d::ui::Widget*)_widgetFacebook->getChildByName("facebookBg")->getChildByName("ui_mail");
    _facebookGiveHeartsParent = (cocos2d::ui::Widget*)_widgetFacebook->getChildByName("facebookBg")->getChildByName("ui_giveLives");
    //_facebookInviteFriendsParent = (cocos2d::ui::Widget*)_widgetFacebook->getChildByName("ui_invitar");

    
    switch(_actualPage)
    {
        case FACEBOOK_CONNECT_PAGE:
        {
            _facebookTitle->setString(TTLanguageManager::getInstance()->getTag(FACEBOOK_GENERAL_TITLE));
        
        }
        break;
        case FACEBOOK_MAIL_PAGE:
        {
            _facebookTitle->setString(TTLanguageManager::getInstance()->getTag(FACEBOOK_MAIL_TITLE));
            
            _tableFrame = (cocos2d::ui::Widget*)_facebookMailParent->getChildByName("marco");
            _btnAcceptMail = (cocos2d::ui::Button*)_facebookMailParent->getChildByName("btnOk");
            _btnAcceptMail->addTouchEventListener(CC_CALLBACK_2(TTUIFacebook::onClickAcceptMail,this));
            _btnAcceptMailLabel = (cocos2d::ui::TextBMFont*)_facebookMailParent->getChildByName("btnOk")->getChildByName("btnOkLabel");
            
            // Creamos la tabla del mail
            _tableSize = Size(((95*_tableFrame->getContentSize().width)/100),((95*_tableFrame->getContentSize().height)/100));
            _tableMail = new TTUIMailTable();
            float _xAdjust = (_tableFrame->getContentSize().width-_tableSize.width)/2;
            float _yAdjust = (_tableFrame->getContentSize().height-_tableSize.height)/2;
            _tableMail->setPosition(Point(_xAdjust,_yAdjust));
            _tableFrame->addChild(_tableMail);
        }
        break;
        case FACEBOOK_SEND_HEART_PAGE:
        case FACEBOOK_ASK_FOR_HEARTS_PAGE:
        {
            _tableFrame = (cocos2d::ui::Widget*)_facebookGiveHeartsParent->getChildByName("marco");
            _btnAllFriends = (cocos2d::ui::Button*)_facebookGiveHeartsParent->getChildByName("btnAll");
            _btnAllFriends->addTouchEventListener(CC_CALLBACK_2(TTUIFacebook::onAllFriendsBtn,this));
            _btnSendHearts = (cocos2d::ui::Button*)_facebookGiveHeartsParent->getChildByName("btnSend");
            _btnSendHearts->addTouchEventListener(CC_CALLBACK_2(TTUIFacebook::onClickSendOrAskHeartsBtn,this));
            _btnSendHeartsLabel = (cocos2d::ui::TextBMFont*)_btnSendHearts->getChildByName("btnSendLabel");
            _btnAllFriendsLabel = (cocos2d::ui::TextBMFont*)_btnAllFriends->getChildByName("btnAllLabel");
            _btnAllFriendsLabel->setString(TTLanguageManager::getInstance()->getTag(ALL_BTN));
            
            if(_actualPage == FACEBOOK_SEND_HEART_PAGE)
            {
                _facebookTitle->setString(TTLanguageManager::getInstance()->getTag(FACEBOOK_GIVE_HEARTS_TITLE));
                _btnSendHeartsLabel->setString(TTLanguageManager::getInstance()->getTag(SEND_BTN));
                // Enviamos evento de analitica a Parse
                std::map<std::string,std::string> _dimensions;
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_GIVE_LIVES_MENU,_dimensions);
            }
            else
            {
                _facebookTitle->setString(TTLanguageManager::getInstance()->getTag(FACEBOOK_ASK_HEARTS_TITLE));
                _btnSendHeartsLabel->setString(TTLanguageManager::getInstance()->getTag(ASK_BTN));
                // Enviamos evento de analitica a Parse
                std::map<std::string,std::string> _dimensions;
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_ASK_LIVES_MENU,_dimensions);
            }
            
            // Creamos la tabla para invitar amigos
            _tableSize = Size(((95*_tableFrame->getContentSize().width)/100),((95*_tableFrame->getContentSize().height)/100));
            _tableFriends = new TTUIFriendsTable();
            float _xAdjust = (_tableFrame->getContentSize().width-_tableSize.width)/2;
            float _yAdjust = (_tableFrame->getContentSize().height-_tableSize.height)/2;
            _tableFriends->setPosition(Point(_xAdjust,_yAdjust));
            _tableFrame->addChild(_tableFriends);
        }
        break;
    }
    
    // Creamos el bg
    _bgUI = Sprite::create("pixelBg.png");
    _bgUI->retain();
    _bgUI->setAnchorPoint(Vec2(0.5,0.5));
    _bgUI->setPosition(Vec2(_screenSize.width/2,_screenSize.height/2));
    _bgUI->setScale(_screenSize.width,_screenSize.height);
    
    // Inicializamos las variables
    _selectedUsers.clear();
}

void TTUIFacebook::pauseUI()
{
    _blockMenus = true;
    _bgUI->setVisible(false);
}

void TTUIFacebook::restoreUI()
{
    _blockMenus = false;
    _bgUI->setVisible(true);
    
    // Verificamos si hay que cerrar el menú
    if(_actualPage == FACEBOOK_CONNECT_PAGE && TTFacebook::getInstance()->isConnected())
    {
        onClickExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
    }
}

void TTUIFacebook::showPage()
{
    _blockMenus = false;
    
    //Background
    if(_bgUI->getParent() != nullptr)
        _bgUI->removeFromParentAndCleanup(false);
    _actualParent->addChild(_bgUI,30);
    _bgUI->setVisible(true);
    
    //Widget
    if(_uiFacebookLayer->getParent() != nullptr)
        _uiFacebookLayer->removeFromParentAndCleanup(false);
    _actualParent->addChild(_uiFacebookLayer,31);
    _uiFacebookLayer->setVisible(true);
    _widgetFacebook->setVisible(true);
    
    _facebookMailParent->setVisible(false);
    _facebookGeneralParent->setVisible(false);
    _facebookGiveHeartsParent->setVisible(false);
    //_facebookInviteFriendsParent->setVisible(false);
    
    if(TTFacebook::getInstance()->isConnected())
    {
        switch(_actualPage)
        {
            case FACEBOOK_CONNECT_PAGE:
            {
                _facebookGeneralParent->setVisible(true);
            }
            break;
            case FACEBOOK_MAIL_PAGE:
            {
                _facebookMailParent->setVisible(true);
                _tableMail->reload();
                _btnAcceptMailLabel->setString(TTLanguageManager::getInstance()->getTag("ACCEPT"));
                // Enviamos evento de analitica a Parse
                std::map<std::string,std::string> _dimensions;
                TTParseManager::getInstance()->sendAnalyticEvent(ANALYTICS_MAIL_MENU,_dimensions);
            }
            break;
            case FACEBOOK_SEND_HEART_PAGE:
            case FACEBOOK_ASK_FOR_HEARTS_PAGE:
            {
                _facebookGiveHeartsParent->setVisible(true);
                _tableFriends->reload();
            }
            break;
        }
    }
    else
    {
        //Mostramos UI de conexión
        _facebookGeneralParent->setVisible(true);
        _actualPage = FACEBOOK_CONNECT_PAGE;
    }
    
    // Entrada del panel
    cocostudio::ActionManagerEx::getInstance()->playActionByName("UI_FACEBOOK.ExportJson","panel_enter");
    TTAudioManager::getInstance()->playAudio("PANEL_DOWN");
}

void TTUIFacebook::showFacebookPage(Node* _parent, ACTIVE_FACEBOOK_MENU _page)
{
    _actualParent = _parent;
    _actualPage = _page;
    TTUIStackManager::getInstance()->showUILoading(_parent);
    preloadResources();
}

void TTUIFacebook::exitMenu()
{
    _uiFacebookLayer->setVisible(false);
    _widgetFacebook->setVisible(false);
    _bgUI->setVisible(false);
    TTUIStackManager::getInstance()->closeUI();
}

void TTUIFacebook::onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (_blockMenus )
        return;
    cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_FACEBOOK.ExportJson","panel_enter")->stop();
    
    TTAudioManager::getInstance()->playAudio("BUTTON_EXIT");
    
    _blockMenus = true;
    ActionObject* action = cocostudio::ActionManagerEx::getInstance()->getActionByName("UI_FACEBOOK.ExportJson","panel_exit");
    DelayTime *delayExitMenu = DelayTime::create(action->getTotalTime());
    CallFunc *hideSettings = CallFunc::create(std::bind([&](){
        exitMenu();
        //sendEvent(EVENT_GAME_CONTINUE);
        destroyInstance();
    }));
    _uiFacebookLayer->getParent()->runAction(Sequence::create(delayExitMenu,hideSettings,NULL));
    action->play();
    
    TTAudioManager::getInstance()->playAudio("PANEL_UP");
}

void TTUIFacebook::onAllFriendsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    bool _markAll = false;
    if(_selectedUsers.size() != _tableFriends->_users.size())
    {
        _selectedUsers.clear();
        _markAll = true;
        _selectedUsers.clear();
        for(int i = 0; i < _tableFriends->_users.size(); i++)
        {
            _selectedUsers.push_back(_tableFriends->_users.at(i));
        }
    }
    else
    {
        _selectedUsers.clear();
    }
    _tableFriends->markAllCells(_markAll);
}

void TTUIFacebook::onClickSendOrAskHeartsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED || _selectedUsers.size() == 0)
        return;
    
    std::string _idUsers = "";
    for(int i = 0; i < _selectedUsers.size(); i++)
    {
        _idUsers += _selectedUsers[i]->getId();
        if(i < _selectedUsers.size()-1) _idUsers += ",";
    }
    
    if(_actualPage == FACEBOOK_SEND_HEART_PAGE)
    {
        TTFacebook::getInstance()->sendOrAskHeart(_idUsers,_RequestTypeSendHeart);
    }
    else
    {
        TTFacebook::getInstance()->sendOrAskHeart(_idUsers,_RequestTypeAskforHeart);
    }
}

void TTUIFacebook::onClickAcceptMail(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    
    TTFacebook::getInstance()->acceptAllUserRequest();
    TTUIMap::getInstance()->updateUIValues();
    TTUIMap::getInstance()->_alertMail->setVisible(false);
    onClickExitBtn(pSender,eEventType);
}

void TTUIFacebook::onClickConnectFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    if (eEventType!=ui::Widget::TouchEventType::ENDED)
        return;
    if (!TTUIStackManager::getInstance()->isUIOnTop(UI_FACEBOOK))
    {
        return;
    }

    TTFacebook::getInstance()->connectUserToFacebook();
}

void TTUIFacebook::onEvent_FacebookSendRequest(EventCustom* e)
{
    onClickExitBtn(nullptr,ui::Widget::TouchEventType::ENDED);
}

void TTUIFacebook::onEvent_FacebookConnect(EventCustom* e)
{
    
}
