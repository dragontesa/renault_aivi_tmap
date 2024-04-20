#include "MapViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include <QWindow>
#include <QMouseEvent>
#include <QQuickItem>
#include "RouteManager.h"
#include "StringHelper.h"
#include "SimulationViewMediator.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"
#include "PlatformHelper.h"
#include "VSM_Map.h"
#include "VSM_Map_Settings.h"

using namespace SKT;

bool MapViewMediator::handleMap( int event, const QVariant& data )
{
    switch ( event ) {
    case Activated:
        {
            if (mSkyboxEnabled) {
                auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));
                VSMCameraConfig config;
                auto success = VSMMap_Setting_GetCameraConfig(handle, config);
                success = VSMMap_SetTiltAngle(handle, config.max3dAngles[10], true);
                mSkyboxEnabled = false;
            }
        }
        break;
    case RendererTouched:
        {
            //T_DEBUGV( "RendererTouched" );
            // Map <=> Navi
            // Map <=> Simul

            // 17.9.18 hskim prevent to show map control view when navi menu view or setting main view is shown
            if ( mNaviMenuViewActivated || mSettingMainViewActivated ) {
                T_DEBUG( "#1" );
                return true;
            }

            // SW4에서 모의 주행 시 터치를 막기로 함. 17.05.31
            // TODO. 모의 주행 종료 후 상단에 주소가 남는 경우가 있어 종료 시에는
            // 표시되지 않도록 수정하여야 함.
//            if (mMapViewMode == MapViewMode::Simul)
//                return true;

            // 17.7.24 hskim 내비메뉴와 같이 조회 화면이나 맵 터치를 허용하는 경우, MapControlView를 띄우지 않기 위해 추가
            if (mMapViewMode == MapViewMode::MapInquiry || mMapViewMode == MapViewMode::MenuNavi) {
                return true;
            }

            if ( mMapViewMode == MapViewMode::Normal &&
                 mMapViewModeReason == MapViewMode::RecentTileGridViewLostFocus ) {
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,mMapViewMode},{QStringLiteral("reason"),MapViewMode::None}});
            }

            if ( mMapViewMode != MapViewMode::MapControl && mMapViewMode != MapViewMode::NormalMapControl ) {
                QVariantMap body;
                body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapControlView);
                body[QStringLiteral("lastMapViewMode")] = mMapViewMode;
                sendNotification(Constants::CreateView, body);
            }
            else {
                sendNotification(MapConstants::ResetMapViewTimer);
            }
        }
        break;
    case ViewLevelChanged:
        {
            QVariantMap m {
                {"propertyValue", data},
                {"type", NavigationPropertyProtocol::TmapViewLevel }
            };
            sendNotification( NavigationPropertyConstants::setNavigationProperty, m );

            sendNotification( MapConstants::GlobalMapViewViewLevelChanged, data );
        }
        break;
    case AreaNameChanged:
        {
            mRendererAreaName = QQmlProperty::read(mRenderer, QStringLiteral("areaName")).toString();
        }
        break;
    case ObjectHitting:
        {
            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);

            // Poi 말풍선 동작을 제한할 뷰 리스트
            const QList<int> exceptionViewList = {
                // 환경 설정 > 지도 설정 > 주행 중 지도 글씨 크기(UID v1.25 186P 오류 사항)
                ViewRegistry::SettingMapCharSizeView,
                ViewRegistry::SearchResultView,
                ViewRegistry::SearchResultDetailView,
                ViewRegistry::RouteCandidateView,
                ViewRegistry::RouteSummaryView,
                ViewRegistry::RouteSettingView,
            };
            bool hasExceptionView = false;
            for (auto view : exceptionViewList) {
                if (mViewList.contains(view)) {
                    auto viewVisible = mViewVisibleMap[view];
                    if (viewVisible) {
                        hasExceptionView = true;
                    }
                    break;
                }
            }

            auto m = data.toMap()["hitTestResult"].toMap();
            auto isCallout = m["callout"].toBool();
            if (!isCallout) {
                sendNotification(RouteConstants::HitSearchMarker, m);
            }

            if (!hasExceptionView) {
                if (isCallout) {
                    auto type = m["type"].toInt();
                    if (type == VSM_HITCALLBACK_TYPE_TRAFFIC) {
                        QVariantMap m2;
                        m2["title"] = m["name"];
                        m2["message"] = m["contents"];
                        m2[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                        m2[CommonConstants::ParentObjectName] = CommonConstants::ContentRootForCommonDialog;
                        PlatformHelper::instance()->setParentForSystemPopup(m2);
                        sendNotification(Constants::CreateView, m2);
                    } else {
                        auto extra = m["extras"].toMap();
                        auto name2 = extra["name2"].toString();
                        m["name2"] = name2;
                        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::PoiTabView);

                        m[CommonConstants::ParentObjectName] = CommonConstants::ContentRootForCommonDialog;
                        PlatformHelper::instance()->setParentForSystemPopup(m);
                        sendNotification(Constants::CreateView, m);
                    }

                    // callout에 의해 popup이 띄우지면 맵 조회 모드 끝낸다.
                    m.clear();
                    m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::MapControlView);
                    sendNotification(Constants::CloseView, m);
                } 
            }
        }
        break;
    case Hitting:
        {
            sendNotification(RouteConstants::Hitting, data);
        }
        break;
    case ViewModeChanged:
        {
            auto viewMode = data.toInt();
            QVariantMap m {
                {"propertyValue", viewMode},
                {"type", NavigationPropertyProtocol::TmapViewMode }
            };
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
            sendNotification(MapConstants::GlobalMapViewViewModeChanged, viewMode);
        }
        break;

    case MoveModeChanged:
        {
            auto moveMode = data.toInt();
            QVariantMap m {
                {"propertyValue", moveMode},
                {"type", NavigationPropertyProtocol::TmapMapMoveMode}
            };
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
            sendNotification(MapConstants::GlobalMapViewMoveModeChanged, moveMode);
        }
        break;
    case RendererSizeChanged:
        {
        }
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal, data);
        sendNotification(ZoneConstants::BlockNaviMenu, data);
        break;
    }

    return true;
}

