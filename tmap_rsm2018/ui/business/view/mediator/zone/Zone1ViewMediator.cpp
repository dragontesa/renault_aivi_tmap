#include "Zone1ViewMediator.h"

#include <QQmlProperty>

#include "Constants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "navigationdata.h"
#include "ViewRegistry.h"
#include "ApplicationConstants.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SearchConstants.h"

#include "rprequest.h"
#include "PlatformHelper.h"

#ifdef BUILD_TARGET
#   include "hmiZonetype.h"
#   define  POPUP_SURFACE_ID    (2003)
#   define  TOAST_SURFACE_ID    (3003)
#endif

#include "PlatformHelper.h"
#include "StringHelper.h"

#define APPID_ENAV  3

namespace SKT {

Zone1ViewMediator::Zone1ViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent),
      mKeyboardOwner(CommonConstants::Empty)
{

}

QList<int> Zone1ViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        ZoneConstants::SelectZone1TextInput,
        ZoneConstants::SelectZone1Title,
        ZoneConstants::FocusZone1TextInput,
        ZoneConstants::UpdateZone1,
        ZoneConstants::FocusZone1TextInput,        
        ZoneConstants::UpdateModal,
        ZoneConstants::UpdateSearchResultCount,
        ZoneConstants::UpdateSearchText,
        ZoneConstants::RequestZone1Text,
#ifdef QT_DEBUG
        NavigationControllerConstants::ChangeOnlineStatus,
        RouteConstants::RequestCommandResponsed,
        RgConstants::BlockReroute,
        MapConstants::ApplyMapViewMode,
        MapConstants::UpdateMapViewPadding,
        MapConstants::GlobalMapViewViewLevelChanged,
        RouteConstants::SetTvas,
        RouteConstants::CancelRoute,
        RouteConstants::ResumeRoute,
        RgConstants::UpdateGpsTime,
        RgConstants::UpdatePeriodicRerouteArea,
        SearchConstants::RequestCommand,
        RouteConstants::RequestCommandResponsed,
        RouteConstants::RerouteCommandRequested,
        NavigationPropertyConstants::setNavigationProperty,
        ZoneConstants::RegistSoftkey,
#endif
        KeyboardConstants::NotifyKeyboardOwner,
        NavigationControllerConstants::UpdateEditFieldText,
        ApplicationConstants::ChangeToApp,
        ApplicationConstants::ChangeToWidget,
        ApplicationConstants::ReplyToViewList,
    };

    return interests;
}

void Zone1ViewMediator::onActiveNotificationType(uint activeNotification)
{
    qDebug() << "Zone1ViewMediator::onActiveNotificationType(" << activeNotification << ")";
}

void Zone1ViewMediator::onButtonPressed(uchar appId, uint buttonId, uchar buttonState)
{
#ifdef BUILD_TARGET
    if (appId != APPID_ENAV) {
        // 홈 화면 등에서 누른 경우 내비화면으로 전환
        // 19.06.28 ymhong.
        if (buttonId == BTN_NAVI_FEEDBACK) {
            PlatformHelper::instance()->switchToApp();
        }
        return;
    }

    switch (buttonId) {
    case BTN_Z1_SEARCH:
    case BTN_Z1_SEARCH_TEXT_AREA:
    case BTN_Z1_SEARCH_TEXT_002_FREE_MODE:
        onTextInputFocusActivated(QVariant{});
        break;

    case BTN_Z1_SEARCH_NUMBER:
        pulldownKeyboard();
        break;

    case BTN_Z1_SEARCH_KEYBOARD:
        pullupKeyboard();
        PlatformHelper::instance()->setEditFieldText(mSearchBarText);
        break;

    case BTN_Z3_BACK:
        sendNotification(KeyboardConstants::SystemKeyboardBackPressed, QVariantMap{
                             {"key", -3} //CodeCancel Keyboard.js
                         });
        sendNotification(KeyboardConstants::ProcessSystemKeyboard, QVariantMap{
                             {"key", -3} //CodeCancel Keyboard.js
                         });
        break;
    }
#endif
}

