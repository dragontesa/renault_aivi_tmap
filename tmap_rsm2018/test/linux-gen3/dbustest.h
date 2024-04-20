#ifndef DBUSTEST_H
#define DBUSTEST_H

#include <QObject>

#include "navigationextserviceinterface.h"
#include "guicontrolinterface.h"

namespace SKT {

class PlatformControllerGen3;
class DbusTest : public QObject
{
    Q_OBJECT
public:
    DbusTest(PlatformControllerGen3* controller);
    ~DbusTest();

    void init();
    void cleanUp();
    void runTest();

private slots:
    // from GuiControl
    void onKeyInfoForwarding(GCKeyInfo data);

    // from NavigationExt
    void onNaviSigNaviAnnouncementStatus(uint u32NaviAnnouncementStatus);
    void onNaviSignalSpeedValue(ushort u8SpeedValue);
    void onPlayBeepStatus(uchar uBeepStatus);
    void onPlayMelodyStatus(uchar uMelodyStatus);
    void onResetToFactorySettings();

private:
    int writeToSM();
    void readFromSM();
    static int writeData(const char*data, qint64 len);

private:
    PlatformControllerGen3* m_controller;
    BoschGuiCmAiGuicontrolInterface *m_guiControl;
    OrgBoschCmNavigationNavigationExtServiceInterface *m_navigationExt;
};

}

#endif // DBUSTEST_H
