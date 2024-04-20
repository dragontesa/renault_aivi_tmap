#ifndef KEYBOARDVIEWMEDIATOR_H
#define KEYBOARDVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class KeyboardViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        SelectZone1Title,
        SelectZone1TextInput,
        FocusZone1TextInput,
        ProcessKeyboard,
        ResetCsw,
        RegistCsw,
        RemoveCsw,
    };

    Q_INVOKABLE KeyboardViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void loadSwKeyboardIfNeeded();
    void processKeyboard(const QVariant& v);
    void applyPreference(const QVariant& v);
    void changedPreference(const QVariant &v);
    void loadSwKeyboard();

private:
    QString mOwner;
    bool mSwKeyboardLoad = false;
};

}

#endif // KEYBOARDVIEWMEDIATOR_H
