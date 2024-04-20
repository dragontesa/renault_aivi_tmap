#ifndef RECENTSEARCHPAGEVIEWMEDIATOR_H
#define RECENTSEARCHPAGEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class RecentSearchPageViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        BackwardClicked,
        SelectZone1TextInput,
        FocusZone1TextInput,
        ActivatedForNavi,
        CloseView,
        HideKeyboard,
        CreateView,
        FocusCsw,
        ResetCsw,
        RegistCsw,
        RequestRecentSearch,
        ChangeSearchTabViewPage,
        RecentSearchTabActivated,
        ShowKeyboard,
        PulldownKeyboard,
        SelectSoftkey,
        QueryInDriveMode,
        RequestViewList,
        UpdateView,
    };
    Q_INVOKABLE RecentSearchPageViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    void startRouteGuidance();
    void close();
    void pulldownKeyboard();
    void pullupKeyboard();

    bool handleNotification( const mvc::INotificationPtr& note );

    bool isVisible() const;
    bool isActive() const;

private:
    QVariantMap mStartRouteGuidanceMap;
    bool mIsGadgetSearchMode = false;
    QString mText;
};

}

#endif // RECENTSEARCHPAGEVIEWMEDIATOR_H
