#include "RouteSearchMediator.h"

#include <QDir>

#include "PreferenceConstants.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "SearchConstants.h"
#include "rprequest.h"
#include "../environment.h"
#include "SettingConstants.h"

#include "../environment.h"

#include "ApplicationConstants.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "MapConstants.h"
#include "mediator/app/MapViewMode.h"
#include "StringHelper.h"
#include "ViewRegistry.h"
#include "NavibookmarkConstants.h"
#include "SimulatorConstants.h"
#include "KeyboardConstants.h"
#include "VsmNaviProxy.h"
#include "RouteSearchProxy.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PlatformHelper.h"
#include "../app/SystemPopupMediator.h"

#define TvasFileExt     QStringLiteral(".db")
#define TvasBackupExt   QStringLiteral(".dbak")
#define RmCommand       QStringLiteral("rm")
#define MvCommand       QStringLiteral("mv");
#define CommandKey      QStringLiteral("Command")
#define TvasFileNameKey QStringLiteral("TvasFileName")
#define RouteFileName   QStringLiteral("tvas_route.db")


namespace SKT
{

RouteSearchMediator::RouteSearchMediator()
    : AbstractMediator()
    , mIsInDriving(false)
    , mIsTvasBackup(true)
    , mIsRerouteForCalcDone(false)
    , mRequestDriveMode(false)
    , mIsRg(false)
    , mapView(nullptr)
{
    mTimer.setInterval(10*1000);
    mTimer.setSingleShot(true);

    connect(&mTimer, &QTimer::timeout, [&](){
        if (!mIsRerouteForCalcDone) {
            // local 경로 탐색시에만 재탐색 요청을 한다.
            if (!mIsServerRequested) {
                if (mMapViewMode == MapViewMode::Navi) {
                    QVariantMap m;
                    m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce); // RequestReRouteByBreakaway
                    m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_RE_USER_DES);
                    sendNotification( RouteConstants::RequestReroute, m );
                } else {
                    mTimer.start();
                    return;
                }
            }
            mIsRerouteForCalcDone = true;
        }
    });
}

QList<int> RouteSearchMediator::listNotificationInterests()
{
    return QList<int> {
        RgConstants::PrepareStartRouteGuidance,
        RgConstants::StartRouteGuidance,
        RgConstants::StartRouteGuidanceContinuously,
        RgConstants::StopRouteGuidance,
        RgConstants::SwitchToLocal,
        RouteConstants::ClearRouteDirectory,
        RouteConstants::CloseRouteCandidateView,
        RouteConstants::RequestReroute,
        SearchConstants::RequestCommand,
        RouteConstants::RequestRouteSummaryDetail,
        RouteConstants::RestoreTvas,
        RouteConstants::ShowErrorDialog,
        SearchConstants::ResponseRegisterSendToCar,
        SearchConstants::ResponseFindSendToCar,
        SearchConstants::ResponseRemoveSendToCar,
        MapConstants::ApplyMapViewMode,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        SettingConstants::ApplySetting,
        SettingConstants::SavedSetting,
        SettingConstants::ChangedSetting,
        MapConstants::RegistGlobalMapViewRenderer,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        PreferenceConstants::ApplyPreference,
        NavigationControllerConstants::NavigationAppeared,
        RouteConstants::RequestRouteFailed
    };
}

