#ifndef __TTUIFacebook_SCENE_H__
#define __TTUIFacebook_SCENE_H__

#include "thegame.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "extensions/cocos-ext.h"
#include "TTFacebook.h"

using namespace screw::facebook;
using namespace cocos2d;
using namespace cocostudio;
using namespace extension;

#define WIDTH_UI_FACEBOOK_SPACE             95
#define HEIGHT_UI_FACEBOOK_SPACE            85

typedef enum ACTIVE_FACEBOOK_MENU
{
    FACEBOOK_CONNECT_PAGE,
    FACEBOOK_MAIL_PAGE,
    FACEBOOK_SEND_HEART_PAGE,
    FACEBOOK_ASK_FOR_HEARTS_PAGE,
}ACTIVE_FACEBOOK_MENU;

/*
 *
 * Clase TTUIFriendCell
 *
 */
class TTUIFriendCell : public TableViewCell
{
public:
    
    TTUIFriendCell();
    void update(GraphUser *user, bool selectedCell);
    void updatePhoto(std::string _uid);
    
public:
    
    cocos2d::Layer*             _uiCellLayer;
    cocos2d::ui::Widget*        _widgetCell;
    
    cocos2d::ui::TextBMFont*    _usrName;
    cocos2d::ui::Widget*        _tickOn;
    cocos2d::ui::Widget*        _userPhotoParent;
    Sprite*                     _userPhoto;
    
    GraphUser *_user;
};

/*
 *
 * Clase TTUIMailCell
 *
 */
class TTUIMailCell : public TableViewCell
{
public:
    
    TTUIMailCell();
    void update(GraphRequest *request);
    
    void updatePhoto(std::string _uid);

public:
    
    cocos2d::Layer*             _uiCellLayer;
    cocos2d::ui::Widget*        _widgetCell;
    
    cocos2d::ui::Widget*        _userPhotoParent;
    Sprite*                     _userPhoto;
    
    cocos2d::ui::Widget*        _imgReciveHeart;
    cocos2d::ui::Widget*        _imgSendHeart;
    
    cocos2d::ui::Text*          _referenceText;
    Label*                      _messageMail;
    
    GraphRequest*               _request;
};

/*
 *
 * Clase TTUIFriendsTable
 *
 */
class TTUIFriendsTable : public Node, public TableViewDataSource, public TableViewDelegate
{
public:
    
    TTUIFriendsTable();
    
    virtual Size cellSizeForTable(TableView *table);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
    
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
    virtual void scrollViewDidScroll(ScrollView* view) {}
    virtual void scrollViewDidZoom(ScrollView* view) {}
    
    void reload();
    void onExit() override;
    
    void markAllCells(bool mark);

    void onEvent_userPhotoUpdated(EventCustom* e);

public:
    
    TableView*          _table;
    Vector<GraphUser *> _users;
};

/*
 *
 * Clase TTUIMailTable
 *
 */
class TTUIMailTable : public Node, public TableViewDataSource, public TableViewDelegate
{
public:
    
    TTUIMailTable();
    
    virtual Size cellSizeForTable(TableView *table);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
    
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
    virtual void scrollViewDidScroll(ScrollView* view) {}
    virtual void scrollViewDidZoom(ScrollView* view) {}
    
    void reload();
    void onExit() override;
    
    void onEvent_userPhotoUpdated(EventCustom* e);
    
public:
    
    TableView*              _table;
    Vector<GraphRequest *>  _request;
};

/*
 *
 * Clase TTUIFacebook
 *
 */
class CC_DLL TTUIFacebook: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTUIFacebook* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
	TTUIFacebook(){}
    
public:
    
    void preloadResources();
    void resourceLoaded(Texture2D* texture);
    
    void init();
    ~TTUIFacebook();
    
    void pauseUI();
    void restoreUI();

    void showFacebookPage(Node* _parent, ACTIVE_FACEBOOK_MENU _page);
    void showPage();
    
    void onClickExitBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
private:
    
    void exitMenu();
    void onAllFriendsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickSendOrAskHeartsBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickAcceptMail(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onClickConnectFacebookBtn(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void onEvent_FacebookSendRequest(EventCustom* e);
    void onEvent_FacebookConnect(EventCustom* e);
    
public:
    
    bool _blockMenus;
    
    Node*                       _actualParent;
    ACTIVE_FACEBOOK_MENU        _actualPage;
    
    cocos2d::Layer*             _uiFacebookLayer;
    cocos2d::ui::Widget*        _widgetFacebook;
    
    cocos2d::ui::TextBMFont*    _facebookTitle;
    cocos2d::ui::Button*        _btnExit;
    Sprite*                     _bgUI;
    
    // Padres de todas las secciones
    cocos2d::ui::Widget*        _facebookMailParent;
    cocos2d::ui::Widget*        _facebookGeneralParent;
    cocos2d::ui::Widget*        _facebookGiveHeartsParent;
    cocos2d::ui::Widget*        _facebookInviteFriendsParent;
    
    // General
    cocos2d::ui::Button*         _connectBtn;
    cocos2d::ui::TextBMFont*     _connectBtnLabel;
    
    cocos2d::ui::Widget*         _tableFrame;
    
    //Mail
    TTUIMailTable*               _tableMail;
    cocos2d::ui::Button*         _btnAcceptMail;
    cocos2d::ui::TextBMFont*     _btnAcceptMailLabel;
    
    // Dar corazones
    TTUIFriendsTable*            _tableFriends;
    Size                         _tableSize;
    std::vector<GraphUser*>      _selectedUsers;
    cocos2d::ui::Button*         _btnAllFriends;
    cocos2d::ui::TextBMFont*     _btnAllFriendsLabel;
    cocos2d::ui::Button*         _btnSendHearts;
    cocos2d::ui::TextBMFont*     _btnSendHeartsLabel;
    
    //General
    cocos2d::ui::TextBMFont*         _generalText1;
    cocos2d::ui::TextBMFont*         _generalText2;
    cocos2d::ui::TextBMFont*         _generalText3;
    cocos2d::ui::TextBMFont*         _generalText4;
    cocos2d::ui::TextBMFont*         _generalBtnText;
    
};

#endif  // __TTUIFacebook_SCENE_H__
