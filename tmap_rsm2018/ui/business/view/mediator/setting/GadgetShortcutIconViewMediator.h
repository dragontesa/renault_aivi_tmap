#ifndef GADGETSHORTCUTICONVIEWMEDIATOR_H
#define GADGETSHORTCUTICONVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class GadgetShortcutIconViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        CreateView,
        SelectZone1Title,
        ModalChanged,
        CloseView,
        RequestViewList,
        TouchGadgetShotcutIcon,
        RequestLang,
        ResetCsw,
        RegistCsw,
    };


    Q_INVOKABLE GadgetShortcutIconViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool mOnline;
};

}

#endif // GADGETSHORTCUTICONVIEWMEDIATOR_H