bool RouteSearchMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case RouteConstants::RequestRouteFailed:
        // VR에서 Request한 경우에는 성공에 관계 없이 미리 mIsInDriving을 설정한다.
        // VR화면에서 내비 화면으로 전환하면 서 CancelRoute에 의해 경유지 목록이 clear되는 이슈가 있음.
        // 19.11.25 ymhong.
        mIsInDriving = false;
        break;

    case RouteConstants::ShowErrorDialog: {
            mDialogData = noti->getBody().toMap();
            QTimer::singleShot(1, this, [this](){
                sendNotification(Constants::CreateView, mDialogData);
            });
        }
        break;

        case PreferenceConstants::ApplyPreference:
        {
            auto m = noti->getBody().toMap();
            auto req = m["request"].toMap();
            auto section = req[CommonConstants::Section].toString();
            if (section == RgConstants::PastPos) {
                auto result = m["response"].value<QMap<QString,PreferenceVo>>();
                auto sessionId = result[RgConstants::PastSessionId].value().toString();
                auto x = result[RgConstants::PastPosX].value().toInt();
                auto y = result[RgConstants::PastPosY].value().toInt();
                auto yyyyMMddhhmmss = result[RgConstants::PastDate].value().toString();
                auto date = QDateTime::fromString(yyyyMMddhhmmss, "yyyyMMddhhmmss");
                auto rpProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                rpProxy->saveLastGoalData(sessionId, date, QPoint{x, y});
            }
        }
        break;
    case RouteConstants::ClearRouteDirectory:
        if (mIsTvasBackup) {
            backup();
            mIsTvasBackup = false;
        }
        if (mIsInDriving) {
            deleteTvasFilesWithoutRouteFile();
        } else {
            deleteTvasFiles();
        }
        break;

    case RouteConstants::CloseRouteCandidateView:
        if (!mIsInDriving) {
            qDebug() << "[OTP] CloseRouteCandidateView.";

            sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                 {"preferencesName",PreferenceConstants::Rg},
                 {CommonConstants::Section,QStringLiteral("Termination")},
                 {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
                 {CommonConstants::Value,true}
             });
            sendNotification(RouteConstants::CancelRoute, QVariantMap{{"clear",true}});
            sendNotification(RouteConstants::ClearVia);

            deleteTvasFiles();
            deleteBackupFiles();
        } else {
            auto m = noti->getBody().toMap();
            if (!m.isEmpty()) {
                if (m.contains("restore") && m["restore"].toBool()) {
                    // 주행 중 경유지 선택 화면을 뛰어 경로 탐색 후 취소하여 다시 주행화면으로 복귀 하는 상황.
                    restore();
                    sendNotification(RouteConstants::CancelRoute, QVariantMap{{"clear",true}});
                    sendNotification(RouteConstants::ResumeRoute, m);
                    sendNotification(RouteConstants::RestoreViaData);
                    mIsTvasBackup = true;
                } else if (m.contains("backup") && m["backup"].toBool()) {
                    // 주행 중 목적지 설정하여 경로 탐색이 이루어진 상황.
                }
            }
        }
        break;

    case SearchConstants::RequestCommand:
        {
            auto m = noti->getBody().toMap();
            auto command = m[CommandKey].toString();
            mIsServerRequested = m[RPREQ_KEY_Online].toBool();
            if (command == "경로 요청") {
                auto files = m[TvasFileNameKey].value<QStringListPtr>();
                for (int i=0; i<files->count(); i++) {
                    auto file = files->at(i);
                    mTvasFiles.append(file);
                }
            }
            if (m.contains(RPREQ_KEY_Apply)) {
                // VR 명령으로 requestRoute()한 경우 미리 mIsInDriving을 설정한다.
                auto apply = m[RPREQ_KEY_Apply].toBool();
                if (apply) {
                    mIsInDriving = true;
                }
            }
        }
        break;

    case RgConstants::PrepareStartRouteGuidance:
        prepareStartRouteGuidance(noti->getBody().toMap());
        break;

    case RgConstants::StartRouteGuidance:
        {
            auto m = noti->getBody().toMap();
            mSelectedTvasId = m["routeId"].toInt();
            mIsInDriving = true;
            mIsTvasBackup = true;
            mIsContinous = false;            

            // 경로 안내 후 다른 목적지로 탐색 후 에러가 발생하면
            // 이전 경로로 안내를 원할히 수행하기 위해 via 데이터를 백업함.
            // 이것을 하지 않으면 재탐색 시 목적지가 실패한 목적지로 바뀜.
            sendNotification(RouteConstants::BackupViaData);

            auto command = m[CommonConstants::Command].toString();
            if (command == "ChangeOption") {
                // 경로 옵션 변경의 경우 재탐색을 통해 수행 되고
                // 재탐색시에는 SDI정보를 수집하므로 10초뒤 강제 재탐색을 하지 않아도 됨
                // 19.06.29 ymhong.
            } else {
                auto requestReroute = true;

                // 과부하 모드에서는 최적길만 통신으로 내려 받음.
                // 나머지 경로 옵션은 비통신으로 내려 받았기 때문에 SDI정보 획득을 위해
                // 재탐색이 필요하다. requestSDI로 재탐색을 할지 결정한다.
                // 19.07.18 ymhong.
                if (m.contains(RPREQ_KEY_RerouteForSDI)) {
                    requestReroute = m[RPREQ_KEY_RerouteForSDI].toBool();
                    if (requestReroute) {
                        mIsServerRequested = false;
                    }
                }

                if (requestReroute) {
                    mIsRerouteForCalcDone = false;
                    mTimer.start();
                }
            }
        }
        break;

    case RgConstants::SwitchToLocal:
        mIsServerRequested = false;
        break;

    case RgConstants::StartRouteGuidanceContinuously:
        {
            auto m = noti->getBody().toMap();
            auto index = m[QStringLiteral("selectedRouteIndex")].toInt() + 1;
            QString tvas = QString("tvas_route.db");

            // 경로 이어 하기 시 mTvasFiles는 비어 있는 것을 확인.
            // 한번도 경로 탐색이 된 적이 없으므로...
//            Q_ASSERT(0 == mTvasFiles.count());
            if (mTvasFiles.count() == 0)
               T_DEBUG(QStringLiteral("[UI] warning! tvas files is not ready while rg starting continously"));

            mTvasFiles.append(tvas);
            mIsContinous = true;

            mIsRerouteForCalcDone = false;
//            mTimer.start();
        }
        break;

    case RgConstants::StopRouteGuidance:
        mIsInDriving = false;
        mIsTvasBackup = true;
        deleteBackupFiles();
        deleteTvasFiles();

        mTimer.stop();
        mIsRerouteForCalcDone = false;
        break;

    case RouteConstants::RequestRouteSummaryDetail:
        {
            // 경로 상세 화면으로 이행 시
            auto m = noti->getBody().toMap();
            if (m.contains("routeId")) {
                auto routeOption = m["routeOption"].toInt();
                auto tvas = m["tvas"].toString();

                QVariantList list;
                list << routeOption << tvas;
                QStringList colors;

                auto routeId = m["selectedId"].toInt();
                switch (routeId) {
                case 0: colors.append("routeRed"); break;
                case 1: colors.append("routeBlue");break;
                case 2: colors.append("routeGreen");break;
                }

                if (colors.isEmpty()) {
                    sendNotification(RouteConstants::SetTvas, list);
                } else {
                    sendNotification(RouteConstants::SetTvas, QVariantMap{
                                         {"list", list},
                                         {"colors", colors}
                                     });
                }
            }
        }
        break;

    case RouteConstants::RestoreTvas:
        if (restore()) {
            // 경로 탐색 에러 발생 후 이전 tvas로 복구. (주행중이었다면...)
            sendNotification(RouteConstants::ResumeRoute);
            // via 데이터 복구
            // 이것을 하지 않으면 경로 탐색 실패한 via가 설정된 상태로 남아 있어
            // 재탐색 시 목적지가 바뀌는 문제가 발생한다.
            // 17.07.20 ymhong.
            sendNotification(RouteConstants::RestoreViaData);

            // RouteCandidateView 경로 재 설정.
            sendNotification(RouteConstants::RestoreRoute);

            mIsTvasBackup = true;
        }
        break;

    case RouteConstants::RequestReroute:
        mIsRerouteForCalcDone = true;
        break;

    case NavigationControllerConstants::NavigationAppeared:
        mForeground = true;
        if (!mSendToCar.isEmpty()) {
            responseSendToCar();
        }
        break;

    case SearchConstants::ResponseFindSendToCar:
        {
            mSendToCar  = noti->getBody().value<QVariantMap>();
            if (mForeground) {
                responseSendToCar();
            }
        }
        break;

    case  SearchConstants::ResponseRemoveSendToCar:
        {
            QVariantMap data = noti->getBody().value<QVariantMap>();
            //SendToCar데이타 요청: 삭제여부를 확인하기  위함
            data.insert(CommonConstants::Type, NavigationControllerProtocol::RequestFindSendToCar);
            sendNotification( NavigationControllerConstants::ApplyController, data );
        }
        break;

    case MapConstants::ApplyMapViewMode:
        {
            auto m = noti->getBody().toMap();
            mMapViewMode = (MapViewMode::ModeK)m[CommonConstants::Mode].toInt();
        }
        break;

    case SettingConstants::DialogAccepted:
        {
            QVariantMap data = noti->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != QStringLiteral("RouteSearchMediator"))
                break;

            // 주행모드로 변경
            mDestData = m;

            if (mIsRg) {
                auto address = StringHelper::instance()->fromCp949(m[CommonConstants::Address].toByteArray().constData());

                showConfirmDialog(address);
            } else {
                sendByDestination(mDestData);

                // GridView가 Send2Car popup보다 뒤에 있는 경우
                // 강제로 닫아준다.
                auto tileGridView = ViewRegistry::objectName(ViewRegistry::RecentTileGridView);
                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, tileGridView}});
            }

            removeSend2CarData(mDestData);
        }
        break;

    case SettingConstants::DialogCanceled:
        {
            QVariantMap data = noti->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != QStringLiteral("RouteSearchMediator"))
                break;
            mRequestDriveMode = false;
            // 경로설정 후 삭제 요청
            removeSend2CarData(m);
        }
        break;

    case MapConstants::RegistGlobalMapViewRenderer:
        {
            auto args = noti->getBody().value<QVariantMap>();
            mapView = args[QStringLiteral("mapView")].value<QObject*>();

            //TMCRSM-850 - mapView핸들이 들어왔을 때 유종 설정을 한다.
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->SetUserOilType(mapView);
        }
        break;

    //case SettingConstants::SavedSetting:
     case SettingConstants::ApplySetting:
     case SettingConstants::ChangedSetting:
        {
            QVariantMap data = noti->getBody().value<QVariantMap>();
            QString section = data[CommonConstants::Section].toString();
            QString key;
            int value;
            if ( data.contains(CommonConstants::Key ) ) {
                 key = data[CommonConstants::Key].toString();
            } else {
                auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
                for ( auto setting : settings ) {
                    key = setting->key().toString();
                    if (key != SettingConstants::OilKind)
                            continue;
                    value = setting->value().toInt();
                }
            }
            if (section == SettingConstants::MapSubOilType &&
                key == SettingConstants::OilKind) {
                auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();

                if (data.contains(CommonConstants::Setting)) {
                    value = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    naviProxy->setFuelType(value);
                }
                naviProxy->SetUserOilType(mapView);
            } else if (section == SettingConstants::MapSubChargeType) {
                if (key == SettingConstants::ChargeType) {
                    if (data.contains(CommonConstants::Setting)) {
                        value = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                        auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                        naviProxy->setChargeType(value);
                    }
                }
            } else if (section == SettingConstants::RouteSearch) {
                if (key == SettingConstants::HipassMount) {
                    if (data.contains(CommonConstants::Setting)) {
                        auto isHipass = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                        sendNotification(RouteConstants::SetHipass, isHipass);
                    }
                }
            } else if (section == SettingConstants::GuidSubVoiceGuidTime) {
                applyVoiceGuidanceVocalPoint();
            } else if (section == SettingConstants::GuidSubCamera) {
                applyCameraGuidance();
                applyAttentionGuidance();
            } else if (section == SettingConstants::GuidSubAttentionSection) {
                applyAttentionGuidance();
            } else if (section == SettingConstants::GuidMain && key == SettingConstants::RoadViolation ) {
                applyRoadViolation();
            } else if (section == SettingConstants::GuidMain && key == SettingConstants::SafeDrivingSoundAtMute) {
                applySafeDrivingSoundAtMute();
            } else if (section == SettingConstants::MapSubSpeedResponse) {
                applySpeedLevel();
            } else if (section == SettingConstants::GuidSubVoice) {
                applyVoice();
            }
        }
        break;
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            QVariantMap m = noti->getBody().toMap();
            mIsRg = m["isRg"].toBool();
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->SetUserOilType(mapView);
        }
        break;

    default:
        break;
    }

    return true;
}

