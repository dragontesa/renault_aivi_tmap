#ifndef MANAGEROUTEGUIDANCECOMMAND_H
#define MANAGEROUTEGUIDANCECOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageRouteGuidanceCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageRouteGuidanceCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );

private:
    void playSound(int voiceId);
    void playSoundTmapStartRouteGuidance();
};

}

#endif // MANAGEROUTEGUIDANCECOMMAND_H
