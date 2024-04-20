#ifndef MANAGEVSMNAVICOMMAND_H
#define MANAGEVSMNAVICOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageVsmNaviCommand : public SimpleCommand, public TShareable<ManageVsmNaviCommand>
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageVsmNaviCommand(QObject *parent = 0);
    virtual ~ManageVsmNaviCommand();

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    void summaryRoutes(const QVariant &data);

    void getDetailRoute(int routeId);

    bool setTvas(const QVariantList &data);
    bool setTvas(const QVariantList &list, const QStringList &colors, bool switchToLocal);
};

}//SKT

#endif // MANAGEVSMNAVICOMMAND_H