void RouteSearchMediator::showConfirmDialog(const QString &address)
{
    int x = mDestData["x"].toInt();
    int y = mDestData["y"].toInt();
    int poiId = mDestData[SearchConstants::PoiID].toInt();
    int navSeq = mDestData[SearchConstants::NavSeq].toInt();
    int rpFlag = mDestData[SearchConstants::RpFlag].toInt();
    QString name = StringHelper::instance()->fromCp949(mDestData[CommonConstants::Alias].toByteArray().constData());

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("name")] = name;
    m[SearchConstants::PoiID] = poiId;
    m[SearchConstants::NavSeq] = navSeq;
    m[SearchConstants::RpFlag] = rpFlag;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m[QStringLiteral("okCallback")] = QStringLiteral("closeAllView");
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

bool RouteSearchMediator::sendByDestination(const QVariantMap &data)
{
    int x = data[CommonConstants::X].toInt();
    int y = data[CommonConstants::Y].toInt();
    int poiId = data[SearchConstants::PoiID].toInt();
    int navSeq = data[SearchConstants::NavSeq].toInt();
    int rpFlag = data[SearchConstants::RpFlag].toInt();
    int poleCate = 0;// data[SearchConstants::OpiPoleCat].toInt();
    // int opiPrice = jsVal.property( SearchConstants::OpiPrice).toInt();
    QString name = StringHelper::instance()->fromCp949(data[CommonConstants::Alias].toByteArray().constData());
    QString address = StringHelper::instance()->fromCp949(data[CommonConstants::Address].toByteArray().constData());


    QVariantMap body;
    body[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, body);
    body.clear();

    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Name] = name;
    body[SearchConstants::PoiID] = poiId;
    body[SearchConstants::NavSeq] = navSeq;
    body[SearchConstants::RpFlag] = rpFlag;

    sendNotification(RouteConstants::SetArrival, body);

    // navi bookmark > select address
    QVariantMap bookmark;
    bookmark.insert( CommonConstants::Name, name);
    bookmark.insert( CommonConstants::Address, address);
    bookmark.insert( CommonConstants::X, x);
    bookmark.insert( CommonConstants::Y, y);
    bookmark.insert( SearchConstants::PoiID, poiId);
    bookmark.insert( SearchConstants::NavSeq, navSeq);
    bookmark.insert( SearchConstants::RpFlag, rpFlag);
    bookmark.insert( SearchConstants::OpiPoleCate, poleCate);
    // bookmark.insert( SearchConstants::OpiPrice, opiPrice);
    sendNotification(NavibookmarkConstants::SelectNavibookAddress,bookmark);

    sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
    return true;
}

