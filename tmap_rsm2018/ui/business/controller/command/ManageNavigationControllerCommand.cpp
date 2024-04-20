#include "ManageNavigationControllerCommand.h"
#include <QQmlProperty>
#include "Constants.h"
#include "RequestCommand.h"
#include "NavigationControllerProxy.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "ApplicationConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProxy.h"
#include "PreferenceConstants.h"
#include "CommonConstants.h"
#include "../common/navigationcontroller.h"
#include "SoundConstants.h"
#include "RgConstants.h"
#include "ViewRegistry.h"
#include "OffscreenRenderer.h"
#include "PlatformHelper.h"
#include "VsmNaviProxy.h"
#include "MapConstants.h"
#include "uiconstants.h"
#include "SearchConstants.h"

using namespace SKT;

MangeNavigationControllerCommand::MangeNavigationControllerCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

MangeNavigationControllerCommand::~MangeNavigationControllerCommand()
{
    //T_DEBUG( "I'm done..." );
}

bool MangeNavigationControllerCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case NavigationControllerConstants::ApplyController: {
        auto controllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto data = noti->getBody().value<QVariantMap>();        
        auto notifyMethod = std::bind(&MangeNavigationControllerCommand::onResponseCommand, this, std::placeholders::_1);
        auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
        controllerProxy->applyCommand( req );
    }
    break;

    case NavigationControllerConstants::RequestController: {
        auto controllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto owner = noti->getSource().data();
        QVariantMap data = noti->getBody().value<QVariantMap>();
        data[CommonConstants::Owner] = QVariant(reinterpret_cast<qulonglong>(owner));
        auto notifyMethod = std::bind(&MangeNavigationControllerCommand::onResponseCommand, this, std::placeholders::_1);
        auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
        controllerProxy->requestCommand( req );
    }
    break;

    case NavigationControllerConstants::QueryOnlineStatus:
        {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            bool isOnline = proxy->getOnlineState();
            sendNotification(NavigationControllerConstants::ResponseOnlineStatus, isOnline);
        }
        break;

    case NavigationControllerConstants::RequestChannelServerPolicyFinished:
        {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto isOnline = proxy->getOnlineState();
            auto isChannelServerPolicyFinished = proxy->isChannelServerPolicyFinished();
            auto isChannelServerPolicyDone = proxy->isChannelServerPolicyDone();
            sendNotification(NavigationControllerConstants::ReplyToChannelServerPolicyFinished, QVariantMap{
                                 {"isOnline", isOnline},
                                 {"isChannelServerPolicyDone", isChannelServerPolicyDone},
                                 {"isChannelServerPolicyFinished", isChannelServerPolicyFinished}
                             });
        }
        break;

    case NavigationControllerConstants::ChannelServerPolicyFinished:
        {
            auto m = noti->getBody().toMap();
            auto isChannelServerPolicyFinished = m["isChannelServerPolicyFinished"].toBool();
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->setChannelPolicyReceived(isChannelServerPolicyFinished);

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setChannelPolicyReceived(isChannelServerPolicyFinished);

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->setChannelPolicyReceived(isChannelServerPolicyFinished);
        }
        break;

    case NavigationControllerConstants::ModifiedUserData: {
         auto body = noti->getBody().value<QVariantMap>();
         auto dataType = body[CommonConstants::Type].toInt();
         QVariant data;
         if (body.contains(QStringLiteral("data")))
             data  = body[QStringLiteral("data")];

         auto controllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
          controllerProxy->notifyUserDataModified(dataType, data);
        }
    break;

    case NavigationControllerConstants::UpdatedUserProfile:
    case NavigationControllerConstants::ChangedUserProfile:
        {
            auto profileId = noti->getBody().toInt();
            auto rg = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            bool isrg = rg->isRg();
            QVariantMap m;
            m[QStringLiteral("profileId")] = profileId;
            m[QStringLiteral("isRg")]  = isrg;
            auto appNoteId = noti->getId() == NavigationControllerConstants::UpdatedUserProfile ?
                    ApplicationConstants::UpdatedUserProfile : ApplicationConstants::ChangedUserProfile;
            sendNotification(appNoteId, m);
        }
        break;

    case NavigationControllerConstants::RequestSkyState:
        {
            auto controllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto sunState = controllerProxy->getSunState();
            auto weatherState = controllerProxy->getWeatherState();
            QVariantMap m;
            m["sunState"] = sunState;
            m["weatherState"] = weatherState;
            T_DEBUG( QString("RequestSkyState #1 sunState: %1").arg(sunState) );
            sendNotification(NavigationControllerConstants::ResponseSkyState, m);
        }
        break;

    case NavigationControllerConstants::ShowSettingVolume:
        {
            auto controllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            if (controllerProxy->isSpcxRgEnabled()) {
                // do nothing.
            } else {
                auto enable = noti->getBody().toBool();
                if (enable) {
                    playVolumeSound();
                    controllerProxy->showSysVolumeHMI(true);
                } else {
                    //??
                }
            }
        }
        break;
    case NavigationControllerConstants::SpeakPhreaseSample:
        playVolumeSound();
        break;

    case NavigationControllerConstants::CallZoneApi:
        callZoneApi(noti->getBody());
        break;

    case SoundConstants::PlayBeep:
        {
            auto id = noti->getBody().toInt();
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->beep(id);
        }
        break;
    case NavigationControllerConstants::StartMapInClusterRenderer:
        {
            auto renderer = dynamic_cast<OffscreenRenderer*>(noti->getBody().value<QObject*>());
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->registerMapInClusterRenderer( renderer );
            renderer->start();
        }
        break;
    case NavigationControllerConstants::ShowGadget:
        {
            auto body = noti->getBody().toMap();
            auto type = body[QStringLiteral("type")].toInt();
            auto active = body[QStringLiteral("active")].toBool();
            auto asWindow = body[QStringLiteral("asWindow")].toBool();
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->showGadget( type, active, asWindow );
        }
        break;
    case NavigationControllerConstants::StartGadgetRenderer:
        {
            auto body = noti->getBody().toMap();
            auto type = body[QStringLiteral("type")].toInt();
            auto renderer = dynamic_cast<OffscreenRenderer*>(body[QStringLiteral("renderer")].value<QObject*>());
            auto imageProvider = body[QStringLiteral("imageProvider")].value<QObject*>();
            auto gadgetDebugWindow = body[QStringLiteral("gadgetDebugWindow")].value<QQuickWindow*>();
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->registerGadgetRenderer( renderer, imageProvider, gadgetDebugWindow );
            renderer->start();
        }
        break;
    case NavigationControllerConstants::SaveGadgetImage:
        {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->saveGadgetImage(noti->getBody().toBool());
        }
        break;
    case NavigationControllerConstants::SendNotiGetVoiceGuidanceActive: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->sendNotiGetVoiceGuidanceActive();
        }
        break;

    //TMCRSM-2421:내비게이션의 경고음 안내 설정 상태를system에 전달하는 인터페이스
    case NavigationControllerConstants::SendNotiSetOverSpeedSoundAlertInfoActive: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->applySafeDrivingSoundAtMute();
        }
        break;

    case NavigationControllerConstants::FileGpsSetPos: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto percent = noti->getBody().toDouble();
            proxy->fileGpsSetPos(percent);
        }
        break;
    case NavigationControllerConstants::FileGpsRewind: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->fileGpsRewind();
        }
        break;
    case NavigationControllerConstants::FileGpsFastForward: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->fileGpsFastForward();
        }
        break;

    case NavigationControllerConstants::ReplyToViewList:{
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto body = noti->getBody().toMap();
            auto viewList = body[CommonConstants::Value].value<QList<int>>();
            auto findId = body[CommonConstants::ViewId].value<int>();
            proxy->sendReplyToViewList(viewList, findId);
        }
        break;

    case NavigationControllerConstants::SwitchToAppIfNecessary:{
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->switchToAppIfNecessary();
            break;
        }

    case RgConstants::ChangedUiState:
        {
//            sendNotification(RgConstants::ChangedUiState,
//                QVariantMap{
//                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::CrossImageView)},
//                    {QStringLiteral("item"), RgConstants::CrossImageVisible},
//                    {CommonConstants::Value, visible}
//                }
            auto m = noti->getBody().toMap();
            auto caller = m[CommonConstants::Caller].toString();
            auto item = m["item"].toString();
            auto value = m[CommonConstants::Value].toBool();

            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->onCrossImageVisibleChanged(value);
        }
        break;
    case MapConstants::GlobalMapViewStartedToRender: {
            qDebug() << "[CTX]MangeNavigationControllerCommand GlobalMapViewStartedToRender";
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->onMapStartedToRender();
        }
        break;

    case NavigationControllerConstants::AppLostCswFocus: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->setAppLostCswFocus();
        }
        break;

    case NavigationControllerConstants::ReserveAppSwitchJob: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->reserveAppSwitchJob(noti->getBody().toMap());
        }
        break;

    case NavigationControllerConstants::ReserveSystemSwitchJob: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->reserveSystemSwitchJob(noti->getBody().toMap());
        }
        break;

    case NavigationControllerConstants::RequestAroundSearch: {
            auto instanceId = noti->getBody().toInt();

            switch (instanceId) {
            case UIConstants::GadgetIconFuel:
                PlatformHelper::instance()->requestAroundSearchPatrol(SearchConstants::CategoryPatrol);
                break;
            case UIConstants::GadgetIconParking:
                PlatformHelper::instance()->requestAroundSearchParking();
                break;
            }
        }
        break;

    case NavigationControllerConstants::RequestContactGadgetSearch: {
            auto searchString = noti->getBody().toString();

            // Zone1에 설정된 mSearchBarText와 맞지 않으면 UpdateSearchCount가 갱신되지 않음.
            sendNotification(ZoneConstants::UpdateSearchText, searchString);

            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::TextSearchView);
            m[CommonConstants::Caller] = CommonConstants::System;
            m[CommonConstants::Owner ] = CommonConstants::System;
            m[SearchConstants::Query] = searchString;
            m[CommonConstants::Visible] = SearchConstants::AlwaysShow;
            //m[CommonConstants::Z] = CommonConstants::MAX_INT_VALUE;
            m[CommonConstants::ReFocus] = true;
            m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::TextSearchView);
            sendNotification(SearchConstants::UpdateView, m);
            sendNotification(Constants::CreateView, m);
        }
        break;

    case ZoneConstants::CswBackAccepted: {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->emitCswBackKeyAccepted();
        }
        break;

    case ApplicationConstants::UserClickOTP:
        {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setUpdateContinueRg();
        }
        break;

    case ApplicationConstants::ChangedSession:
        {
            qDebug() << "[OTP] ChangedSesstion";

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            if (rgProxy->isUpdateContinueRg()) {
                qDebug() << "[OTP] Update Continue RG";

                auto isRg = rgProxy->isRg();
                if (isRg) {
                    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                    searchProxy->saveViaToPreference();

                    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                        {"preferencesName",PreferenceConstants::Rg},
                        {CommonConstants::Section,QStringLiteral("Termination")},
                        {CommonConstants::Key,QStringLiteral("selectedRouteIndex")},
                        {CommonConstants::Value,0}
                    });
                }
                // 경로 이어하기를 위한 플래그 저장.
                auto isRgTerminated = !isRg;

                sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                    {"preferencesName",PreferenceConstants::Rg},
                    {CommonConstants::Section,QStringLiteral("Termination")},
                    {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
                    {CommonConstants::Value,isRgTerminated}
                });
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