void Zone1ViewMediator::onKeyboardInputFinished(uchar appId, const QString &searchbarText)
{
    if (appId != APPID_ENAV)
        return;

    // TMCRSM-3708
    // 보쉬에서 더 이상 onKeyboardInputFinished에서 전달된 searchbarText를 사용하지 말라는 가이드가 있어서
    // 미리 캐싱해 두었던 검색어로 검색을 하도록 수정.
    // 19.09.04 ymhong

#ifdef BUILD_TARGET
    qDebug() << "[@@@@] Zone1ViewMediator::onKeyboardInputFinished(searchText:" << searchbarText << ")";
    auto confirmKeyCode = -101; // Keyboard.js
    QVariantMap m;
    m["key"] = confirmKeyCode;
    m["text"] = mSearchBarText;//searchbarText;
    sendNotification(KeyboardConstants::SystemKeyboardFinished, m);
    sendNotification(KeyboardConstants::ProcessSystemKeyboard, m);
#endif
}

void Zone1ViewMediator::onKeyboardKeyPressed(uchar appId, const QString &searchbarText, const QString &keyPressed, bool isCtrl, bool IsSpeedLockActive)
{
    Q_UNUSED(isCtrl)
    Q_UNUSED(IsSpeedLockActive)

    if (appId != APPID_ENAV)
        return;

    mSearchBarText = searchbarText;

#ifdef BUILD_TARGET
    qDebug() << "[@@@@] Zone1ViewMediator::onKeyboardKeyPressed(searchText:" << searchbarText << ", keyPressed:" << keyPressed << ")";

    // Zone2에서 키보드입력을 사용하는 경우(OTP, 즐겨찾기 명칭 등)
    // 이 메시지를 직접 처리한다.
    // 19.03.07 ymhong.
    if (keyPressed == SYSTEM_SHIFT_KEY) {
        // 전달하지 않음.
    } else {
        sendNotification(KeyboardConstants::SystemKeyboardPressed, QVariantMap{
                             {"text", searchbarText },
                             {"key", keyPressed},
                             {"ctrl", isCtrl},
                             {"speedLock", IsSpeedLockActive}
                         });
    }

    //PlatformHelper::instance()->setEditFieldTextForKeyboardKeyPressed(searchbarText);

#if (0) // ZONE1인 경우 더이상 keyPressed는 전달되지 않음 19.06.26 ymhong
    auto backspacePressed = keyPressed == SYSTEM_BACKSPACE_KEY;
    if (searchbarText == "" && !backspacePressed) {
        // 키보드 입력중에 키보드의 이전키가 아닌 hide로 감춘 경우
        // 키보드를 다시 show 하면 searchText가 공백으로 keyPressed에 이전에 입력중이던
        // 문자열이 전달됨
        // 즐겨 찾기 명칭 편집 시 팝업의 '취소'를 눌러 닫는 경우.
        return;
    }
#endif

    QVariantMap m;
    //owner는 KeyboardViewMediator에서 설정.
    m["key"] = keyPressed;
    m["text"] = searchbarText;

    sendNotification(KeyboardConstants::ProcessSystemKeyboard, m);
#endif
}

void Zone1ViewMediator::onPullupButtonPressed(uchar appId, uchar buttonId, uchar buttonState)
{
#ifdef BUILD_TARGET
    qDebug() << "Zone1ViewMediator::onPullupButtonPressed(" << buttonId << ", " << buttonState << ")";
#endif
}

void Zone1ViewMediator::onTogglePressed(uchar appId, uint buttonId, uchar buttonState, bool toggleValue)
{
    qDebug() << "Zone1ViewMediator::onTogglePressed(" << appId << ", " << buttonId << ", " << buttonState << ", " << toggleValue << ")";
}

