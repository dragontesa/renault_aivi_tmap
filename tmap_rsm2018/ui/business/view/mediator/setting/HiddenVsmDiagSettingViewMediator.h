#ifndef BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENVSMDIAGSETTINGVIEWMEDIATOR_H_
#define BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENVSMDIAGSETTINGVIEWMEDIATOR_H_
#include "mediator/AbstractViewMediator.h"
#include "KeyboardKeyInputProxy.h"

namespace SKT {
class HiddenVsmDiagSettingViewMediator: public AbstractViewMediator
{
	Q_OBJECT
	Q_ENUMS( EventK )
public:
	enum EventK {
		Activated,
		Deactivated,
		RequestVsmHiddenSetting,
		SaveVsmHiddenSetting,
		ApplyKeySetting,
		RegistKeyboardInput,
	    ShowKeyboard,
		HideKeyboard,
        SetText,
	};

    Q_INVOKABLE HiddenVsmDiagSettingViewMediator( QObject* viewComponent= nullptr );

    QList<int> listNotificationInterests();

    protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void applyKeySetting(const QVariant& data);
    void saveSetting(const QVariant& data);

private:
    KeyboardKeyInputProxy mKeyboardProxy;
};

} // namespace SKT

#endif // BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENVSMDIAGSETTINGVIEWMEDIATOR_H_
