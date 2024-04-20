#include "NavigationControllerProxy.h"
#include "RequestCommand.h"
#include "Constants.h"
#include "RgConstants.h"
#include "SettingProxy.h"
#include "SettingDto.h"

#include <QDebug>
#include <math.h>

#include "StringHelper.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "LangConstants.h"
#include "QmlAppHelper.h"
#include "SettingConstants.h"
#include "HiddenSettingConstants.h"
#include "SearchConstants.h"
#include "ApplicationConstants.h"
#include "AddressConstants.h"
#include "InterfaceConstants.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "RouteManager.h"
#include "SimulatorConstants.h"
#include "ViewRegistry.h"
#include "gpsmanagerinterface.h"
#include "sessionmanagerinterface.h"
#include "PlatformHelper.h"
#include "ImageStreamProvider.h"
#include "MultiSenseManager.h"
#include "../common/hardkey.h"
#include "UtilConstants.h"
#include "rprequest.h"
#include "MapInClusterImageStreamProvider.h"
#include "uiconstants.h"
#include "IconGadgetProvider.h"
#include "ResourceResolver.h"
#include "GadgetImageStreamProvider.h"
#include "mediator/app/MapViewMode.h"
#include "ToastsConstants.h"
#include "../environment.h"
#include "SearchConstants.h"
#include "searchrequest.h"
#include "PreferenceConstants.h"
#include "TRandomUtils.h"
#include "KeyboardConstants.h"

#include <QTimer>
#include "hostnames.h"

#define MAXCOUNT_AUTHOTPTRY  3
#define WAIT_AUTHRETRYTIME   (1000*90) //ms

#define GADGET_TOUCH_MOVE       255
#define GADGET_TOUCH_PRESSED    1
#define GADGET_TOUCH_RELEASED   2
#define GADGET_SWITCH_SETTING   3

#define GADGET_TOUCH_TERM 1500

#define CONTACT_GADGET_KEY  (SearchConstants::ContactGadgetKey)

namespace SKT {

NavigationControllerProxy::NavigationControllerProxy(QObject* controller, QObject* property, char languageIndex, QVariantMap gadgetSignal, QVariantMap gpsRfSignal, QVariantMap extraData)
    : mController(qobject_cast<NavigationController*>(controller) ),
      mProperty(qobject_cast<NavigationProperty*>(property)),
      mNightMode(0),
      mFirstAppear(true),
      mIsOnline(false),
      mProfileManager(ProfileManager::instance()),
      mSatellite(nullptr),
      mDateFormat(""),
      mTimeFormat(""),
      mIsBluetooth(false),
      mCountAuthOTPFail(0),
      mLang("")
{
    memset(&mRgConfig, 0, sizeof(mRgConfig));    

    mGadgetTouchTimer.setInterval(GADGET_TOUCH_TERM);
    connect(&mGadgetTouchTimer, &QTimer::timeout, this, [this](){
        if (mTouchX != 0 && mTouchY !=0) {
            onGadgetLongTouched();
        }
    });

    mProfileManager->initProfileManager(controller);

    connect(controller, SIGNAL(sunStateChanged(SKT::NavigationController::SunState)),
            this, SLOT(setSunState(SKT::NavigationController::SunState)));
    connect(controller, SIGNAL(weatherStateChanged(SKT::NavigationController::WeatherState)),
            this, SLOT(setWeatherState(SKT::NavigationController::WeatherState)));
    connect(controller, SIGNAL(navigationVolumeChanged(int)), this, SLOT(navigationVolumeChanged(int)));
    connect(controller, SIGNAL(muteStateChanged(bool)), this, SLOT(setMuteState(bool)));
    connect(controller, SIGNAL(appForegroundChanged(bool)), this, SLOT(navigationAppeared(bool)));
    connect(controller, SIGNAL(onlineChanged(bool)), this, SLOT(onlineChanged(bool)));
    //connect(controller, SIGNAL(cswPressed(int)), this, SLOT(processCsw(int)));
    connect(controller, SIGNAL(showGadget(int,bool,bool)), this, SLOT(onShowGadget(int,bool,bool)));

    //connect(controller, SIGNAL(gadgetActivated(uint,QList<int>,int)), this, SLOT(onGadgetActivated(uint,QList<int>,int)));
    connect(controller, SIGNAL(gadgetTouched(uint,uint,uint,uint,int)), this, SLOT(onGadgetTouched(uint,uint,uint,uint,int)));

    connect(controller, SIGNAL(mapViewModeChanged(int)), this, SLOT(onMapViewModeChanged(int)));
    connect(controller, SIGNAL(viewLevelChanged(int, int)), this, SLOT(onViewLevelChanged(int, int)));
    connect(controller, SIGNAL(trafficInfoScreenChanged(bool)), this, SLOT(onTrafficInfoScreenChanged(bool)));
    connect(controller, SIGNAL(moveToCurrentPositionOccur()), this, SLOT(onMoveToCurrentPosition()));
    connect(controller, SIGNAL(goViaPointEditScreenOccur()), this, SLOT(onGoViaPointEditScreen()));
    connect(controller, SIGNAL(showOnMap(int,int,bool)), this, SLOT(onShowOnMap(int,int,bool)));

    connect(controller, SIGNAL(requestRouteOccur(int,bool)), this, SLOT(onRequestRoute(int,bool)));
    connect(controller, SIGNAL(requestRouteOccur(TS_ROUTE_POS, int, bool)), this, SLOT(onRequestRoute(TS_ROUTE_POS, int, bool)));
    connect(controller, SIGNAL(requestRouteOccur(QList<TS_ROUTE_POS>, int, bool)), this, SLOT(onRequestRoute(QList<TS_ROUTE_POS>, int, bool)));
    connect(controller, SIGNAL(requestRouteOccur(QRoutePosListPtr, int, bool)), this, SLOT(onRequestRoute(QRoutePosListPtr, int, bool)));
    connect(controller, SIGNAL(requestReRouteOccur(int)), this, SLOT(onRequestReRoute(int)));
    connect(controller, SIGNAL(requestReRouteOccur(TS_ROUTE_POS, int)), this, SLOT(onRequestReRoute(TS_ROUTE_POS, int)));
    connect(controller, SIGNAL(requestReRouteOccur(QList<TS_ROUTE_POS>, int)), this, SLOT(onRequestReRoute(QList<TS_ROUTE_POS>, int)));
    connect(controller, SIGNAL(requestFreeTextSearchResultOccur(const QString&,int,int,int,int,int,int)), this, SLOT(onRequestFreeTextSearchResultOccur(const QString&,int,int,int,int,int,int)));
    connect(controller, SIGNAL(requestSearchAddressOccur(const QVariant&)), this, SLOT(onRequestSearchAddressOccur(const QVariant&)));
    connect(controller, SIGNAL(requestSDSSearchAddressOccur(const QVariant&)), this, SLOT(onRequestSDSSearchAddressOccur(const QVariant&)));
    connect(controller, SIGNAL(showDestinationFreeTextSearchInputOccur(const QString&)), this, SLOT(onShowDestinationFreeTextSearchInputOccur(const QString&)));
    connect(controller, SIGNAL(showCalculateTour(TS_ROUTE_POS, int, bool)), this, SLOT(onShowCalculateTour(TS_ROUTE_POS, int, bool)));

    connect(controller, SIGNAL(requestAroundSearchParkingOccur()), this, SLOT(requestAroundSearchParking()));
    connect(controller, SIGNAL(requestAroundSearchPatrolOccur(int)), this, SLOT(requestAroundSearchPatrol(int)));
    connect(controller, SIGNAL(cancelRouteOccur()), this, SLOT(onCancelRoute()));
    connect(controller, SIGNAL(waypointChanged(TS_ROUTE_POS, int)), this, SLOT(onWaypointChanged(TS_ROUTE_POS, int)));
    connect(controller, SIGNAL(waypointInserted(TS_ROUTE_POS, int)), this, SLOT(onWaypointInserted(TS_ROUTE_POS, int)));
    connect(controller, SIGNAL(waypointInsertedFromSelectInMap(int)), this, SLOT(onWaypointInsertedFromSelectInMap(int)));
    connect(controller, SIGNAL(waypointsDeleted(int, int)), this, SLOT(onWaypointsDeleted(int, int)));
    connect(controller, SIGNAL(replaceWaypoint(uint)), this, SLOT(onReplaceWaypoint(uint)));
    connect(controller, SIGNAL(speakPhreaseSampleOccur(int)), this, SLOT(onSpeakPhreaseSampleOccur(int)));

    connect(controller, SIGNAL(addFavoritesPositionOccur(TS_ROUTE_POS)), this, SLOT(onAddFavoritesPosition(TS_ROUTE_POS)));
    connect(controller, SIGNAL(rePlayVoiceGuidanceOccur()), this, SLOT(onRePlayVoiceGuidance()));
    connect(controller, SIGNAL(languageChanged(QString)), this, SLOT(onLanguageChanged(QString)));
    connect(controller, SIGNAL(resetToFactorySettings()), this, SLOT(onResetToFactorySettings()));
    connect(controller, SIGNAL(dateFormatChanged(QString)), this, SLOT(onDateFormatChanged(QString)));
    connect(controller, SIGNAL(timeFormatChanged(QString)), this, SLOT(onTimeFormatChanged(QString)));
    connect(controller, SIGNAL(goToStandbyOccur()), this, SLOT(onGoToStandby()));
    connect(controller, SIGNAL(goToRunningOccur()), this, SLOT(onGoToRunning()));
    connect(controller, SIGNAL(goRouteSearchScreenOccur()), this, SLOT(onGoRouteSearchScreen()));
    connect(controller, SIGNAL(sdiAlertForMuteChanged(bool)), this, SLOT(onSDIAlertForMuteChanged(bool)));
    connect(mController->sessionManager()->qobject(), SIGNAL(sessionCreated(bool,int,int)), this, SLOT(onSessionCreated(bool,int,int)));
    connect(controller, SIGNAL(searchTextChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
    connect(controller, SIGNAL(autoCompleteTextChanged(QString)), this, SLOT(onAutoCompleteTextChanged(QString)));

    connect(mController->gpsManager()->qobject(), SIGNAL(satelliteUpdated(SKT::SatelliteDataPtr)),
            this, SLOT(onSatelliteUpdated(SKT::SatelliteDataPtr)));

    // 18.4.27 hskim map in cluster
    connect(controller, SIGNAL(startMapInCluster(int)), this, SLOT(onStartMapInCluster(int)));
    connect(controller, SIGNAL(stopMapInCluster()), this, SLOT(onStopMapInCluster()));
    connect(controller, SIGNAL(changeMapInClusterLayout(int)), this, SLOT(onChangeMapInClusterLayout(int)));


    connect(controller, SIGNAL(dialPOICallStatusChanged(char)), this, SLOT(onDialPOICallStatusChanged(char)));
    connect(controller, SIGNAL(callServiceStatusChanged(char)), this, SLOT(onCallServiceStatusChanged(char)));
    //connect(controller, SIGNAL(isHFPAvailable(bool)), this, SLOT(onIsHFPAvailable(bool)));

    connect(controller, SIGNAL(platformMediaMounted(const QString&)), this, SLOT(onMediaMounted(const QString&)));
    connect(controller, SIGNAL(platformMediaUnmounted()), this, SLOT(onMediaUnmounted()));
    connect(controller, SIGNAL(platformMediaRemounted(bool)), this, SIGNAL(mediaRemounted(bool)));
    connect(controller, SIGNAL(platformMediaUnmounted()), this, SLOT(onMediaUnmounted()));

    connect(controller, SIGNAL(languageIndexStatusChanged(char)), this, SLOT(onLanguageIndexStatusChanged(char)));
    connect(controller, SIGNAL(colorStateStatusChanged(char)), this, SLOT(onColorStateStatusChanged(char)));
    connect(controller, SIGNAL(mexModeStatusChanged(char)), this, SLOT(onMexModeStatusChanged(char)));

    connect(controller, SIGNAL(applyHardKeyOccur(int,int)), this, SLOT(onApplyHardKey(int,int)));

    //TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
    connect(controller, SIGNAL(setVoiceGuidanceOccur(bool)), this, SLOT(onSetVoiceGuidanceOccur(bool)));
    connect(controller, SIGNAL(setAlramGuidanceOccur(bool)), this, SLOT(onSetAlramGuidanceOccur(bool)));

    //TMCRSM-2452:VR로부터 좌표값으로 상세보기 화면으로 이동 명령을 받는 인터페이스
    connect(controller, SIGNAL(goPoiDetailScreenOccur(const QVariant&)), this, SLOT(onGoPoiDetailScreenOccur(const QVariant&)));
    
    connect(controller, SIGNAL(hmiStateChanged(uint)), this, SLOT(onHmiStateChanged(uint)));
    connect(controller, SIGNAL(clientStatusGuiSenseReady()), this, SLOT(onClientStatusGuiSenseReady()));
    connect(this, SIGNAL(appLostCswFocus()), controller, SLOT(onAppLostCswFocus()));
    connect(controller, SIGNAL(setPauseMap(bool)), this, SLOT(setPauseMap(bool)));
    connect(controller, SIGNAL(updateEditFieldText()), this, SLOT(onUpdateEditFieldText()));

    connect(controller, SIGNAL(naviSignalRFAntennaStatusOccur(uchar, uchar, uchar)), this, SLOT(onNaviSignalRFAntennaStatusOccur(uchar, uchar, uchar)));
    connect(controller, SIGNAL(showAdjustCurrentLocationScreen(TS_ROUTE_POS)), this, SLOT(onShowAdjustCurrentLocationScreen(TS_ROUTE_POS)));
#ifdef QT_DEBUG
    connect(controller, SIGNAL(debugKeyPressed(int)), this, SLOT(onDebugKeyPressed(int)));
    connect(controller, SIGNAL(showMonkeyTest()), this, SLOT(showMonkeyTest()));
#endif

    connect(controller, SIGNAL(spcxRgEnabled(bool)), this, SLOT(onSpcxRgEnabled(bool)));
    connect(controller, SIGNAL(channelReroutePolicyReceived(const QString&)), this, SLOT(onChannelReroutePolicyReceived(const QString&)));
    connect(controller, SIGNAL(nddsServerPolicyFinished(bool)), this, SLOT(onChannelServerPolicyFinished(bool)));

    connect(controller, SIGNAL(contextSwitchRequested(QVariant)), this, SLOT(onContextSwitchRequested(QVariant)));
    connect(controller, SIGNAL(contextStateAccepted()), this, SLOT(onContextStateAccepted()));
    connect(controller, SIGNAL(contextStateCompleted()), this, SLOT(onContextStateCompleted()));
    connect(controller, SIGNAL(platformNaviGadgetFocusStatusChanged(uint, bool)), this, SLOT(onPlatformNaviGadgetFocusStatusChanged(uint,bool)));
    connect(controller, SIGNAL(platformNaviGadgetFocusInteractionTypeChanged(uint, uint)), this, SLOT(onPlatformNaviGadgetFocusInteractionTypeChanged(uint, uint)));

    connect(this, SIGNAL(cswBackKeyAccepted()), controller, SLOT(onCswBackKeyAccepted()));

    setSunState(mController->sunState());
    // initDriveAutoConfig();  // fixed!, it got replaced with auto level map configuretion by engineer

    applySpeedLevel();
    applyVoiceGuidance();

    initVoiceList();
    applyVoice();

    onLanguageIndexStatusChanged(languageIndex);

    auto mexMode = extraData["mexMode"].toInt();
    onMexModeStatusChanged(mexMode);

    auto colorState = extraData["colorState"].toInt();
    onColorStateStatusChanged(colorState);

    if ( gpsRfSignal["signaled"].toBool() ) {
        onNaviSignalRFAntennaStatusOccur(gpsRfSignal["u8RFAntenna"].toUInt(), gpsRfSignal["u8RFAntennaStatus"].toUInt(), gpsRfSignal["u8ConnectionCurrent"].toUInt());
    }
}

NavigationController* NavigationControllerProxy::getControllerHandle()
{
    return mController;
}

QString NavigationControllerProxy::activeUserProfileName() const
{
    return mProfileManager->activeUserProfileName();
}

QString NavigationControllerProxy::lastUserProfileName() const
{
    return mProfileManager->lastUserProfileName();
}

void NavigationControllerProxy::addMapView( QObject* mapView )
{
    mMapViewList.append( mapView );
}

void NavigationControllerProxy::removeMapView( QObject* mapView )
{
    mMapViewList.removeOne( mapView );
}

void NavigationControllerProxy::applyCommand(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData[CommonConstants::Type].toInt();
    switch(type) {
    case NavigationControllerProtocol::ApplySpeedLevel:
        if (reqData.contains(SettingConstants::SpeedResponseMapUse)) {
            bool use = reqData[SettingConstants::SpeedResponseMapUse].toBool();
            applySpeedLevel(use);
        } else {
            applySpeedLevel();
        }
        break;
    case NavigationControllerProtocol::ApplyCameraGuidance:
        applyCameraGuidance();
        //sendNotiGetVoiceGuidanceActive();
        break;
    case NavigationControllerProtocol::ApplyAttentionGuidance:
        applyAttentionGuidance();
        //sendNotiGetVoiceGuidanceActive();
        break;
    case NavigationControllerProtocol::ApplyRoadViolation:
        applyRoadViolation();
        break;
    case NavigationControllerProtocol::ApplySafeDrivingSoundAtMute:
        applySafeDrivingSoundAtMute();
        break;
    case NavigationControllerProtocol::ApplyVoiceGuidance:
        applyVoiceGuidance();
        break;
    case NavigationControllerProtocol::ApplyFontScale:
        applyFontScale();
        break;
    case NavigationControllerProtocol::ApplyVoice:
        applyVoice();
        break;
    case NavigationControllerProtocol::ApplyNaviVolume: {
            int volume = reqData[SettingConstants::NaviVolume].toInt();
            applyNaviVolume(volume);
        }
        break;
    case NavigationControllerProtocol::ApplyNaviMute: {
            bool mute = reqData[SettingConstants::NaviMute].toBool();
            applyNaviMute(mute);
        }
        break;        
    case NavigationControllerProtocol::ApplyAutoFuelType: {
            int fueltype = reqData[SettingConstants::FuelType].toInt();
            applyAutoFuelType(fueltype);
        }
        break;


    case NavigationControllerProtocol::ChangeLanguage: {
            auto lang = reqData[LangConstants::Lang].toString();
            changeLanguage(lang);
        }
        break;
     case NavigationControllerProtocol::CallPhoneNumber: {
            QString  phoneNumber = reqData[SearchConstants::PhoneNumber].toString();
            callPhoneNumber(phoneNumber);
            // deprecated which directly call to platform, by lckxx 
            // int value = callPhoneNumber(phoneNumber) ?  SearchConstants::CallStatusSuccess :  SearchConstants::CallStatusFail;
            // sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::CallStatus, value}});
        }
        break;
     case NavigationControllerProtocol::CreateSessionByOTP: {
            if (mCountAuthOTPFail >= MAXCOUNT_AUTHOTPTRY)
                break;
            QString otpCode = reqData[CommonConstants::Value].toString();
            mController->sessionManager()->createSessionByOTP(otpCode);
        }
        break;
    case NavigationControllerProtocol::ClearSession:
        clearSession();
        break;
    case NavigationControllerProtocol::ClearAllSession:
        clearAllSession();
        break;
    case NavigationControllerProtocol::ClearSessionByProfileId: {
            int id = reqData[CommonConstants::Value].toInt();
            mController->sessionManager()->clearSessionByProfileId(id);
         }
         break;
    case NavigationControllerProtocol::RequestRegisterSendToCar:
        mProfileManager->requestRemoteRegisterSendToCar(reqData);
        break;
    case NavigationControllerProtocol::RequestFindSendToCar:
        mProfileManager->requestRemoteFindSendToCar();
        break;
    case NavigationControllerProtocol::RequestRemoveSendToCar:
        mProfileManager->requestRemoteRemoveSendToCar(reqData);
        break;
    case NavigationControllerProtocol::RequestResetCurrentUserData: {        
          mProfileManager->onResetUserData(mProfileManager->activeUserProfileId());
        }
        break;
    case NavigationControllerProtocol::RequestResetAllUserData: {
            for(int i=0; i<6; i++) {
                mProfileManager->onResetUserData(i);
            }
        }
        break;
     case NavigationControllerProtocol::ApplyVoiceGuidanceVocalPoint: {
            bool send = reqData[SettingConstants::Value].toBool();
            applyVoiceGuidanceVocalPoint(send);
        }
        break;

     //TMCRSM-2437:VR로부터 검색 후 검색결과 요청
     case NavigationControllerProtocol::ResponsedFreeTextSearchResult: {
            QVariantMap  response = reqData[SearchConstants::Request].toMap();
            QVariant results = QVariant::fromValue(reqData[SearchConstants::Result]);
            QString method = response[SearchConstants::Response].toString();

            auto requestRoute = false;

            if (method == CONTACT_GADGET_KEY) {
                // Contact가젯 검색 결과인 경우
                auto vList = results.value<QVariantList>();
                if (vList.count() == 1) {

                    auto data = vList.at(0).value<QVariantList>();
                    auto poiId = data[SearchConstants::ResultPOIID].toInt();
                    auto rpFlag = data[SearchConstants::ResultRpFlag].toInt();
                    auto x = data[SearchConstants::ResultX].toInt();
                    auto y = data[SearchConstants::ResultY].toInt();
                    auto navSeq = data[SearchConstants::ResultNavseq].toInt();
                    auto name = data[SearchConstants::ResultName].toString();
                    auto address = data[SearchConstants::ResultAddress].toString();

                    QVariantMap body;
                    body[SearchConstants::Clear] = true;
                    sendNotification(RouteConstants::SetDeparture, body);
                    body.clear();

                    body[CommonConstants::X] = x;
                    body[CommonConstants::Y] = y;
                    body[CommonConstants::Name] = name;
                    body[CommonConstants::Address] = address;
                    body[SearchConstants::PoiID] = poiId;
                    body[SearchConstants::NavSeq] = navSeq;
                    body[SearchConstants::RpFlag] = rpFlag;

                    sendNotification(RouteConstants::SetArrival, body);
                    sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
                    requestRoute = true;
                }
            } else {
                // VR 검색 결과인 경우
                // 검색 결과창을 닫기 전에 화면에 표시중인 마커를 제거하도록 요청
                sendNotification(SearchConstants::RequestRemoveMarker);

                // 검색 결과 창을 닫는다.
                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::SearchResultView)}});
            }

            if (!requestRoute){
                QMetaObject::invokeMethod(mController->qobject(), method.toStdString().c_str(),
                Q_ARG(const QVariant&, results));
            } else {
                // 검색 결과창을 닫기 전에 화면에 표시중인 마커를 제거하도록 요청
                sendNotification(SearchConstants::RequestRemoveMarker);

                // 검색 결과 창을 닫는다.
                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::SearchResultView)}});
            }
        }
        break;
    }
}

