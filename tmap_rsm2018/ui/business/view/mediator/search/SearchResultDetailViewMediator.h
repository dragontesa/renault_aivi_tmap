#ifndef SEARCHRESULTDETAILVIEWMEDIATOR_H
#define SEARCHRESULTDETAILVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SearchResultDetailViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        AddFavorite,
        RemoveFromFavorite,
        QueryIsFavorite,
        CreateView,
        CloseView,
        UpdateView,
        QueryInDriveMode,
        SelectZone1Title,
        ReturnToFavoriteView,
        ReturnToRecentDestinationView,
        ReturnToRouteSettingView,
        RequestSearch,
        RequestPositionInScreenCenter,
        RequestInTheWaypoint,
        RequestViaMenuText,
        ViewSizeChanged,
        RequestAddMarker,
        RequestRemoveMarker,
        RequestAtThePOI,
        RequestRouteGuidanceState,
        ModalChanged,
        DrawRoute,
        CallPhoneNumber,
        ResetCsw,
        RegistCsw,
        ShowToast,
        RequestRegisterSendToCar,
        RequestFindSendToCar,
        RequestRemoveSendToCar,
        QueryOnlineStatus,
        ShowProgressIndicator,
        HideProgressIndicator,
        RequestAddressType,
        RemoveSearchQueue,
        RequestLang,
        ShowView,
        UpdateMapView,
        HideTabViewContent,
        RequestSetting,
    };

    Q_INVOKABLE SearchResultDetailViewMediator( QObject* viewComponent = nullptr );
    virtual ~SearchResultDetailViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void onRemove();

private:
    void addFavorite(const QVariant &data);
    void removeFromFavorite(const QVariant &);
    void close();
    QString caller() const;
    void queryInDriveMode();
    bool sendByDestination(const QVariant &data);
    void sendToVia(const QVariant &data);
    bool sendAddSearchMarker(const QVariant &data);
    bool sendRemoveSearchMarker(const QVariant &data);
    void showConfirmDialog(const QString &address);

    void closeView(const QString& viewName);
    bool fromSearchResultView() const;
    void requestAddressType();
    bool isVisible() const;
    bool isActive() const;

private:
    QVariant mDestData;
    bool mIsFavoriteUpdated;
    bool mIsRg;
    bool mIsRoadName;
    QList<int> mViewList;
};

}

#endif // SEARCHRESULTDETAILVIEWMEDIATOR_H
