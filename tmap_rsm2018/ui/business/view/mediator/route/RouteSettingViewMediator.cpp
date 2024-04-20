#include "RouteSettingViewMediator.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "MapConstants.h"
#include "NavigationControllerConstants.h"
#include "ZoneConstants.h"
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"
#include "PlatformHelper.h"
#include "QmlAppHelper.h"
#include "LangConstants.h"

#include <QTimer>
#include <QDebug>

#define DEPRECATE_CANCEL_ROUTE  // RouteConstants::CancelRoute를 사용하지 않기 위한 리펙토링 define
                                // 경유지 설정 화면에서 경로가 표시 되지 않도록 기존에 CancelRoute를
                                // 호출 하였으나 VSMMap_SetShowRoute()의 추가로 필요없게 되었음.

namespace SKT {

RouteSettingViewMediator::RouteSettingViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
}

RouteSettingViewMediator::~RouteSettingViewMediator()
{
    // onEvent() 의 Deactivated: 처리를 여기서 해준다.
    // close가 아닌... RouteRegistView가 떠 있을 때 불필요하게 RouteRegistView의 csw가 정리된다.
    // 19.03.25 ymhong.

    // 외부 인터페이스에 의해 닫히는 경우 close()가 호출되지 않아 여기서 정리 작업을 한다.
    if (!mCloseCalled) {
        sendNotification(RouteConstants::ClearViaMarker);
        sendNotification(NavigationControllerConstants::ResetCSW);
    }
}

QList<int> RouteSettingViewMediator::listNotificationInterests()
{
    static QList<int> interests {
        RouteConstants::CloseRouteSettingView,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        RouteConstants::UpdateViaInfo,
        RouteConstants::UpdateView,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        RouteConstants::CheckViaValidateIsOk,
        RouteConstants::CheckViaValidateIsFail,
        RouteConstants::RequestRouteUsingViaAsArrival,
        RouteConstants::RequestViaMenuText,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList
    };

    return interests;
}

bool RouteSettingViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            auto lang = QmlAppHelper::instance()->language();
            QQmlProperty::write(mViewComponent, "lang", lang);
        }
        break;

    case PoiClicked:
        {
            auto m = data.toMap();
            auto index = m["index"].toInt();
            sendNotification(NavigationControllerConstants::ResetCSW);

            QVariantMap body;

            body["slot"] = index;
            sendNotification(RouteConstants::SelectViaSlot, body);

            body["url"] = ViewRegistry::url(ViewRegistry::RouteRegistView);
            body["titleText"] = getTitleText(index);
            body["viaIndex"] = index;
            PlatformHelper::instance()->setParentForSystemPopup(body);
            sendNotification(Constants::CreateView, body);

            mViaMenuText = getViaMenuText(index);
        }
        break;

    case Activated:
        {
            sendNotification(MapConstants::ShowCarvata, false);
            sendNotification(RgConstants::BlockReroute, true);
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});

            // setup via marker
            sendNotification(RouteConstants::ClearViaMarker);
            sendNotification(RouteConstants::ShowViaMarker);

            // update mRect, sync()
            sendNotification(RouteConstants::RequestViaInfo);

            // drawMBRAll(...)
            sendNotification(MapConstants::DrawMarkerAll, mRect);
            sendNotification(MapConstants::EnableCallout, false);

            sendNotification(MapConstants::ShowRoute, QVariantMap{{"show", false}});

            sendNotification(SearchConstants::UpdateTextSearchZone1, false);
        }
        break;

    case SetupTitleAndZoneMenu:
        {
            auto m = data.toMap();
            auto title = m["title"];
            auto menus = m["menus"];
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::SelectZone1Title, title);
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, menus);
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{});
        sendNotification(MapConstants::EnableCallout, true);


        break;

    case ViewSizeChanged:
        {
            auto m = data.toMap();
            sendNotification(MapConstants::UpdateMapViewPadding, m);

            sendNotification(MapConstants::UpdateMapView, 0);
#ifndef DEPRECATE_CANCEL_ROUTE
            QTimer::singleShot(50, this, [&](){
                sendNotification(RouteConstants::CancelRoute);
            });
#endif
        }
        break;

    case PoiDeleteClicked:
        {
            int index = data.toInt();
            //if (index != 0) {
                sendNotification(RouteConstants::RemoveRouteManagerItem, index);
            //}
        }
        break;

    case ItemMoveDown:
        {
            QVariantMap m;
            m["index"] = data.toInt();
            m["offset"] = 1;
            sendNotification(RouteConstants::MoveRouteManagerItem, m);
        }
        break;

    case ItemMoveUp:
        {
            QVariantMap m;
            m["index"] = data.toInt();
            m["offset"] = -1;
            sendNotification(RouteConstants::MoveRouteManagerItem, m);
        }
        break;

    case Sync:
        sendNotification(RouteConstants::RequestViaInfo);
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case SelectCSW:
        sendNotification(NavigationControllerConstants::SelectCSW, data);
        break;
    }

    return false;
}