void Zone1ViewMediator::onPopupStatus(uint surfaceId, uint popupStatus)
{
#ifdef BUILD_TARGET
    if (surfaceId == POPUP_SURFACE_ID) {
        auto show = popupStatus != 0;

        PlatformHelper::instance()->showPopupWindow(show);
    }
#endif
}

bool Zone1ViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case RegistCommonKeyboardInput:
        sendNotification(KeyboardConstants::RegistCommonKeyboardInput, data);
        break;
    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case PullDownKeyboard:
        pulldownKeyboard();
        break;

    case PullUpKeyboard:
        pullupKeyboard();
        break;

    case TextInputFocusActivated:
        onTextInputFocusActivated(data);
        break;

    case Mediated: {
            QVariant v = QVariant::fromValue(mViewComponent);
            sendNotification(ZoneConstants::RegistZone1, v);
            sendNotification(KeyboardConstants::RegistKeyboardInput, data);

#ifdef BUILD_TARGET
            // 타겟에서는 fake 배경 이미지를 노출하지 않음.
            QQmlProperty::write(mViewComponent, "visible", false);
#endif

#ifdef QT_DEBUG
            QQmlProperty::write(mViewComponent, "onlineStatusVisible", true);
#endif

#ifdef BUILD_TARGET
            // NoMap일 경우 layermanager가 initialize 되었을 때 Zone1을 TITLE로 설정해야 함.
            // 19.07.03 ymhong
            auto noMap = QQmlProperty::read(mViewComponent, "noMap").toBool();
            if (noMap) {
                PlatformHelper::instance()->appHasNoMap();
            }

            PlatformHelper::instance()->setZone1SignalHandler(this);
            // 타겟에서는 임시 ui를 보여줄 필요가 없다.
            QQmlProperty::write(mViewComponent, "visible", false);

            QVariantMap m;
            m["create"] = true;
            sendNotification(KeyboardConstants::ShowKeyboard, m);
#endif
        }
        break;

    case ToggleGuidView:
        {
#ifdef QT_DEBUG
            QVariantMap m;
            if (mGuidViewVisible) {
                m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::TestGuidView);
                sendNotification(Constants::CloseView, m);
            } else {
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::TestGuidView);
                sendNotification(Constants::CreateView, m);
            }
            mGuidViewVisible = !mGuidViewVisible;
#endif
        }
        break;

    case RequestNomalize:
        {
            auto vals = data.value<QVariantMap>();
            QString text = vals[CommonConstants::Text].toString();
#ifndef BUILD_TARGET
            text = StringHelper::instance()->normalized(text);
            //QMetaObject::invokeMethod(mViewComponent, "normalized", Q_ARG(QVariant,text));
#endif

        }
        break;

    default:
        return false;
    }
    return true;
}

