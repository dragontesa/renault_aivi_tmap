#ifndef MANAGETESTCOMMAND_H
#define MANAGETESTCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageTestCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageTestCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );
    void captureScreenshot();
    void exportLog2USB();
};

}

#endif // MANAGETESTCOMMAND_H
