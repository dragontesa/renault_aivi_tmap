#include "TMacroCommand.h"

using namespace SKT;
using namespace SKT::mvc;

TMacroCommand::TMacroCommand()
{
}

TMacroCommand::~TMacroCommand()
{
}

void TMacroCommand::addSubCommand( const QMetaObject* commandClassRef )
{
    mSubCommands.append(commandClassRef);
}

bool TMacroCommand::execute( const INotificationPtr& note )
{
    bool ret = false;

    QMutableListIterator<const QMetaObject*> it( mSubCommands );
    while ( it.hasNext() ) {
        auto& mo = it.next();
        auto commandInstance = (ICommand*)mo->newInstance();
        auto res = commandInstance->execute( note );
        if ( res ) {
            ret = true;
        }

        it.remove();
    }

    return ret;
}
