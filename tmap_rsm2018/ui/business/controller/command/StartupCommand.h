#ifndef STARTUPCOMMAND_H
#define STARTUPCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class StartupCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE StartupCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );
};

}

#endif // STARTUPCOMMAND_H