void NavigationControllerProxy::requestCommand(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData[CommonConstants::Type].toInt();
    switch(type) {
     case NavigationControllerProtocol::GetAppVersion:
        getAppVersion(reqData);
        break;
     case NavigationControllerProtocol::GetGPSInfo:
        getGPSInfo();
        break;
     case NavigationControllerProtocol::GetNaviVolume:
        getNaviVolume();
        break;
     case NavigationControllerProtocol::GetNaviMute:
        getNaviMute();
        break;
     case NavigationControllerProtocol::GetDateFormat:
        getDateFormat();
        break;
     case NavigationControllerProtocol::GetTimeFormat:
        getTimeFormat();
        break;

     case NavigationControllerProtocol::GetBluetoothStatus:
        sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::BluetoothStatus, mIsBluetooth}});
        break;

     case NavigationControllerProtocol::GetAuthOTPstatus: {
            QByteArray sessionKey = mController->sessionManager()->sessionKey();
            bool bAuthOTP = (sessionKey.size() > 0) ? true : false;
            sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::AuthOTPStatus, bAuthOTP}});
       }
       break;

     case NavigationControllerProtocol::GetDeviceId:
        sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::DeviceId, mController->getDeviceId()}});
        break;


    case NavigationControllerProtocol::GetAutoFuelType:
       sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::FuelType, mController->getCarconfigurationValue(VIHICLE_FUEL)}});
       break;

    case NavigationControllerProtocol::GetAuthOTPTryAvailable: {
        bool bAvailable = true;
        if (mCountAuthOTPFail >= MAXCOUNT_AUTHOTPTRY) {
            if (mTime.msecsTo(QTime::currentTime()) >= WAIT_AUTHRETRYTIME)  {
               mCountAuthOTPFail = 0;
               bAvailable = true;
            }else {
                bAvailable = false;
            }
        }
        sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::AuthOTPTryAvailable, bAvailable}});
      }
      break;

    case NavigationControllerProtocol::GetLang:
       sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{CommonConstants::Lang, mLang}});
       break;

    case NavigationControllerProtocol::GetSyncedProfileId: {
            int i = 0;
            for(i=0; i<6; i++) {
                if (QByteArray() != mController->sessionManager()->sessionKey(i))
                    break;
            }
            if (i >= 6) {
                i = -1;
            }
            sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::SyncedProfileId, i}});
        }
        break;
    case NavigationControllerProtocol::GetCurrentProfileId: {
            sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::CurrentProfileId, mProfileManager->activeUserProfileId()
}});
        }
        break;
     case NavigationControllerProtocol::GetMapError: {
            int err = mProperty->getErrorMap();
            sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{CommonConstants::NoMapData, err}});
        }
        break;

    case NavigationControllerProtocol::GetSyncedProfileName: {
            QString strProfileName = "";
            int i = 0;
            for(i=0; i<6; i++) {
                if (QByteArray() != mController->sessionManager()->sessionKey(i)) {
                    #ifdef BUILD_TARGET
                        strProfileName = lastUserProfileName();
                    #else
                        if (i) {
                            strProfileName = QString("User %1").arg(i);
                        } else {
                            strProfileName = QStringLiteral("Guest");
                        }
                    #endif
                    break;
                }
            }
            sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::SyncedProfileName, strProfileName}});
        }
        break;
    }
}

void NavigationControllerProxy::setSunState(SKT::NavigationController::SunState sunState)
{
    T_DEBUG( QString("#1 sunState: %1").arg(sunState) );
    setNightMode(sunState == SKT::NavigationController::SunNight);

    QVariantMap m;
    m["sunState"] = sunState;
    sendNotification(NavigationControllerConstants::ResponseSkyState, m);

    sendNotification(NavigationControllerConstants::RequestSkyUpdate);
}

void NavigationControllerProxy::setWeatherState(SKT::NavigationController::WeatherState weatherState)
{
    QVariantMap m;
    m["weatherState"] = weatherState;
    sendNotification(NavigationControllerConstants::ResponseSkyState, m);

    sendNotification(NavigationControllerConstants::RequestSkyUpdate);
}

void NavigationControllerProxy::setNightMode(bool nightMode)
{
    mNightMode = nightMode ? NightMode::Night : NightMode::Day;
}

int NavigationControllerProxy::getNightMode()
{
    return mNightMode;
}

bool NavigationControllerProxy::getAppVersion(QVariantMap data)
{
    QString appVersion = mProperty->getTmapAppVersion();
    QString mapVersion = mProperty->getTmapMapVersion();
    QString sdiVersion = mProperty->getTmapSDIVersion();
    QString localMapVersion = mProperty->getTmapLocalMapVersion();
    QString localNetworkVersion = mProperty->getTmapLocalNetworkVersion();
    QString localPoiVersion = mProperty->getTmapLocalPoiVersion();

    data[SettingConstants::AppVersion] = appVersion;
    data[SettingConstants::MapVersion] = mapVersion;
    data[SettingConstants::SDIVersion] = sdiVersion;
    data[SettingConstants::LocalMapVersion] = localMapVersion;
    data[SettingConstants::LocalNetworkVersion] = localNetworkVersion;
    data[SettingConstants::LocalPoiVersion] = localPoiVersion;

    sendNotification(NavigationControllerConstants::ResponseController, data);

    return true;
}

static void decimalDegreesToDegreesMinutesSeconds(double degrees, int &d, int &m, double &s)
{
    double int_part;
    double fra_part;

    fra_part = modf(degrees, &int_part);
    d = int_part;
    fra_part = modf(fra_part * 60.0f, &int_part);
    m = int_part;
    s = fra_part * 60.0f;
}