void RouteSearchMediator::closeAllView()
{
    sendNotification(SearchConstants::CloseRecentTileGridView);
    sendNotification(RgConstants::CloseNaviMenuAll);
    sendNotification(SimulatorConstants::CloseSimulationView);
    sendNotification(RouteConstants::CloseRouteCandidateView);
    sendNotification(RouteConstants::CloseRouteSummaryView);
    sendNotification(KeyboardConstants::HideKeyboard);
    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});
}

void RouteSearchMediator::removeSend2CarData(QVariantMap dataS2C)
{
    // 경로설정 후 삭제 요청
    dataS2C.insert( CommonConstants::Type, NavigationControllerProtocol::RequestRemoveSendToCar);
    sendNotification( NavigationControllerConstants::ApplyController, dataS2C );
}

void RouteSearchMediator::backup()
{
    deleteBackupFiles();

    QDir dir(routePath());

    for (QString filename : mTvasFiles) {
        QString target = filename;
        if (target.endsWith(TvasFileExt)) {
            target.chop(QString(TvasFileExt).length());
            target.append(TvasBackupExt);

            dir.rename(filename, target);

            mTvasBackupFiles.append(target);
        }
    }
}

void RouteSearchMediator::deleteBackupFiles()
{
    mTvasBackupFiles.clear();

    QStringList list;

    QDir dir(routePath());
    dir.setNameFilters(QStringList{"*" + TvasBackupExt});
    dir.setFilter(QDir::Files);
    auto files = dir.entryList();
    for (QString file : files) {
        list.append(file);
    }

    deleteFiles(list);
}