bool RouteSettingViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        updateIsRg(note->getBody().toMap());
        break;

    case NavigationControllerConstants::ChangeLanguage:
        changeLanguage(note->getBody());
        break;

    case RouteConstants::CloseRouteSettingView:
        {
            QVariant v = note->getBody();
            QQmlProperty::write( mViewComponent, CommonConstants::Visible, false );
        }
        break;

    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        procSoftkey(note->getBody());
        break;

    case RouteConstants::UpdateViaInfo:
        sync(note->getBody());
        break;

    case RouteConstants::UpdateView: {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::ObjectName)) {
                auto objName = data[CommonConstants::ObjectName].toString();
                if (objName != objectName)
                    return false;
            }

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){               
               if (CommonConstants::Url == it.key() || CommonConstants::ObjectName == it.key())
                   continue;
               QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
        }
        break;

    case RouteConstants::CheckViaValidateIsOk:
        {
            auto m = note->getBody().toMap();
            auto arrivalPos = m["arrivalPos"].toPoint();
            if (arrivalPos.isNull()) {
                QVariantMap m;
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);// ErrorView);
                m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
                m["title"] = tr("알림");
                m["message"] = tr("목적지가 없습니다.\n경유지를 목적지로 설정하여\n경로를 탐색하시겠습니까?");
                m["menus"] = QStringList{ tr("취소"), tr("확인") };
                PlatformHelper::instance()->setParentForSystemPopup(m);
                sendNotification(Constants::CreateView, m);
            } else {
                processSearch(mSearchMap);
                close();
            }
        }
        break;

    case RouteConstants::CheckViaValidateIsFail:
        addViaFail(note->getBody());
        break;

    case RouteConstants::RequestRouteUsingViaAsArrival:
        if (note->getBody().toBool()) {
            // 경유지를 목적지로 이용 다이얼로그에서 확인을 누른 경우
            processSearch(mSearchMap);
            close();
        }
        break;

    case RouteConstants::RequestViaMenuText:
        sendNotification(RouteConstants::ReplyToViaMenuText, mViaMenuText);
        break;

    case SettingConstants::DialogAccepted:
        {
            auto m = note->getBody().toMap();
            auto message = m["message"].toString();

            if (message == tr("목적지가 없습니다.\n경유지를 목적지로 설정하여\n경로를 탐색하시겠습니까?")) {
                sendNotification(RouteConstants::RequestRouteUsingViaAsArrival, true);
            } else {
                // 1. 경로탐색 최대거리를 초과하였습니다. 경로를 재설정 해주세요.
                // 2. 목적지가 없습니다. 경유지를 목적지로 설정하여 경로를 탐색하시겠습니까?
            }
        }
        break;
    case SettingConstants::DialogCanceled:
        sendNotification(RouteConstants::RequestRouteUsingViaAsArrival, false);
        break;

    case ApplicationConstants::ReplyToViewList: {
        auto m = note->getBody().toMap();
        auto requestView = m[CommonConstants::ObjectName].toString();
        if (requestView == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {
            auto viewList = m[ApplicationConstants::ViewIdList].value<QList<int>>();
            mIsRouteCandidateViewExist = viewList.contains(ViewRegistry::RouteCandidateView);
        }
        break;
    }

    default:
        return false;
    }

    return true;
}

