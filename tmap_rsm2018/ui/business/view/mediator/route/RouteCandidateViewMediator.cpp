#include "RouteCandidateViewMediator.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "LangConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "mediator/app/MapViewMode.h"
#include "RouteManager.h"
#include "ViewRegistry.h"
#include "RouteSearchRequest.h"
#include "../common/rprequest.h"
#include "NavibookmarkProtocol.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "PreferenceConstants.h"
#include "ToastsConstants.h"
#include "RouteSummaryHelper.h"
#include "SoftkeyMenu.h"
#include "PlatformHelper.h"

#define USE_SOFTKEY_INDEX_MENU


using namespace SKT;

// TeRouteOption의 순서와
// RSM2018에서 다른 경로 옵션의 우선 순위가 달라 순서를 바꾼다.
// 경로 옵션 순서를 결정하기 위해 사용.
// 최적, 최소, 무료, 최단, 고속도로, 초보자 순
int routeToInt(int e) {
    switch (e) {
    case /*E_ROUTEOPTION_OPTIMUM*/1: return 1;
    case /*E_ROUTEOPTION_SHORTTIME*/32: return 2;
    case /*E_ROUTEOPTION_FREEWAY*/4: return 3;
    case /*E_ROUTEOPTION_SHORTESTWAY*/8: return 4;
    case /*E_ROUTEOPTION_EXPRESSWAY*/2: return 5;
    case /*E_ROUTEOPTION_EASY*/16: return 6;
    default:
        Q_ASSERT(0 && "Invalid route option value.");
        break;
    }
}

bool routeCompareLessThan(int l, int r) {
    auto l2 = routeToInt(l);
    auto r2 = routeToInt(r);

    return l2 < r2;
}

RouteCandidateViewMediator::RouteCandidateViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{    
    mInitialized = false;
    mRouteIndex = 0;
    mUpdateCompleted = false;
    mDisableSoftkeyMenu = false;
    mIsOnline = false;
    mIsSoftkeyCreated = false;
    mIsActivated = true;
    mIsLocalRequested = false;
    mIsServerRequested = false;
    mIsServerReceived = false;
    mBlockSoftkey = false;
    mSelectedRouteIndex = -1;
    mIsCountdownCanceled = false;
    mUpdateMapViewPaddingForce = false;
}

RouteCandidateViewMediator::~RouteCandidateViewMediator()
{
}

QList<int> RouteCandidateViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        SettingConstants::ChangedOtherRouteSetting,
        RouteConstants::CloseRouteCandidateView,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        ZoneConstants::DoSoftkeyIndexMenuOptionReleased,
        RouteConstants::RequestRoute,
        RouteConstants::RequestRouteWithOtherOptions,
        RouteConstants::RestoreRoute,
        ZoneConstants::ShowOptionMenu,
        RouteConstants::UpdateRouteSummary,
        RouteConstants::UpdateRouteCandidate,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChannelServerPolicyFinished,
        NavigationControllerConstants::ReplyToChannelServerPolicyFinished,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        RgConstants::CloseNaviMenuAll,
        RouteConstants::ErrorViewClosed,
        RouteConstants::ResetRouteCandidateOtherPass,
        RouteConstants::ResetRouteCandidateView,
        RouteConstants::BlockSetTvas,
        RgConstants::ReplyToCurrentSpeed,
        ApplicationConstants::ChangeToApp,
        ApplicationConstants::ChangeToWidget,
        ApplicationConstants::ReplyToGadgetState,
        ApplicationConstants::ChangedGadgetState
    };

    return notes;
}