bool NavigationControllerProxy::getGPSInfo()
{
    QVariantMap m;
    SKT::SatelliteDataPtr  satellite = mSatellite;
    if(!satellite) {
        sendNotification(NavigationControllerConstants::ResponseController,  m  );
        return false;
    }

    QDateTime localTime = satellite->localDateTime();
    QDate date = localTime.date();
    QTime time = localTime.time();

    //날짜
    m[SettingConstants::GPSInfoDateY    ] = date.year();
    m[SettingConstants::GPSInfoDateM    ] = date.month();
    m[SettingConstants::GPSInfoDateD    ] = date.day();

    //시간
    m[SettingConstants::GPSInfoTimeH    ] = time.hour();
    m[SettingConstants::GPSInfoTimeM    ] = time.minute();
    m[SettingConstants::GPSInfoTimeS    ] = time.second();
    //위성수
    m[SettingConstants::GPSInfoSVCnt    ] = satellite->satellitesInUse;

    int degrees, minutes;
    double seconds;
    //경도
    decimalDegreesToDegreesMinutesSeconds(satellite->longitude, degrees, minutes, seconds);
    m[SettingConstants::GPSInfoCpxD     ] = degrees;
    m[SettingConstants::GPSInfoCpxM     ] = minutes;
    m[SettingConstants::GPSInfoCpxS     ] = seconds;
    // 위도
    decimalDegreesToDegreesMinutesSeconds(satellite->latitude, degrees, minutes, seconds);
    m[SettingConstants::GPSInfoCpyD     ] = degrees;
    m[SettingConstants::GPSInfoCpyM     ] = minutes;
    m[SettingConstants::GPSInfoCpyS     ] = seconds;
    //측위방식
    m[SettingConstants::GPSInfoMode     ] = satellite->fixType;
    //속도
    m[SettingConstants::GPSInfoSpeed    ] = int(satellite->speed * NMEA_KNOT_TO_KM_PER_HOUR);
    //고도
    m[SettingConstants::GPSInfAltitude  ] = satellite->altitude;
    //HDO
    m[SettingConstants::GPSInfodHdop    ] = satellite->HDOP;


    //위성 정보
    QVariantList list;
    for (const NMEA::NmeaSatellite& sat : satellite->satellites) {
        if (sat.tracked) {
            QVariantMap arg;

            arg[SettingConstants::GPSSVinfoId] = sat.id;
            arg[SettingConstants::GPSSVinfoDb] = sat.snr;
            arg[SettingConstants::GPSSVinfoElevation] = sat.elevation;
            arg[SettingConstants::GPSSVinfoAzimuth]   = sat.azimuth;

            list.append(arg);
        }
    }
    m[SettingConstants::GPSSVInfo      ] = list;    // not sure that the count is same as satellitesInUse value.

    sendNotification(NavigationControllerConstants::ResponseController,  m  );
    return true;
}

void NavigationControllerProxy::sendNotiNaviVolume(int volume) {
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::NaviVolume, volume}});
}

void NavigationControllerProxy::sendNotiNaviMute(bool mute) {
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::NaviMute, mute}});
}

void NavigationControllerProxy::notifyUserDataModified(int dataType, const QVariant& data) {
    if (mIsOnline && mChannelServerPolicyFinished)
       mProfileManager->onUserDataModified(dataType, data);
}

bool NavigationControllerProxy::getNaviVolume()
{
    int naviVolume = mController->navigationVolume();
    sendNotiNaviVolume(naviVolume);

    QVariantMap body;
    body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapVolume );
    body.insert( QStringLiteral("propertyValue"),QVariant::fromValue(naviVolume));
    sendNotification( NavigationPropertyConstants::setNavigationProperty, body );

    return true;
}

bool NavigationControllerProxy::getNaviMute()
{
    bool mute = mController->muteState(1);
    sendNotiNaviMute(mute);
    return true;
}

// 17.12.8 hskim not used
bool NavigationControllerProxy::applyNightMode(bool automatically /*=false*/)
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapMain, SettingConstants::NightMode );
    auto value = setting->value().toInt();
    bool nightMode = false;
    if (value == NightMode::Auto) {
        int value  = getNightMode();
        if(!value)
            return false;
        nightMode = (value == NightMode::Night) ? true : false;
    } else {
        if (automatically)
            return false;
        nightMode = (value == NightMode::Night) ? true : false;
    }
    // 17.12.8 hskim
    for ( auto mapView : mMapViewList ) {
        //auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
        mapView->setProperty("nightMode", QVariant::fromValue(nightMode));
    }
    return true;
}

bool NavigationControllerProxy::applySpeedLevel()
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapSubSpeedResponse , SettingConstants::SpeedResponseMapUse );
    auto sppeedZoomLevel = setting->value().toBool();
    return applySpeedLevel(sppeedZoomLevel);
}

bool NavigationControllerProxy::applySpeedLevel(bool sppeedZoomLevel) {
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapSubSpeedResponse , SettingConstants::Distance  );
    auto distance = setting->value().toInt();

    VSM_NsTmapLevelConfig config;
    memset(&config, 0, sizeof(VSM_NsTmapLevelConfig));
    switch(distance){
    case MapMode::Far:
        config.m_n80kmLevel = 90;
        config.m_n40to80kmLevel = 100;
        config.m_n40kmLevel = 110;
        config.m_n80kmLevel_C = 90;
        config.m_n40to80kmLevel_C = 100;
        config.m_n40kmLevel_C = 110;
        break;
    case MapMode::Normal:
        config.m_n80kmLevel = 95;
        config.m_n40to80kmLevel = 105;
        config.m_n40kmLevel = 115;
        config.m_n80kmLevel_C = 95;
        config.m_n40to80kmLevel_C = 105;
        config.m_n40kmLevel_C = 115;
        break;
    case MapMode::Near:
        config.m_n80kmLevel = 100;
        config.m_n40to80kmLevel = 110;
        config.m_n40kmLevel = 120;
        config.m_n80kmLevel_C = 100;
        config.m_n40to80kmLevel_C = 110;
        config.m_n40kmLevel_C = 120;
        break;
    default:
        return false;
    }
    config.m_bSpeedZoomLevel = sppeedZoomLevel; // Tmap 사양의 설정 사용 On/Off
    config.m_bAutoZoomInOutMode = VSM_TRUE; // must be TRUE to get effect for the drvie auto config, by lckxx 2018.6.25


     // 속도반응 맵 사용
     bool success = VSMNavi_SetLevelConfig(config);
     if (!success)
        T_DEBUG(QStringLiteral("set levelconfig fails ! "));
     
    // 속도반응 레벨 설정
    extern bool applyVsmMapAutoConfig(const QVariantMap& config);
    success = applyVsmMapAutoConfig(QVariantMap());
    if (!success)
        T_DEBUG(QStringLiteral("set drive autoconfig fails !"));

    return success;
}

bool NavigationControllerProxy::applyCameraGuidance(bool send /*=true*/) {
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    QList<QString> listGuidCamera;
    listGuidCamera.append(SettingConstants::CrackdownOnSpeedViolation);               // 0.과속단속
    listGuidCamera.append(SettingConstants::SectionControl);                          // 1.구간단속
    listGuidCamera.append(SettingConstants::SignalControl);                           // 2.신호/과속단속
    listGuidCamera.append(SettingConstants::DetachableCrackdown);                     // 3.이동식단속
    listGuidCamera.append(SettingConstants::CrackdownWithBusExclusiveCar);            // 4.버스전용차로단속
    listGuidCamera.append(SettingConstants::ParkingControl);                          // 5.주차단속
    listGuidCamera.append(SettingConstants::InterruptRegulation);                     // 6.끼어들기단속
    listGuidCamera.append(SettingConstants::MonitoringOfRoadShoulder);                // 7.갓길감시단속
    listGuidCamera.append(SettingConstants::CollectingTrafficInformation);            // 8.교통정보수집
    listGuidCamera.append(SettingConstants::ControlOfVariableVehicles);               // 9.가변차로단속
    memset(&mRgConfig.bAndoCameraType[0], 0, sizeof(mRgConfig.bAndoCameraType));
    for (int i=0; i< listGuidCamera.count(); i++) {
        auto setting = proxy->setting( SettingConstants::GuidSubCamera, listGuidCamera.at(i) );
        auto value = setting->value().toInt();
        mRgConfig.bAndoCameraType[i] = value;
    }
    if( send)
        return VSMNavi_SetGuidanceConfig(mRgConfig);
    return true;
}

bool NavigationControllerProxy::applyAttentionGuidance(bool send /*=true*/)
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    QList<QString> listGuidAttention;
    listGuidAttention.append(SettingConstants::FrequentSignalViolation);               //  0.신호 위반 단속
    listGuidAttention.append(SettingConstants::InterruptRegulation);                   //  1.끼어들기 단속
    listGuidAttention.append(SettingConstants::ParkingControl);                        //  2.주정차 단속
    listGuidAttention.append(SettingConstants::CrackdownWithBusExclusiveCar);          //  3.버스전용차로 단속
    listGuidAttention.append(SettingConstants::MonitoringOfRoadShoulder);              //  4.갓길 구간 단속
    listGuidAttention.append(SettingConstants::SpeedBump);                             //  5.과속 방지턱
    listGuidAttention.append(SettingConstants::ChildProtectedArea);                    //  6.어린이 보호구역
    listGuidAttention.append(SettingConstants::AccidentFrequent);                      //  7.사고 다발 구간
    listGuidAttention.append(SettingConstants::SharpCurve);                            //  8.급커브 구간
    listGuidAttention.append(SettingConstants::FogCaution);                            //  9.안개 주의 구간
    listGuidAttention.append(SettingConstants::CollectingTrafficInformation);          // 10.교통정보 수집 구간
    listGuidAttention.append(SettingConstants::SleepShelter);                          // 11.졸음 쉼터
    listGuidAttention.append(SettingConstants::AttentionToWildAnimals);                // 12.야생동물주의
    listGuidAttention.append(SettingConstants::RailroadXing);                          // 13.철길건널목
    listGuidAttention.append(SettingConstants::SteepSlopeSection);                     // 14.급경사구간
    listGuidAttention.append(SettingConstants::OneWay);                                // 15.일방통행
    listGuidAttention.append(SettingConstants::TrafficJamArea);                        // 16.교통혼잡지역
    listGuidAttention.append(SettingConstants::ChangeSectionOfCar);                    // 17.차로변경구간
    listGuidAttention.append(SettingConstants::ChargingStation);                       // 18.충전소
    listGuidAttention.append(SettingConstants::Tunnel);                                // 19.터널
    listGuidAttention.append(SettingConstants::RestArea);                              // 20.휴게소
    listGuidAttention.append(SettingConstants::FeeStation);                            // 21.톨게이트
    listGuidAttention.append(SettingConstants::BadSight);                              // 22.시야불량
    memset(&mRgConfig.bAndoEtcType[0], 0, sizeof(mRgConfig.bAndoEtcType));
    QString section;
    for (int i=0; i< listGuidAttention.count(); i++) {
        if (listGuidAttention.at(i) == SettingConstants::InterruptRegulation ||
            listGuidAttention.at(i) == SettingConstants::ParkingControl ||
            listGuidAttention.at(i) == SettingConstants::CrackdownWithBusExclusiveCar ||
            listGuidAttention.at(i) == SettingConstants::MonitoringOfRoadShoulder ||
            listGuidAttention.at(i) == SettingConstants::CollectingTrafficInformation){
            section = SettingConstants::GuidSubCamera;
        } else {
            section = SettingConstants::GuidSubAttentionSection;
        }
        auto setting = proxy->setting( section, listGuidAttention.at(i) );
        auto value = setting->value().toInt();
        mRgConfig.bAndoEtcType[i] = value;
    }
    if (send)
        return VSMNavi_SetGuidanceConfig(mRgConfig);
    return true;
}

bool NavigationControllerProxy::applyRoadViolation(bool send /*= true*/)
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::GuidMain, SettingConstants::RoadViolation );
    auto value = setting->value().toInt();

    // - 고정식/이동식 과속카메라 안내거리  0 , 1, 2 (1Km,600m,300m)
    mRgConfig.nAndoCameraDist                  = value;

    if (send) {
        return VSMNavi_SetGuidanceConfig(mRgConfig);
    }
    return true;
}

bool NavigationControllerProxy::getSafeDrivingSoundAtMute()
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::GuidMain, SettingConstants::SafeDrivingSoundAtMute );
    return setting->value().toBool();
}


void NavigationControllerProxy::showSysVolumeHMI(bool enabled)
{
    QMetaObject::invokeMethod(mController->qobject(), "naviVolumeTouched", Q_ARG(bool, enabled));
}

bool NavigationControllerProxy::applySafeDrivingSoundAtMute(bool send /*= true*/)
{    
    qDebug() << " applySafeDrivingSoundAtMute : " << send;
    bool mute = mController->muteState(1);
    bool active = getSafeDrivingSoundAtMute();
    if (mute) {
        mRgConfig.nAndoOverSpeedVoice = active;
    } else {
        mRgConfig.nAndoOverSpeedVoice = 1;
    }
    sendNotiSetOverSpeedSoundAlertInfoActive(mRgConfig.nAndoOverSpeedVoice);
    if (send) {
        return VSMNavi_SetGuidanceConfig(mRgConfig);
    }
    return true;
}

bool NavigationControllerProxy::applyVoiceGuidanceVocalPoint(bool send /*= true*/)
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto settingKm2 = proxy->setting( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::Km2 );
    auto valueKm2 = settingKm2->value().toInt();
    auto settingKm1 = proxy->setting( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::Km1 );
    auto valueKm1 = settingKm1->value().toInt();
    auto settingM600 = proxy->setting( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::M600 );
    auto valueM600 = settingM600->value().toInt();
    auto settingM300 = proxy->setting( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::M300 );
    auto valueM300 = settingM300->value().toInt();

    // - 고속도로 2Km / 1Km / 500m / 300m 이전
    mRgConfig.bGuideHighwayDist[0] = valueKm2;
    mRgConfig.bGuideHighwayDist[1] = valueKm1;
    mRgConfig.bGuideHighwayDist[2] = valueM600;
    mRgConfig.bGuideHighwayDist[3] = valueM300;

    // - 일반도로 1Km / 500m / 300m 이전
    mRgConfig.bGuideNormalwayDist[0] = valueKm1;
    mRgConfig.bGuideNormalwayDist[1] = valueM600;
    mRgConfig.bGuideNormalwayDist[2] = valueM300;
    if (send)
        return VSMNavi_SetGuidanceConfig(mRgConfig);
    return true;
}