void RouteSettingViewMediator::addViaFail(const QVariant &data)
{
    auto m = data.toMap();
    auto errorMessage = m["errorMessage"].toString();

    m.clear();
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
    m[QStringLiteral("title")] = tr("알림");
    m[QStringLiteral("message")] = errorMessage;
    m["showTimer"] = true;
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void RouteSettingViewMediator::changeLanguage(const QVariant &data)
{
    auto m = data.toMap();
    auto lang = m[LangConstants::Lang].toString();

    QQmlProperty::write(mViewComponent, "lang", lang);

    QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
}

void RouteSettingViewMediator::processSearch(const QVariant &data)
{
    QString theCaller = caller();
    bool callerIsRouteCandidateView = theCaller == ViewRegistry::objectName(ViewRegistry::RouteCandidateView);

    sendNotification( RgConstants::StopRouteGuidance );
    sendNotification( RouteConstants::CancelRoute );

    // 경우에 따라 caller가 RouteCandidateView 이지만 해당 뷰가 닫히는 경우가 있다.
    // 경로 탐색을 마치기 전에 경로 요약 화면에서 경유지 설정 화면으로 왔는데 탐색이 실패한 경우.
    // 이때 경로탐색을 하면 RouteCandidateView가 없어서 맵뷰로 복귀 됨
    // 뷰가 닫힌 경우 뷰를 다시 생성할 수 있도록 처리한다.
    // 19.01.11 ymhong.
    auto viewName = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
    sendNotification( ApplicationConstants::RequestViewList, QVariantMap {
                          { CommonConstants::ObjectName, viewName }
                      } );

    QVariantMap m;
    if (mIsRouteCandidateViewExist && callerIsRouteCandidateView) {
        // 경로 요약 화면을 표시하기 전 백업해 두었던 경로 요약 정보를 초기화 한다.
        // 새로 경로 탐색을 요청 하므로 필요 없음
        sendNotification(RouteConstants::ResetRouteCandidateView);

        m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
        sendNotification(Constants::ShowView, m);
        // TMCRSM-776
        // 온라인에서 다른 경로를 설정한 경우 설정값이 RouteCandidateView에 남아 있기 때문에
        // 재탐색 시 환경 설정에서 선택한 값으로 초기화 해준다.
        // 18.06.05

        // TMCRSM-2002
        // 경로 탐색중(다른 경로, 경유지 설정 포함)에는 환경 설정과 별개로 설정값을 따로 유지
    } else {
        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteCandidateView);
        m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
        sendNotification(Constants::CreateView, m);
    }

    QMetaObject::invokeMethod(mViewComponent, "hide");
    close();

    if (callerIsRouteCandidateView) {
        m.clear();
        m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
        sendNotification(RouteConstants::RequestRoute, m);
    }
    sendNotification(RouteConstants::SelectViaSlot, QVariantMap{{"slot", -1}});
}

void RouteSettingViewMediator::processSearchFail(const QVariant &data)
{

    auto m = data.toMap();

    auto errorMessage = m["errorMessage"].toString();

    errorMessage += tr("\n경로를 재설정 해주세요.");

    sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::ErrorView)},
                                                        {CommonConstants::Caller,ViewRegistry::objectName(ViewRegistry::RouteSettingView)},
                                                        {"errorMessage", errorMessage},
                                                        {"showErrorCode", false},
                     });
}

void RouteSettingViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m["name"].toString() != "RouteSettingView")
        return;

    QString function = m["function"].toString();
    if (function == "back") {
        auto theCaller = caller();

        if (theCaller == ViewRegistry::objectName(ViewRegistry::MapControlView) ||
            theCaller == ViewRegistry::objectName(ViewRegistry::SearchResultDetailView)) {
            // 경유지 선택 시 마커 표시 이슈 때문에 경로 취소가 되기 때문에
            // 주행 중 경유지 선택 화면으로 들어왔다 뒤로 가기를 누르면
            // 주행 모드를 복구
            if (mIsRg) {
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Navi}});
                sendNotification(ZoneConstants::SelectZone1TextInput);
                sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
                sendNotification(MapConstants::UpdateMapViewPadding);
                sendNotification(RouteConstants::ResumeRoute);
            } else {
                sendNotification(ZoneConstants::SelectZone1TextInput);
                sendNotification(MapConstants::UpdateMapViewPadding);
            }

            // ManageVsmNaviCommand.cpp 에서 아래 코드를 조건에 따라 호출 하도록 변경
            // 18.01.05 ymhong.
            //sendNotification(RgConstants::ChangeRouteGuideBindState, QVariant::fromValue(1));
        } else if (theCaller == ViewRegistry::objectName(ViewRegistry::ErrorView)) {
            sendNotification(RouteConstants::ClearRouteDirectory);
            sendNotification(RouteConstants::RestoreViaData);
            sendNotification(RouteConstants::ResumeRoute);

            if (!mIsRg) {
                sendNotification(RouteConstants::CancelRoute);
            }
        } else {
            sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName, theCaller}});

            // 경로 요약화면의 경우 자체적으로 tvas 복구. (중복 처리 방지!!)
            if (theCaller != ViewRegistry::objectName(ViewRegistry::RouteCandidateView))
                sendNotification(RouteConstants::RestoreRoute);
        }
        sendNotification(RouteConstants::RestoreViaData);
        sendNotification(RouteConstants::SelectViaSlot, QVariantMap{{"slot", -1}});
        close();
    } else if (function == "menu") {
        QString menu = m["menu"].toString();
        auto index = -1;
        if (m.contains("index")) {
            index = m["index"].toInt();
            menu.clear();
        }

        if (index == MenuRoutePlanning || menu == tr("경로탐색")) {
            mSearchMap = data;

            // 출발지, 경유지, 목적지가 가까운지 검사 -> 경로 탐색이 아닌 경유지 추가 시점에서 체크.
            sendNotification(RouteConstants::CheckViaValidate);
            //processSearch(data);
        }
    }
}

void RouteSettingViewMediator::close()
{
    sendNotification(NavigationControllerConstants::ResetCSW);

    sendNotification(RouteConstants::ClearViaMarker);
    QMetaObject::invokeMethod(mViewComponent, "close");

    mCloseCalled = true;
}

void RouteSettingViewMediator::sync(const QVariant &data)
{
    QVariantMap m = data.toMap();

    QMetaObject::invokeMethod(mViewComponent, "updateStartPoi", Q_ARG(QVariant, m["Via1"].toString()));
    QMetaObject::invokeMethod(mViewComponent, "updateVia1", Q_ARG(QVariant, m["Via2"].toString()));
    QMetaObject::invokeMethod(mViewComponent, "updateVia2", Q_ARG(QVariant, m["Via3"].toString()));
    QMetaObject::invokeMethod(mViewComponent, "updateVia3", Q_ARG(QVariant, m["Via4"].toString()));
    QMetaObject::invokeMethod(mViewComponent, "updateEndPoi", Q_ARG(QVariant, m["Via5"].toString()));

    QMetaObject::invokeMethod(mViewComponent, "updateVisualItems");

    QList<QPoint> points;
    for (auto i = 0; i < 5; i++) {
        auto key = QString("Via%1Addr").arg(i+1);
        auto value = m[key];
        if (!value.isNull())
            points.append(value.toPoint());
    }
    if (points.count()) {
        QRect rect = { points[0], points[0] };
        for (auto i = 1; i<points.count(); i++) {
            auto point = points[i];
            auto x = point.x();
            auto y = point.y();
            if (x < rect.left())
                rect.setLeft(x);
            if (rect.right() < x)
                rect.setRight(x);
            if (y < rect.top())
                rect.setTop(y);
            if (rect.bottom() < y)
                rect.setBottom(y);
        }

        mRect = rect;
    }
}

void RouteSettingViewMediator::updateIsRg(const QVariantMap &m)
{
    mIsRg = m["isRg"].toBool();
}

QString RouteSettingViewMediator::caller() const
{
    return QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
}

QString RouteSettingViewMediator::getTitleText(int index) const
{
    switch (index) {
    case 0:
        return tr("출발지 등록");
    case 1:
    case 2:
    case 3:
        return tr("경유지 등록");
    case 4:
        return tr("목적지 등록");

    default:
        return QString();
    }
}

QString RouteSettingViewMediator::getViaMenuText(int index) const
{
    switch (index) {
    case 0:
        return tr("출발지로");
    case 1:
    case 2:
    case 3:
        return tr("경유지로");
    case 4:
        return tr("목적지로");

    default:
        return QString();
    }
}

} //SKT
