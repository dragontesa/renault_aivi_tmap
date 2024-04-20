#ifndef BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENDIAGSETTINGVIEWMEDIATOR_H_

#include "mediator/AbstractViewMediator.h"

namespace SKT {
class HiddenDiagSettingViewMediator : public AbstractViewMediator
{
	Q_OBJECT
	Q_ENUMS( EventK )
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
		ForceCrashApp,
        DisplayCenteralMapInClusterInWindow,
        DisplaySideMapInClusterInWindow,
		DisplayHudMapInClusterInWindow,
        DeleteFile,
        GetFileSize,
        DisplayGadgetShortcutIcon,
        OutputSdiLog
	};


    Q_INVOKABLE HiddenDiagSettingViewMediator( QObject* viewComponent= nullptr );

    QList<int> listNotificationInterests();
    
    protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void applyKeySetting(const QVariant& data);
    void saveSetting(const QVariant& data);
    QString sizeHuman(double size);
    bool removeDir(const QString &dirName);
    void deletFile(const QVariant& data);
    void getFileSize(const QVariant& data);
    double getDirSize(const QString& dirName);
};

} // namespace SKT
#endif //BUSINESS_VIEW_MEDIATOR_SETTING_HIDDENDIAGSETTINGVIEWMEDIATOR_H_