bool RouteCandidateViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Mediated: {
            sendNotification(ApplicationConstants::RequestGadgetState);
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
            sendNotification(NavigationControllerConstants::RequestChannelServerPolicyFinished);
            sendNotification(RouteConstants::PrepareRouteCandidate);

            connect(&mTvasRescanTimer, SIGNAL(timeout()), this, SLOT(rescanTvas()));
            connect(&mDrawTimer,SIGNAL(timeout()), this, SLOT(drawRouteAll()));
        }
        break;

    case Activated: {
            qDebug() << "[RouteCandidate]Activated !!!";
            if (mIsGadgetMode) {
                qDebug() << "[RouteCandidate] skip RouteCandidateView.Activated. gadget mode !!!";
                return true;
            }

            // 언어 확인
            sendNotification( NavigationControllerConstants::RequestController, QVariantMap{
                                  {"type", NavigationControllerProtocol::GetLang }
                              } );

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));

            QVariantMap m = data.value<QVariantMap>();
            QString title = m["title"].toString();
            sendNotification(ZoneConstants::SelectZone1Title, title);

            // mDisableSoftkeyMenu : 경로 이어가기 할 경우 이미 softkey가 설정 된 상태
            // mRouteSummaryVariantMap 가 있을 경우 onUpdateRouteSummary() 에서
            auto disableSoftkeyMenu =
                    mDisableSoftkeyMenu ||
                    (!mRouteSummaryVariantMap.isEmpty() && mSelectedRouteIndex < 0);

            if ( !disableSoftkeyMenu ) { // !mDisableSoftkeyMenu && mRouteSummaryVariantMap.isEmpty()) {
                QStringList menus = m["menus"].toStringList();
                setupSoftkeyMenu(menus);

                updateOtherPassMenu(mIsOnline);
                mIsSoftkeyCreated = true;
            }
            mDisableSoftkeyMenu = false;

            if (!mInitialized) {
                mInitialized = true;
            } else {
                if (!mWillBeDeleted && !mBlockSetTvas) {
                    restoreTvas();

                    updateMapViewPadding();

                    if ( !mRouteSummaryVariantMap.isEmpty() ) {
                        onUpdateRouteSummary(mRouteSummaryVariantMap);
                        mRouteSummaryVariantMap.clear();
                    }

                    // VSM 엔진이 View Size를 변경 후 바로 DrawRouteAll()을 호출 하면
                    // 제대로 경로 표시가 안됨.
                    // 한 템포 쉬어서 다음 이벤트 루프때(렌더링 타임?) 호출 되도록 함.
                    // 17.07.21 ymhong.

                    // local timer 사용 코드
                    // QTimer::singleShot(...) 사용 직후 경로 요약 화면이 삭제 되는 경우
                    // timeout 시점에서 코드가 실행 되어 프로그램이 죽는 경우가 있음.
                    mDrawTimer.stop();
                    mDrawTimer.setSingleShot(true);
                    mDrawTimer.setInterval(1);
                    mDrawTimer.start();

                    // Qt-BUG
                    // Fixed a crash when connecting a signal to a lambda or functor while
                    // using Qt::UniqueConnection (note: the connection is still not unique).
                    // 19.11.16 ymhong
#if (0)
                    connect(&mDrawTimer, &QTimer::timeout, this, [this](){
                        drawRouteAll();
                    },Qt::UniqueConnection);
#endif
                }
            }

            sendNotification(RgConstants::BlockReroute, true);
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(MapConstants::ShowTrafficInfoOnRouteLineForce, true);
            sendNotification(MapConstants::EnableCallout, false);
            mIsActivated = true;
            mBlockSoftkey = false;

            sendNotification(MapConstants::EnableCallout, false);
            sendNotification(MapConstants::ShowRoute, QVariantMap{{"show", true}});

            mRescanCount = 0;
        }
        break;

    case Deactivated: {
            mIsActivated = false;
            sendNotification(NavigationControllerConstants::ResetCSW);
            sendNotification(MapConstants::ShowTrafficInfoOnRouteLineForce, false);
            sendNotification(MapConstants::EnableCallout, true);

            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case SetupSoftkeyMenu:
        {
            auto menus = data.toStringList();
            auto enableOption = false;

            setupSoftkeyMenu(menus, enableOption);
            updateOtherPassMenu(mIsOnline);
        }
        break;

    case VisibleChanged:
        sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
        break;

    case RouteSelected:
        {
            QVariantMap m = data.value<QVariantMap>();
            int routeId = m["routeId"].toInt();

            if (mRpStatus.isValidRouteIndex(routeId)) {
                auto newRouteId = mRpStatus.routeIdByIndex(routeId);
                auto option = mRouteOptions.at(routeId);

                m["routeId"] = newRouteId;
                m["option"] = option;

                mSelectedRouteIndex = routeId;
                mDistance = m["distance"].toInt();
                mFee = m["fee"].toInt();
                mRemainTime = m["remainTime"].toInt();
                sendNotification(RouteConstants::SelectRoute, m);

                sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                     {"preferencesName",PreferenceConstants::Rg},
                     {CommonConstants::Section,QStringLiteral("Termination")},
                     {CommonConstants::Key,QStringLiteral("selectedRouteIndex")},
                     {CommonConstants::Value,mSelectedRouteIndex}
                 });

                sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                     {"preferencesName",PreferenceConstants::Rg},
                     {CommonConstants::Section,QStringLiteral("Termination")},
                     {CommonConstants::Key,QStringLiteral("selectedRouteOption")},
                     {CommonConstants::Value,option}
                 });

                auto vo = model->getData(newRouteId);
                auto roads = vo->roadInfo().split(";");
                if (roads.count() >= 2) {
                    auto from = roads.at(0);
                    auto to = roads.at(1);
                    updateRoadInfo(from, to);
                } else if (roads.count() > 0){
                    auto from = roads.at(0);
                    updateRoadInfo(from, QString());
                }
                drawRouteAll();

                // 무료 경로 이지만 요금이 있을 경우 팝업으로 알려준다.
                // UID 2.01 167p
                // 19.04.11 ymhong.
                if (option == E_ROUTEOPTION_FREEWAY && (0 < mFee)) {
                    // Popup 다이얼로그를 띄움
                    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                    m[QStringLiteral("title")] = tr("알림");
                    m[QStringLiteral("message")] = tr("유료도로가 포함된 경로만 존재합니다.<br>최소비용 경로로 안내합니다.");
                    PlatformHelper::instance()->setParentForSystemPopup(m);
                    sendNotification(Constants::CreateView, m);
                }
            }
        }
        break;

    case RequestCancelCountdown:
        if (mIsLocalRequested) {
            cancelCountdownMenu();
        } else {
            if (mUpdateCompleted) {
                cancelCountdownMenu();
            }
        }
        break;

    case ViewSizeChanged:
        onViewSizeChanged();
        break;

    case ReturnClicked:
        {
            auto isRequested = data.toBool();
            if (!isRequested) {
                cancelCountdownMenu();
            }
            drawRouteAll();
        }
        break;

    case RequestRoute:
        requestRouteSingle(data);
        break;

    case RetranslateUi:
        {
            QVariantMap m = data.value<QVariantMap>();
            QString title = m["title"].toString();
            sendNotification(ZoneConstants::SelectZone1Title, title);

            QStringList menus = m["menus"].toStringList();
            setupSoftkeyMenu(menus);

            updateOtherPassMenu(mIsOnline);
        }
        break;

    default:
        return false;
    }

    return true;
}

