#include "ManageTestCommand.h"
#include "SettingDto.h"
#include "SettingProxy.h"
#include "NavigationControllerProxy.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "VsmNaviProxy.h"
#include "gpsmanagerinterface.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "TestConstants.h"
#include "HiddenSettingConstants.h"
#include "ToastsConstants.h"
#include "SettingConstants.h"
#include <stdlib.h>
#include <QSettings>

// api it takes a screenshot on genivi3
#ifdef PLATFORM_GEN3
#include <ilm/ilm_control.h>
#endif

#include "VSM_Map_Settings.h"
#include "VSM.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "LangConstants.h"


namespace SKT {

extern QString vsmIniPath;

VSM_DriveAutoConfig vsmAutoLevelDefaultConfig(int reqSpeedMode=-1)
{
    VSM_DriveAutoConfig tv;
    memset(&tv, 0, sizeof(tv));

    tv.mLowSpeedMax      = 40;
    tv.mMiddleSpeedMax   = 80;
    tv.mHighSpeedMax     = -1;

    // portrait --> AIVI
    tv.mLowSpeedViewLevel_P      = 11.5f;
    tv.mMiddleSpeedViewLevel_P   = 10.5f;
    tv.mHighSpeedViewLevel_P     =  9.5f;

    tv.mLowSpeedTiltAngle_P      = 12.0f;
    tv.mMiddleSpeedTiltAngle_P   = 18.0f;
    tv.mHighSpeedTiltAngle_P     = 23.5f;

    //  클러스터일 경우
    tv.mLowSpeedViewLevel_C = 11.5f;
    tv.mMiddleSpeedViewLevel_C = 10.5f;
    tv.mHighSpeedViewLevel_C = 9.5f;

    tv.mLowSpeedTiltAngle_C = 12.0f;
    tv.mMiddleSpeedTiltAngle_C = 18.0f;
    tv.mHighSpeedTiltAngle_C = 23.5f;

    // landscape --> PIVI or Desktop
    tv.mLowSpeedViewLevel_L      = 11.0f;
    tv.mMiddleSpeedViewLevel_L   = 10.5f;
    tv.mHighSpeedViewLevel_L     = 10.0f;

    tv.mLowSpeedTiltAngle_L      = 40.0f;
    tv.mMiddleSpeedTiltAngle_L   = 25.0f;
    tv.mHighSpeedTiltAngle_L     = 25.0f;

    tv.mTBTDistance1    = 250; // 250
    tv.mTBTDistance2    = 550; // 550
    tv.mTBTDistance3    = 1950; // 1950

    tv.mTBTDistance1Level  = 11.0f; // 11.f;
    tv.mTBTDistance2Level  = 10.5f; // 10.5f
    tv.mTBTDistance3Level  = 10.0f; // 10.f;

    tv.mTBTDistance1TiltAngle  = 14.f; // 40.f
    tv.mTBTDistance2TiltAngle  = 18.f; // 25.f
    tv.mTBTDistance3TiltAngle  = 22.f; // 25.f

    tv.mTBTDistance1_Turn   = 200;
    tv.mTBTDistance2_Turn   = 550; // 550
    tv.mTBTDistance3_Turn   = 1950; // 1950

    tv.mTBTDistance1Level_Turn  = 11.f;
    tv.mTBTDistance2Level_Turn  = 10.5f; // 10.5f
    tv.mTBTDistance3Level_Turn  = 10.0f; // 10.f

    tv.mTBTDistance1TiltAngle_Turn  = 65.f;
    tv.mTBTDistance2TiltAngle_Turn  = 25.f; // 25.f
    tv.mTBTDistance3TiltAngle_Turn  = 25.f; // 25.f

    int speedMapMode = NavigationControllerProxy::MapMode::Normal; // init default
    auto settingProxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = settingProxy->setting( SettingConstants::MapSubSpeedResponse , SettingConstants::Distance  );
    speedMapMode = setting->value().toInt(); // user setting

    if (reqSpeedMode >= 0 && reqSpeedMode < 3) // caller mode
        speedMapMode = (NavigationControllerProxy::MapMode)reqSpeedMode;

    // custom Auto Speed Level for A-IVI
    switch (speedMapMode) {
        case NavigationControllerProxy::MapMode::Normal: {
         tv.mLowSpeedViewLevel_P      = 11.5f;
         tv.mMiddleSpeedViewLevel_P   = 10.5f;
         tv.mHighSpeedViewLevel_P     =  9.5f;

         // change tilt angle - https://docs.google.com/spreadsheets/d/1WSzcxOKjnpzbY8FIfwJpGJgzOCchr3ecQLjh9OOZ_TQ
         tv.mLowSpeedTiltAngle_P      = 12.0f;
         tv.mMiddleSpeedTiltAngle_P   = 18.0f;
         tv.mHighSpeedTiltAngle_P     = 23.5f;
        }
         break;
         case NavigationControllerProxy::MapMode::Near: {
            tv.mLowSpeedViewLevel_P      = 12.f;
            tv.mMiddleSpeedViewLevel_P   = 11.f;
            tv.mHighSpeedViewLevel_P     = 10.f;

            // change tilt angle - https://docs.google.com/spreadsheets/d/1WSzcxOKjnpzbY8FIfwJpGJgzOCchr3ecQLjh9OOZ_TQ
            tv.mLowSpeedTiltAngle_P      = 10.0f;
            tv.mMiddleSpeedTiltAngle_P   = 14.0f;
            tv.mHighSpeedTiltAngle_P     = 22.0f;
        }
        break;
        case NavigationControllerProxy::MapMode::Far: {
            tv.mLowSpeedViewLevel_P      = 11.f;
            tv.mMiddleSpeedViewLevel_P   = 10.f;
            tv.mHighSpeedViewLevel_P     =  9.f;

            // change tilt angle - https://docs.google.com/spreadsheets/d/1WSzcxOKjnpzbY8FIfwJpGJgzOCchr3ecQLjh9OOZ_TQ
            tv.mLowSpeedTiltAngle_P      = 14.0f;
            tv.mMiddleSpeedTiltAngle_P   = 22.0f;
            tv.mHighSpeedTiltAngle_P     = 25.0f;
        }
        break;
    }

    return tv;
}

bool applyVsmMapAutoConfig(const QVariantMap& config)
{
    VSM_DriveAutoConfig tv = vsmAutoLevelDefaultConfig();
//    DAC Level (A)    하늘이 보이기 시작하는 각도 (B) 최대 기울일 수 있는 각도 (C)
//    9    30      25
//    9.5  29.5    23.5
//    10   29      22
//    10.5 22.75   18
//    11   23.5    14
//    11.5 22.75   12
//    12   21      10

    if (config.contains(HiddenSettingConstants::VsmAutoLevelLowLevel))
        tv.mLowSpeedViewLevel_P = config[HiddenSettingConstants::VsmAutoLevelLowLevel].toFloat();
    if (config.contains(HiddenSettingConstants::VsmAutoLevelMiddleLevel))
        tv.mMiddleSpeedViewLevel_P = config[HiddenSettingConstants::VsmAutoLevelMiddleLevel].toFloat();
    if (config.contains(HiddenSettingConstants::VsmAutoLevelHighLevel))
        tv.mHighSpeedViewLevel_P = config[HiddenSettingConstants::VsmAutoLevelHighLevel].toFloat();

    if (config.contains(HiddenSettingConstants::VsmAutoLevelLowTilt))
        tv.mLowSpeedTiltAngle_P  = config[HiddenSettingConstants::VsmAutoLevelLowTilt].toFloat();
    if (config.contains(HiddenSettingConstants::VsmAutoLevelMiddleTilt))
        tv.mMiddleSpeedTiltAngle_P  = config[HiddenSettingConstants::VsmAutoLevelMiddleTilt].toFloat();
    if (config.contains(HiddenSettingConstants::VsmAutoLevelHighTilt))
        tv.mHighSpeedTiltAngle_P  = config[HiddenSettingConstants::VsmAutoLevelHighTilt].toFloat();

    bool success = VSMNavi_SetDriveAutoConfig(tv);

    return success;
}

// defined in MapConfigDebugUiControl
extern void applyVsmMapCustomConfig();

void setVsmIniSetting(const QString& section, const QString& key, const QString& value)
{
    QFile vsmIniFile( vsmIniPath );
     // create ini file if there are not
    if (!vsmIniFile.exists()) {
       vsmIniFile.open(QFile::WriteOnly);
       vsmIniFile.close();
    }

    if (vsmIniFile.exists()) {
        QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
        if (!section.isEmpty())
           vsmIni.beginGroup(section);

        vsmIni.setValue(key, value);
    }
}

QVariant getVsmIniSetting(const QString& section, const QString& key)
{
     QFile vsmIniFile( vsmIniPath );
    if (vsmIniFile.exists()) {
        QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
        if (!section.isEmpty())
           vsmIni.beginGroup(section);

        return vsmIni.value(key);
    } 
    return QVariant();
}

ManageTestCommand::ManageTestCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

bool ManageTestCommand::execute( const mvc::INotificationPtr& noti )
{
    switch ( noti->getId() ) {
    case TestConstants::RunTest: {
        return true;
    }
    case HiddenSettingConstants::CaptureScreenshot:
     captureScreenshot();
    break;
    case HiddenSettingConstants::DoExportLogsToUSB:
         exportLog2USB();
    break;
    case HiddenSettingConstants::ChangeGpsLogUsage: {
        auto enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        //2019-10-04 sync with GpsLog & MMFBLog by sangbong.woo
        gpsservice->changeMMFeedbackLog(enabled);
        proxy->changeGpsLogUsage(enabled);
    }
    break;
    case HiddenSettingConstants::ApplyGpsTimeBias: {
        int bias = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        proxy->applyGpsTimeBias(bias);
        qDebug() << "^^^ gps time bias changed:" << bias;
    }
    break;
    case HiddenSettingConstants::ChangeGpsMapmatch: // handle dynamically connection of gps location signal for gps timestamp 2017.12.22
    {
        auto enabled = noti->getBody().toBool();
        auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        rgProxy->connectGpsLocation(enabled);
    }
    break;
#ifdef TMAP_TMC_QC_TESTING
    case HiddenSettingConstants::ChangeMapmatchingDebugText: {
        auto proxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        auto enable = noti->getBody().toBool();
        proxy->setUpdateRpDebugText(enable);
    }
    break;

    case HiddenSettingConstants::ChangeMapmatchingFeedback: {
        auto enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        gpsservice->changeMMFeedback(enabled);
        if (!enabled) {
            // turn off all mmfb
            gpsservice->changeOnlyTunnelMMFB(false);
        }
    }
    break;
    case HiddenSettingConstants::ChangeOnlyTunnelMMFB: {
        auto enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        if (enabled) {
            gpsservice->changeMMFeedback(true);
        }
        gpsservice->changeOnlyTunnelMMFB(enabled);
    }
    break;
    case HiddenSettingConstants::ChangeMMFeedbackLog: {
        auto enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        //2019-10-04 sync with GpsLog & MMFBLog by sangbong.woo
        gpsservice->changeMMFeedbackLog(enabled);
        proxy->changeGpsLogUsage(enabled);
    }
    break;
    case HiddenSettingConstants::ChangeMMFeedbackGpsMethod: {
        int value = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        gpsservice->changeMMFeedbackGpsMethod(value);
    }
    break;
    case HiddenSettingConstants::ChangeMMFeedbackAngleOnTunnel: {
        int  value = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        gpsservice->changeMMFeedbackAngleOnTunnel(value);
    }
    break;
    case HiddenSettingConstants::ChangeMMFeedbackDrOnly: {
        auto enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        gpsservice->changeMMFeedbackGpsMethod(enabled ? 2 : 1); // ensure Dbus DR only ,  -- 0: GPS(Sock), 1:GPS(Dbus), 2: DR(DBus)
        gpsservice->changeMMFeedbackAngleOnTunnel(1*enabled); // use DR instead of road line, -- 0: road line, 1:DR
    }
    break;
    case HiddenSettingConstants::ChangeMMFeedbackDrCEP: {
       auto enabled = noti->getBody().toBool();
       auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
       GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
       gpsservice->changeMMFeedbackDRCep(enabled);       
       if (enabled) {
           auto settingProxy = Facade::instance()->retrieveProxy<SettingProxy>();
           auto setting = settingProxy->setting( HiddenSettingConstants::HiddenSetting , HiddenSettingConstants::MmfbDrCepRange);
           auto method = setting->value().toFloat();
           gpsservice->changeMMFeedbackDRCepRange(method);

           setting = settingProxy->setting( HiddenSettingConstants::HiddenSetting , HiddenSettingConstants::MmfbDrCepMMRange);
           method = setting->value().toFloat();
           gpsservice->changeMMFeedbackDRCepMMRange(method);
       }
    }
    break;        
    case HiddenSettingConstants::ChangeMMFeedbackDrCEPrange: {
       auto method = noti->getBody().toInt();
       auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
       GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();       
       auto settingProxy = Facade::instance()->retrieveProxy<SettingProxy>();
       auto setting = settingProxy->setting( HiddenSettingConstants::HiddenSetting , HiddenSettingConstants::MmfbDrCep);
       if (setting->value().toBool()) {
            gpsservice->changeMMFeedbackDRCepRange(method);
       }
    }
    break;

    case HiddenSettingConstants::ChangeMMFeedbackDrCEPMMrange: {
       auto method = noti->getBody().toInt();
       auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
       GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
       auto settingProxy = Facade::instance()->retrieveProxy<SettingProxy>();
       auto setting = settingProxy->setting( HiddenSettingConstants::HiddenSetting , HiddenSettingConstants::MmfbDrCep);
       if (setting->value().toBool()) {
            gpsservice->changeMMFeedbackDRCepMMRange(method);
       }
    }
    break;

    case HiddenSettingConstants::ChangeGpsLogToKML: {
        auto proxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        GpsManagerInterface* gpsservice = proxy->getGpsServiceHandle();
        gpsservice->convertGpsLogToKML();
    }
    break;
#endif
    case HiddenSettingConstants::ChangeTnappServerIpPort: {
        auto m = noti->getBody().toMap();
        qDebug() << "^^^ tnapp server ip/port" << m[QStringLiteral("ip")].toString() << "/" << m[QStringLiteral("port")].toInt();
    }
    break;
    case HiddenSettingConstants::ChangeNddsServer: {
        int n012 = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        proxy->changeNddsServer((n012 == HiddenSettingConstants::K_Commercial));
        qDebug() << "^^^ ndds commerce server is " << ((n012==HiddenSettingConstants::K_Commercial)?"enabled:":"disabled:");
    }
    break;

    case HiddenSettingConstants::ChangeAvNextServer: {
        auto avnext = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        proxy->changeAvNextServer(avnext);
        qDebug() << "^^^ avnext server gets changed to " << avnext;
    }
    break;

    case HiddenSettingConstants::ChangeSafeHelperServerIpPort: {

    }
    break;
    case HiddenSettingConstants::ChangeTvasVersion: {
        int  ver = noti->getBody().toInt() + 6;
        auto rpProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        auto vsmProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
        rpProxy->setTvasVersion(ver);
        vsmProxy->setTvasVersion(ver);
    }
    break;
    case HiddenSettingConstants::EnableDaynightTimeset: {
         auto enabled = noti->getBody().toBool();
         auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
         proxy->enableSunstateTimeset(enabled);
    }
    break;
    case HiddenSettingConstants::ForceCrashApp:
       {
          char* null = nullptr;
          null[0] = 0x1;
       }
       break;
    case HiddenSettingConstants::ChangeVsmLevelSettingUi: {
        bool enabled = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto mapviews = proxy->getMapviewHandle();
        for (auto map : mapviews) {
            VSMMap_Setting_SetShowDebugGui(map, enabled);
        }
    }
        break;
    case HiddenSettingConstants::ApplyVsmLastDebugSetting: {
        applyVsmMapCustomConfig();
    }
        break;
    case HiddenSettingConstants::ChangeVsmServer: {
        int server = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        proxy->changeVsmServer(server);
    }
        break;
    case HiddenSettingConstants::ChangeVsmConfigId: {
        auto id = noti->getBody().toString();
        setVsmIniSetting(QStringLiteral("VsmConfig"),QStringLiteral("AppConfigId"), id);
        // it needs restart app
    }
        break;
    case HiddenSettingConstants::ChangeVsmConfigVersion: {
        auto ver = noti->getBody().toString();
        setVsmIniSetting(QStringLiteral("VsmConfig"),QStringLiteral("AppConfigVer"), ver);
        // it needs restart app
    }
        break;
    case HiddenSettingConstants::ChangeVsmAutoLevelMap: {
        auto data = noti->getBody().toMap();
        int method = data[HiddenSettingConstants::VsmAutoLevelMap].toInt();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        if (method == 0) {
            proxy->applySpeedLevel();
        }
        else {
            applyVsmMapAutoConfig(data);
        }
        // save into db
        QList<SettingDtoPtr> sets;
        QMapIterator<QString,QVariant> it(data);
        QString section = HiddenSettingConstants::HiddenVsmSetting;
        while (it.hasNext()) {
            it.next();
            auto dto = t_new_shared<SettingDto>(
                        0,
                        section,
                        it.key(),
                        it.value()
                        );
            sets.append(dto);
        }
        QVariantMap body;
        body[CommonConstants::Section] = section;
        body[CommonConstants::Settings] = QVariant::fromValue(sets);

        sendNotification(SettingConstants::SaveHiddenSetting, body);

    }
        break;
#ifdef TMAP_TMC_QC_TESTING
    case HiddenSettingConstants::ChangeVsmUseFps: {
        // set tmap fps 
        int enabled = noti->getBody().toInt();
        setVsmIniSetting(QStringLiteral("VsmTest"),QString("UseFps"), QString("%1").arg(enabled));
    }
    break;

    case HiddenSettingConstants::ChangeVsmDisable3dObjects: {
        // disable 3d objects
        auto disabled = noti->getBody().toInt();
        setVsmIniSetting(QStringLiteral("VsmTest"),QStringLiteral("Disable3dObject"), QString("%1").arg(disabled));
        if (disabled) {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto mapviews = proxy->getMapviewHandle();
            for (auto map : mapviews) {
                 VSMMap_Setting_SetEnableBuildingPattern(map, VSM_FALSE);
                 VSMMap_Setting_SetBuildingFilterMode(map, VSM_BUILDING_FILTER_L1);
                 // todo: remove landmark
            }
        }
    }
    break;
    case HiddenSettingConstants::ChangeVsmGLAntiAliasing: {
        auto ms = noti->getBody().toInt() % 4; // multi samples per pixel
        setVsmIniSetting(QStringLiteral("VsmTest"), QStringLiteral("GLAntiAliasing"), QString("%1").arg(ms) );
    }
    break;

    case HiddenSettingConstants::ChangeVsmFlickSpeedTest: {
        int enabled = noti->getBody().toInt();
        setVsmIniSetting(QStringLiteral("VsmTest"),QString("UseFst"), QString("%1").arg(enabled));
    }
    break;
#endif

    #ifdef TMAP_TMC_DEV_TESTING
    case HiddenSettingConstants::MapInClusterStartStop: {
        auto isStart = noti->getBody().toBool();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        NavigationController* controller = proxy->getControllerHandle();

        if (isStart)
           QMetaObject::invokeMethod(controller->qobject(), "startMapInCluster");
        else 
           QMetaObject::invokeMethod(controller->qobject(), "stopMapInCluster");

    }
    break;
    case HiddenSettingConstants::ChangeMapInClusterLayout: {
        int layout = noti->getBody().toInt();
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        NavigationController* controller = proxy->getControllerHandle();
        QMetaObject::invokeMethod(controller->qobject(), "changeMapInClusterLayout", Q_ARG(int,layout));
    }
    break;
    case HiddenSettingConstants::ForceToOnlineState: {
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        NavigationController* controller = proxy->getControllerHandle();
        QMetaObject::invokeMethod(controller->qobject(), "setOnline", Q_ARG(bool,true));
    }
    break;
    case HiddenSettingConstants::ChangeLanguage: {
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto value= noti->getBody().toInt();
        if ( value == 0 ) {
            proxy->changeLanguage( LangConstants::Ko );
        }
        if ( value == 1 ) {
            proxy->changeLanguage( LangConstants::En );
        }        
    }
    break;
    #endif
    default: break;
    }

    return false;
}

void ManageTestCommand::captureScreenshot()
{
    QString saveDir;
    #ifdef PLATFORM_GEN3
    saveDir =  QStringLiteral("/var/opt/bosch/navdata/log/screenshot");
    // saveDir = QStringLiteral("/home/root/navigation_kor/screenshot");
    #elif defined(PLATFORM_QNX)
    #else
    return;
    #endif

    QString prefix(QStringLiteral("screenshot_"));

    // make directory 
    QDir d(saveDir);
    if (!d.exists(saveDir)) d.mkpath(saveDir);

    // make filename
    QString saveFileName=saveDir.append(QChar('/'))+prefix+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+QStringLiteral(".bmp");
    int success = 0;

    #ifdef PLATFORM_GEN3
        #if 0
       success = system(QStringLiteral("LayerManagerControl dump screen 1 to %1").arg(saveFileName).toLatin1().constData());
        #else
        success = ilm_takeScreenshot(1,saveFileName.toLatin1().constData());
        #endif
    #elif defined(PLATFORM_QNX)
    #else
    #endif

    QVariantMap m;
    if (success == 0)
       m[CommonConstants::Message] = tr("스크린샷 완료!");
    else 
       m[CommonConstants::Message] = QString("%1[%2]").arg(tr("스크린샷 실패!")).arg(success);
    m[CommonConstants::Position] = ToastsConstants::ToastUp;
    m[CommonConstants::Duration] = 3000;
    sendNotification(ToastsConstants::ShowToast,QVariant::fromValue(m));

    // remove old file, maximum is 50 files
    QStringList filters;
    filters<<prefix+QStringLiteral("*.bmp");
    QList<QFileInfo> files = d.entryInfoList(filters,QDir::Files|QDir::NoSymLinks|QDir::Readable,QDir::Name);
    int cnt = files.count() - 70; // maximum 70 files 
    int i = 0;
    while ( i < cnt ) {
        QString oldfile = files.at(i).absoluteFilePath();
        QFile::remove(oldfile);
        i++;
    }
}


void ManageTestCommand::exportLog2USB()
{
    QString logPath; // gps log
    #ifdef PLATFORM_GEN3
    logPath =  QStringLiteral("/var/opt/bosch/navdata/log");
    #elif defined(PLATFORM_QNX)
    #else
    return;
    #endif

    // check whether files are there
    QDir logDir(logPath);
    if (!logDir.exists()) {
        qDebug() << QStringLiteral("export2usb, gps log directory not found!!");
        return;
    }

    qDebug() << QStringLiteral("export2usb, log files begin --------------");

    auto naviProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    QString mediaPath = naviProxy->mountedMediaPath(); // /dev/media/AAM_dev_sda
    if (mediaPath.isEmpty()) {
      QVariantMap m;
      m[CommonConstants::Message] = QStringLiteral("export2usb, Not found the media mounted!");
      m[CommonConstants::Position] = ToastsConstants::ToastUp;
      m[CommonConstants::Duration] = 3000;
      sendNotification(ToastsConstants::ShowToast,QVariant::fromValue(m));
      return;
    }

   // copy sub dirs under log except file_gps
    QString lxcBoundMediaPath(QStringLiteral("/media/usb_dev"));
    QString bckDirPath(lxcBoundMediaPath);
    bckDirPath.append(QStringLiteral("/ExportedLog_"));
    bckDirPath.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));

    auto cpLogs = [&](const QString& from, const QString& to) {
        QDir dir(from);
        for (auto copyFile : dir.entryList(QDir::Files)) {
            QString src = from+QChar('/')+copyFile;
            QString dst = to+QChar('/')+copyFile;
            if (!QFile::copy(src, dst))
               qWarning() << "failed to copy file to usb :" << copyFile;
        }
    };

    QStringList filters;
    filters << "sys_gps" << "app_log";

    for (auto copyDir : logDir.entryList(filters,QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString src(logPath);
        src.append(QChar('/')+copyDir);
        QString dst(bckDirPath);
        dst.append(QChar('/')+copyDir);
        // create dst dir
        QDir dir(dst);
        if (dir.mkpath(dst) == false) {
            qDebug() << "failed to create export dir:" << dst;
            break;
        }
        cpLogs(src, dst);
    }
}

} // namespace SKT
