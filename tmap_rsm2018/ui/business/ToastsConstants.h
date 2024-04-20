#ifndef TOASTSCONSTANTS_H
#define TOASTSCONSTANTS_H

#include "Constants.h"

namespace SKT {

class ToastsConstants {
public:
    enum ToastsPositionK {
        ToastUp,
        ToastCenter,
        ToastDown
    };

    enum NotificationK {
        ShowToast = Constants::Toasts
    };
};

}

#endif