bool NavigationControllerProxy::applyVoiceGuidance()
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();

    // 차종( 0 : 1종  1: 2종  2: 3종   3: 4종  4: 5종  5: 6종 )
    //mRgConfig.nVechileType						= 0;
    // 경로상 POI 안내종류 설정 ( 0: 휘발유 1: 경유 2: LGP )
    //mRgConfig.nGuidePoiOnrouteKind				= 0;
    // 경로상 POI 안내종류 설정 ( 0: 전체 1: SK에너지 2: S-Oil 3: 오일뱅크 4: GS칼텍스 )
    //mRgConfig.nGuidePoiOnrouteStationKind		= 0;

    //////////////////////////////////////////////////////////////////////////
    // 안내 스팩

    // 회전 안내시 지점별 안내 여부
    // - 고속도로 2Km / 1Km / 500m / 300m 이전
    //memset(&mRgConfig.bGuideHighwayDist[0], 0, sizeof(mRgConfig.bGuideHighwayDist));
    // - 일반도로 1Km / 500m / 300m 이전
    //memset(&mRgConfig.bGuideNormalwayDist[0], 0, sizeof(mRgConfig.bGuideNormalwayDist));
    applyVoiceGuidanceVocalPoint(false);

    // 회전 안내시 지점별 안내 거리
    // - 안내 거리 지정. ( 단위 m ) 인덱스 0:고속도로, 1:일반도로
    //memset(&mRgConfig.nGuideDist[0][0][0], 0, sizeof(mRgConfig.nGuideDist));

    // 기타 안내 여부
    // - 터널구간 음성 안내
    //mRgConfig.bGuideTunnelVoice				= true;
    // - 휴게소 음성 안내
    //mRgConfig.bGuideRestAreaVoice				= true;
    // - 현재도로 음성 안내
    applySafeDrivingSoundAtMute(false);
    // - IC/JC 음성 안내
    //mRgConfig.bGuideIcJcVoice					= true;
    // - 고속도로 지정차로 이미지 안내
    //mRgConfig.bGuideHighwayLaneImg			= true;
    // - 고속도로 지정차로 음성 안내
    //mRgConfig.bGuideHighwayLaneVoice			= true;

    // 경로상 POI 안내
    //mRgConfig.bGuidePoiOnroute				= true;

    //////////////////////////////////////////////////////////////////////////
    // 안전운전

    // 안전운전 음성 안내
    // - 안도 속도 카메라 음성 안내 (0: 안내안함, 1: 항상 안내, 2: 초과 시 안내)
    //mRgConfig.nAndoCameraVoice				= 1;
    // - 기타 안전운전 음성안내
    //mRgConfig.bAndoEtcVoice					= 1;
    // - 규정 속도 초과음 안내 ( 0: 안내안함, 1: 규정속도 초과시, 2:규정속도+10%초과시 )
    mRgConfig.nAndoOverSpeedVoice				= 1;

    // 안전운전 표출
    // - 카메라 항목 안내 설정
    applyCameraGuidance(false);
    // - 기타 안전운전 안내 (교통수집,일반통행,사고다발,급커브...)
    applyAttentionGuidance(false);

    // 고정식 과속카메라 안내 (0: 해제, 1: 설정, 2:초과시)
    //mRgConfig.nAndoFixCamera					= 1;
    // 이동식 과속카메라 안내 (0: 해제, 1: 주간/야간, 2: 주간)
    //mRgConfig.nAndoMoveCamera					= 1;
    //T map 4.1 고정식(신호과속, 과속)&이동식 과속 카메라 경고음 설정 추가 : 2013.05.15 flintlock
    // - 고정식 과속카메라 경고음 설정
    mRgConfig.bAndoFixCameraOverSpeed         = true;
    // - 이동식 과속카메라 경고음 설정
    mRgConfig.bAndoMoveCameraOverSpeed        = true;
    // - 고정식 과속카메라 안내거리 (1Km,600m,300m)
    //memset(&mRgConfig.bAndoFixCameraDist[0], 0, sizeof(mRgConfig.bAndoFixCameraDist));
    // - 이동식 과속카메라 안내거리 (1Km,600m,300m)
    //memset(&mRgConfig.bAndoMoveCameraDist[0], 0, sizeof(mRgConfig.bAndoMoveCameraDist));
    // 안전운전 안내시 지점별 안내 거리
    // - 안전운전 거리 지정. ( 단위 m ) 기본값 2000 / 1000 / 600 / 300 / 0 - 사용안함
    //memset(&mRgConfig.nAndoDist[0][0], 0, sizeof(mRgConfig.nAndoDist));

    // 과속방지턱 안내 도로 종별 설정 (0 : 전체도로 1: 주요도로)
    //mRgConfig.nAndoSpeedBumpRoadtype           = 0;
    // 과속방지턱 안내 주/야간 설정 (0;항상 1:야간 2:해제)
    //mRgConfig.nAndoSpeedBumpWholeDay           = 0;


    //////////////////////////////////////////////////////////////////////////
    // 기타 음성 시나리오
    // - 시도 경계 안내
    //mRgConfig.bGuideCityPass                  = true;
    // - GPS 재 연결시 안내
    //mRgConfig.bGuideGPSConnect		        = true;
    // - GPS시계 알림 기능
    //mRgConfig.bGPSClock				        = true;
    // - 졸음운전 알림
    //mRgConfig.bNotiSleepiness		            = true;
    // - 에코 드라이브 급가속 음성 안내 여부
    //mRgConfig.bAcceleration			        = true;
    // - 에코 드라이브 공회전 음성 안내 여부
    //mRgConfig.bStoppage				        = true;
    // - GPS 광고 시보 안내
    //mRgConfig.bGPSClockAd                     = true;
    // - 지역경계 광고 안내
    //mRgConfig.bCityBoundaryAd		            = true;
    // - 고정식/이동식 과속카메라 안내거리
    applyRoadViolation(false);


    return VSMNavi_SetGuidanceConfig(mRgConfig);
}

bool NavigationControllerProxy::applyFontScale()
{
    VSM_Float reqValue = 0.0;
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapSubMapCharacter, SettingConstants::Character );
    auto value = setting->value().toInt();
    switch(value) {
    case FontSize::SMALL:
        reqValue = 0.8;
        break;
    case FontSize::NORMAL:
        reqValue = 1.0;
        break;
    case FontSize::LARGE:
        reqValue = 1.2;
        break;
    default:
        return false;
    }

    for ( auto mapView : mMapViewList ) {
        auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
        VSMMap_Setting_SetPOIScaleForNaviMode(handle, reqValue); // 17.12.8 hskim appy multi map view.
        VSMMap_Setting_SetPOIScaleForcedApply(handle, true); // 18.03.22 chunjae.lee  TMCRSM-563-주행중 지도 글씨 크기 관련 건

    }
    return true;
}

bool NavigationControllerProxy::applyNaviVolume(int volume)
{
    mController->setNavigationVolume(volume);
    return true;
}

bool NavigationControllerProxy::applyNaviMute(bool mute)
{
    QVariantMap body;
    body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapIsMute );
    body.insert( QStringLiteral("propertyValue"),QVariant::fromValue(mute));
    sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
    return mController->setMuteState(1, mute);
}

void NavigationControllerProxy::initVoiceList()
{
    std::vector<VSM_NsGuideWdbInfo> guideDB;
    mVoiceList.clear();

    if (VSMNavi_GetGuideDBList(guideDB)) {
        for (auto it = guideDB.begin(); it != guideDB.end(); ++it) {
            const VSM_NsGuideWdbInfo &p = *it;

            // must be downloaded
            if (p.isDownloaded) {
                QVariantMap v;
                v.insert(QStringLiteral("code"), QString::fromLatin1(p.code));
                v.insert(QStringLiteral("title"), QString::fromUtf8(p.title));
                v.insert(QStringLiteral("dicType"), p.supportedDicType);
                mVoiceList.push_back(v);

            }
        }
    }
}

bool NavigationControllerProxy::getOnlineState() const
{
    return mIsOnline;
}

void NavigationControllerProxy::changeNddsServer(bool commerce)
{
    connect(this,SIGNAL(nddsServerChanged(bool)),mController->qobject(), SLOT(changeNddsServer(bool)),Qt::DirectConnection);
    emit nddsServerChanged(commerce);
    disconnect(SIGNAL(nddsServerChanged(bool)));
}

void NavigationControllerProxy::changeVsmServer(int serverType)
{
    connect(this,SIGNAL(vsmServerChanged(int)),mController->qobject(), SLOT(changeVsmServer(int)),Qt::DirectConnection);
    emit vsmServerChanged(serverType);
    disconnect(SIGNAL(vsmServerChanged(int)));
}

void NavigationControllerProxy::changeAvNextServer(int serverType)
{
   connect(this,SIGNAL(avnextServerChanged(int)),mController->qobject(), SLOT(changeAvNext(int)),Qt::DirectConnection);
    emit avnextServerChanged(serverType);
    disconnect(SIGNAL(changeAvNext(int)));
}

void NavigationControllerProxy::enableSunstateTimeset(bool enabled)
{
    if (enabled)
        connect(mController->qobject(), SIGNAL(setDaynightTimeset(const QList<QDateTime>&)),
            this, SLOT(updatedDaynightTimeset(const QList<QDateTime>&)));
    else
        disconnect(mController->qobject(), SIGNAL(setDaynightTimeset(const QList<QDateTime>&)));
}

void NavigationControllerProxy::updatedDaynightTimeset(const QList<QDateTime>& timeset)
{
    sendNotification(HiddenSettingConstants::UpdateDaynightTimeset,QVariant::fromValue(timeset));
}

void NavigationControllerProxy::showPopup(const QString &viewName)
{

}

void NavigationControllerProxy::hidePopup(const QString &viewName)
{

}

void NavigationControllerProxy::beep(int id)
{
    PlatformHelper::instance()->beep(id);
}

void NavigationControllerProxy::setZone1(const QVariant &v)
{
    QObject* object = dynamic_cast<QObject*>(mController);
    QVariant ret;
    QMetaObject::invokeMethod(object, "platformController", Q_RETURN_ARG(QVariant, ret));
    QObject* platformController = ret.value<QObject*>();
    if (platformController) {
        auto m = v.toMap();
        auto type = m["type"].toInt();
        QMetaObject::invokeMethod(platformController,
                                  "setupZone1",
                                  Q_ARG(uint, type),
                                  Q_ARG(QVariant, v));

    } else {
        qDebug() << "Platform controller is null";
    }
}

void NavigationControllerProxy::setZone3(const QVariant &v)
{
    QObject* object = dynamic_cast<QObject*>(mController);
    QVariant ret;
    QMetaObject::invokeMethod(object, "platformController", Q_RETURN_ARG(QVariant, ret));
    QObject* platformController = ret.value<QObject*>();
    if (platformController) {
        auto m = v.toMap();
        auto type = m["type"].toInt();
        QMetaObject::invokeMethod(platformController,
                                  "setupZone3",
                                  Q_ARG(uint, type),
                                  Q_ARG(QVariant, v));

    } else {
        qDebug() << "Platform controller is null";
    }
}

void NavigationControllerProxy::setKeyboard(const QVariant &v)
{
    QObject* object = dynamic_cast<QObject*>(mController);
    QVariant ret;
    QMetaObject::invokeMethod(object, "platformController", Q_RETURN_ARG(QVariant, ret));
    QObject* platformController = ret.value<QObject*>();
    if (platformController) {
        auto m = v.toMap();
        uchar type = m["type"].toUInt();
        QMetaObject::invokeMethod(platformController, "setupKeyboard", Q_ARG(uchar, type));
    }
}

QList<void*> NavigationControllerProxy::getMapviewHandle() const
{
    QList<void*> ret;
    for ( auto mapView : mMapViewList ) {
        auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
        ret.append( handle );
    }

    return ret;
}

const QList<QVariantMap>* NavigationControllerProxy::getVoiceList() const
{
    return &mVoiceList;
}

int NavigationControllerProxy::getSunState()
{
    return mController->sunState();
}

int NavigationControllerProxy::getWeatherState()
{
    return mController->weatherState();
}

void NavigationControllerProxy::sendNotiGetVoiceGuidanceActive()
{
#if 1
    sendNotiSetVoiceGuidanceActive(!mController->muteState(1));
#else
    bool isVoiceGuidanceActive  = false;
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    QList<SettingDtoPtr> ret = proxy->settings(SettingConstants::GuidSubCamera);
    for ( auto item : ret ) {
        if (item->value().toInt()) {
            isVoiceGuidanceActive  = true;
            break;
        }
    }

    if (!isVoiceGuidanceActive ) {
        ret = proxy->settings(SettingConstants::GuidSubAttentionSection);
        for ( auto item : ret ) {
           if (item->value().toInt()) {
               isVoiceGuidanceActive  = true;
               break;
           }
        }
    }
    sendNotiSetVoiceGuidanceActive(isVoiceGuidanceActive);
#endif
}

//TMCRSM-2389:내비게이션의 음성안내 설정 상태를system에 전달하는 인터페이스
void NavigationControllerProxy::sendNotiSetVoiceGuidanceActive(bool active)
{
    QVariantMap body;
    body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapVoiceGuidanceType );
    body.insert( QStringLiteral("propertyValue"),QVariant::fromValue(active));
    sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
}

//TMCRSM-2421:내비게이션의 경고음 안내 설정 상태를system에 전달하는 인터페이스
void NavigationControllerProxy::sendNotiSetOverSpeedSoundAlertInfoActive(bool active)
{
    QVariantMap body;
    body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapOverSpeedAlertActive);
    body.insert( QStringLiteral("propertyValue"),QVariant::fromValue(active));
    sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
}

bool NavigationControllerProxy::applyVoice()
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::GuidSubVoice, SettingConstants::Male );
    auto value = setting->value().toInt();
    QString code = value ? "MALE" : "FEMALE";

    if (mVoiceList.empty()) {
        return false;
    }

    VSM_Int32 guideIdx = 0;
    for (const QVariantMap &v : mVoiceList) {
        if (code == v[QStringLiteral("code")].toString()) {
            break;
        }
        ++guideIdx;
    }

    if (guideIdx >= mVoiceList.size()) {
        return false;
    }

    // load base guide db
    VSMNavi_LoadGuideDB(guideIdx);

    // load direction name db
    const QVariantMap &v = mVoiceList[guideIdx];
    int dicType = v[QStringLiteral("dicType")].toInt();
    switch (dicType) {
    case VSM_GuideDicMode_Streaming:
        VSMNavi_LoadGuideDicDB(VSM_GuideDicMode_Streaming);
        break;
    case VSM_GuideDicMode_TTS:
        VSMNavi_LoadGuideDicDB(VSM_GuideDicMode_TTS);
        break;
    default:
        VSMNavi_LoadGuideDicDB(VSM_GuideDicMode_None);
        break;
    }

    // TMCRSM-2157. for SDI spec
    // 19.04.26 ymhong
    auto ttsPath = Environment::getRGDataPath();
    VSMNavi_SetSoundPath((VSM_Char*)ttsPath.toStdString().c_str());

    // 보행자 사고다발 구간 입니다.
    VSM_Char szFileName[1024];
    sprintf(szFileName, "%s/%s", ttsPath.toStdString().c_str(), "SDI_PEDESTRIAN_ACCIDENT");
    const char tts_euckr[] = {
        '\xBA', '\xB8', '\xC7', '\xE0', '\xC0',
        '\xDA', '\x20', '\xBB', '\xE7', '\xB0',
        '\xED', '\xB4', '\xD9', '\xB9', '\xDF',
        '\x20', '\xB1', '\xB8', '\xB0', '\xA3',
        '\x20', '\xC0', '\xD4', '\xB4', '\xCF',
        '\xB4', '\xD9', '\x2E', '\x0'
    };
    qDebug() << szFileName;
    VSMNavi_SaveFileTTS((VSM_Char*)tts_euckr, (VSM_Char*)szFileName);

    return true;
}