bool RouteCandidateViewMediator::handleNotification( const mvc::INotificationPtr& noti )
{
    switch (noti->getId()) {
    case ApplicationConstants::ReplyToGadgetState:
    case ApplicationConstants::ChangedGadgetState:
        {
            auto m = noti->getBody().toMap();
            mIsGadgetMode = m["visible"].toBool();
        }
        break;

    case NavigationControllerConstants::ChangeLanguage:
        {
            auto m = noti->getBody().toMap();
            auto lang = m[LangConstants::Lang].toString();
            QQmlProperty::write( mViewComponent, "lang", QVariant::fromValue(lang) );

            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        }
        break;

    case NavigationControllerConstants::ChannelServerPolicyFinished:
    case NavigationControllerConstants::ReplyToChannelServerPolicyFinished:
        {
            auto m = noti->getBody().toMap();
            auto isOnline = m["isOnline"].toBool();
            auto isChannelServerPolicyFinished = m["isChannelServerPolicyFinished"].toBool();
            auto isChannelServerPolicyDone = m["isChannelServerPolicyDone"].toBool();
            mIsOnline = isOnline && isChannelServerPolicyFinished;
            mChannelServerPolicyFinished = isChannelServerPolicyFinished;
            mChannelServerPolicyDone = isChannelServerPolicyDone;
            QQmlProperty::write(mViewComponent, "isOnline", mIsOnline);
            if (noti->getId() == NavigationControllerConstants::ChannelServerPolicyFinished) {
                if (mIsServerRequested || mIsLocalRequested) {
                    if (mIsSoftkeyCreated) {
                        updateOtherPassMenu(mIsOnline);
                    }
                    requestRoute();
                }
            }
        }
        break;

    case NavigationControllerConstants::ResponseController: {
            auto m = noti->getBody().toMap();
            if (m.contains(CommonConstants::Lang)) {
                auto value = m[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "lang", QVariant::fromValue(value) );
            }
        }
        break;

    case RouteConstants::BlockSetTvas:
        mBlockSetTvas = noti->getBody().toBool();
        break;

    case RgConstants::ReplyToCurrentSpeed:
        mCurrentSpeed = noti->getBody().toInt();
        break;

    case RgConstants::CloseNaviMenuAll:
        // Home키를 눌러 현재 창 모두 종료시킬 때 tvas 데이터를 restore 하지 않도록 수정
        // 모의주행 시 Home키를 눌렀을 때 다른 view가 close 되면 서 일시적으로
        // 경로 요약 화면이 onActivate 호출(이때 tvas 데이터를 restore함)
        mWillBeDeleted = true;
        break;

    case NavigationControllerConstants::ResponseOnlineStatus:
        // 제일 먼저 online 상태를 체크한다.
        // 주의. Activated event보다 앞선다.
        mIsOnline = noti->getBody().toBool() && mChannelServerPolicyFinished;
        QQmlProperty::write(mViewComponent, "isOnline", mIsOnline);
        break;

    case NavigationControllerConstants::ChangeOnlineStatus:
        mIsOnline = noti->getBody().toBool() && mChannelServerPolicyFinished;
        QQmlProperty::write(mViewComponent, "isOnline", mIsOnline);

        // Hide후 다른 화면이로 이동하였을 때에는 처리하지 않음.
        // 다른 화면에서 소프트키가 바뀌는 사이드 발생.
        // 17.09.19 ymhong
        if (mIsActivated && mChannelServerPolicyDone) {
            // 아직 소프트키가 설정되지 않은 경우 소프트키 설정 시 mIsOnline을 반영하기 때문에
            // 여기서는 설정된 경우만 처리한다.
            if (mIsSoftkeyCreated) {
                updateOtherPassMenu(mIsOnline);
            }
            requestRoute();
        }
        break;

    case RouteConstants::ResetRouteCandidateOtherPass:
        mForceOtherPass = true;
        break;

    case RouteConstants::ResetRouteCandidateView:
        mRouteSummaryVariantMap.clear();
        break;

    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedOtherRouteSetting:
        {
            auto changedOtherRouteSetting = noti->getId() == SettingConstants::ChangedOtherRouteSetting;
            // 다른 경로 선택 시 SettingRouteAlternativeView에서도 ApplySetting
            // 을 처리하기 때문에 무시한다.

            if (mForceOtherPass) {
                changedOtherRouteSetting = true;
                mForceOtherPass = false;
            }

            if (mIsActivated || changedOtherRouteSetting) {
                auto m = noti->getBody().toMap();
                auto section = m[CommonConstants::Section].toString();

                if (changedOtherRouteSetting) {
                    // 다른 경로를 설정 후 복귀한 경우 원활한 재탐색 처리를 위해 리셋.
                    reset();
                }

                if (section == SettingConstants::RouteAlternateSelection) {
                    if (applyRouteSetting(noti->getBody())) {
                        // 경로 옵션을 먼저 얻어온 후 경로 요청을 한다.
                        mIsLocalRequested = false;
                        mIsServerRequested = false;
                        mIsServerReceived = false;

                        if (model == nullptr) {
                            model = t_new_shared<RouteCandidateListModel>();
                        }
                        if (!model->count()) {
                            addDummyData();
                            QMetaObject::invokeMethod(mViewComponent, "resetModel", Q_ARG(QVariant, QVariant::fromValue(model)));
                        }

                        requestRoute();
                    }
                }
            }
        }
        break;

    case RouteConstants::UpdateRouteSummary:
        {
            auto m = noti->getBody().toMap();
            auto visible = QQmlProperty::read(mViewComponent, "visible").toBool();
            if (visible) {
                onUpdateRouteSummary(m);
            } else {
                mRouteSummaryVariantMap = m;
            }
        }
        break;

    case ZoneConstants::ShowOptionMenu:
        QMetaObject::invokeMethod(mViewComponent, "showOptionMenu");
        break;

    case RouteConstants::CloseRouteCandidateView:
        sendNotification(MapConstants::ShowCarvata, true);

        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});
        if (!mIsGadgetMode) {
            sendNotification(MapConstants::UpdateMapViewPadding);
        }
        break;

    case RouteConstants::UpdateRouteCandidate:
        {
            // 경로 요청 후 Tvas 데이터가 생성 되고 응답을 받음.
            auto m = noti->getBody().toMap();
            auto resCommand = m["resCommand"].toMap();
            auto reqCommand = m["reqCommand"].toMap();

            int errorCode = resCommand[RPREQ_KEY_ErrorCode].toInt();
            int errorOk = 0;
            if (errorCode != errorOk) {
                openErrorView(errorCode);
            } else {
                QVariantList list;

                auto filenames = reqCommand["TvasFileName"].value<QStringListPtr>();
                auto isServerRequested = reqCommand[RPREQ_KEY_Online].toBool();
                auto byVr = reqCommand["byVr"].toBool();
                auto switchToLocal = resCommand[RPREQ_KEY_SwitchToLocal].toBool();
                auto urgentMode = resCommand[RPREQ_KEY_ServerUrgentMode].toBool();

                if (switchToLocal || urgentMode) {
                    mRpStatus.switchToLocal();
                    mRouteOptions = QList<int>{E_ROUTEOPTION_OPTIMUM, E_ROUTEOPTION_SHORTESTWAY, E_ROUTEOPTION_FREEWAY};
                    qSort(mRouteOptions.begin(), mRouteOptions.end(), routeCompareLessThan);
                    mRpStatus.setOptions(mRouteOptions);
                    mIsServerRequested = false;
                    mIsLocalRequested = true;

                    auto list = mRpStatus.getTvasDataList(false);
                    auto colors = mRpStatus.getRouteColorList(false);
                    sendNotification(RouteConstants::SetTvas, QVariantMap{
                                         {"list", list},
                                         {"colors", colors},
                                         {RPREQ_KEY_SwitchToLocal, true}
                                     });

                    auto option = mRpStatus.option(0);
                    requestLocalRouteSummary(option, false);

                    if (switchToLocal) {
                        showSwitchToLocalToast();
                    } else {
                        mIsUrgentMode = true;
                    }
                } else {
                    mRpStatus.updateRpDone(filenames);

                    if (byVr) {
                        auto options = reqCommand["RPOptions"].value<QRouteOptionListPtr>();
                        auto option = options->at(0);
                        list = mRpStatus.getTvasDataList(false/*isOnline*/);

                        sendNotification(RouteConstants::SetTvas, list);
                        requestLocalRouteSummary(option, byVr);
                    } else {
                        if (isServerRequested) {
                            auto opt1 = mRouteOptions[1];
                            auto opt2 = mRouteOptions[2];
                            if (opt1 < opt2) {
                                for (int i=0; i<filenames->count(); i++) {
                                    list.append(mRouteOptions[i]);
                                    list.append(filenames->at(i));
                                }
                            } else {
                                list.append(mRouteOptions[0]);
                                list.append(filenames->at(0));
                                list.append(mRouteOptions[2]);
                                list.append(filenames->at(2));
                                list.append(mRouteOptions[1]);
                                list.append(filenames->at(1));
                            }
                            sendNotification(RouteConstants::SetTvas, list);
                            requestServerRouteSummary(byVr);
                        } else {
                            auto options = reqCommand["RPOptions"].value<QRouteOptionListPtr>();
                            auto option = options->at(0);
                            list = mRpStatus.getTvasDataList(false/*isOnline*/);
                            auto colors = mRpStatus.getRouteColorList(false);

                            sendNotification(RouteConstants::SetTvas, QVariantMap{
                                                 {"list", list},
                                                 {"colors", colors}
                                             });
                            requestLocalRouteSummary(option, false);
                        }
                    }
                }

                // 경로 탐색 성공 시 최근 목적지로 등록
                // UID v2.01 148p
                // 19.04.11 ymhong
                sendNotification(RgConstants::RegistRecentDestination);
            }
        }
        break;

    case ZoneConstants::DoSoftKeyFunction:
        procSoftkey(noti->getBody());
        break;

    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        procSoftkey(noti->getBody());
        break;

    case ZoneConstants::DoSoftkeyIndexMenuOptionReleased:
        {
            auto m = noti->getBody().toMap();
            if (m["name"].toString() == ViewRegistry::objectName(ViewRegistry::RouteCandidateView)) {
                auto index = m["index"].toInt();
                if (index == OptionRouteSummary) {
                    openRouteSummaryView();
                }
            }
        }
        break;

    case RouteConstants::RequestRoute:
        {
            auto v = noti->getBody();
            auto m = v.toMap();
            reset();
            setupRouteOptions(m);

            auto caller = m[CommonConstants::Caller].toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::SettingRouteAlternateView)){
                QQmlProperty::write(mViewComponent, CommonConstants::Visible, true);
            } else if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)){
                QQmlProperty::write(mViewComponent, CommonConstants::Caller, caller);
                addDummyData();
                QMetaObject::invokeMethod(mViewComponent, "resetModel", Q_ARG(QVariant, QVariant::fromValue(model)));
            }

            auto byVr = m["byVr"].toBool();
            if (byVr)
                requestRouteByVr();
            else {
                mIsActivated = true; // 강제 활성화. 3개의 경로 옵션을 가져오기 위해.
                requestRoute();
            }
        }
        break;

    case RouteConstants::RequestRouteWithOtherOptions:
        // 다른 경로에서 옵션을 변경 시...
        reset();
        setupRouteOptions(noti->getBody().toMap());
        mRpStatus.setOptions(mRouteOptions);
        requestRoute();
        break;

    case RouteConstants::RestoreRoute:
        {
            restoreTvas();

            auto selectedId = mSelectedRouteIndex;

            QVariantMap m {{"routeId", selectedId}};
            sendNotification(RouteConstants::SelectRoute, m);

            sendNotification(MapConstants::UpdateMapView, selectedId);
        }
        break;

    case RouteConstants::ErrorViewClosed:
        postErrorViewClosed(noti->getBody());
        break;

    default:
        return false;
    }

    return true;
}

