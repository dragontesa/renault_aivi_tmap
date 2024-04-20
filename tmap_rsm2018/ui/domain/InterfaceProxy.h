#ifndef INTERFACEPROXY_H
#define INTERFACEPROXY_H

#include "AbstractProxy.h"

namespace SKT
{

class InterfaceProxy : public AbstractProxy
{
    Q_OBJECT

public:
    InterfaceProxy();

    void showDeleteWaypointListScreen(bool restoreVia=true);
    void insertWaypoint(int type, const QPoint &pos);
};

}//SKT

#endif // INTERFACEPROXY_H
