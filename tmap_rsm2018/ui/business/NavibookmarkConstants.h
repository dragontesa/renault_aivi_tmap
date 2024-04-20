#ifndef NAVIBOOKMARKCONSTANTS_H
#define NAVIBOOKMARKCONSTANTS_H

#include "Constants.h"

namespace SKT {

class NavibookmarkConstants {
public:
    enum NotificationK {
        NewNavibookSession = Constants::Navibook,
//		NavibookSessionStarted,
//		StopNavibookSession,
//		NavibookSessionStoped,
        SelectNavibookSearch,
        SelectNavibookAddress,
        RegistNavibookBookmark,

        RegistBookmarkOk
    };
};

}

#endif
