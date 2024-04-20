#ifndef TEXTSEARCHVIEWMEDIATOR_H
#define TEXTSEARCHVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TextSearchViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        CreateView,
        CloseView,
        SelectZone1TextInput,
        FocusZone1TextInput,
        RegistKeyboardInput,
        ShowKeyboard,
        HideKeyboard,
        RequestSearch,
        ReturnToFavorite,
        ReturnToViaSetting,
        ReturnToNaviMenu,
        RouteSettingUpdateView,
        ResetCsw,
        RegistCsw,
        RequestAddressType,
        ChangeSearchTabViewPage,
        RequestRecentSearch,
        ShowProgressIndicator,
        HideProgressIndicator,
        RemoveSearchQueue,
        RequestViewList,
        HideView,
        PulldownKeyboard,
        RequestLang,
        UpdateSearchResultCount,        
        ShowToast,
        UpdateView,
        CloseAllViews,
        QueryInDriveMode,
        Deactivated,
        QueryOnlineStatus,
        RequestRouteGuidanceState,
        BackClicked,
    };
    Q_INVOKABLE TextSearchViewMediator( QObject* viewComponent = nullptr );


    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

    bool SearchBySisulNamePrediction(QVariantMap body);

    void goBack();
    void sendToVia(const QVariant &data);
    bool sendByDestination(const QVariant &data);
    void updateSearchResultCount(const QString& text, int searchCount, bool show = false);
    void pulldownKeyboard();
    void pullupKeyboard();
    void queryInDriveMode();
    void showConfirmDialog(const QString &address);

    bool isVisible() const;
    bool isActive() const;

private:
    bool isCallerBookmark() const;
    bool isCallerViaSetting() const;

private:
    QString mText;
    int mUpdateTextCount = 0;
    int mSearchResultCount;
    QVariant mDestData;
    bool mIsRg;
    bool mUpdateZone1 = false;
    bool mIsPulldown = false;
};

}
#endif // TEXTSEARCHVIEWMEDIATOR_H