void NavigationControllerProxy::navigationVolumeChanged(int volume)
{
    sendNotiNaviVolume(volume);
}

void NavigationControllerProxy::navigationAppeared(bool appeared)
{
    if (appeared) {
        if (mFirstAppear) {
            qDebug() << Q_FUNC_INFO << "(appared =" << appeared << ")";

            sendNotification(NavigationControllerConstants::FirstNavigationAppeared);
        }
        sendNotification(NavigationControllerConstants::NavigationAppeared);

        if (mFirstAppear && PlatformHelper::instance()->isRgContinueEnable()) {
            sendNotification(RgConstants::ShowRouteGuidanceContinuously);
        } else {
            runReservedAppSwitchJob();
        }

        // 미리 등록해둔 작업을 수행. (경로 이어하기)
        // 19.07.06 ymhong
        runReservedAppSwitchJob();

        qDebug() << "[CTX] navigationAppeared()";
        mIsResetNaviDone = false;
    } else {
        sendNotification(NavigationControllerConstants::NavigationDisappeared);
        sendNotification(ZoneConstants::UpdateSearchText, "");

        runReservedSystemSwitchJob();
        mIsInSwitchToApp = false;
    }
    mFirstAppear = false;

    // app이 forground로 표시될 때 zone1 표시 요청
    sendNotification(ZoneConstants::UpdateZone1, appeared);
    mForeground = appeared;
}

void NavigationControllerProxy::onlineChanged(bool online)
{
    mIsOnline = online;
    sendNotification(NavigationControllerConstants::ChangeOnlineStatus, online);
    sendNotification( UtilConstants::HideProgressIndicator );

    mProfileManager->onOnlineStateChanged(online);
}

//void NavigationControllerProxy::processCsw(int key)
//{
//    sendNotification(NavigationControllerConstants::ApplyCSW, key);
//}

void NavigationControllerProxy::changeLanguage(const QString& lang)
{
    mLang = lang;

    QmlAppHelper::instance()->setLanguage( lang );

    QVariantMap body;
    body.insert(LangConstants::Lang, lang);
    sendNotification(NavigationControllerConstants::ChangeLanguage, body);
}

void NavigationControllerProxy::registerMapInClusterRenderer(OffscreenRenderer* renderer)
{
    renderer->setImageStreamProvider( t_new_shared<MapInClusterImageStreamProvider>( mController ) ) ;
}

bool NavigationControllerProxy::registerGadgetRenderer(OffscreenRenderer* renderer, QObject* imageProvider, QQuickWindow* gadgetDebugWindow)
{
    bool ret = false;

    if ( mGadgetRenderer == nullptr ) {
        ret = true;
        mGadgetRenderer = renderer;
        mGadgetRenderer->setImageStreamProvider( t_new_shared<GadgetImageStreamProvider>( mController, imageProvider, gadgetDebugWindow ) ) ;
    }

    return ret;
}

void NavigationControllerProxy::registerOffscreenRenderer(OffscreenRenderer* renderer)
{
    // add...
}

void NavigationControllerProxy::mapViewInitialized()
{
    mController->onReadyToRunGadget();
}

void NavigationControllerProxy::showGadget(int type, bool show, bool asWindow)
{
    onShowGadget(type, show, asWindow);
}

static bool isTmapGadget(int id)
{
    switch (id) {
    case UIConstants::GadgetXXXL:
    case UIConstants::GadgetXXL:
    case UIConstants::GadgetXL:
    case UIConstants::GadgetLarge:
    case UIConstants::GadgetMedium:
    case UIConstants::GadgetSmall:
    case UIConstants::GadgetIconFuel:
    case UIConstants::GadgetIconParking:
    case UIConstants::GadgetIconAqi: // FineDust gadget (not AQI) 19.06.19 ymhong
        return true;

    default:
        return false;
    }
}

static bool isTmapGadgetSearchIconTouched(int id, int x, int y, bool isRg)
{
    QRect bb;
    switch (id) {
    case UIConstants::GadgetXXXL:
        bb.setRect(0, 705, 83, 83);
        break;
    case UIConstants::GadgetXXL:
        bb.setRect(0, 332, 63, 58);
        break;
    case UIConstants::GadgetXL:
        bb.setRect(0, 332, 63, 58);
        break;
    case UIConstants::GadgetLarge:
        if (isRg) {
            bb.setRect(0, 332, 63, 58);
        } else {
            bb.setRect((396/2)-32, 332, 63, 58);
        }
        break;
    case UIConstants::GadgetMedium:
        if (isRg) {
            bb.setRect(0, 133, 63, 58);
        } else {
            bb.setRect((396/2)-32, 133, 63, 58);
        }
        break;

    case UIConstants::GadgetSmall:
    default:
        return false;
    }

    auto ret = bb.contains(x, y);

    qDebug() << "isTmapGadgetSearchIconTouched(" << id << "," << x << "," << y << ", isRg:" << isRg << " = " << ret;

    return ret;
}

void NavigationControllerProxy::onShowGadget(int type, bool show, bool asWindow)
{
    //qDebug() << "[Gadget]NavigationControllerProxy::onShowGadget " << " type: " << type << " show: " << show << " asWindow: " << asWindow;

    if (isTmapGadget(type)) {
        QVariantMap body;
        body.insert( QStringLiteral("type"), type );
        body.insert( QStringLiteral("show"), show );
        body.insert( QStringLiteral("asWindow"), asWindow );
        sendNotification(ApplicationConstants::ShowGadget, body);

        if (type == mFocusWidget) {
            sendNotification(ApplicationConstants::FocusToWidget, QVariantMap{
                                 {CommonConstants::FocusIndex, type},
                                 {CommonConstants::Focus, true}
                             });
        }
    }
}

void NavigationControllerProxy::onGadgetActivated(uint activate,QList<int> instanceIds,int consumerId)
{

}

void NavigationControllerProxy::onGadgetTouched(uint touchState, uint xPos, uint yPos, uint timeStamp, int instanceId, bool byInterface)
{
    qDebug() << "[NavigationControllerProxy]onGadgetTouched: instanceId= " << instanceId << " touchState: " << touchState << " xPos: " << xPos << " yPos: " << yPos;

    // Tmap 가젯이 아니면 무시한다.
    if ( !isTmapGadget(instanceId) ) {
        return;
    }

    mTouchX = xPos;
    mTouchY = yPos;

    if (touchState == GADGET_TOUCH_PRESSED){
        mTouchState = touchState;

        // CSW interaction으로 pressed가 호출 되면 타이머 설정을 하지 않음.
        if (!byInterface) {
            mGadgetTouchTimer.start();
            sendGadgetPressedNotification(instanceId, true, true);
        }

    } else if (touchState == GADGET_TOUCH_RELEASED || touchState == GADGET_TOUCH_MOVE) {
        mGadgetTouchTimer.stop();

        auto focused = false;
        if (mWidgetFocusMap.contains(instanceId)) {
            focused = mWidgetFocusMap[instanceId];
        }

        if (!byInterface) {
            sendGadgetPressedNotification(instanceId, focused, false);
        }
    }

    qDebug() << "onGadgetTouched(" << instanceId << " : " << xPos << ", " << yPos << " = " << touchState;
    if(mTouchState != GADGET_SWITCH_SETTING)
    {
        if (touchState == GADGET_TOUCH_RELEASED) {
            mTouchState = touchState;

            if (isTmapGadget(instanceId) && !mIsInSwitchToApp) {
                // update mIsRg
                sendNotification(RgConstants::UpdateRouteGuidanceState);

                if (isTmapGadgetSearchIconTouched(instanceId, xPos, yPos, mIsRg)) {
                    sendNotification(ApplicationConstants::ResetNavi);
                    mIsResetNaviDone = true;
                    QTimer::singleShot(0, this, [&](){
                        mController->stopSendingGadgetImage();

                        sendNotification(Constants::CreateView, QVariantMap{
                                             {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SearchTabView)},
                                             {CommonConstants::Caller, CommonConstants::System}
                                         });
                        PlatformHelper::instance()->disableRgContinue();

                        qDebug() << "[NavigationControllerProxy]onGadgetTouched: switchToApp";
                        PlatformHelper::instance()->switchToApp();
                    });
                    sendNotification(SearchConstants::SetGadgetSearchMode, true);
                } else {
                    if (UIConstants::GadgetIconAqi == instanceId) {
                        mController->stopSendingGadgetImage();

                        // AQI가 아닌 Air quality gadget을 터치했을 때 이벤트가 들어옴
                        // 추후 제대로 연결될 때 까지 AQI화면으로의 전환을 막음
                        // 19.04.23 ymhong.

                        // 우분투에서는 히든 메뉴에서 테스트 할 수 있도록 수정
                        // 19.04.30 ymhong.

                        sendNotification(ApplicationConstants::ResetNavi);
                        mIsResetNaviDone = true;

                        // AQI 가젯 아이콘 클릭 시뮬레이션
                        qDebug() << "AQI Gadget Icon...";
                        sendNotification(Constants::CreateView, QVariantMap{
                                             {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SettingAQIdetailView)}
                                         });

                        PlatformHelper::instance()->disableRgContinue();
                        PlatformHelper::instance()->switchToApp();

                    } else {
                        mController->stopSendingGadgetImage();

                        switch (instanceId) {
                        case UIConstants::GadgetIconFuel:
                        case UIConstants::GadgetIconParking:
                            sendNotification(ApplicationConstants::ResetNavi);
                            mIsResetNaviDone = true;

                            PlatformHelper::instance()->disableRgContinue();
                            // app이 foreground가 되면 검색이 실행 되도록 한다.
                            // 19.07.06 ymhong
                            sendNotification(NavigationControllerConstants::ReserveAppSwitchJob, QVariantMap{
                                                 {CommonConstants::Command, NavigationControllerConstants::RequestAroundSearch},
                                                 {CommonConstants::Arg, instanceId}
                            });
                            break;

                        case UIConstants::GadgetXXXL:
                        case UIConstants::GadgetXXL:
                        case UIConstants::GadgetXL:
                        case UIConstants::GadgetLarge:
                        case UIConstants::GadgetMedium:
                        case UIConstants::GadgetSmall:
                            if (!mFirstAppear) {
                                sendNotification(ApplicationConstants::ResetNavi);
                                mIsResetNaviDone = true;
                            }
                            break;
                        }
                        // 가젯 터치 시 화면 전환 하도록 한다.
                        PlatformHelper::instance()->switchToApp();
                    }
                }
                mIsInSwitchToApp = true;
            }//isTmapGadget()
        }
    }
}

void NavigationControllerProxy::onGadgetLongTouched()
{
    mTouchState = GADGET_SWITCH_SETTING;

    qDebug() << "[NavigationControllerProxy]onGadgetTouched: switchToSettingView";
    PlatformHelper::instance()->switchToSettingView();
}

void NavigationControllerProxy::onGadgetFocused(uint instanceId, bool focus)
{
    qDebug() << "[GADGET] onGadgetFocused(" << instanceId << ", " << focus << ")";
    if (isTmapGadget(instanceId)) {
        sendNotification(ApplicationConstants::FocusToWidget, QVariantMap{
                             {CommonConstants::FocusIndex, instanceId},
                             {CommonConstants::Focus, focus}
                         });
        if (focus) {
            mFocusWidget = instanceId;
        } else {
            if (mFocusWidget == instanceId) {
                mFocusWidget = -1;
            }
        }
    }
}

void NavigationControllerProxy::onGadgetFocusInteraction(uint instanceId, int interactionType)
{
    enum FocusInteractionType {
        enPress = 0,
        enRelease,
        enLongPress = 4,
        enLongPressRelease
    };

    if (isTmapGadget(instanceId)) {
        auto focus = false;
        if (mWidgetFocusMap.contains(instanceId)) {
            focus = mWidgetFocusMap[instanceId];
        }
        switch (interactionType) {
        case enPress:
            qDebug() << "[Gadget] onGadgetFocusInteraction( enPress )";
            onGadgetTouched(GADGET_TOUCH_PRESSED, 1, 1, 0, instanceId, true);
            sendGadgetPressedNotification(instanceId, true, true);
            break;
        case enRelease:
            qDebug() << "[Gadget] onGadgetFocusInteraction( enRelease )";
            onGadgetTouched(GADGET_TOUCH_RELEASED, 1, 1, 0, instanceId, true);
            sendGadgetPressedNotification(instanceId, focus, false);
            break;
        case enLongPress:
            qDebug() << "[Gadget] onGadgetFocusInteraction( enLongPress )";
            onGadgetLongTouched();
            break;
        case enLongPressRelease:
            qDebug() << "[Gadget] onGadgetFocusInteraction( enLongPressRelease )";
            break;
        }
    }
}

void NavigationControllerProxy::onMapViewModeChanged(int mode)
{
    // 주어진 값으로 View mode 변경.
    sendNotification(MapConstants::ChangeViewMode, mode);
}
void NavigationControllerProxy::onViewLevelChanged(int level,int sub_level)
{
    // 주어진 값으로 View level 변경.
    sendNotification(MapConstants::ChangeViewLevel, QVariantMap{{"level", level}, {"subLevel", sub_level}});
}
void NavigationControllerProxy::onTrafficInfoScreenChanged(bool isShow)
{
    // 주어진 값으로 교통정보 표시 설정을 변경.

    sendNotification(NavigationControllerConstants::ShowTrafficInfo, isShow);
    for ( auto mapView : mMapViewList ) {
        VSMMap_SetShowTrafficInfoOnMap(mapView, isShow);
    }
}
void NavigationControllerProxy::onMoveToCurrentPosition()
{
    // 'Qt::Key_1'
    // 현위치로 이동
    sendNotification(MapConstants::ResetMapPosition);
}
void NavigationControllerProxy::onGoViaPointEditScreen()
{
    sendNotification(InterfaceConstants::ShowDeleteWaypointListScreen);
}

