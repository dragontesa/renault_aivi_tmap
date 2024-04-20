#ifndef TESTCONSTANTS_H
#define TESTCONSTANTS_H

#include "Constants.h"

namespace SKT {

class TestConstants {
    Q_GADGET
public:
    enum NotificationK {
        RunTest = Constants::Test,
        AddMessage,
        UpdateMapTheme,
        LogSdiData
    };
    enum TestMouseAction {
        MouseActionClick,
        MouseActionPress,
        MouseActionRelease,
        MouseActionMove,
        MouseActionDoubleClick,
        TouchActionPress,
        TouchActionRelease,
        TouchActionMove
    };
    Q_ENUM(TestMouseAction)

    enum TestKeypad {
       KeypadClicked = 99999
    };
    Q_ENUM(TestKeypad)

    TestConstants();

    enum MapType {
        CC,
        CS,
        D,
        GXXL,
        GXL,
        GX,
        GL
    };

    static const float C_C_CS_C_DEF;
    static const float C_S_CS_C_DEF;
    static const float D_CS_C_DEF;
    static const float GXXL_CS_C_DEF;
    static const float GXL_CS_C_DEF;
    static const float GL_CS_C_DEF;
    static const float C_C_RLT_DEF;
    static const float C_S_RLT_DEF;
    static const float D_RLT_DEF;
    static const float GXXL_RLT_DEF;
    static const float GXL_RLT_DEF;
    static const float GL_RLT_DEF;
    static const float C_C_RLTT_DEF;
    static const float C_S_RLTT_DEF;
    static const float D_RLTT_DEF;
    static const float GXXL_RLTT_DEF;
    static const float GXL_RLTT_DEF;
    static const float GL_RLTT_DEF;

    static const QString C_C_CS_C;
    static const QString C_C_CS_T;
    static const QString C_S_CS_C;
    static const QString C_S_CS_T;

    static const QString D_CS_C;
    static const QString D_CS_T;
    static const QString GXXL_CS_C;
    static const QString GXL_CS_C;
    static const QString GL_CS_C;
    static const QString GXXL_CS_T;
    static const QString GXL_CS_T;
    static const QString GL_CS_T;

    static const QString C_C_RLT;
    static const QString C_S_RLT;
    static const QString D_RLT;
    static const QString GXXL_RLT;
    static const QString GXL_RLT;
    static const QString GL_RLT;

    static const QString C_C_RLTT;
    static const QString C_S_RLTT;
    static const QString D_RLTT;
    static const QString GXXL_RLTT;
    static const QString GXL_RLTT;
    static const QString GL_RLTT;

    static const QString C_C_RLOR;
    static const QString C_S_RLOR;
    static const QString D_RLOR;
    static const QString GXXL_RLOR;
    static const QString GXL_RLOR;
    static const QString GL_RLOR;
};

}

#endif
