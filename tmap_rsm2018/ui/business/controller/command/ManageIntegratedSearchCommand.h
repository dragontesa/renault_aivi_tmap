#ifndef MANAGEINTEGRATEDSEARCHCOMMAND_H
#define MANAGEINTEGRATEDSEARCHCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageIntegratedSearchCommand : public SimpleCommand, public TShareable<ManageIntegratedSearchCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageIntegratedSearchCommand(QObject *parent = 0);

    ~ManageIntegratedSearchCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    bool onResponseCommand( const mvc::INotificationPtr& note );
    void resultCache(const QVariantMap &data, bool always=false);
    void resultFavor(const QVariantMap &data);
    void resultRcntDest(const QVariantMap &data);

    void requestGadgetSearchMode();
    void setGadgetSearchMode(bool b);
    void setCurrentGpsTime(QDateTime currentTime);
    void requestGpsTime();

};

}

#endif // MANAGEINTEGRATEDSEARCHCOMMAND_H
