#ifndef RECENTDESTINATIONVIEWMEDIATOR_H
#define RECENTDESTINATIONVIEWMEDIATOR_H

#include "RecentDestinationPageViewMediator.h"

namespace SKT {

class RecentDestinationViewMediator : public RecentDestinationPageViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    Q_INVOKABLE RecentDestinationViewMediator( QObject* viewComponent = nullptr );
};

}

#endif // RECENTDESTINATIONVIEWMEDIATOR_H