bool Zone1ViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case ZoneConstants::SelectZone1Title: {
#ifdef BUILD_TARGET
            QVariantMap m;
            m["text"] = note->getBody();
            m["updateRegion"] = true;
            PlatformHelper::instance()->setZone1(TITLE, m);
#else
            QMetaObject::invokeMethod(mViewComponent, "setTitle", Q_ARG(QVariant,note->getBody()));
#endif
        }
        break;

    case ZoneConstants::SelectZone1TextInput: {
            //2017.11.10 cjlee - 텍스트를 쓰기 위해 함수 에 인자 추가
            auto data = note->getBody().value<QVariantMap>();
            auto text = data[CommonConstants::Text].toString();
            auto sender = QString::fromLatin1(note->getSource()->metaObject()->className());
            qDebug() << "ZoneConstnats::SelectZone1TextInput(" << text << ") sender: " << sender;

#ifdef BUILD_TARGET
            sendNotification(ApplicationConstants::RequestViewList);

            bool skip = false;
            bool skipNavSearchBar = false;
            if (sender.contains(ViewRegistry::objectName(ViewRegistry::TextSearchView))) {
                auto caller = data[CommonConstants::Caller].toString();
                if (caller != ViewRegistry::objectName(ViewRegistry::RouteSettingView) &&
                    caller != ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                    skip = true;

                    qDebug() << "ZoneConstnats::SelectZone1TextInput( skip, caller: " << caller << " )";
                }
            } else if (sender.contains(ViewRegistry::objectName(ViewRegistry::RecentSearchPageView))) {
                skip = true;
            } else {
                // 가젯 모드로 부터 복귀했을 때 각뷰가 Activate되면서 불필요한 NAV_SEARCH_BAR를 설정함.
                // 뷰를 체크해서 실제 NAV_SEARCH_BAR를 설정하는 경우인지 검사
                // 19.07.21 ymhong

                int lastView = 0;
                for (auto iter=mViewList.rbegin(); iter !=mViewList.rend(); iter++) {
                    auto viewId = *iter;
                    auto viewData = mViewListData[viewId];
                    auto visible = viewData[ApplicationConstants::Visible].toBool();
                    auto active = viewData[ApplicationConstants::Active].toBool();

//                    auto a = visible ? "V" : " ";
//                    auto b = active ? "O" : "X";
//                    qDebug() << "[VIEW] " << a << b << " : " << ViewRegistry::objectName(viewId);

                    if (!visible || !active)
                        continue;

                    switch (viewId) {
                    case ViewRegistry::ProgressIndicatorView:
                        continue;

                    default:
                        break;
                    }
                    lastView = viewId;
                }

                //qDebug() << "[VIEW] lastView: " << ViewRegistry::objectName(lastView);

                switch (lastView) {
                case ViewRegistry::BookmarkPageView:
                case ViewRegistry::RecentDestinationPageView:
                    {
                        // 최근목적지와 즐겨찾기의 경우 편집 모드인지 체크해서 편집모드라면 skip처리한다.
                        // 해당 편집 모드뷰의 타이틀이 유지 되어야 함.
                        // 절대 NAV_SEARCH_BAR로 변경 되어서는 안됨.
                        // 19.07.21 ymhong
                        auto viewId = lastView == ViewRegistry::BookmarkPageView ? ViewRegistry::BookmarkView
                                                                                 : ViewRegistry::RecentDestinationEditView;
                        if (mViewList.contains(viewId)) {
                            auto viewVisible = mViewListData[viewId][CommonConstants::Visible].toBool();
                            if (viewVisible) {
                                skipNavSearchBar = true;
                            }
                        }
                    }
                    break;

                case ViewRegistry::MapView:
                    break;

                default:
                    // 가끔 앱으로 전환 후에도 Widget이 lastView인 경우가 있음
                    // NavigationControllerProxy에서 보낸 경우 확실히 text input을 셋팅
                    // 19.08.06 ymhong
                    if (!sender.contains("NavigationControllerProxy")) {
                        skipNavSearchBar = true;
                    }
                    break;
                }
            }
            if (skip) {
                // TextSearchView에서 setZone1()을 중복 호출 함.
            } else {
                QVariantMap m;
                m["text"] = text;
                if (text.length() > 0) {
                    PlatformHelper::instance()->setZone1(SEARCH_TEXT_1, m);
                } else {
                    if (!skipNavSearchBar) {
                        PlatformHelper::instance()->setZone1(NAV_SEARCH_BAR, m);
                    }
                }
            }
#else
            QMetaObject::invokeMethod(mViewComponent, "setTextInput", Q_ARG(QVariant,data[CommonConstants::Text]));
#endif
        }
        break;

    case ZoneConstants::UpdateZone1:
        updateZone1(note->getBody());
        break;

    case ZoneConstants::FocusZone1TextInput:
        T_DEBUG( QString( "FocusZone1TextInput %1" ).arg( note->getSource()->metaObject()->className() ) );
        QMetaObject::invokeMethod(mViewComponent, "__inputFieldForceActiveFocus");
        break;

    case ZoneConstants::UpdateSearchText:
        mSearchBarText = note->getBody().toString();
        break;

    case ZoneConstants::UpdateSearchResultCount:
        {
            auto m = note->getBody().toMap();
            auto source = note->getSource()->metaObject()->className();
            m["source"] = source;

            // 텍스트 검색이 아닌 검색 결과의 경우 키보드를 감춘다.
            if (!m.contains("show")) {
                auto caller = m["caller"].toString();

                // TextSearchView에서 스크롤을 하여 키보드를 내린 후(pulldown)에는
                // 키보드가 노출 되지 않도록 isKeyboardPulldown()을 검사한다.
                // 19.03.22 ymhong.
                m["show"] =
                        (caller == ViewRegistry::objectName(ViewRegistry::TextSearchView)) &&
                        !PlatformHelper::instance()->isKeyboardPulldown();
            }

            // Zone1에 설정된 mSearchBarText와 맞지 않으면 UpdateSearchCount가 갱신되지 않음.
            auto text = m["text"].toString();
            if (mSearchBarText == text) {
                PlatformHelper::instance()->setupKeyboard(m);
                //PlatformHelper::instance()->setEditFieldText(text);
            }
#ifndef BUILD_TARGET
            auto count = m["count"].toInt();
            QQmlProperty::write(mViewComponent, "searchResultCount", count);
#endif
        }
        break;