void RouteCandidateViewMediator::drawRouteAll()
{
    auto index = mRpStatus.routeIdByIndex(mSelectedRouteIndex);

    sendNotification(MapConstants::DrawRouteAll);

    sendNotification(MapConstants::UpdateMapView, index);
}

void RouteCandidateViewMediator::rescanTvas()
{
    qDebug() << "[RESCAN] rescanTvas: " << mRescanCount;

    restoreTvas();

    QVariantMap m;
    m[CommonConstants::Type] = mRescanType;
    if (mRescanType == "local") {
        if (mSelectedRouteIndex < 0) {
            m["routeId"] = 0;
            m["routeOption"] = E_ROUTEOPTION_OPTIMUM;
        } else {
            auto requestedIndex = mRequestedIndex;
            auto routeId = mRpStatus.routeIdByIndex(requestedIndex);
            auto routeOption = mRouteOptions.at(requestedIndex);

            m["routeId"] = routeId;
            m["routeOption"] = routeOption;
        }
    }

    sendNotification(RouteConstants::RequestRouteSummary, m);

    mRescanCount++;
}

bool RouteCandidateViewMediator::applyRouteSetting(const QVariant &data)
{
    // 비통신 상황에서는 최적, 최단, 무료만 선택 되어야 함.
    QVariantMap m = data.toMap();
    if (m[CommonConstants::Section].toString() == SettingConstants::RouteAlternateSelection) {
        mRouteOptions.clear();

        bool useDefaultRouteOptions = !mIsOnline;

        if (!useDefaultRouteOptions) {
            auto settings = m["settings"].value<QList<SettingDtoPtr>>();
            for (auto setting : settings) {
                if (setting->value().toInt() == 1) {
                    QString key = setting->key().toString();
                    mRouteOptions.append(static_cast<int>(RouteSummaryHelper::routeToEnum(key)));
                }
            }
        } else {
            mRouteOptions << E_ROUTEOPTION_OPTIMUM << E_ROUTEOPTION_SHORTESTWAY << E_ROUTEOPTION_FREEWAY;
        }
        // TMCRSM-372. 경로 옵션이 환경설정의 옵션 순서대로 정렬 되도록 수정.
        qSort(mRouteOptions.begin(), mRouteOptions.end(), routeCompareLessThan);

        mRpStatus.setOptions(mRouteOptions);

        return true;
    }
    return false;
}

void RouteCandidateViewMediator::onViewSizeChanged()
{
    if (0 == mRouteIndex || mUpdateMapViewPaddingForce) {
        // 처음 경로 요약 데이터를 받을때에만 맵뷰의 패딩을 설정한다.
        // 경오 요약 데이터를 받는 중에 모의 주행이나 경로 요약 화면으로 이동 시
        // 빈 화면이 나올 수 있음.
        // 17.07.21 ymhong.

        updateMapViewPadding();

        sendNotification(MapConstants::UpdateMapView, mRouteIndex);

        mUpdateMapViewPaddingForce = false;

        drawRouteAll();
    }
}

void RouteCandidateViewMediator::openErrorView(int errorCode)
{
}

void RouteCandidateViewMediator::openRouteSummaryView()
{
    if (!mRpStatus.isValidRouteIndex(mSelectedRouteIndex))
        return;

    sendNotification(NavigationControllerConstants::ResetCSW);

    auto routeId = mRpStatus.routeIdByIndex(mSelectedRouteIndex);
    auto routeOption = mRouteOptions.at(mSelectedRouteIndex);
    auto isServerData = mRpStatus.isServerData(mSelectedRouteIndex);

    QVariantMap m;
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSummaryView);
    m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
    m["routeId"] = routeId;
    m["routeOption"] = routeOption;
    m["isServerData"] = isServerData;
    m["filename"] = mRpStatus.getTvasFilename(routeId, mIsOnline);
    m[CommonConstants::Visible] = false;

    sendNotification(Constants::CreateView, m);
}

void RouteCandidateViewMediator::openSimulationView()
{
    // 현재 속도가 10km/h 이상인 경우 모의 주행 진입하지 않음
    sendNotification(RgConstants::RequestCurrentSpeed);

    qDebug() << "RouteCandidateViewMediator::openSimulationView(" << mCurrentSpeed << "km/h)";

    if (mCurrentSpeed < MAX_SPEED_TO_OPEN_SIMULATION_VIEW) {
        sendNotification(NavigationControllerConstants::ResetCSW);

        sendNotification(Constants::CreateView,
            QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::SimulationView)},{CommonConstants::Caller,ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}}
        );

        auto routeId = mRpStatus.routeIdByIndex(mSelectedRouteIndex);
        sendNotification(SimulatorConstants::StartSimulator, routeId);//mSelectedRouteIndex);

        QQmlProperty::write( mViewComponent, CommonConstants::Visible, false );
    } else {
        sendNotification(ToastsConstants::ShowToast, QVariantMap{
                             {CommonConstants::Position, ToastsConstants::ToastUp},
                             {CommonConstants::Message, "안전을 위해 주행 중에는 모의주행을 실행할 수 없습니다."}
                         });
    }
}