void MapViewMediator::updateMapScale()
{
}

void MapViewMediator::onViewLevelChanged(qreal viewLevel)
{
    //qDebug() << "onViewLevelChanged(" << viewLevel << ")";
}

void MapViewMediator::handleMap( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case MapConstants::UpdateMapView:
        {
            auto v = note->getBody();
            auto routeId = v.toInt();
            if (v.isNull()) {
                T_DEBUG("body must have value.");
            }
            QMetaObject::invokeMethod(mViewComponent, "updateMap", Q_ARG(QVariant, routeId));
        }
        break;

    case MapConstants::UpdateMapViewPadding:
        {
            QString srcName = DebugUtils::name( note->getSource() );
            auto data = note->getBody().toMap();
            auto l = data["l"].toInt();
            auto r = data["r"].toInt();
            auto t = data["t"].toInt();
            auto b = data["b"].toInt();
            qDebug() << "[GlobalMap]UpdatePadding source: " << srcName << " l: " << l << " r: " << r << " t: " << t << " b: " << b;
            QMetaObject::invokeMethod(mViewComponent, "updatePadding", Q_ARG(QVariant, note->getBody()));
        }
        break;
    case MapConstants::PauseMapView:
        QMetaObject::invokeMethod(mViewComponent, "pause");
        break;

    case MapConstants::ResumeMapView:
        QMetaObject::invokeMethod(mViewComponent, "resume");
        break;

    case MapConstants::ShowTrafficInfoOnMap:
        QQmlProperty::write(mViewComponent, "showTrafficInfoOnMap", note->getBody());
        break;

    case MapConstants::ShowTrafficInfoOnRouteLine:
        QQmlProperty::write(mViewComponent, "showTrafficInfoOnRouteLine", note->getBody());
        break;

    case MapConstants::ShowTrafficInfoOnRouteLineForce:
        QQmlProperty::write(mViewComponent, "showTrafficInfoOnRouteLineForce", note->getBody());
        break;

    case MapConstants::EnableCallout:
        QQmlProperty::write(mViewComponent, "enableCallout", note->getBody());
        break;

    case MapConstants::ChangeViewMode:
        if (mMapViewMode == MapViewMode::Navi || mMapViewMode == MapViewMode::Normal) {
            QString srcName = DebugUtils::name( note->getSource() );
            qDebug() << "[GlobalMap]ChangeViewMode source: " << srcName;
            QMetaObject::invokeMethod(mViewComponent, "setMapMode", Q_ARG(QVariant, note->getBody().toInt()));
        }
        break;

    case MapConstants::ChangeViewLevel:
        {            
            // 17.12.8 hskim from navigation controller proxy
            auto m = note->getBody().toMap();
            m["mapView"] = QVariant::fromValue(mRenderer);            
            sendNotification(MapConstants::ChangeViewLevelUsingApi, m);
        }
        break;

    case MapConstants::ShowRoute:
        {
            auto m = note->getBody().toMap();
            auto show = m["show"].toBool();
            QQmlProperty::write(mViewComponent, "enableShowRoute", show);
            qDebug() << "[MAP] MapConstants::ShowRoute(" << show << ") : " << note->getSource().data();
        }
        break;

    case MapConstants::DrawMarkerAll:
        QMetaObject::invokeMethod(mViewComponent, "drawMarkAll", Q_ARG(QVariant, note->getBody()));
        break;

    case MapConstants::ShowVolume:
    case MapConstants::HideVolume:
        {
            auto volumeVisible = note->getId() == MapConstants::ShowVolume;
            QQmlProperty::write(mViewComponent, "volumeVisible", volumeVisible);
        }
        break;
    }
}
