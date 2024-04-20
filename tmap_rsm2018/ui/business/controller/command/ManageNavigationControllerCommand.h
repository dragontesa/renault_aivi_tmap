#ifndef MANGENAVIGATIONCONTROLLERCOMMAND_H
#define MANGENAVIGATIONCONTROLLERCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class MangeNavigationControllerCommand : public SimpleCommand, public TShareable<MangeNavigationControllerCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE MangeNavigationControllerCommand(QObject *parent = 0);

    ~MangeNavigationControllerCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    bool onResponseCommand( const mvc::INotificationPtr& note );

    void showSettingVolume(const QVariant& v);
    void callZoneApi(const QVariant& v);
    void playVolumeSound();
};

}

#endif // MANGENAVIGATIONCONTROLLERCOMMAND_H
