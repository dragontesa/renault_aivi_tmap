#ifndef INTERFACECONSTANTS_H
#define INTERFACECONSTANTS_H

#include "Constants.h"

namespace SKT
{

class InterfaceConstants {
    Q_GADGET

public:
    enum NotificationK {
        ShowDeleteWaypointListScreen = Constants::Interface,
        InsertWaypoint,
        InsertWaypointFromSelectInMap,
        ReplaceWaypoint,
        ApplyWaypointListChange
    };
};

}//SKT

#endif // INTERFACECONSTANTS_H
