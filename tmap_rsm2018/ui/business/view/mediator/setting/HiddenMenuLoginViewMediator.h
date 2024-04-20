/*
 * HiddenMenuLoginViewMediator.h
 *
 *  Created on: 2017. 12. 12.
 */

#ifndef UI_BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENMENULOGINVIEWMEDIATOR_H_
#define UI_BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENMENULOGINVIEWMEDIATOR_H_

#include "mediator/AbstractViewMediator.h"
#include "KeyboardKeyInputProxy.h"

namespace SKT {

class HiddenMenuLoginViewMediator: public AbstractViewMediator {
	Q_OBJECT
	Q_ENUMS( EventK )
public:
	enum EventK {
		Activated,
		Deactivated,
		CreateView,
		ConfirmPasswd,
		RegistKeyboardInput,
		ShowKeyboard,
		HideKeyboard,
		SelectSoftkey,
        ClearText
	};
	Q_INVOKABLE HiddenMenuLoginViewMediator( QObject* viewComponent = nullptr );
    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    KeyboardKeyInputProxy mKeyboardProxy;
};

} /* namespace SKT */

#endif /* UI_BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENMENULOGINVIEWMEDIATOR_H_ */
