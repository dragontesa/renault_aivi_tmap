#ifndef MEDIATEVIEWCOMMAND_H
#define MEDIATEVIEWCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class MediateViewCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE MediateViewCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );
};

}

#endif // MEDIATEVIEWCOMMAND_H