bool MangeNavigationControllerCommand::onResponseCommand( const mvc::INotificationPtr& note )
{
    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqData = req->getReqData();
    QVariantMap resData = req->getResData();
    QVariantMap body;    
    return true;
}

//#define TEST_FONT_VIEW
#ifdef BUILD_TARGET
#   define USE_UI_VOLUME_VIEW   1
#else
#   define USE_UI_VOLUME_VIEW   1
#endif

void MangeNavigationControllerCommand::showSettingVolume(const QVariant &v)
{
    auto b = v.toBool();
    if (b) {
        playVolumeSound();
    }

//#if USE_UI_VOLUME_VIEW
//    if (b) {
//        sendNotification(Constants::CreateView,
//            QVariantMap{{CommonConstants::Url,
//#   ifdef TEST_FONT_VIEW
//            ViewRegistry::url( ViewRegistry::TestFontView )
//#   else
//            ViewRegistry::url( ViewRegistry::SettingVolumeView )
//#   endif
//        }});
//    }
//#else
//#   if(1)
//        PlatformHelper::instance()->showVolume(b);
//#   else
//    // Zone3 test view.
//    sendNotification(Constants::CreateView,
//        QVariantMap{{CommonConstants::Url,
//        ViewRegistry::url( ViewRegistry::ZoneApiTestView )}});
//#   endif
//#endif
}

void MangeNavigationControllerCommand::callZoneApi(const QVariant &v)
{
    auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    auto m = v.toMap();
    auto type = m["type"].toInt();
    auto api = m["api"].toString();

    if (api == "keyboard") {
        proxy->setKeyboard(v);
    } else {
        if (type < 100) {
            proxy->setZone1(v);
        } else {
            proxy->setZone3(v);
        }
    }
}

void MangeNavigationControllerCommand::playVolumeSound()
{
    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    naviProxy->soundPlayVolume();
}
