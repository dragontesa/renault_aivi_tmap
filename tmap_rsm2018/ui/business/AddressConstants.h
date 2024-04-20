#ifndef ADDRESSCONSTANTS_H
#define ADDRESSCONSTANTS_H

#include "Constants.h"

namespace SKT {

class AddressConstants {
public:
    enum NotificationK {
        RequestAddress = Constants::Address,
        ResponseAddress,
    };
};

}

#endif
