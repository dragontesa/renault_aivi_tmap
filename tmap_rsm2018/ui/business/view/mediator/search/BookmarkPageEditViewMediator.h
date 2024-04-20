#ifndef BOOKMARKPAGEEDITVIEWMEDIATOR_H
#define BOOKMARKPAGEEDITVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "KeyboardKeyInputProxy.h"

namespace SKT
{

class BookmarkPageEditViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )

public:
    enum EventK {
        ShowKeyboard,
        HideKeyboard,
        EditFavoriteAlias,
        ShowDeleteAllPopup,
        ModalChanged,
        DeleteAllFavoriteItems,
        UncheckAllFavorite,
        AboutToClose,
        ClearText,
        CancelKeylPressed,
        RegistCsw,
    };

    Q_INVOKABLE BookmarkPageEditViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests() Q_DECL_OVERRIDE;

protected:
    bool onEvent(int event, const QVariant& data) Q_DECL_OVERRIDE;

private:
    bool handleNotification(const mvc::INotificationPtr& note) Q_DECL_OVERRIDE;

    void close();
    void onAccepted(const QVariant& v);
    void onCanceled(const QVariant& v);
    void showDeleteAllPopup();
    void resetKeyboard();

    KeyboardKeyInputProxy mKeyboardProxy;
};

}

#endif // BOOKMARKPAGEEDITVIEWMEDIATOR_H