void RouteCandidateViewMediator::onUpdateRouteSummary(const QVariantMap &m)
{
    if ( !m.isEmpty() ) {
        auto byContinue = m[QStringLiteral("byContinue")].toBool();
        auto byVr = m["byVr"].toBool();
        bool applyTimer = true;
        mIsContinued = byContinue;

        if (m.contains(QStringLiteral("apply")))
            applyTimer = m["apply"].toBool();

        if (byContinue || byVr) {
            if (byContinue) {
                auto optionKey = QStringLiteral("routeOption");
                Q_ASSERT(m.contains(optionKey));

                // byVr인 경우 이미 추가가 되었음.
                auto routeOption = m[optionKey].toInt();
                mRouteOptions.append(routeOption);
            }

            QQmlProperty::write(mViewComponent, "modelCount", 1);

            if (applyTimer) {
                setupStartGuideMenu();
            }
        }

        auto updateDone = false;

        if (m.contains("summary")) {
            auto summary = m[QStringLiteral("summary")];
            if (summary.isValid()) {
                updateRouteSummary(summary, byContinue || byVr, applyTimer);
                updateDone = true;
            } else {
                mRescanType = "local";
            }
        } else {
            auto summaries = m["summaries"].toList();
            if (summaries.size()) {
                int index = 0;
                for (auto summary : summaries) {
                    auto option = mRouteOptions.at(index);
                    ++index;
                    auto vo = summary.value<RouteSummaryVoPtr>();
                    auto type = RouteSummaryHelper::routeToString(static_cast<TeRouteOption>(option));
                    vo->setType(type);
                    updateRouteSummary(summary, false);
                }
                updateDone = true;
            } else {
                mRescanType = "server";
            }
        }

        if (updateDone) {
            if (mSelectedRouteIndex < 0) {
                mSelectedRouteIndex = 0;
                updateStartRouteGuidanceMenu(true);
            }
        } else {
            if (mRescanCount < 5) {
                mTvasRescanTimer.setInterval(1000);
                mTvasRescanTimer.setSingleShot(true);
                mTvasRescanTimer.start();
            }
        }
    }
}

void RouteCandidateViewMediator::updateRouteSummary(const QVariant &body, bool byContinue, bool applyTimer)
{
    if (body.isNull()) {
        requestRouteSingle();
        return;
    }
    if (model.isNull()) {
        model = t_new_shared<RouteCandidateListModel>();
    }
    if (model->validDataCount() == 3 || mUpdateCompleted) {
        return;
    }

    auto vo = body.value<RouteSummaryVoPtr>();

    if (byContinue) {
        if (0 < mRouteOptions.count()) {
            mRpStatus.setOptions(mRouteOptions);
            auto option = mRouteOptions.at(0);
            mRpStatus.updateRpDone((TeRouteOption)option);
        }
    }

    model->append(vo);

    if (byContinue) {
        // 경로 이어하기 시 더 이상 byContinue가 설정 되지 않음
        // VR 인터페이스에 의해 경로 탐색을 할 때 byContinue가 활성화.
        // 19.01.16 ymhong.
    }
    ++mRouteIndex;

    if (mIsServerRequested && mIsOnline) {
        mIsServerReceived = true;
    }

    auto option = RouteSummaryHelper::routeToEnum(vo->type());
    auto index = mRpStatus.optionIndex(option);

    QStringList roadInfo = vo->roadInfo().split(";");
    QString from = roadInfo.count() ? roadInfo[0] : "";
    QString to = roadInfo.count() > 1 ? roadInfo[1] : "";

    updateModel(from, to);

    if (byContinue || mIsLocalRequested) {
        QMetaObject::invokeMethod(mViewComponent, "selectRouteCandidate", Q_ARG(QVariant, QVariant::fromValue(index)));
    } else if (mIsServerReceived && mRouteIndex==3) { // 20190926 lckxx(feat.ymhong) fix TMCRSM-3899, 동일한 경로에 대해서 3번씩 'SelectRoute' Noti 발생하지 않도록 수정.
        QMetaObject::invokeMethod(mViewComponent, "selectRouteCandidate", Q_ARG(QVariant, QVariant::fromValue(0)));
    }

    if (byContinue || model->validDataCount() == 3) { // mRpStatus.doneCount() ==3) {// mRouteIndex == 3) {
        mUpdateCompleted = true;

        // 경로 안내를 이어서 할 경우 SelectSoftkeyMenuMode noti를 날린 이후
        // Activate event가 발생하면 서 다시 softkey menu를 설정하게 됨.
        // 중복 설정을 막기 위해 이어하는 경우에 한해서 mDisableSoftkeyMenu를 설정(1회용)
        if (byContinue)
            mDisableSoftkeyMenu = true;

        if (mIsActivated) {
            // 경로 요청에 따라 경로 요약 데이터를 받는 중 모의주행이나 경로상세 화면으로 이동할 때
            // 다른 화면으로 이동 후 소프트키가 설정되는 경우가 있음.
            // 뷰가 activate상태가 아닐 때(다른 뷰일 때) 소프트키 메뉴를 변경하지 않도록 처리.
            // 17.07.21 ymhong.

            if (!mIsLocalRequested && applyTimer) {
                setupStartGuideMenu();
            } else {
                cancelCountdownMenu();
            }
        }
    } else {
        //TMCRSM-806
        //로컬에서 경로 탐색 시 최적경로 표출 후 타이머 노출 되도록 수정.
        if (mIsLocalRequested && mRouteIndex == 1) {
            setupStartGuideMenu();
        } else {
            cancelCountdownMenu();
        }
    }
}

