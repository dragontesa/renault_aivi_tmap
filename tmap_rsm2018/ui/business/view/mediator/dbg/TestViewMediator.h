#ifndef TESTVIEWMEDIATOR_H
#define TESTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        GotoBusan,
        GotoSokcho,
        Korean,
        English,
        Widget,
        WidgetXL,
        WithMapData,
        NoMapData,
        Etc,
        Test,
        UserProfileChanged,
        GotoStandBy,
        GotoRunning,
        SearchTabView,
        CreateSession,
        MonkeyTestRandClick,
        MonkeyTestMapControl,
        ViewListClicked,
        MultiMapViewClicked,
        DestroyMapInCluster,
        CreateMapInCluster,
        ChangeMultiSenseMode,
        DisplayCenteralMapInClusterWindow,
        DisplaySideMapInClusterWindow,
		DisplayHudMapInClusterWindow,
        ClearFavorites,
        AddFavoritesInMaximum,
        DbusEncrypt,
        DbusEncryptMt,
        QaesEncrypt,
        QaesEncryptMt,
        GadgetIconFuel,
        GadgetIconParking,
        StopGadgetIconFuel,
        StopGadgetIconParking,
        GadgetSaveImage,        
        ScreenCapture,
        FileGpsSetPos,
        FileGpsRewind,
        FileGpsFastForward,
        AddressSearchVr,
        AddressSearchVr2,
        ShowProgressIndicator,
        ShowGadgetXXL,
        ShowGadgetXL,
        ShowGadgetL,
        ShowGadgetM,
        ShowGadgetS,
        QuitGadget,

        C_C_CS_C,
        C_C_CS_T,
        C_S_CS_C,
        C_S_CS_T,
        D_CS_C,
        D_CS_T,
        GXXL_CS_C,
        GXL_CS_C,
        GL_CS_C,
        GXXL_CS_T,
        GXL_CS_T,
        GL_CS_T,

        C_C_RLT,
        C_S_RLT,
        D_RLT,
        GXXL_RLT,
        GXL_RLT,
        GL_RLT,

        C_C_RLTT,
        C_S_RLTT,
        D_RLTT,
        GXXL_RLTT,
        GXL_RLTT,
        GL_RLTT,

        RecordTouch,
        SimPanning,
        SimPinchIn,
        SimPinchOut,

        SkyStateDawn,
        SkyStateDusk,

        ClearTouch,

        GadgetBackground,

        GpsAntennaOn,
        GpsAntennaOff,

        CrossImageVnext,

        GadgetFocus
    };
    Q_INVOKABLE TestViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool eventFilter(QObject *target, QEvent *event );
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    void testEncrypt( int event, const QVariant& data );

    void monkeyTestRandClick();
    void monkeyTestMapControl();

    void addMessage( const QString& message );

    bool mIsUnderRecordingTouch = false;

    struct TouchPoint{
        int action = 0;
        int interval = 0;
        QPointF localPos;
        QPointF windowPos;
        QPointF screenPos;
    };

    QList<TouchPoint> mTouchPoints;
    QElapsedTimer mTouchPointElapsedTimer;
    bool mIsTouchDragging = false;
    int mTouchButtonIndex = -1;
};

}

#endif // TESTVIEWMEDIATOR_H
