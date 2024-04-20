#ifndef MANAGENAVIGATIONPROPERTYCOMMAND_H
#define MANAGENAVIGATIONPROPERTYCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageNavigationPropertyCommand : public SimpleCommand, public TShareable<ManageNavigationPropertyCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageNavigationPropertyCommand(QObject *parent = 0);

    ~ManageNavigationPropertyCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    bool onResponseCommand( const mvc::INotificationPtr& note );
};

}

#endif // MANAGENAVIGATIONPROPERTYCOMMAND_H
