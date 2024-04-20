#ifndef MANAGEINTERFACECOMMAND_H
#define MANAGEINTERFACECOMMAND_H

#include "SimpleCommand.h"

namespace SKT
{

class ManageInterfaceCommand : public SimpleCommand
{
    Q_OBJECT

public:
    Q_INVOKABLE ManageInterfaceCommand(QObject *parent=nullptr);

    bool execute(const mvc::INotificationPtr &note);

private:
    void showWaypointEditView();
};

}

#endif // MANAGEINTERFACECOMMAND_H