#ifdef QT_DEBUG
    case ZoneConstants::RegistSoftkey:
        QQmlProperty::write(mViewComponent, "activeView", note->getBody());
        break;

    case NavigationControllerConstants::ChangeOnlineStatus:
        {
            auto online = note->getBody().toBool();
            QQmlProperty::write(mViewComponent, "online", online);
        }
        break;

    case RgConstants::BlockReroute:
        {
            bool enableReroute = note->getBody().toInt() == 0;
            QQmlProperty::write(mViewComponent, "enableReroute", enableReroute);
        }
        break;

    case RgConstants::UpdateGpsTime:
        QQmlProperty::write(mViewComponent, "gpsTime", note->getBody());
        break;

    case MapConstants::ApplyMapViewMode:
        {
            auto m = note->getBody().toMap();
            auto mode = m[CommonConstants::Mode].toInt();
            QString s;
            /*
             enum ModeK {
                Normal = 0, // 초기 모드(경로없는 Navi 모드)
                MapControl, // 맵을 터치했을 때
                MapInquiry, // 조회 모드
                Navi, //
                MenuNavi, // 17.10.10 hskim 내비메뉴, 환경설정 화면 맵 주행 상태 표시를 위해 추가(sdi 표시, 클릭x)
                Simul,
                NormalMapControl
            };
            */
            switch (mode) {
            case 0: s = "N"; break;
            case 1: s = "M"; break;
            case 2: s = "Q"; break;
            case 3: s = "V"; break;
            case 4: s = "v"; break;
            case 5: s = "S"; break;
            case 6: s = "C"; break;
            }
            QQmlProperty::write(mViewComponent, "mapMode", s);
        }
        break;

    case MapConstants::UpdateMapViewPadding:
        updateMapViewPadding(note->getBody());
        break;

    case MapConstants::GlobalMapViewViewLevelChanged:
        QQmlProperty::write(mViewComponent, "mapViewLevel", note->getBody());
        break;

    case RgConstants::UpdatePeriodicRerouteArea:
        QQmlProperty::write(mViewComponent, "periodicRerouteArea", note->getBody().toBool());
        break;

    case RouteConstants::SetTvas:
    case RouteConstants::ResumeRoute:
        {
            QQmlProperty::write(mViewComponent, "setTvas", true);
            if (note->getId() == RouteConstants::SetTvas) {
                auto list = note->getBody().toList();
                auto filenames = QStringList{};
                for (auto i=0; i<list.length(); i++) {
                    if (i%2 == 1) {
                        filenames += list[i].toString();
                    }
                }
                QMetaObject::invokeMethod(mViewComponent, "setTvasFilename", Q_ARG(QVariant, filenames));
            }
        }
        break;
    case RouteConstants::CancelRoute:
        QQmlProperty::write(mViewComponent, "setTvas", false);
        break;

    case SearchConstants::RequestCommand:
        {
            auto m = note->getBody().toMap();
            auto requestType = m["RequestType"].toInt();
            switch (requestType) {
            case RPRequest::RequestRouteSummary:
                break;
            case RPRequest::RequestCancel:
                QQmlProperty::write(mViewComponent, "rpRequest", false);
                break;
            case RPRequest::RequestRoute:
            case RPRequest::RequestReRouteForce:
            case RPRequest::RequestReRouteByAuto:
            case RPRequest::RequestReRouteByBreakaway:
                QQmlProperty::write(mViewComponent, "rpRequest", true);
                break;
            }
        }
        break;

    case RouteConstants::RerouteCommandRequested:
        QQmlProperty::write(mViewComponent, "rpRequest", true);
        break;

    case RouteConstants::RequestCommandResponsed:
        {
            auto m = note->getBody().toMap();
            updateViaInfo(m);
            auto reqType = m["RequestType"].toInt();
            if (reqType != RPRequest::RequestCancel) {
                QQmlProperty::write(mViewComponent, "rpRequest", false);
            }
        }
        break;
    case NavigationPropertyConstants::setNavigationProperty:
        {
            auto m = note->getBody().toMap();
            auto type = m["type"].toInt();
            if (type == NavigationPropertyProtocol::TmapNaviStatus) {
                auto value = m["propertyValue"].toInt();
                QString s;
                switch (value) {
                case TMAPNAVISTATUS_UNKNOWN: s = "Unknown"; break;
                case TMAPNAVISTATUS_BUSY: s = "Busy"; break;
                case TMAPNAVISTATUS_MAP_UPDATING: s = "Map update"; break;
                case TMAPNAVISTATUS_ANDO: s = "Ando"; break;
                case TMAPNAVISTATUS_PLAYROUTE: s = "RG"; break;
                case TMAPNAVISTATUS_CALCULATING_ROUTE: s = "RP..."; break;
                case TMAPNAVISTATUS_MAP_RECOVERY:
                case TMAPNAVISTATUS_STANDBY:
                    s = "NG";
                    break;
                }
                QQmlProperty::write(mViewComponent, "naviStatus", s);
            }
        }
        break;
