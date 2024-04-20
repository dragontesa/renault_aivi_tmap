#ifndef BOOKMARKPAGEVIEWMEDIATOR_H
#define BOOKMARKPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class BookmarkPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
    Q_ENUMS( MenuK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        FocusCsw,
        ResetCsw,
        RegistCsw,
        DeleteSelectedFavoriteItems,
        CreateView,
        SelectZone1TextInput,
        ActivatedForNavi,
        CloseView,
        EditFavoriteClicked,
        SelectZone1Title,
        AddVia,
        FavoriteItemSelectedTwice,
        EditFavoriteAliasClicked,
        EditFavoriteDone,
        FavoriteDetailClicked,
        FavoriteItemChecked,
        FavoriteItemUnchecked,
        FavoriteItemPressed,
        FavoriteListScrolled,
        FavoriteItemReleased,
        RequestFavorite,
        ChangeSearchTabViewPage,
        DeleteAllClicked,
        ShowMaxFavoriteDialog,

        // 17.10.24 hskim show softkey for standalone running
        UpdateSoftkey,

        BackToViaSetting,

        ShowZone3,
        HideZone3,
        RetranslateUi,
    };
    enum MenuK {
        MenuAddBookmark = 0,
        MenuEditBookmark,
        MenuDeleteAll,
        MenuDeleteSelected,
        OptionSortByName,
        OptionSortByOrder
    };
    Q_INVOKABLE BookmarkPageViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    void backToViaSetting();

    QString deleteMenuMessage(int count) const;
    QString deleteAllMenuMessage() const;

    void showDeleteAllFavoriteDialog();
    void showEditFavoriteAliasDialog(const QVariant &data);
    void updateEditFavoriteMenu(int checked);
    void deleteSelectedFavoriteItems(const QVariant &data);
    void deleteAllFavoriteItems();
    void editFavoriteAlias(const QVariant &data);
    void recoverMenuFromEdit();
    void showChangeDestinationConfirmDialog(const QString &name, const QString &address);
    void startRouteGuidance();
    void close();
    void updateBookmarkPageView();

    void updateSoftkey();
    QVariantList setupOptionMenu2(int count);

    bool mIsActivate;
    QVariantMap mStartRouteGuidanceMap;
    int mSelectedFavoriteItem;
    bool mIsEditMode;
    int mEditType; // page
    bool mIsGadgetSearchMode = false;
    bool mIsRg = false;
    bool mIsRequested = false;
    bool mIsRegistMode = false;
};

}

#endif // BOOKMARKPAGEVIEWMEDIATOR_H