void NavigationControllerProxy::onShowOnMap(int worldX, int worldY, bool isKorea)
{
    for ( auto mapView : mMapViewList ) {
        // show toast message if there is not korea, it is requested from bosch 2019.3.8
        if (!isKorea) {
            QVariantMap toast;
            toast[CommonConstants::Message] = tr("유효하지 않은 지역입니다");
            sendNotification(ToastsConstants::ShowToast, QVariant(toast));
            return;
        }
        auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
        // fixed TMCRSM-2475  BEGIN
        QVariantMap oldMarker{{CommonConstants::Index,-999}};
        sendNotification(RouteConstants::RemoveSearchMarker, oldMarker );
        // InQuiry 모드 대신 MapCotnrol 모드의 자동 복원 등 UI Flow를 공유하자
        QVariantMap body;
        body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapControlView);
        sendNotification(Constants::CreateView, body);
        VSMMap_MapMoveTo(handle, worldX, worldY, true);
        QVariantMap marker;
        marker[CommonConstants::Index] = -999;
        marker[SearchConstants::FocusIndex] = -999;
        marker[CommonConstants::X] = worldX;
        marker[CommonConstants::Y] = worldY;
        sendNotification(RouteConstants::AddSearchMarker, marker );
        sendNotification(MapConstants::ResetMapViewTimer);
        // fixed TMCRSM-2475  END
    }
}

void NavigationControllerProxy::onRequestRoute(int option, bool apply)
{
    qDebug() << "[INTERFACE] NavigationControllerProxy::onRequestRoute(0)";
    QVariantMap m;
#if (0)
    // 테스트
    // 주어진 위치와 옵션으로 경로 안내를 요청함.

    // 출발지 현재 위치로 재설정 및 경유지 초기화
    m[SearchConstants::Clear] = true;
    m["byVr"] = true;
    sendNotification(RouteConstants::SetDeparture, m);

    m.clear();
    m["byVr"] = true;
    m[CommonConstants::X] = 66513375;
    m[CommonConstants::Y] = 19670485;
    sendNotification(RouteConstants::SetArrival, m);

    m.clear();
#endif

    // 주어진 옵션으로 경로 안내를 요청함.

    m["routeOption"] = option;
    m["apply"] = apply;
    sendNotification(RgConstants::StartRouteGuidanceByVr, m);
    if (!mForeground) 
       PlatformHelper::instance()->switchToApp();
       
}
void NavigationControllerProxy::onRequestRoute(TS_ROUTE_POS pos, int option, bool apply)
{
    qDebug() << "[INTERFACE] NavigationControllerProxy::onRequestRoute(1)";
    auto points = QList<TS_ROUTE_POS>{};
    points.append(pos);

    onRequestRoute(points, option, apply);
}

void NavigationControllerProxy::onRequestRoute(QList<TS_ROUTE_POS> wayPoints, int option, bool apply)
{
    qDebug() << "[INTERFACE] NavigationControllerProxy::onRequestRoute(2)";
    auto *points = new QList<TS_ROUTE_POS>();
    *points = wayPoints;
    onRequestRoute(QRoutePosListPtr{points}, option, apply);
}

void NavigationControllerProxy::onRequestRoute(QRoutePosListPtr wayPoints, int option, bool apply)
{
    qDebug() << "[INTERFACE] NavigationControllerProxy::onRequestRoute(3)";
    QVariantMap m;
    m["points"] = QVariant::fromValue(wayPoints);
    m["option"] = option;
    m["apply"] = apply;
    sendNotification(InterfaceConstants::ApplyWaypointListChange, m);
    if (!mForeground) 
       PlatformHelper::instance()->switchToApp();
}

void NavigationControllerProxy::onRequestReRoute(int option)
{
    // 주어진 옵션으로 경로 안내를 재 요청함.
    sendNotification(RgConstants::ChangeRouteOption, QVariant(option));
}

void NavigationControllerProxy::onRequestReRoute(TS_ROUTE_POS pos, int option)
{
    // 주어진 위치와 옵션으로 경로 안내를 재 요청함.
    QVariantMap m;

    // 출발지 현재 위치로 재설정 및 경유지 초기화
    m[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, m);

    m.clear();
    m[CommonConstants::X] = pos.tp.x;
    m[CommonConstants::Y] = pos.tp.y;

    // 목적지 재설정
    sendNotification(RouteConstants::SetArrival, m);

    // 경로 안내 재요청
    sendNotification(RgConstants::ChangeRouteOption, QVariant(option));
}
void NavigationControllerProxy::onRequestReRoute(QList<TS_ROUTE_POS> wayPoint, int option)
{
    // 주어진 경유지리스트와 옵션으로 경로 안내를 재 요청함.

    QVariantMap m;
    QVariantList points;
    for (TS_ROUTE_POS pos : wayPoint) {
        points << QPoint(pos.tp.x, pos.tp.y);
    }
    m["points"] = points;
    m["byVr"] = true;

    // 경유지 리스트 업데이트
    sendNotification(RouteConstants::SetVia, m);

    // 옵션으로 경로 요청
    sendNotification(RgConstants::ChangeRouteOption, QVariant(option));
}

void NavigationControllerProxy::onShowCalculateTour(TS_ROUTE_POS pos, int option, bool apply)
{
    // 만약 처음 화면 전환하는 것이라면 최근 목적지 그리드뷰가 뜨지 않도록 한다.
    sendNotification(ZoneConstants::IgnoreRecentTileGridView);

    onRequestRoute(pos, option, apply);

    PlatformHelper::instance()->switchToApp();
}

//TMCRSM-2437:VR로 부터 검색요청
void NavigationControllerProxy::onRequestFreeTextSearchResultOccur(
    const QString& searchWords,
    int scope,
    int searchType,
    int category,
    int destIndex,
    int fromIndex,
    int maxCnt
    )
{
    qDebug() << "FTS ncproxy: search: " << searchWords;
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QTimer::singleShot(1, this, [=](){
        qDebug() << "FTS Proxy #1 " << QTime::currentTime().toString("mm:ss:zzz");
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultView);
        m[CommonConstants::Caller] = CommonConstants::System;
        m[CommonConstants::Owner] = CommonConstants::System;
        m[CommonConstants::Response] = "responsedFreeTextSearchResult";
//        m[SearchConstants::SearchType] = CommonConstants::Name;
        m[SearchConstants::SearchType] = (searchType==0)?CommonConstants::Name:SearchConstants::Around; // 현재 지원에서는 카테고리 검색을 주변 검색으로 처리
        m[CommonConstants::Kind] = category; // search by category
        m[SearchConstants::Query] = searchWords;
        m[CommonConstants::Visible] = SearchConstants::ViewNoResult;
        //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
        m[CommonConstants::ReFocus   ] = true;
        m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultView);


        sendNotification(SearchConstants::UpdateView, m);

        sendNotification(Constants::CreateView, m);
    });

    // TODO: response results
    //QVariant results;
    //QMetaObject::invokeMethod(mController->qobject(), "responsedFreeTextSearchResult",
    //Q_ARG(const QVariant&, results));
    
}


void NavigationControllerProxy::onRequestSearchAddressOccur(const QVariant& request, const QString& resultHandler)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    bool searchCityCenter = request.toMap().value(QStringLiteral("citycenter")).toBool();
    QTimer::singleShot(1, this, [=](){
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultView);
        m[CommonConstants::Caller] = CommonConstants::System;
        m[CommonConstants::Owner] = CommonConstants::System;
        m[SearchConstants::SearchType] = (!searchCityCenter)?CommonConstants::Address:CommonConstants::Name;
        m[CommonConstants::Response] = resultHandler;
        m[SearchConstants::Params] = request.toMap();
        m[SearchConstants::Kind] = (!searchCityCenter)?SearchConstants::CategorySearchAddressVr:SearchConstants::CategoryNone;
        m[SearchConstants::Query] =  request.toMap().value(QStringLiteral("words")).toString(); // to search city center
        m[CommonConstants::Visible] = SearchConstants::DialogNoResult;

        //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
        m[CommonConstants::ReFocus   ] = true;
        m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultView);
        sendNotification(SearchConstants::UpdateView, m);

        sendNotification(Constants::CreateView, m);
    });
}

void NavigationControllerProxy::onRequestSDSSearchAddressOccur(const QVariant& request)
{
    onRequestSearchAddressOccur(request, QStringLiteral("responsedSDSSearchAddress"));
}

void NavigationControllerProxy::onShowDestinationFreeTextSearchInputOccur(const QString& searchString)
{
    if (searchString.isEmpty()) {
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
        m[QStringLiteral("title")] = tr("알림");
        m[QStringLiteral("message")] = tr("연락처에 주소를 입력하면\n해당 주소로 길안내를 받을 수 있습니다.");
        PlatformHelper::instance()->setParentForSystemPopup(m);
        sendNotification(Constants::CreateView, m);
    } else {
        PlatformHelper::instance()->disableRgContinue();

        sendNotification(ApplicationConstants::ResetNavi);
        mIsResetNaviDone = true;

        // Contact gadget에서 요청이 온 경우 내비앱으로 전환하도록 요청
        // 19.04.05 ymhong
        sendNotification(NavigationControllerConstants::SwitchToAppIfNecessary);

        sendNotification(NavigationControllerConstants::ReserveAppSwitchJob, QVariantMap{
                             {CommonConstants::Command, NavigationControllerConstants::RequestContactGadgetSearch},
                             {CommonConstants::Arg, searchString}
                         });
    }
}

void NavigationControllerProxy::requestAroundSearchParking()
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QTimer::singleShot(1, this, [=](){
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultView);
        m[CommonConstants::Caller] = CommonConstants::System;
        m[CommonConstants::Owner] = CommonConstants::System;
        m[SearchConstants::SearchType] = CommonConstants::Around;
        m[SearchConstants::Kind] = SearchConstants::CategoryParkingSp;
        m[CommonConstants::Visible] = SearchConstants::ViewNoResult;

        //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
        m[CommonConstants::ReFocus   ] = false;
        m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultView);
        sendNotification(SearchConstants::UpdateView, m);

        sendNotification(Constants::CreateView, m);
    });
}

void NavigationControllerProxy::requestAroundSearchPatrol(int type)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QTimer::singleShot(1, this, [=](){
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultView);
        m[CommonConstants::Caller] = CommonConstants::System;
        m[CommonConstants::Owner] = CommonConstants::System;
        m[SearchConstants::SearchType] = CommonConstants::Around;
        m[SearchConstants::Kind] = type;
        m[CommonConstants::Visible] = SearchConstants::ViewNoResult;

        //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
        m[CommonConstants::ReFocus   ] = true;
        m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultView);
        sendNotification(SearchConstants::UpdateView, m);

        sendNotification(Constants::CreateView, m);
    });
}

void NavigationControllerProxy::onCancelRoute()
{
    // 경로안내를 취소함.
    sendNotification(RgConstants::StopRouteGuidance);
}

// 주어진 위치를 해당 Type의 경유지에 추가함.
void NavigationControllerProxy::onWaypointChanged(TS_ROUTE_POS pos, int type)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QTimer::singleShot(1, this, [=](){
        QVariantMap m;

        m[CommonConstants::Slot] = type;
        sendNotification(RouteConstants::SelectViaSlot, m);

        m.clear();
        m[CommonConstants::X] = pos.tp.x;
        m[CommonConstants::Y] = pos.tp.y;
        m[CommonConstants::Caller] = "Interface";
        sendNotification(RouteConstants::AddVia, m);
    });
}

void NavigationControllerProxy::onWaypointInserted(TS_ROUTE_POS pos, int type)
{
    qDebug() << "[INTERFACE] NavigationControllerProxy::onWaypointInserted(TS_ROUTE_POS pos, int type)";
    QVariantMap m;
    m[CommonConstants::X] = pos.tp.x;
    m[CommonConstants::Y] = pos.tp.y;
    m[CommonConstants::Type] = type;
    sendNotification(InterfaceConstants::InsertWaypoint, m);
    if (!mForeground) 
       PlatformHelper::instance()->switchToApp();
}

void NavigationControllerProxy::onWaypointsDeleted(int firstIndex, int lastIndex)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QTimer::singleShot(1, this, [=](){
        auto index0 = firstIndex + 1;
        auto index1 = lastIndex + 1;
        sendNotification(RouteConstants::DeleteVia, QVariantMap{
                             {"begin", index0},
                             {"end", index1}
                         });

        sendNotification(Constants::CreateView, QVariantMap{
                             {CommonConstants::Url, ViewRegistry::url(ViewRegistry::RouteSettingView)}
                         });
    });
}

void NavigationControllerProxy::onWaypointInsertedFromSelectInMap(int slot)
{
    qDebug() << "NavigationControllerProxy::onWaypointInsertedFromSelectInMap(" << slot << ")";
    sendNotification(InterfaceConstants::InsertWaypointFromSelectInMap, slot);
}

void NavigationControllerProxy::onReplaceWaypoint(uint index)
{
    sendNotification(InterfaceConstants::ReplaceWaypoint, QVariantMap{
                         {"index", index}
                     });
}

void NavigationControllerProxy::onAddFavoritesPosition(TS_ROUTE_POS pos)
{
    // 주어진 위치로 즐겨찾기 추가함.
    QVariantMap m {
        { CommonConstants::X, pos.tp.x },
        { CommonConstants::Y, pos.tp.y },
        { CommonConstants::Type, CommonConstants::Favorite }
    };

    sendNotification(AddressConstants::RequestAddress, m);
}
void NavigationControllerProxy::onRePlayVoiceGuidance()
{
    // 마지막 경로안내 사운드를 다시 송출함.
}
void NavigationControllerProxy::onLanguageChanged(QString lang)
{
    qDebug() << "NavigationControllerProxy onLanguageChanged lang=" + lang;
    changeLanguage(lang);
}
void NavigationControllerProxy::onResetToFactorySettings()
{
    // 설정의 값을 리셋함. 6개의 설정을 모두 초기화 함.
    sendNotification(SettingConstants::ResetToFactorySettings);
}
void NavigationControllerProxy::onDateFormatChanged(QString format)
{
    mDateFormat = format;

    // UI를 주어진 날짜 포멧으로 변경함.
    QVariantMap m {
        { QStringLiteral("format"), format }
    };
    sendNotification(NavigationControllerConstants::ChangeDateFormat, m);	
}
void NavigationControllerProxy::onTimeFormatChanged(QString format)
{
    mTimeFormat = format;

    // UI를 주어진 시간 포멧으로 변경함.

#if (0)
    // 테스트
    QVariantMap m;

    m[QStringLiteral("format")] = "h:mm";
    sendNotification(NavigationControllerConstants::ChangeTimeFormat, m);
#endif
    QVariantMap m {
        { QStringLiteral("format"), format }
    };
    sendNotification(NavigationControllerConstants::ChangeTimeFormat, m);	
}

