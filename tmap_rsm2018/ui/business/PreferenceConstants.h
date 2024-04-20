#ifndef PREFERENCECONSTANTS_H
#define PREFERENCECONSTANTS_H

#include "Constants.h"

namespace SKT {

class PreferenceConstants {
public:
    enum NotificationK {
        AddPreferences = Constants::Preference,
        RemovePreferences,
        RequestPreference,
        ApplyPreference,
        SavePreference,
        ChangedPreference
    };

    static const QString Rg;
    static const QString Test;
};

}

#endif