#endif //QT_DEBUG
    case KeyboardConstants::NotifyKeyboardOwner: {
            auto v = note->getBody().value<QVariant>();
            auto owner = v.toString();
            mKeyboardOwner = owner;
        }
        break;

    case ZoneConstants::UpdateModal:
        {
#ifdef QT_DEBUG
            auto m = note->getBody().toMap();;
            auto modal = m[QStringLiteral("modal")].toBool();
            QMetaObject::invokeMethod(mViewComponent, "updateModal", Q_ARG(QVariant, modal));
#endif
        }
        break;

    case NavigationControllerConstants::UpdateEditFieldText:
        // Bosch 요청
        // 1.4.3.1 Zone3PullUpListState 의 변경이 생겼을 때 SetEditfieldText로 마지막 searchBarText전달.
        // 19.05.29 ymhong.
        qDebug() << "NavigationControllerConstants::UpdateEditFieldText:" << mSearchBarText;
        PlatformHelper::instance()->setEditFieldText(mSearchBarText);
        break;

    case ApplicationConstants::ChangeToApp:
#ifndef BUILD_TARGET
        QQmlProperty::write(mViewComponent, "visible", true);
#endif
        break;
    case ApplicationConstants::ChangeToWidget:
        QQmlProperty::write(mViewComponent, "visible", false);
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto m = note->getBody().toMap();
            mViewList = m[ApplicationConstants::ViewIdList].value<QList<int>>();
            mViewListData = m[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
        }
        break;

    case ZoneConstants::RequestZone1Text:
        sendNotification(ZoneConstants::ReplyToZone1Text, mSearchBarText);
        break;
    }

    return false;
}

