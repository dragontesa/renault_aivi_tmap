#ifndef BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENBOSCHSETTINGVIEWMEDIATOR_H_
#define BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENBOSCHSETTINGVIEWMEDIATOR_H_
#include "mediator/AbstractViewMediator.h"

namespace SKT {

class HiddenBoschSettingViewMediator: public AbstractViewMediator
{
    Q_OBJECT

    public:
	enum EventK {
		Activated,
		Deactivated,
		RequestHiddenSetting,
		SaveHiddenSetting,
		ApplyKeySetting,
		RegistKeyboardInput,
	    ShowKeyboard,
		HideKeyboard,
        ResetCsw,
        RegistCsw,
	};
	Q_ENUM( EventK )



    Q_INVOKABLE HiddenBoschSettingViewMediator( QObject* viewComponent= nullptr );

    QList<int> listNotificationInterests();
    
    protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void applyKeySetting(const QVariant& data);
    void saveSetting(const QVariant& data);
};
}

#endif // BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENBOSCHSETTINGVIEWMEDIATOR_H_