void RouteCandidateViewMediator::requestRoute()
{
    if (0 == mRouteIndex) {
        mIsUrgentMode = false;
    }
    if (mIsOnline) {
        if (0 == mRouteIndex) {
            requestRouteMulti();
            mIsServerRequested = true;
            mIsLocalRequested = false;
        } else {
            // 이미 진행 된 경우라면 로컬 검색이므로 계속 로컬 검색을 한다.
            if (!mUpdateCompleted) {
                if (mIsLocalRequested) {
                    // legacy에서는 로컬 검색이 하나가 완료 되면 다음 검색이 자동으로 진행되었으나
                    // '조회' 버튼을 눌러야 검색이 되는 사양으로 변경
                    // 아래 로컬 검색 요청은 필요하지 않음
                    // 18.08.17 ymhong.

                    //requestRouteSingle();
                } else {
                    int option = 1;
                    requestLocalRouteSummary(option);
                }
            }
        }
    } else {
        if (mIsServerRequested) {
            if (mIsServerReceived) {
                // 이미 서버 경로 요청 데이터(tvas)는 전부 있는 상태 이므로 굳이
                // 로컬 검색을 하지 않음.
            } else {
                // 아직 서버 데이터를 받지 못하였다면
                // 로컬 검색을 수행
//                RouteSearchRequest req(this);
//                req.cancel();

//                mIsServerRequested = false;
//                mIsLocalRequested = true;

//                mRouteIndex = 0;
//                requestRouteSingle();

//                showSwitchToLocalToast();
            }
        } else {
            if (mRouteIndex == 0) {
                requestRouteSingle();
                mIsLocalRequested = true;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//  로컬 초기 경로 탐색
//-----------------------------------------------------------------------------
void RouteCandidateViewMediator::requestRouteSingle()
{
    RouteSearchRequest req(this);
    if (0 == mRouteIndex && !mUpdateCompleted) {
        sendNotification(RouteConstants::ClearRouteDirectory);
        req.cancel();

        // 이어 하기 이후 경로 재탐색(경유지 추가 후)이 이루어진 경우
        // 경로 옵션이 1개 밖에 없어서 DB에 요청하여 선택된 경로 3개를 가져오는 것 부터
        // 시작 되도록 처리.
        if (mRouteOptions.count() == 1) {
            sendNotification(RouteConstants::PrepareRouteCandidate);
            return;
        }
    }

    if (!mUpdateCompleted) {
        TeRouteOption option = static_cast<TeRouteOption>(mRouteOptions.at(mRouteIndex));

        int routeId = mRouteIndex + 1;
        req.request(option, routeId);

        QList<TeRouteOption> list { option };
        auto isServer = false;
        mRpStatus.request(list, isServer);

        QQmlProperty::write(mViewComponent, "isRequested", true);
    }
}

//-----------------------------------------------------------------------------
//  통신 연결 후 '조회' 버튼 눌렀을 때
//-----------------------------------------------------------------------------
void RouteCandidateViewMediator::requestRouteSingle(const QVariant& v)
{
    auto m = v.toMap();
    auto index = m["index"].toInt();
    auto routeId = index + 1;
    auto option = static_cast<TeRouteOption>(mRouteOptions.at(index));

    RouteSearchRequest req(this);
    req.request(option, routeId);

    QList<TeRouteOption> list { option };
    auto isServer = false;
    mRpStatus.request(list, isServer);

    QQmlProperty::write(mViewComponent, "isRequested", true);

    mRequestedIndex = index;
    mRescanCount = 0;
}

//-----------------------------------------------------------------------------
//  통신 연결 상태에서 초기 경로 탐색
//-----------------------------------------------------------------------------
void RouteCandidateViewMediator::requestRouteMulti()
{
    RouteSearchRequest req(this);
    req.cancel();

    sendNotification(RouteConstants::ClearRouteDirectory);

    QList<TeRouteOption> options;
    for (int option : mRouteOptions) {
        options.push_back(static_cast<TeRouteOption>(option));
    }
    req.request(options);
    auto isServer = true;
    mRpStatus.request(options, isServer);

    QQmlProperty::write(mViewComponent, "isRequested", true);
}

void RouteCandidateViewMediator::requestRouteByVr()
{
    int option = mRouteOptions.at(0);

    RouteSearchRequest req(this);
    req.cancel();

    sendNotification(RouteConstants::ClearRouteDirectory);

    req.requestByVr(static_cast<TeRouteOption>(option), mIsOnline);
    QList<TeRouteOption> options;
    options.push_back(static_cast<TeRouteOption>(option));
    mRpStatus.request(options, mIsOnline);

    QQmlProperty::write(mViewComponent, "isRequested", true);
}

void RouteCandidateViewMediator::setupRouteOptions(const QVariantMap &m)
{
    if (m.contains("options")) {
        QStringList list = m["options"].toStringList();
        Q_ASSERT(list.count() == 3 || list.count() == 0);

        if (list.count() > 0) {
            mRouteOptions.clear();
            for (QString s : list) {
                mRouteOptions.append(static_cast<int>(RouteSummaryHelper::routeToEnum(s)));
            }
            qSort(mRouteOptions.begin(), mRouteOptions.end(), routeCompareLessThan);
        }
    } else if (m.contains("byVr")) {
        auto byVr = m["byVr"].toBool();
        if (byVr) {
            auto option = m["routeOption"].toInt();

            mRouteOptions.clear();
            mRouteOptions.append(option);
        }
    }
    mRpStatus.setOptions(mRouteOptions);
}

void RouteCandidateViewMediator::requestLocalRouteSummary(int option, bool byVr)
{
    QVariantMap m;
    auto routeId = mRpStatus.routeIdByOption(static_cast<TeRouteOption>(option));

    m["type"] = "local";
    m["byVr"] = byVr;
    m["routeId"] = routeId;
    m["routeOption"] = option;

    sendNotification(RouteConstants::RequestRouteSummary, m);
}

void RouteCandidateViewMediator::requestServerRouteSummary(bool byVr)
{
    QVariantMap m;
    m["type"] = "server";
    m["byVr"] = byVr;

    if (byVr) {
        QVariantList routeOptions;
        for (int option : mRouteOptions) {
            routeOptions.append(option);
        }
        m["routeOptions"] = routeOptions;
    } else {
        m["routeOption"] = mRouteOptions.at(0);
    }

    sendNotification(RouteConstants::RequestRouteSummary, m);
}

void RouteCandidateViewMediator::reset()
{
    mRouteIndex = 0;
    mUpdateCompleted = false;
    mIsServerRequested = false;
    mIsServerReceived = false;
    mIsLocalRequested = false;
    mSelectedRouteIndex = -1;
    if (model) {
        model->clear();

        QMetaObject::invokeMethod(mViewComponent, "resetModel", Q_ARG(QVariant, QVariant::fromValue(model)));
    }
    sendNotification(RouteConstants::CancelRoute);
    mRpStatus.reset();
    mRescanCount = 0;
}

void RouteCandidateViewMediator::restoreTvas()
{
    QVariantList v;
    if (mIsContinued) {
        if (mRouteOptions.count() != 1) {
            qDebug() << "NG !!!";
        }
        v.append(mRouteOptions[0]);
        v.append(QString("tvas_route.db"));
        sendNotification(RouteConstants::SetTvas, v);
    } else {
        auto isOnline = mIsServerRequested;
        v = mRpStatus.getTvasDataList(isOnline);
        if (!isOnline) {
            // local일 때만 적용.
            // online일 때에는 적용 필요 없음.
            auto colors = mRpStatus.getRouteColorList(isOnline);
            sendNotification(RouteConstants::SetTvas, QVariantMap{
                                 {"list", v},
                                 {"colors", colors}
                             });
        } else {
            sendNotification(RouteConstants::SetTvas, v);
        }
    }
}

void RouteCandidateViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m["name"].toString() != "RouteCandidateView")
        return;

    if (mBlockSoftkey) {
        const char *reason = mBlockSoftkeyReason.toLatin1().constData();
        T_DEBUGV("RouteCandidateViewMediator::procSoftkey() is blocked by %s", reason);
        return;
    }

    QString function = m["function"].toString();
    if (function == "menu") {
        QString menu = m["menu"].toString();
        auto index = -1;
        if (m.contains("index")) {
            index = m["index"].toInt();
            menu.clear();
        }

        sendNotification(NavigationControllerConstants::ResetCSW);

        if (index == MenuWaypoints || menu == tr("경유지 설정")) {
            blockSoftkey("Via setting");

            QString caller = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSettingView);
            body[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, body);
            sendNotification(Constants::HideView, QVariantMap{{CommonConstants::ObjectName,caller}});
            sendNotification(RouteConstants::BackupViaData);

            // 경유지 추가 후 경로 탐색시 타이머가 표시 되도록...
            // mShowTimer가 true이면 타이머가 표시되지 않음
            mShowTimer = false;
        } else if (index == MenuStart || menu == tr("안내시작") || menu == QStringLiteral("StartGuideButton.qml")) {
            blockSoftkey("StartGuide");

            if (mSelectedRouteIndex < 0) {
                mSelectedRouteIndex = 0;
            }

            auto filename = mRpStatus.getTvasFilename(mSelectedRouteIndex, mIsOnline);
            auto routeIndex = 0;
            if (filename.contains("2")) {
                routeIndex = 1;
            } else if (filename.contains("3")) {
                routeIndex = 2;
            }

            QVariantMap m;
            m["routeId"] = routeIndex;
            m["filename"] = filename;
            m["routeOption"] = mRouteOptions.at(mSelectedRouteIndex);
            if (mIsUrgentMode) {
                m[RPREQ_KEY_RerouteForSDI] = routeIndex != 0;
            }
            sendNotification(RgConstants::PrepareStartRouteGuidance, m);

            sendNotification(RgConstants::StartRouteGuidance, m);
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});

#ifdef QT_DEBUG
            // 자동안내가 된 시점에서 back키를 누르는 시뮬레이션.
//            m.clear();
//            m["name"] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
//            m["function"] = "back";
//            procSoftkey(m);
#endif
        } else if (index == MenuAlternatives || menu == tr("다른 경로")) {
            blockSoftkey("OtherPass");

            QString caller = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);

            // 다른 경로 뜨면 어짜피 mIsActivate가 false가 되어야 함.
            // 다른 경로 뷰가 뜨면서 Setting을 Request 하게 되는 데 onActivated 이전에 처리가 되어서
            // 현재 View가 activate인 상태에서 ApplySetting을 받게 되어
            // 다른 경로 뷰가 받아야할 noti를 현재 뷰에서 처리하게 되어 문제가 발생함.
            // 이를 막기 위해 mIsActivate를 미리 false로 처리.
            mIsActivated = false;

            // 다른 경로 호출 후 경로 탐색하면 경로 선택에 의해 다시 카운트 다운 취소상태가 될 수 있어야 함.
            mIsCountdownCanceled = false;

            // 이전 경로 탐색 요청은 취소
            RouteSearchRequest req(this);
            req.cancel();

            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SettingRouteAlternateView);
            body[CommonConstants::Caller] = caller;
            body[SettingConstants::Settings] = makeRouteTypeSettings();
            sendNotification(Constants::CreateView, body);

            sendNotification(Constants::HideView, QVariantMap{{CommonConstants::ObjectName,caller}});

            // 다른 경로 화면으로 갔다가 복귀 시 ViewSizeChanged 에서 맵뷰 패딩을 강제 업데이트 하기 위한 플래그
            // 17.09.12 ymhong
            mUpdateMapViewPaddingForce = true;

            QStringList routeTypes;
            if (model) {
                auto count = model->count();
                if (count < 3) {
                    routeTypes += RouteSummaryHelper::routeToString(E_ROUTEOPTION_OPTIMUM);
                    routeTypes += RouteSummaryHelper::routeToString(E_ROUTEOPTION_SHORTESTWAY);
                    routeTypes += RouteSummaryHelper::routeToString(E_ROUTEOPTION_FREEWAY);
                } else {
                    for (int i=0; i<count; i++) {
                        auto vo = model->get(i).value<RouteSummaryVo*>();
                        routeTypes += vo->type();
                    }
                }
                sendNotification(RouteConstants::SelectRouteTypes, routeTypes);
            }
        } else if (index == OptionRouteSummary || menu == "경로 요약") {
            // 좀더 빠르게 띄우기 위해 openRouteSummaryView()를 옮김
            // 19.06.21 ymhong
            sendNotification(MapConstants::UpdateMapView, mSelectedRouteIndex);
            sendNotification(Constants::ShowView, QVariantMap{
                                 {CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteSummaryView)}
                             });

            // tvas 설정.
            QVariantMap m;
            auto routeId = mRpStatus.routeIdByIndex(mSelectedRouteIndex);
            auto routeOption = mRouteOptions.at(mSelectedRouteIndex);
            m["routeId"] = routeId;
            m["selectedId"] = mSelectedRouteIndex;
            m["distance"] = mDistance;
            m["fee"] = mFee;
            m["remainTime"] = mRemainTime;
            m["routeOption"] = routeOption;

            if (mIsContinued) {
                m["tvas"] = "tvas_route.db";
            } else {
                m["tvas"] = mRpStatus.getTvasFilename(mSelectedRouteIndex, mIsServerRequested);
            }
            sendNotification(RouteConstants::RequestRouteSummaryDetail, m);

            // RequestRouteSummaryDetail이 visible속성에 영향을 받음.
            QQmlProperty::write( mViewComponent, CommonConstants::Visible, false );
        } else if (index == OptionDrivingSimulation || menu == "모의 주행") {
            openSimulationView();
            QMetaObject::invokeMethod(mViewComponent, "cancelDrawRouteAll");
        }
    } else if (function == "back") {
        blockSoftkey("goBack()");
        goBack();

#ifdef QT_DEBUG
        // 테스트로 뒤로가기를 누른 시점에 안내 시작 자동안내가 시작되는 상황을 시뮬레이션
//        QVariantMap m;
//        m["name"] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
//        m["function"] = "menu";
//        m["menu"] = "StartGuideButton.qml";
        //procSoftkey(m);
#endif
    } else if (function == "option") {
        //QMetaObject::invokeMethod(mViewComponent, "showOptionMenu");

        //cancelCountdownMenu();
    }
}

