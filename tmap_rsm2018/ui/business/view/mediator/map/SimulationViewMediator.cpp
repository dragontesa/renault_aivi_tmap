#include "SimulationViewMediator.h"
#include <QVariant>
#include "INotification.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SettingConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "VSM_Navi.h"
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"
#include "LangConstants.h"
#include "NavigationControllerConstants.h"
#include "SettingDto.h"
#include "RouteConstants.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "PlatformHelper.h"
#include "QmlAppHelper.h"

namespace SKT {

SimulationViewMediator::SimulationViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
}

SimulationViewMediator::~SimulationViewMediator()
{
    sendNotification(RgConstants::ChangeRouteGuideBindState, QVariant::fromValue(1));
}

QList<int> SimulationViewMediator::listNotificationInterests()
{
    static QList<int> noties = {
        SimulatorConstants::UpdateSimulator,
        SimulatorConstants::CloseSimulationView,
        SimulatorConstants::StopSimulator,
        ZoneConstants::DoSoftKeyFunction,
        SimulatorConstants::ShowSimulatorControl,
        SimulatorConstants::HideSimulatorControl,
        MapConstants::MapControlViewActivated,
        SettingConstants::ApplySetting,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::NavigationDisappeared,
        NavigationControllerConstants::NavigationAppeared,
        PreferenceConstants::ApplyPreference
    };
    return noties;
}

bool SimulationViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
            sendNotification(RouteConstants::BlockSetTvas, true);
#ifndef TEST_GADGET_RG
            sendNotification(RgConstants::ChangeRouteGuideBindState, QVariant::fromValue(0));
#endif
            auto lang = QmlAppHelper::instance()->language();
            QQmlProperty::write(mViewComponent, "lang", lang);
        }
        break;

    case SetupTitle:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case Activated:
        {
            if (false == mAppForeground) {
                return true;
            }

            // 현재 맵의 viewMode 요청.
            QVariantMap m;
            m[CommonConstants::Section] = "DirectionGuid";
            sendNotification( SettingConstants::RequestSetting, m );

            if (!mMapViewLevelInitialized) {
                // 속도 반응지도 사용 여부
                m[CommonConstants::Section] = SettingConstants::MapSubSpeedResponse;
                m[CommonConstants::Key] = SettingConstants::SpeedResponseMapUse;
                sendNotification(SettingConstants::RequestSetting, m);

                // 속도 반응지도 사용하지 않으면 마지막 설정한 View level을 얻어옴.
                if (false == mUseSpeedResponseMap) {
                    QVariantMap body;
                    body[SettingConstants::PreferencesName] = PreferenceConstants::Rg;
                    body[CommonConstants::Section] = SettingConstants::MapMain;
                    body[CommonConstants::Key] = "viewLevel";
                    sendNotification( PreferenceConstants::RequestPreference, body );
                }
            }

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Simul}});
            sendNotification(RgConstants::BlockReroute, true);
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeySimulMode);
            sendNotification(MapConstants::ShowCarvata, true);
            sendNotification(MapConstants::UpdateMapViewPadding);

            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);

            sendNotification(MapConstants::ShowRoute, QVariantMap{{"show", true}});
            sendNotification(MapConstants::EnableCallout, false);

            sendNotification(NavigationControllerConstants::ResetCSW);
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));
        }
        break;

    case Deactivated:
        sendNotification(RouteConstants::BlockSetTvas, false);
        break;

    case ResetTimer:
        sendNotification(MapConstants::ResetMapViewTimer);
        break;

    case VolumeClicked:
        sendNotification(NavigationControllerConstants::ShowSettingVolume, true);
        break;

    default:
        break;
    }
    return true;
}

