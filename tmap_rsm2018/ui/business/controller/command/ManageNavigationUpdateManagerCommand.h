#ifndef MANGENAVIGATIONUPDATEMANAGERCOMMAND_H
#define MANGENAVIGATIONUPDATEMANAGERCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class MangeNavigationUpdateManagerCommand : public SimpleCommand, public TShareable<MangeNavigationUpdateManagerCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE MangeNavigationUpdateManagerCommand(QObject *parent = 0);

    ~MangeNavigationUpdateManagerCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    bool onResponseCommand( const mvc::INotificationPtr& note );
};

}

#endif // MANGENAVIGATIONUPDATEMANAGERCOMMAND_H
