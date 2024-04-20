#ifndef ZONECONSTANTS_H
#define ZONECONSTANTS_H

#include "Constants.h"
#include "CommonConstants.h"
#include <QString>
#include <QObject>

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT ZoneConstants : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString Function                                 MEMBER Function                             CONSTANT)
    Q_PROPERTY(QString Menu                                     MEMBER Menu                                 CONSTANT)
    Q_PROPERTY(QString Menus2                                   MEMBER Menus2                               CONSTANT)
    Q_PROPERTY(QString OptionMenu                               MEMBER OptionMenu                           CONSTANT)
    Q_PROPERTY(QString OptionMenu2                              MEMBER OptionMenu2                          CONSTANT)
    Q_PROPERTY(QString Index                                    MEMBER Index                                CONSTANT)
    Q_PROPERTY(QString Back                                     MEMBER Back                                 CONSTANT)
    Q_PROPERTY(QString Option                                   MEMBER Option                               CONSTANT)
    Q_PROPERTY(QString Navi                                     MEMBER Navi                                 CONSTANT)


public:
    enum NotificationK {
        ShowOptionMenu = Constants::Zone,
        DoSoftKeyFunction, // will be deprecated (use DoSoftkeyIndexMenuFunction)
        DoSoftkeyIndexMenuFunction,
        DoSoftkeyOption, // will be deprecated (use DoSoftkeyIndexMenuOption)
        DoSoftkeyOptionReleased,
        DoSoftkeyIndexMenuOption,
        DoSoftkeyIndexMenuOptionReleased,
        RegistZone1,
        RegistZone3,
        RegistSoftkey,
        ShowSoftKeyBackground,
        SelectZone1TextInput,
        FocusZone1TextInput,
        FocusActivatedZone1TextInput,
        SelectZone1Title,
        SelectSoftkeyMenuMode, // will be deprecated (use SelectSoftkeyIndexMenuMode)
        SelectSoftkeyIndexMenuMode,
        SelectSoftkeyMenuWithTabMode, // will be deprecated (use SelectSoftkeyIndexMenuWithTabMode)
        SelectSoftkeyIndexMenuWithTabMode,
        SelectSoftkeyNaviMode,
        SelectSoftkeyNormalDriveMode,
        SelectSoftkeyToggleMode, // will be deprecated (use SelectSoftkeyToggleIndexMode)
        SelectSoftkeyToggleIndexMode,
        SelectSoftkeySimulMode,
        UpdateSoftkeyAddress,
        EditSoftkeyMenu, // will be deprecated (use EditSoftkeyIndexMenu)
        EditSoftkeyIndexMenu,
        UpdateSoftkeyRouteGuide,
        UpdatePoi,
        ShowRecentTileGridView,
        IgnoreRecentTileGridView, // SwitchToApp으로 전환 시 최근목적지그리드뷰가 문제가 되는 경우 사용.
        UpdateModal,
        CswOptionTriggered,
        CswBackTriggered,
        CswBackAccepted,
        FaderClicked,
        BlockSoftkeyMenu,
        BlockNaviMenu,
        ShowZone3,
        HideZone3,
        UpdateSearchResultCount,
        UpdateZone1,
        PrintBuffersize,
        UpdateSearchText,
        RequestZone1Text,
        ReplyToZone1Text,
    };

    enum Zone3ShowType {
        Immediately = 0,
        Animation
    };

    ZoneConstants();

    static ZoneConstants* instance() {
        static ZoneConstants Instance;
        return &Instance;
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new ZoneConstants;
    }


    static const QString Function;
    static const QString Menu;
    static const QString Menus2;
    static const QString OptionMenu;
    static const QString OptionMenu2;
    static const QString Index;
    static const QString Back;
    static const QString Option;
    static const QString Navi;    

};

}

#endif