void RouteCandidateViewMediator::goBack()
{
    RouteSearchRequest req(this);
    req.cancel();

    QVariantMap m;
    m["restore"] = true;
    sendNotification(RouteConstants::CloseRouteCandidateView, m);
    sendNotification(MapConstants::UpdateMapView, mSelectedRouteIndex);

    sendNotification(RgConstants::StopRouteGuidance);
}

void RouteCandidateViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void RouteCandidateViewMediator::cancelCountdownMenu()
{
    auto dataLoaded = QQmlProperty::read(mViewComponent, "dataLoaded").toBool();

    if (/*!mIsCountdownCanceled && */dataLoaded) {
        QStringList menus { tr("다른 경로"), tr("경유지 설정"), tr("안내시작") };
        setupSoftkeyMenu(menus);

        updateOtherPassMenu(mIsOnline);
        mIsSoftkeyCreated = true;
        mIsCountdownCanceled = true;
    }
}

void RouteCandidateViewMediator::updateStartRouteGuidanceMenu(bool enable)
{
    QVariantMap m;
    auto index = 2;
    auto menu = enable ? tr("안내시작") : tr("안내시작") + ":disabled";
    m["index"] = index;

#ifndef USE_SOFTKEY_INDEX_MENU
    m["menu"] = menu;
    sendNotification(ZoneConstants::EditSoftkeyMenu, m);
#else
    m["menu"] = QVariant::fromValue(SoftkeyMenu{index, menu});
    sendNotification(ZoneConstants::EditSoftkeyIndexMenu, m);
#endif
}

void RouteCandidateViewMediator::updateOtherPassMenu(bool enable)
{
    QVariantMap m;
    auto index = 0;
    auto menu = enable ? tr("다른 경로") : tr("다른 경로") + ":disabled";
    m["index"] = 0;

#ifndef USE_SOFTKEY_INDEX_MENU
    m["menu"] = menu;
    sendNotification(ZoneConstants::EditSoftkeyMenu, m);
#else
    m["menu"] = QVariant::fromValue(SoftkeyMenu{index, menu});
    sendNotification(ZoneConstants::EditSoftkeyIndexMenu, m);
#endif
}

void RouteCandidateViewMediator::updateMapViewPadding()
{
    // TMCRSM-3670
    // 목적지 변경 팝업이 뜬 상태에서 가젯으로 갔을 때
    // 5초후 경로 요약 화면으로 이행 하면서 UpdateMapViewPadding을 호출 하면서
    // 가젯 화면이 이상해짐.
    if (mIsGadgetMode) {
        return;
    }

    auto t = QQmlProperty::read(mViewComponent, "paddingTop").toInt();
    auto b = QQmlProperty::read(mViewComponent, "paddingBottom").toInt();
    auto m = QVariantMap{ {"t",t}, {"b", b} };

    sendNotification(MapConstants::UpdateMapViewPadding, m);
}

