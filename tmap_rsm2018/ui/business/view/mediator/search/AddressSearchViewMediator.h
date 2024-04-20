#ifndef ADDRESSSEARCHVIEW_H
#define ADDRESSSEARCHVIEW_H

#include "mediator/AbstractViewMediator.h"
#include "KeyboardKeyInputProxy.h"

namespace SKT {

class AddressSearchViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,
        SelectZone1Title,
        RegistKeyboardInput,
        ShowKeyboard,
        HideKeyboard,
        UpdateKeyboard,
        AddressSearchRequested,
        Mediated,
        ChangeSoftKey,
        ReturnToFavorite,
        ReturnToViaSetting,
        ReturnToNaviMenu,
        CloseView,
        RequestViewList,
        ClearKeyBuffer,
        ResetCsw,
        RegistCsw,
        UpdateView,
    };

    enum SearchType {
        RoadSearch = 0,
        LotSearch
    };

    Q_INVOKABLE AddressSearchViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void onAccepted(const QVariant&);

private:
    QObject* mBar;
    QObject* mList;
    QString mLcd;
    QString mMcd;
    QString mScd;
    bool mInitialRequestProcessed = false;
    KeyboardKeyInputProxy mKeyboardProxy;
};

}

#endif // ADDRESSSEARCHVIEW_H
