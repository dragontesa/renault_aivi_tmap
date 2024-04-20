#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT Constants
{
    Q_GADGET
public:
    Constants();

    enum NotificationK {
        // default
        None = 0,
        Startup,
        MediateView,
        RemoveView,

        // group
        Rg = 1000,
        Route = 2000,
        Search = 3000,
        Navibook = 4000,
        Map = 5000,
        Zone = 6000,
        Simulator = 7000,
        Setting = 8000,
        Keyboard = 9000,
        Preference = 10000,
        Toasts = 11000,
        NaviController = 12000,
        Sound = 13000,
        Test = 14000,
        Address = 15000,
        NaviProperty = 16000,
        Util = 17000,
        UpdateManager = 19000,
		Application = 20000,
        MultiSense = 21000,
        Interface = 22000,

        // common
        CreateView = 90000,
        ShowView,
        HideView,
        CloseView
    };
    Q_ENUM( NotificationK )

    static QString enumToString( const char *enumerator, int value );
    static int stringToEnum( const char *enumerator, const QString& value );
};

}

#endif // CONSTANTS_H
