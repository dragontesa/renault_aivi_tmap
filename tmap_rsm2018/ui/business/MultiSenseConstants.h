#ifndef MULTISENSECONSTANTS_H
#define MULTISENSECONSTANTS_H

#include "Constants.h"

namespace SKT {

class MultiSenseConstants {
    Q_GADGET
public:
    enum NotificationK {
        RequestColorState = Constants::MultiSense,
        ReplyToColorState,
        ChangedColorState
    };
    Q_ENUM( NotificationK )

    // static const QString
};

}

#endif