void RouteSearchMediator::deleteTvasFiles()
{
    mTvasFiles.clear();

    QStringList list;

    QDir dir(routePath());
    dir.setNameFilters(QStringList{"*" + TvasFileExt});
    dir.setFilter(QDir::Files);
    auto files = dir.entryList();
    for (QString file : files) {
        list.append(file);
    }

    deleteFiles(list);
}

void RouteSearchMediator::deleteTvasFilesWithoutRouteFile()
{
    mTvasFiles.clear();

    QStringList list;

    QDir dir(routePath());
    dir.setNameFilters(QStringList{"*" + TvasFileExt});
    dir.setFilter(QDir::Files);
    auto files = dir.entryList();
    for (QString file : files) {
        if (file != RouteFileName) {
            list.append(file);
        }
    }

    deleteFiles(list);
}

void RouteSearchMediator::deleteFiles(const QStringList &files)
{
    if (0 < files.count()) {
        QDir dir(routePath());
        for (auto file : files) {
            dir.remove(file);
        }
    }
}

bool RouteSearchMediator::restore()
{
    deleteTvasFilesWithoutRouteFile();

    QDir dir(routePath());

    int backupFilesCount = mTvasBackupFiles.count();

    for (QString filename : mTvasBackupFiles) {
        QString target = filename;
        if (target.endsWith(TvasBackupExt)) {
            target.chop(QString(TvasBackupExt).length());
            target.append(TvasFileExt);

            dir.rename(filename, target);

            mTvasFiles.append(target);
        }
    }

    deleteBackupFiles();

    return 0 < backupFilesCount;
}

