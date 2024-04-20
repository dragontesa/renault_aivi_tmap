#ifndef TESTBOOKMARKPAGEVIEWMEDIATOR_H
#define TESTBOOKMARKPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestBookmarkPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        ResetCsw,
        RegistCsw,
        DeleteSelectedFavoriteItems,
        CreateView,
        BackwardClicked,
        SelectZone1TextInput,
        ActivatedForNavi,
        CloseView,
        EditFavoriteClicked,
        SelectZone1Title,
        AddVia,
        QueryInDriveMode,
        EditFavoriteAliasClicked,
        EditFavoriteAlias,
        EditFavoriteDone,
        DeleteAllFavoriteItems,
        ShowKeyboard,
        HideKeyboard,
        FavoriteDetailClicked,
        FavoriteItemChecked,
        FavoriteItemUnchecked,
        RequestFavorite,
        ChangeSearchTabViewPage,
        ModalChanged,

        // 17.10.24 hskim show softkey for standalone running
        SelectSoftkey
    };
    Q_INVOKABLE TestBookmarkPageViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    QString deleteMenuMessage(int count) const;
    QString deleteAllMenuMessage() const;

    void showEditFavoriteAliasDialog(const QVariant &data);
    void updateEditFavoriteMenu(int checked);
    void deleteSelectedFavoriteItems(const QVariant &data);
    void deleteAllFavoriteItems();
    void editFavoriteAlias(const QVariant &data);
    void recoverMenuFromEdit();
    void showChangeDestinationConfirmDialog(const QString &address);
    void startRouteGuidance();
    void close();

    bool mIsActivate;
    QVariantMap mStartRouteGuidanceMap;
    int mSelectedFavoriteItem;
    bool mIsEditMode;
    int mEditType; // page
};

}

#endif // TESTBOOKMARKPAGEVIEWMEDIATOR_H