void NavigationControllerProxy::onGoToStandby()
{
    // 네비 동작 비활성화 모드로 변경. // 모든 기능 정지.
	sendNotification(NavigationControllerConstants::GoToStandby);
}
void NavigationControllerProxy::onGoToRunning()
{
    // 네비 동작 활성화 모드로 변경.
	sendNotification(NavigationControllerConstants::GoToRunning);
}

void NavigationControllerProxy::onGoRouteSearchScreen()
{
    // 경로요약화면으로 이동.
}
void NavigationControllerProxy::onSDIAlertForMuteChanged(bool mute)
{
    // 설정에서 SDI 경고음 무음 설정 처리
    setAlertForMute(mute);
}

void NavigationControllerProxy::setAlertForMute(bool mute)
{
    QVariantMap body;
    body[CommonConstants::Section] = SettingConstants::GuidMain;
    body[CommonConstants::Key] = SettingConstants::SafeDrivingSoundAtMute;
    body[CommonConstants::Value] = mute;
    sendNotification( SettingConstants::SaveSetting, body );
}

void NavigationControllerProxy::onDialPOICallStatusChanged(char DialPOICallStatus)
{
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::CallStatus, DialPOICallStatus}});
}

void NavigationControllerProxy::onCallServiceStatusChanged(char CallServiceStatus)
{
    mIsBluetooth = CallServiceStatus;
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::BluetoothStatus, mIsBluetooth}});
}


//void NavigationControllerProxy::onIsHFPAvailable(bool PhoneCallStatus)
//{
//    mIsBluetooth = PhoneCallStatus;
//    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::BluetoothStatus, mIsBluetooth}});
//}

void NavigationControllerProxy::onSpeakPhreaseSampleOccur(int type)
{
    sendNotification(NavigationControllerConstants::SpeakPhreaseSample, type);
}

void NavigationControllerProxy::onShowAdjustCurrentLocationScreen(TS_ROUTE_POS pos)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    mDestPos = QPoint { pos.tp.x, pos.tp.y };

    // MapSearchView가 떠 있을 경우 바로 CreateView를 하면 안됨.
    QTimer::singleShot(1, this, [this](){
        PlatformHelper::instance()->disableRgContinue();

        auto m = QVariantMap{
            {CommonConstants::Url, ViewRegistry::url(ViewRegistry::MapSearchView)},
            {CommonConstants::Caller, "Interface"},
            {"command", "ShowAdjustCurrentLocationScreen"},
            {"mapX", mDestPos.x()},
            {"mapY", mDestPos.y()}
        };

        // VR로부터 전달되므로
        // 이 지점에 도달했을 때 아직 VR화면일 수 있음
        // MapView 상태에 따라 VSM 지점 이동 api가 제대로 동작 안할 수 있음
        // 19.08.01 ymhong.
        if (mForeground) {
            sendNotification(Constants::CreateView, m);
        } else {
            // VR을 시스템화면에서 띄웠다면 내비 앱으로 전환해야 함.
            PlatformHelper::instance()->switchToApp();

            sendNotification(NavigationControllerConstants::ReserveAppSwitchJob, QVariantMap{
                                 {CommonConstants::Command, Constants::CreateView},
                                 {CommonConstants::Arg, m}
                             });
        }
    });
}

bool NavigationControllerProxy::callPhoneNumber(QString phoneNumber)
{
    connect(mController->qobject(), SIGNAL(callNumberDone(bool)), this, SLOT(onCallNumberDone(bool)));
    return mController->callNumber(phoneNumber);
}

void NavigationControllerProxy::onCallNumberDone(bool success)
{
    int value = success ? SearchConstants::CallStatusSuccess : SearchConstants::CallStatusFail;
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SearchConstants::CallStatus, value}});
    QObject::disconnect(mController->qobject(),SIGNAL(callNumberDone(bool)),this,0);
}

void NavigationControllerProxy::initDriveAutoConfig()
{
    VSM_DriveAutoConfig tv;
    memset(&tv, 0, sizeof(tv));
    tv.mTBTDistance1Level_Turn = 11.f;
    tv.mTBTDistance1TiltAngle_Turn = 65.f;
    tv.mTBTDistance1_Turn = 200;
    VSMNavi_SetDriveAutoConfig(tv);
}

void NavigationControllerProxy::parseTmapClientBehavior(const QString &serviceItems)
{
    auto items = serviceItems.split("|");
    for (auto item : items) {
        if (item.contains("REROUTE_5MIN=")) {
            int value;
            auto ok = parseTmapClientBehaviorValue(item, value);
            if (ok) {
                auto periodicRerouteTime = 5;
                switch (value) {
                case 100:
                    break;
                case 50:
                    {
                        auto randValue = TRandomUtils::randInt(0, 100);
                        if (randValue < 50) {
                            periodicRerouteTime = 10;
                        }
                    }
                    break;
                case 0:
                    periodicRerouteTime = 10;
                    break;
                }
                const auto ONE_MIN_MILLI_SECOND = 60 * 1000;

                sendNotification(RouteConstants::ChangeReRouteIntervalTime, periodicRerouteTime * ONE_MIN_MILLI_SECOND);
            }
        }
    }
}

bool NavigationControllerProxy::parseTmapClientBehaviorValue(const QString &item, int &ret)
{
    auto pos = item.indexOf("=");
    if (0 <= pos) {
        auto len = item.length();
        auto right = len - pos - 1;
        auto valueStr = item.right(right);
        auto ok = false;
        auto value = valueStr.toInt(&ok);
        if (ok) {
            ret = value;
            return true;
        }
    }
    return false;
}

void NavigationControllerProxy::sendGadgetPressedNotification(int instanceId, bool focused, bool pressed)
{
    sendNotification(ApplicationConstants::FocusToWidget, QVariantMap{
        {CommonConstants::FocusIndex, instanceId},
        {CommonConstants::Focus, focused},
        {CommonConstants::Pressed, pressed}
    });
}

void NavigationControllerProxy::onSatelliteUpdated(SKT::SatelliteDataPtr satellite)
{
    mSatellite =  satellite;

    QVariantMap body;
    body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapSatelliteInfo );
    body.insert( QStringLiteral("propertyValue"),QVariant::fromValue<SKT::SatelliteDataPtr>(mSatellite));
    sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
}

void NavigationControllerProxy::onSessionCreated(bool success, int validateCode, int errCode)
{
    if (success) {
        mCountAuthOTPFail = 0;
    }else {
        if (++mCountAuthOTPFail >= MAXCOUNT_AUTHOTPTRY) {
            mTime = QTime::currentTime();
        }
    }
    sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::SessionCreated, success},
                                                                                   {CommonConstants::Code, validateCode},
                                                                                   {CommonConstants::Error, errCode}});
    if (!success) {
        // Current User is able to use his profile even if unauthorized
    }
    else { // get sync user local data to server one so download favorite, recent destination from server
        //mProfileManager->requestRemoteFindSendToCar();
        mProfileManager->onRequestUserDataDownload();
    }
}

void NavigationControllerProxy::onSearchTextChanged(QString text)
{
    // text로 검색 시작.
}

void NavigationControllerProxy::onAutoCompleteTextChanged(QString text)
{
    // text로 자동완성 시작
}

bool NavigationControllerProxy::getDateFormat()
{
    QVariantMap m {
        { SettingConstants::DateFormat, mDateFormat }
    };
    sendNotification(NavigationControllerConstants::ResponseController, m);
    return true;
}

bool NavigationControllerProxy::getTimeFormat()
{
    QVariantMap m {
        { SettingConstants::TimeFormat, mTimeFormat }
    };
    sendNotification(NavigationControllerConstants::ResponseController, m);
    return true;
}

void NavigationControllerProxy::clearSession()
{
    mController->sessionManager()->clearSession();
}

void NavigationControllerProxy::clearAllSession()
{
    mController->sessionManager()->clearAllSession();
}

// map in cluster

void NavigationControllerProxy::onStartMapInCluster(int layout)
{

    QVariantMap args;
    args["layout"] = layout;
    sendNotification( MapConstants::StartMapInCluster, args );
}

void NavigationControllerProxy::onStopMapInCluster()
{
    sendNotification( MapConstants::StopMapInCluster );
}

void NavigationControllerProxy::onChangeMapInClusterLayout(int layout)
{
    sendNotification( MapConstants::ChangeMapInClusterLayout, layout );
}

QString NavigationControllerProxy::mountedMediaPath() const
{
    return mMountedMediaPath;
}

void NavigationControllerProxy::remountMedia(bool readonly)
{
    QMetaObject::invokeMethod(
        mController->qobject(),
        "platformRemountMedia",
        Q_ARG(int, readonly?4:5),
        Q_ARG(const QString&,mMountedMediaPath)
        );
}

void NavigationControllerProxy::onMediaMounted(const QString& mediaPath)
{
    qDebug() << "navi proxy mounted media~~: " << mediaPath;
    mMountedMediaPath = mediaPath;
}

void NavigationControllerProxy::onMediaUnmounted()
{
    qDebug() << "navi proxy unmounted~~";
    mMountedMediaPath.clear();
}

bool NavigationControllerProxy::applyAutoFuelType(int type)
{
    bool ret = false;
    int fueltype = mController->getCarconfigurationValue(SettingConstants::VIHICLE_FUEL);
    if ( type == fueltype) {
        auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
        QString value;
        switch(type) {
            case SettingConstants::FUEL_ELECTRICITY: {
                    value = QString("%1").arg(SettingConstants::OilTypeElec);
                    proxy->setSetting( SettingConstants::MapSubOilType, SettingConstants::OilKind, value);
                    ret = true;
                }
                break;
            case SettingConstants::FUEL_PETROL:
            case SettingConstants::FUEL_DIESEL:
            case SettingConstants::FUEL_HYBRID:
            case SettingConstants::FUEL_HYBRIDPETROL:
            default:
                break;
        }
    }
    return ret;
}

void NavigationControllerProxy::requestSetting()
{
    qDebug() << "NavigationControllerProxy::requestSetting() #1";

    //속도반응
    applySpeedLevel();
    //음성안내
    applyVoiceGuidance();
    //음성선택:남성/여성
    applyVoice();

    //하이패스 설정
    requestSetting(SettingConstants::RouteSearch, SettingConstants::HipassMount);
    //유종 설정
    requestSetting(SettingConstants::MapSubOilType, SettingConstants::OilKind);
    //전기차 유종 설정
    requestSetting(SettingConstants::MapSubChargeType, SettingConstants::ChargeType);

    // TMCRSM-3426
    // 마지막 맵뷰 레벨
    sendNotification( PreferenceConstants::RequestPreference, QVariantMap{
        { SettingConstants::PreferencesName, PreferenceConstants::Rg },
        { CommonConstants::Section, RgConstants::MapMain },
        { CommonConstants::Key, RgConstants::ViewLevel}
    } );

    //맵 모드
    requestSetting(SettingConstants::DirectionGuid, SettingConstants::MapMode);

    qDebug() << "NavigationControllerProxy::requestSetting() #2";
}

void NavigationControllerProxy::requestSetting(const QString &section, const QString &key)
{
    QVariantMap m;
    m.insert(CommonConstants::Section, section);
    m.insert(CommonConstants::Key, key);
    sendNotification(SettingConstants::RequestSetting, m);
}

void NavigationControllerProxy::onLanguageIndexStatusChanged(char languageIndex)
{
    qDebug() << "[MultiLang]NavigationControllerProxy::onLanguageIndexStatusChanged languageIndex: " << (int)languageIndex;

    QString lang;

    switch ( languageIndex ) {
    case 31: //Korean
        lang = LangConstants::Ko;
        break;
    default:
        lang = LangConstants::En;
        break;
    }

    if ( !lang.isEmpty() ) {
        changeLanguage( lang );
    }
}

void NavigationControllerProxy::onColorStateStatusChanged(char colorState)
{
    qDebug() << "NavigationControllerProxy::onColorStateStatusChanged colorState: " << (int)colorState;

    QmlAppHelper::instance()->setMultiSenseColorState(colorState);
}

void NavigationControllerProxy::onMexModeStatusChanged(char mexMode)
{
    qDebug() << "NavigationControllerProxy::onMexModeStatusChanged mexMode: " << (int)mexMode;
}

void NavigationControllerProxy::onApplyHardKey(int key, int state)
{
    qDebug() << "NavigationControllerProxy::onApplyHardKey key:" << key << ", state:" << state;
    if (key != HardKey::ItcJogDialBack && state != HardKey::AcceptKeyState) {
        return;
    }

    switch(key) {
    case HardKey::ItcJogDialBack:
        // CSW simplification 사양 확정전까지 적용하지 않음.
        // 19.07.12 ymhong.
        // state를 전달. pressed일 때에도 처리하지는 않지만 받는 곳이 있다면 CswBackAccepted를 전달할 수 있도록.
        // release키만 처리한다면 pressed는 KeyForwarding 되어 계속 눌렀을 때 처럼 처리가 됨.
        // 20.01.07 ymhong.
        sendNotification(ZoneConstants::CswBackTriggered, state);
        break;

    case HardKey::ItcJogDialUp:
    case HardKey::ItcJogDialDown:
    case HardKey::ItcJogDialLeft:
    case HardKey::ItcJogDialRight:
    case HardKey::ItcJogDialSelect:
    case HardKey::ItcJogDialOption:
    case HardKey::ItcJogDialSpinCcw:
    case HardKey::ItcJogDialSpinCw:
        sendNotification(NavigationControllerConstants::ApplyCSW, key);
        break;

    case HardKey::HkVolumeUp:
        sendNotification(NavigationControllerConstants::ShowSettingVolume, true);
        sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::NaviVolumeUp, key}});
        break;
    case HardKey::HkVolumeDown:
        sendNotification(NavigationControllerConstants::ShowSettingVolume, true);
        sendNotification(NavigationControllerConstants::ResponseController,QVariantMap{{SettingConstants::NaviVolumeDown, key}});
        break;
    }
}

void NavigationControllerProxy::emitCswBackKeyAccepted()
{
    emit cswBackKeyAccepted();
}

void NavigationControllerProxy::onDebugKeyPressed(int key)
{
    switch (key) {
    case Qt::Key_P: {
            // 정주기 재탐색 강제 발생(디버깅용)
            sendNotification(RgConstants::ForcePeriodicReroute);
        }
        break;

    case Qt::Key_R: {
            // 경로 이탈 재탐색 강제 발생(디버깅용)
            QVariantMap data;
            data.insert( CommonConstants::Command , tr("경로 수동 재탐색") );
            data.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce );
            sendNotification( RouteConstants::RequestReroute, data );
        }
        break;

    case Qt::Key_D:
    case Qt::Key_S:
        {
#if (1)
            auto enable = key == Qt::Key_S;
            sendNotification(RgConstants::EnableSimulatorStopBySpeed, enable);
#else
            auto pressed = (key == Qt::Key_D) ? 1 : 0;
            sendNotification(ZoneConstants::CswBackTriggered, pressed);
#endif
        }
        break;
    }
}