bool SimulationViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case NavigationControllerConstants::ChangeLanguage: {
            auto m = noti->getBody().toMap();
            auto lang = m[LangConstants::Lang].toString();

            QQmlProperty::write(mViewComponent, "lang", lang);
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        }
        break;

    case SettingConstants::ApplySetting:
        applySetting(noti->getBody());
        break;

    case NavigationControllerConstants::NavigationAppeared:
        mAppForeground = true;
        break;

    case NavigationControllerConstants::NavigationDisappeared: {
            mAppForeground = false;

            // UID v2.09
            // 모의주행 중 다른 화면으로 전환 (시스템 홈이나 시스템 메뉴 화면 등) 되면 모의주행 자동 종료되며
            // 별도의 toast pop-up은 띄우지 않음.
            // 19.06.14 ymhong

            // TMCRSM-3689
            // 모의 주행용으로 설정한 tvas가 홈 화면으로 이동 시 유지되고 있어
            // 가젯 화면에서 경로 시작 안내 음성이 나옴.
            // 홈 화면으로 이동시에 경로 취소 처리를 해주어야 함.
            // 19.08.30 ymhong
            auto cancelRoute = true;
            goBack(cancelRoute);
        }
        break;

    case SimulatorConstants::UpdateSimulator: {
            auto m = noti->getBody().toMap();
            auto currentSpeed = m["currentSpeed"].toInt();

            if (MAX_SPEED_TO_OPEN_SIMULATION_VIEW <= currentSpeed) {
                qDebug() << "Simulation mode is canceled. " << currentSpeed << "km/h";
                goBack();
                sendNotification(ToastsConstants::ShowToast, QVariantMap{
                                     {CommonConstants::Position, ToastsConstants::ToastUp},
                                     {CommonConstants::Message, "안전을 위해 주행 중에는 모의주행을 실행할 수 없습니다."}
                                 });
            } else {
                QMetaObject::invokeMethod(mViewComponent, "updateProgress", Q_ARG(QVariant, noti->getBody()));
            }
        }
        break;

    case SimulatorConstants::CloseSimulationView:
        close();
        sendNotification(SimulatorConstants::StopSimulator);
        sendNotification(Constants::ShowView,QVariantMap{
            {CommonConstants::ObjectName,QQmlProperty::read(mViewComponent,CommonConstants::Caller)}});
        break;

    case ZoneConstants::DoSoftKeyFunction:
        procSoftkey(noti->getBody());
        break;

    case SimulatorConstants::ShowSimulatorControl:
        QMetaObject::invokeMethod(mViewComponent, "setSimulationControlVisible", Q_ARG(QVariant, true));
        break;

    case SimulatorConstants::HideSimulatorControl:
        QMetaObject::invokeMethod(mViewComponent, "setSimulationControlVisible", Q_ARG(QVariant, false));
        break;

    case MapConstants::MapControlViewActivated:
        {
            auto enable = noti->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "enableMouseArea", Q_ARG(QVariant, enable));
        }
        break;

    case PreferenceConstants::ApplyPreference:
        {
            auto result = noti->getBody().value<QVariantMap>();
            auto req = result[QStringLiteral("request")].value<QVariantMap>();
            auto preferenceName = req[SettingConstants::PreferencesName].toString();
            auto section = req[SettingConstants::Section].toString();
            auto key = req[SettingConstants::Key].toString();
            if (preferenceName == PreferenceConstants::Rg && section == SettingConstants::MapMain) {
                if (key == "viewLevel") {
                    auto response = result["response"].value<PreferenceVo>();
                    auto viewLevel = response.value().toInt();

                    QMetaObject::invokeMethod(mViewComponent, "setMapViewLevel", Q_ARG(QVariant,viewLevel));

                    mMapViewLevelInitialized = true;
                }
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

void SimulationViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m["name"].toString() != "SimulationView")
        return;

    QString function = m["function"].toString();

    if (function == "back") {
        goBack();
    } else if (function == "option") {
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapModeOptionView);
        m[QStringLiteral("viewMode")] = QStringLiteral("Driving");
        QObject *popupRoot = nullptr;
#if defined(BUILD_TARGET)
        popupRoot = PlatformHelper::instance()->popupRoot();
#endif
        if (popupRoot) {
            m[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
        } else {
            m[CommonConstants::ParentObjectName] = "popupWrap";
        }
        sendNotification(Constants::CreateView, m);
    }
}

void SimulationViewMediator::close()
{
    // 모의 주행 시 맵 터치를 누른 후 뒤로가기를 눌렀을 때 남아있는 맵컨트롤뷰를 닫음
    // 17.09.20 ymhong
    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::MapControlView)}});

    sendNotification(NavigationControllerConstants::ResetCSW);
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void SimulationViewMediator::applySetting(const QVariant &v)
{
    auto m = v.toMap();
    QString section = m[CommonConstants::Section].toString();
    if ( section == SettingConstants::DirectionGuid ) {
        auto settings = m[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
        for (auto setting : settings) {
            if (setting->key() == "MapMode") {
                auto value = setting->value().toInt();
                int viewMode = 0;

                // setting에서는 3D, 주행방향, 정북 순
                // mapview에서는 정북, 주행방향, BirdView(3D) 순
                switch (value) {
                case 0: viewMode = 3; break;
                case 1: viewMode = 2; break;
                case 2: viewMode = 1; break;
                default:
                    qDebug() << "viewMode is illigal !!!";
                    break;
                }
                QQmlProperty::write(mViewComponent, "viewMode", viewMode);
            }
        }
    } else if (section == SettingConstants::MapSubSpeedResponse) {
        auto setting = m[CommonConstants::Setting].value<SettingDtoPtr>();
        auto key = m[SettingConstants::Key].toString();
        if (key == SettingConstants::SpeedResponseMapUse) {
            auto speedResponseMapUse = setting->value().toBool();

            mUseSpeedResponseMap = speedResponseMapUse;
        }
    }
}

void SimulationViewMediator::goBack(bool cancelRoute)
{
    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();

    sendNotification(SimulatorConstants::StopSimulator, QVariantMap{
                         {"cancelRoute", cancelRoute}
                     });
    sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});

    // 모의 주행 중 맵을 터치하여 NormalMapControl모드일 때 백키로 빠져 나오면
    // 볼륨 버튼이 hide 상태에서 복구가 안되기 대문에 ShowVolume을 호출.
    sendNotification(MapConstants::ShowVolume);

    if (caller != ViewRegistry::objectName(ViewRegistry::RouteSummaryView))
        sendNotification(MapConstants::UpdateMapView);

    close();
}

}//SKT
