#ifndef MANAGERGCOMMAND_H
#define MANAGERGCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageSimulatorCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageSimulatorCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );
};

}

#endif // MANAGERGCOMMAND_H
