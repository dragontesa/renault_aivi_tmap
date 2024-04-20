#ifndef NAVIGATIONPROPERTYCONSTANTS_H
#define NAVIGATIONPROPERTYCONSTANTS_H

#include "Constants.h"

namespace SKT {

class NavigationPropertyConstants {
public:
    enum NotificationK {
        setNavigationProperty = Constants::NaviProperty,
    };

    enum HmiState {
        HmiStateDrive = 2,
        HmiStateSleep = 1
    };
};

}

#endif // NAVIGATIONPROPERTYCONSTANTS_H
