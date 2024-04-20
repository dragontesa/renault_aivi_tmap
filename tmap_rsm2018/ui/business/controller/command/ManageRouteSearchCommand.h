#ifndef MANAGEROUTESEARCHCOMMAND_H
#define MANAGEROUTESEARCHCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageRouteSearchCommand : public SimpleCommand, public TShareable<ManageRouteSearchCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageRouteSearchCommand(QObject *parent = 0);

    ~ManageRouteSearchCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    bool onResponseCommand( const mvc::INotificationPtr& note );
    bool onRerouteResponseCommand( const mvc::INotificationPtr& note );
    void prepareRouteCandidate();
    void saveVia();
    void changeRouteOption(const QVariant &);
    void sendRequestCommandResponsed(const QVariantMap& reqCommand);
};

}

#endif // MANAGEROUTESEARCHCOMMAND_H
