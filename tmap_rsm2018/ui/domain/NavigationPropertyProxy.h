#ifndef NAVIGATIONPROPERTYPROXY_H
#define NAVIGATIONPROPERTYPROXY_H

#include "AbstractProxy.h"
#include "../common/navigationproperty.h"

namespace SKT {

T_SHAREABLE( NavigationPropertyProxy );
T_SHAREABLE( RequestCommand );

class NavigationPropertyProxy : public AbstractProxy
{
    Q_OBJECT
public:
    NavigationPropertyProxy(QObject* property);

    void setNavigationProperty(const RequestCommandPtr& req);

private:
    NavigationProperty* mProperty;
};

}

#endif // NAVIGATIONPROPERTYPROXY_H