void RouteCandidateViewMediator::updateModel(const QString& from, const QString& to)
{
    auto model2 = copyModel();
    int routeCount = 3;

    QVariantMap m {{"model", QVariant::fromValue(model2)}, {"from", from}, {"to", to}};

    QMetaObject::invokeMethod(mViewComponent, "updateRouteSummary", Q_ARG(QVariant, m), Q_ARG(QVariant, routeCount));
}

void RouteCandidateViewMediator::updateRoadInfo(const QString &from, const QString &to)
{
    QMetaObject::invokeMethod(mViewComponent, "updateRoadInfo", Q_ARG(QVariant, from), Q_ARG(QVariant, to));
}

QVariant RouteCandidateViewMediator::copyModel() const
{
    RouteCandidateListModelPtr model2(new RouteCandidateListModel);

    int indexTbl[3] = { -1, -1, -1 };
    for (auto i=0; i<model->count(); i++) {
        auto pvo = model->get(i).value<RouteSummaryVo*>();
        if (pvo) {
            auto option = RouteSummaryHelper::routeToEnum(pvo->type());
            auto index = mRpStatus.optionIndex(option);
            if (0 <= index) {
                indexTbl[index] = i;
            } else {
                qDebug() << "Invalid optionIndex";
            }
        }
    }
    auto count = mRpStatus.optionCount();
    for (auto i=0; i<count; i++) {
        auto index = indexTbl[i];
        if (0 <= index) {
            auto pVo = RouteSummaryVoPtr(new RouteSummaryVo);
            *pVo = *(model->get(index).value<RouteSummaryVo*>());
            model2->append(pVo);
        } else {
            RouteSummaryVoPtr dummy(new RouteSummaryVo);
            auto option = static_cast<TeRouteOption>(mRpStatus.option(i));
            auto type = RouteSummaryHelper::routeToString(option);
            dummy->setType(type);
            model2->append(dummy);
        }
    }
    return QVariant::fromValue(model2);
}

void RouteCandidateViewMediator::blockSoftkey(const QString &reason)
{
    mBlockSoftkey = true;
    mBlockSoftkeyReason = reason;
}

void RouteCandidateViewMediator::unblockSoftkey()
{
    if (!mBlockSoftkey) {
        T_DEBUG("RouteCandidateViewMediator::unblockSoftkey() mBlockSoftkey : false !!!");
    } else {
        mBlockSoftkey = false;
    }
}

void RouteCandidateViewMediator::setupStartGuideMenu()
{
    QStringList menus { tr("다른 경로"), tr("경유지 설정"), QStringLiteral("StartGuideButton.qml") };
    if (mShowTimer) {
        menus.replace(2, tr("안내시작"));
    }

#ifndef USE_SOFTKEY_INDEX_MENU
    QVariantMap m;
    QStringList optionMenu { "경로 요약", "모의 주행" };
    m["menus"] = menus;
    m["optionMenu"] = optionMenu;
    sendNotification(ZoneConstants::SelectSoftkeyMenuMode, m);
#else
    setupSoftkeyMenu(menus);
#endif

    updateOtherPassMenu(mIsOnline);
    mIsSoftkeyCreated = true;
    mIsCountdownCanceled = false;
    mShowTimer = true;
}

void RouteCandidateViewMediator::postErrorViewClosed(const QVariant &v)
{
    sendNotification(RouteConstants::CancelRoute);

    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)){
        QVariantMap m;
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSettingView);
        m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::ErrorView);
        sendNotification(Constants::CreateView, m);
    }

    close();
}

void RouteCandidateViewMediator::setupSoftkeyMenu(const QStringList &menus, bool enalbeOption)
{
    QStringList optionMenu { "경로 요약", "모의 주행" };
    QVariantMap m;

#ifndef USE_SOFTKEY_INDEX_MENU
    m["optionMenu"] = optionMenu;
    m["menus"] = menus;
    sendNotification(ZoneConstants::SelectSoftkeyMenuMode, m);
#else
    QVariantList menus2;
    for (auto i = 0; i < menus.length(); i++) {
        menus2.append(QVariant::fromValue(SoftkeyMenu{i, menus[i]}));
    }
    m["menus2"] = menus2;
    m["defaultMenu"] = tr("안내시작");

    // TMCRSM-3095
    // 경로 안내 메뉴가 비활성화일 때 옵션 메뉴를 함께 비활성화 한다.
    // 아직 tvas가 설정된 상태가 아니라 경로 요약이나 모의 주행을 할 수 없는 상태.
    // 19.05.02 ymhong.
    QVariantList options2;
    auto enableStartRg = !menus[MenuStart].contains(":disabled");
    if (enableStartRg && enalbeOption) {
        options2.append(QVariant::fromValue(SoftkeyMenu{OptionRouteSummary, optionMenu[0]}));
        options2.append(QVariant::fromValue(SoftkeyMenu{OptionDrivingSimulation, optionMenu[1]}));
    }
    m["optionMenu2"] = options2;
    sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, m);
#endif
}

void RouteCandidateViewMediator::addDummyData()
{
#if (1)
    for (auto option : mRouteOptions) {
        auto vo = RouteSummaryVoPtr(new RouteSummaryVo);
        auto type = RouteSummaryHelper::routeToString(static_cast<TeRouteOption>(option));
        vo->setType(type);
        model->append(vo);
    }
#else
    auto vo = RouteSummaryVoPtr(new RouteSummaryVo);
    auto type = RouteSummaryHelper::routeToString(E_ROUTEOPTION_OPTIMUM);
    vo->setType(type);
    model->append(vo);
#endif
}

QVariant RouteCandidateViewMediator::makeRouteTypeSettings()
{
    QVariantMap m;
    auto func = [&](TeRouteOption o) {
        auto value = (0 <= mRpStatus.optionIndex(o)) ? 1 : 0;
        auto key = RouteSummaryHelper::routeToString(o);
        m[key] = QVariantMap {
            {CommonConstants::Type, 1},
            {CommonConstants::Section, "RouteAlternateSelection"},
            {CommonConstants::Key, key},
            {CommonConstants::Value, value}
        };
    };
    func(E_ROUTEOPTION_OPTIMUM);
    func(E_ROUTEOPTION_EXPRESSWAY);
    func(E_ROUTEOPTION_FREEWAY);
    func(E_ROUTEOPTION_SHORTESTWAY);
    func(E_ROUTEOPTION_EASY);
    func(E_ROUTEOPTION_SHORTTIME);

    return m;
}

void RouteCandidateViewMediator::showSwitchToLocalToast()
{
    QVariantMap m;
    m["position"] = ToastsConstants::ToastUp;
    m["message"] = tr("네트워크가 불안정하여 로컬 검색으로 전환되었습니다.");
    sendNotification(ToastsConstants::ShowToast, m);
}

bool RouteCandidateViewMediator::activated() const
{
    return mInitialized;
}

void RouteCandidateViewMediator::setActivated(bool activated)
{
    mInitialized = activated;
}