QString RouteSearchMediator::routePath() const
{
    return Environment::getRoutePath();
}

void RouteSearchMediator::prepareStartRouteGuidance(const QVariantMap& m)
{
    auto filename = m["filename"].toString();
    auto routeId = m["routeId"].toInt();
    auto routeOption = m["routeOption"].toInt();
    if (filename.isEmpty()) {
        filename = QString("tvas_route%1.db").arg(routeId + 1);
    }

    QString src = filename;//QString("tvas_route%1.db").arg(routeId + 1);
    QString dest= QString(RouteFileName);
    QDir dir(routePath());

    if (dir.exists(dest)) {
        qDebug() << "old " << dest << " file exist. remove it.";
        dir.remove(dest);
    }
    if (dir.rename(src, dest)) {
    }

    QVariantList vlist;
    vlist.append(routeOption);
    vlist.append(dest);

    sendNotification(RouteConstants::SetTvas, vlist);

    deleteTvasFilesWithoutRouteFile();
    deleteBackupFiles();
}

void RouteSearchMediator::applyVoiceGuidanceVocalPoint(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyVoiceGuidanceVocalPoint );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applyVoiceGuidance(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyVoiceGuidance );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applyCameraGuidance(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyCameraGuidance );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applyAttentionGuidance(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyAttentionGuidance );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applyRoadViolation(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyRoadViolation );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applySafeDrivingSoundAtMute(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySafeDrivingSoundAtMute );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applySpeedLevel(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::applyVoice(bool send /*= true*/) {
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyVoice );
    body.insert( CommonConstants::Value, send );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void RouteSearchMediator::responseSendToCar() {
    QVariantMap data = mSendToCar;
    auto enableSend2Car = true;//PlatformHelper::instance()->isRgContinueEnable(); //+spcx
    if (enableSend2Car && data[CommonConstants::Count].toInt()) {
        //TMCRSM-1805 - MapConstants::ResponseDriveMode를 받는 view를 모두 닫는다.
        qDebug() << "[OTP] responseSendToCar() -> CloseRouteCandidateView";
        closeAllView();

        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
        m[CommonConstants::ParentObjectName] = CommonConstants::ContentRootForCommonDialog;
        m[CommonConstants::Title] = tr("목적지 수신");
        QString dest = StringHelper::instance()->fromCp949(data[CommonConstants::Alias].toByteArray().constData());
        if (dest.isEmpty())
            dest = StringHelper::instance()->fromCp949(data[CommonConstants::Address].toByteArray().constData());
        m[CommonConstants::Message] = QStringLiteral("수신 받은 목적지로 길안내를<br>받으시겠습니까?");
        m[CommonConstants::Message2] = dest;
        m[CommonConstants::Type] = QStringLiteral("c");
        QVariantList v;
        v.append(CommonConstants::Cancel);
        v.append(CommonConstants::Ok);
        m[CommonConstants::Menus] = v;
        m[CommonConstants::Arg] = QVariantMap{{CommonConstants::Caller , tr("RouteSearchMediator")},{CommonConstants::Value, data}};
        m["popupType"] = SystemPopupMediator::Send2Car;
        QObject *popupRoot = nullptr;
#if defined(BUILD_TARGET)
        popupRoot = PlatformHelper::instance()->popupRoot();
#endif
        if (popupRoot) {
            m[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
        } else {
            m[CommonConstants::ParentObjectName] = "popupWrap";
        }
        sendNotification(Constants::CreateView, m);//SystemPopup, m);
    }
    mSendToCar.clear();
}

}//SKT