void NavigationControllerProxy::showMonkeyTest()
{
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SelfTestView)},
            {QStringLiteral("parentObjectName"),"testWrap"}
        });
}

void NavigationControllerProxy::saveGadgetImage(bool save)
{
    mController->saveGadgetImage(save);
}

void NavigationControllerProxy::setMuteState(bool mute)
{
    // from system interface.
    qDebug() << "NavigationControllerProxy::setMuteState(" << mute << ")";
    if (mute) {
        bool active = getSafeDrivingSoundAtMute();
        mRgConfig.nAndoOverSpeedVoice = active;
    } else {
        mRgConfig.nAndoOverSpeedVoice = 1;
    }
    sendNotiSetVoiceGuidanceActive(!mute);
    sendNotiSetOverSpeedSoundAlertInfoActive(mRgConfig.nAndoOverSpeedVoice);
    VSMNavi_SetGuidanceConfig(mRgConfig);
}

//TMCRSM-2446:VR로부터 음성안내on/off 명령을 받는 인터페이스
void NavigationControllerProxy::onSetVoiceGuidanceOccur(bool active)
{
    // from system interface.
    bool alarmMute = !getSafeDrivingSoundAtMute();
    qDebug() << "NavigationControllerProxy::onSetVoiceGuidanceOccur(" << active << ") alarmMute ("<< alarmMute <<")";
    if(!active)
    {
      mController->setMuteState(0, alarmMute);
    }else{
      mController->setMuteState(0, false);
    }

    qDebug() << "onSetVoiceGuidance :: applyNaviMute #4 ["<< alarmMute <<"]" << !active << ")";

    //active가 음성에 대한 값이므로 !active가 음소거 상태값을 의미한다.
    mController->setMuteState(1, !active);

    // RTK의 요청에 따라 mute가 풀리면 데모음성을 플레이한다.
    // 19.07.16 ymhong.
    // 해당 기능은 포함하지 않는 것으로 결정
    // 19.09.11 ymhong.
//    if (active) {
//        sendNotification(NavigationControllerConstants::SpeakPhreaseSample);
//    }
}

void NavigationControllerProxy::onSetAlramGuidanceOccur(bool active)
{
    //active가 음성에 대한 값이므로 !active가 음소거 상태값을 의미한다.
    mController->setMuteState(0, !active);
}

//TMCRSM-2452:VR로부터 좌표값으로 상세보기 화면으로 이동 명령을 받는 인터페이스
void NavigationControllerProxy::onGoPoiDetailScreenOccur(const QVariant& detail)
{
    sendNotification(ApplicationConstants::ResetNavi);
    mIsResetNaviDone = true;

    QVariantMap locationInfo = detail.toMap();
    int tpX = locationInfo[QStringLiteral("x")].toInt();
    int tpY = locationInfo[QStringLiteral("y")].toInt();
    QString poiName = locationInfo[QStringLiteral("name")].toString();
    QString address = locationInfo[QStringLiteral("address")].toString();
    QString phone = locationInfo[QStringLiteral("phone")].toString();

#if 0   // scenario out from bosch 2019-03-14
     // 넘어 온 좌표로 목적지 변경
     sendNotification(RouteConstants::SelectViaSlot, QVariantMap{{CommonConstants::Slot, 4}});

     sendNotification(RouteConstants::InsertVia, QVariantMap{
                             {CommonConstants::X, tpX},
                             {CommonConstants::Y, tpY}
                         });
#endif

    // POI 상세보기 화면 보여주기
    // from system interface.
    qDebug() << "NavigationControllerProxy::onGoPoiDetailScreenOccur(" << tpX << "," << tpY << ")";
    QVariantMap m;
    QVariantList result = QVariantList{};
    SearchConstants::resultInit(result);
    result[SearchConstants::ResultStdName             ] = poiName;
    result[SearchConstants::ResultAddress             ] = address;
    result[SearchConstants::ResultTelNum              ] = phone;
    result[SearchConstants::ResultX                   ] = tpX;
    result[SearchConstants::ResultY                   ] = tpY;
    result[SearchConstants::ResultOnline              ] = mIsOnline;
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultDetailView);
    m[CommonConstants::Result] = result;
    m[CommonConstants::Visible] = false;

    //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
    m[CommonConstants::ReFocus   ] = true;
    m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultDetailView);
    sendNotification(SearchConstants::UpdateView, m);

    sendNotification(Constants::CreateView, m);
}

void NavigationControllerProxy::fileGpsSetPos( double percent )
{
    auto args = QString("%1,%2").arg("seekto").arg(percent);
    mController->gpsManager()->sendExtraCommand(args);
}

void NavigationControllerProxy::fileGpsRewind()
{
    auto args = QString("%1").arg("rewind");
    mController->gpsManager()->sendExtraCommand(args);
}

void NavigationControllerProxy::fileGpsFastForward()
{
    auto args = QString("%1").arg("fast-forward");
    mController->gpsManager()->sendExtraCommand(args);
}

void NavigationControllerProxy::onHmiStateChanged(uint hmiState)
{
    if (hmiState == NavigationPropertyConstants::HmiStateDrive) {
        qDebug() << "[HmiState]Drive";
        sendNotification(MapConstants::ResumeMapView);
    }
    else if (hmiState == NavigationPropertyConstants::HmiStateSleep) {
        qDebug() << "[HmiState]Sleep";
        // todo something
        // 1. stop map rendering
        // 2. lock ui
        sendNotification(MapConstants::PauseMapView);

        // it needs to handle for the response, now just response immediately
        QMetaObject::invokeMethod(mController->qobject(), "naviShutdownReadyFinished",Q_ARG(bool,true));
    }
}

void NavigationControllerProxy::onClientStatusGuiSenseReady()
{
    // TMCRSM-3211. 경로 이어하기 팝업이 표시되지 않음
    // ClientStatus_CLIENT_STATUS_GUI_SENSE_READY 이전에 팝업을 요청하면 무시되는 것 같음
    // MapView Mediate되는 시점에서 옮겨옴
    // 19.05.24 ymhong

    //sendNotification(RgConstants::ShowRouteGuidanceContinuously);
}

void NavigationControllerProxy::setPauseMap(bool b)
{
    sendNotification(NavigationControllerConstants::PauseMapView, b);
}

void NavigationControllerProxy::onUpdateEditFieldText()
{
    sendNotification(NavigationControllerConstants::UpdateEditFieldText);
}

void NavigationControllerProxy::sendReplyToViewList(const QList<int> data, const int findId)
{
    emit responseToViewList(data);
}

void NavigationControllerProxy::switchToAppIfNecessary()
{
    if (!mForeground) {
        PlatformHelper::instance()->switchToApp();
    }
}

void NavigationControllerProxy::requestToViewList(int viewId)
{
    QVariantMap body;
    body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName( ViewRegistry::MapView ));
    body.insert( CommonConstants::Relay, "NavigationControllerProxy");
    body.insert( CommonConstants::ViewId, viewId);
    sendNotification(ApplicationConstants::RequestViewList, body);    
}

void NavigationControllerProxy::onCrossImageVisibleChanged(bool visible)
{
    mController->onCrossImageVisibleChanged(visible);
}

void NavigationControllerProxy::onMapStartedToRender()
{
    qDebug() << "[CTX]NavigationControllerProxy::onMapStartedToRender";
    mController->onMapStartedToRender();
}

void NavigationControllerProxy::setAppLostCswFocus()
{
    emit appLostCswFocus();
}

void NavigationControllerProxy::reserveAppSwitchJob(const QVariantMap &m)
{
    // 강제 예약. SystemSwitchJob에서 등록 하는 경우(아직 forground라 바로 실행)
    auto forceRegist = m["force"].toBool();

    if (mForeground && !forceRegist) {
        runReservedJob(m);
    } else {
        //qDebug() << "[RESERVE] reserveAppSwitchJob(" << m << ")";

        mReservedAppSwitchJob.append(m);
    }
}

void NavigationControllerProxy::reserveSystemSwitchJob(const QVariantMap &m)
{
    if (mForeground) {
        mReservedSystemSwitchJob.append(m);
    } else {
        runReservedJob(m);
    }
}

void NavigationControllerProxy::runReservedJob(const QVariantMap &m)
{
    auto command = m[CommonConstants::Command].toInt();
    auto arg = m[CommonConstants::Arg];
    sendNotification(command, arg);
}

void NavigationControllerProxy::runReservedAppSwitchJob()
{
    if (mReservedAppSwitchJob.size()) {
        auto m = mReservedAppSwitchJob.last();
        auto command = m[CommonConstants::Command].toInt();
        runReservedJob(m);

        //qDebug() << "[RESERVE] runReservedAppSwitchJob() command: " << command << ", size: " << mReservedAppSwitchJob.size();

        mReservedAppSwitchJob.clear();
    }
}

void NavigationControllerProxy::runReservedSystemSwitchJob()
{
    auto hasCloseView = false;
    for (auto m : mReservedSystemSwitchJob) {
        auto command = m[CommonConstants::Command].toInt();
        auto isCloseView = command == Constants::CloseView ||
                           command == SearchConstants::CloseSearchResultView;

        //qDebug() << "[RESERVE] runReservedSystemSwitchJob: command: " << command << ", isCloseView: " << isCloseView;

        if (isCloseView) {
            hasCloseView = true;
        }
        runReservedJob(m);
    }
    mReservedSystemSwitchJob.clear();

    if (hasCloseView) {
        //qDebug() << "[RESERVE] hasCloseView ...";
#if (0)
        sendNotification(NavigationControllerConstants::ReserveAppSwitchJob, QVariantMap{
                             {CommonConstants::Command, ZoneConstants::SelectZone1TextInput},
                             {CommonConstants::Arg, QVariantMap{{CommonConstants::Text, CommonConstants::Empty}}},
                             {"force", true}
                         });
#endif
    }
}

void NavigationControllerProxy::onNaviSignalRFAntennaStatusOccur(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent)
{
    qDebug() << "[RFAntenna]" << "NavigationControllerProxy";

//    QVariantMap map;
//    map["rfAntenna"] = u8RFAntenna;
//    map["rfAntennaStatus"] = u8RFAntennaStatus;
//    map["connectionCurrent"] = u8ConnectionCurrent;
//    sendNotification( NavigationControllerConstants::UpdateRFAntennaStatus, map );

    if ( u8RFAntenna != GPS_Antenna )
        return;

    auto newSignalState = RgSignalState::ON;
    if ( u8RFAntennaStatus != TUN_MSTR_RF_ANTENNA_STATE_OK ) {
        newSignalState = RgSignalState::OFF;
    }
    if ( mSignalState != newSignalState ) {
        mSignalState = newSignalState;
        QVariantMap m;
        m["state"] = mSignalState;
        sendNotification(RgConstants::ChangedSignalState, m);
    }
}

void NavigationControllerProxy::onSpcxRgEnabled(bool enabled)
{
    mIsSpcxRgEnabled = enabled;

    // TMCRSM-3626. If the path is not canceled due to SPCX RG execution,
    // the guidance relay popup is provided.
    if (enabled) {
        PlatformHelper::instance()->disableRgContinue();

        sendNotification(NavigationControllerConstants::SpcxRouteGuidanceStarted);
    }
}

void NavigationControllerProxy::onChannelReroutePolicyReceived(const QString& policy)
{
    qDebug() << "[CHANNEL] reroute policy: " << policy;
    parseTmapClientBehavior(policy);
}

void NavigationControllerProxy::onChannelServerPolicyFinished(bool success)
{
    qDebug() << "[CHANNEL] server policy finished";

    mChannelServerPolicyDone = true;
    mChannelServerPolicyFinished = success;
    sendNotification(NavigationControllerConstants::ChannelServerPolicyFinished, QVariantMap{
                         {"isOnline", mIsOnline},
                         {"isChannelServerPolicyDone", mChannelServerPolicyDone},
                         {"isChannelServerPolicyFinished", mChannelServerPolicyFinished}
                     });

    if (success){
        auto *primaryNddsServer = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX);
        sendNotification(RouteConstants::PrimaryNddsServerUpdated, QString{primaryNddsServer});
    }
}

bool NavigationControllerProxy::isSpcxRgEnabled() const
{
    return mIsSpcxRgEnabled;
}

void NavigationControllerProxy::onContextSwitchRequested(QVariant args)
{
    qDebug() << "[CTX][GadgetSwitch]NavigationControllerProxy::onContextSwitchRequested";
    sendNotification( NavigationControllerConstants::ContextSwitchRequested, args );
    if (!mFirstAppear && !mIsResetNaviDone) {
        sendNotification(RouteConstants::CloseRouteCandidateView);
        sendNotification(ApplicationConstants::ResetNavi);
    }
}

void NavigationControllerProxy::onContextStateAccepted()
{
}

void NavigationControllerProxy::onContextStateCompleted()
{

}

void NavigationControllerProxy::onPlatformNaviGadgetFocusStatusChanged(uint instanceId, bool focus)
{
    // 기존에 포커스인 가젯들의 포커스를 제거한다.
    if (focus || 0 == instanceId) {
        for (auto key : mWidgetFocusMap.keys()) {
            auto focused = mWidgetFocusMap[key];
            if (focused && (key != instanceId)) {
                onGadgetFocused(key, false);
                mWidgetFocusMap[key] = false;
            }
        }
    }

    // 0이면 기존 포커스를 해제 (위에서 처리 됨)
    if (0 != instanceId) {
        onGadgetFocused(instanceId, focus);
        mWidgetFocusMap[instanceId] = focus;
    }
}

void NavigationControllerProxy::onPlatformNaviGadgetFocusInteractionTypeChanged(uint instanceId, uint interactionType)
{
    onGadgetFocusInteraction(instanceId, interactionType);
}

void NavigationControllerProxy::respondToContextSwitchRequest(QVariant args)
{
    mController->respondTocontextSwitchRequest(args);
}

bool NavigationControllerProxy::isChannelServerPolicyFinished() const
{
    return mChannelServerPolicyFinished;
}

bool NavigationControllerProxy::isChannelServerPolicyDone() const
{
    return mChannelServerPolicyDone;
}

void NavigationControllerProxy::setIsRg(bool b)
{
    mIsRg = b;
}

}