void Zone1ViewMediator::updateMapViewPadding(const QVariant &v)
{
    auto m = v.toMap();
    auto l = m["l"].toInt();
    auto t = m["t"].toInt();
    auto r = m["r"].toInt();
    auto b = m["b"].toInt();
    auto str = QString("l:%1,t:%2,r:%3,b:%4").arg(l).arg(t).arg(r).arg(b);
    QQmlProperty::write(mViewComponent, "padding", str);
}

void Zone1ViewMediator::updateViaInfo(const QVariantMap &m)
{
    auto via1 = m["Via1"].value<RoutePosPtr>();
    auto via2 = m["Via2"].value<RoutePosPtr>();
    auto via3 = m["Via3"].value<RoutePosPtr>();
    auto via4 = m["Via4"].value<RoutePosPtr>();
    auto via5 = m["Via5"].value<RoutePosPtr>();

    QString str = "1 - ";
    if (via1) {
        auto x = via1->tp.x;
        auto y = via1->tp.y;

        str += QString("%1,%2").arg(x).arg(y);
        if (0 == x && 0 == y)
            str = "";

        QQmlProperty::write(mViewComponent, "via1", str);
    }
    str = "2 - ";
    if (via2) {
        auto x = via2->tp.x;
        auto y = via2->tp.y;

        str += QString("%1,%2").arg(x).arg(y);
        if (0 == x && 0 == y)
            str = "";

        QQmlProperty::write(mViewComponent, "via2", str);
    }
    str = "3 - ";
    if (via3) {
        auto x = via3->tp.x;
        auto y = via3->tp.y;

        str += QString("%1,%2").arg(x).arg(y);
        if (0 == x && 0 == y)
            str = "";

        QQmlProperty::write(mViewComponent, "via3", str);
    }
    str = "4 - ";
    if (via4) {
        auto x = via4->tp.x;
        auto y = via4->tp.y;

        str += QString("%1,%2").arg(x).arg(y);
        if (0 == x && 0 == y)
            str = "";

        QQmlProperty::write(mViewComponent, "via4", str);
    }
    str = "5 - ";
    if (via5) {
        auto x = via5->tp.x;
        auto y = via5->tp.y;

        str += QString("%1,%2").arg(x).arg(y);
        if (0 == x && 0 == y)
            str = "";

        QQmlProperty::write(mViewComponent, "via5", str);
    }
}

void Zone1ViewMediator::onTextInputFocusActivated(const QVariant &data)
{
    qDebug() << "Zone1ViewMediator::onTextInputFocusActivated()";

    sendNotification(ZoneConstants::FocusActivatedZone1TextInput, data);
    if (mKeyboardOwner == CommonConstants::Empty) {
        // TMCRSM-132
        // 주행중 안도모드에서 맵을 터치 후 화면에 커서가 표시된 상태에서
        // 상단 통합 검색을 터치했을 때 커서 표시 화면은 닫는다.
        // 17.12.05 ymhong.
        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, "MapControlView"}});
        sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::SearchTabView)}});
    }
}

void Zone1ViewMediator::updateZone1(const QVariant &v)
{
    auto appeared = v.toBool();
    if (appeared != mIsAppForground) {
        mIsAppForground = appeared;
        PlatformHelper::instance()->setAppIsAppear(appeared);
    }
}

void Zone1ViewMediator::pulldownKeyboard()
{
    sendNotification(KeyboardConstants::PullDownKeyboard);
}

void Zone1ViewMediator::pullupKeyboard()
{
    sendNotification(KeyboardConstants::PullUpKeyboard);
}

}//SKT
