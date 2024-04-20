#include "MapInClusterHudViewMediator.h"
#include "Constants.h"
#include "MapConstants.h"
#include "QmlAppHelper.h"
#include "NavigationControllerConstants.h"
#include <QQuickItem>
#include <QQuickWindow>
#include "VSM_Map.h"
#include "VsmMapSettingAndTheme.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "VsmMapContants.h"

using namespace SKT;

MapInClusterHudViewMediator::MapInClusterHudViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

MapInClusterHudViewMediator::~MapInClusterHudViewMediator()
{
}

QList<int> MapInClusterHudViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        NavigationPropertyConstants::setNavigationProperty
    };

    return notes;
}

bool MapInClusterHudViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUG( "#1" );

    switch( event ) {
    case Mediated:
        {
            auto m = data.toMap();
            mRenderer = m["renderer"].value<QObject*>();
            auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));

            VSMMap_SetSkyType(handle, VSM_SKY_CLUSTER);
            VSMMap_SetTheme(handle, VSM_THEME_CLUSTER);

            m.clear();
            m["mapView"] = QVariant::fromValue( mRenderer );
            m["type"] = VSM_VIEW_TYPE_HUD;
            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

            m["type"] = QVariant::fromValue( (int)VsmMapType::ClusterHud );
            sendNotification(MapConstants::ApplyMapSetting, m);

//            QJsonObject root;
//            QJsonObject carvatar;
//            carvatar["width"] = 140.0f;
//            root["carvatar"] = carvatar;
//            auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);
//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());

//            VSMMap_SetShowRouteFlag(handle, VSM_ShowRouteFlag_RouteLine | VSM_ShowRouteFlag_RouteLineTBT);
        }
        break;
    case RendererOnDestruction:
        {
            sendNotification(MapConstants::DetachMapViewFromVsmNavi,data);
        }
        break;
    default:
        return true;
    }
        
    return false;
}

bool MapInClusterHudViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case NavigationPropertyConstants::setNavigationProperty:
        {
            auto map = note->getBody().toMap();
            auto type = map["type"].toInt();
            if (  type == NavigationPropertyProtocol::TmapMapMoveMode ) {
                auto moveMode = map["propertyValue"].toInt();
                QQmlProperty::write( mViewComponent, "mapMoveMode", moveMode );
            }
        }
        break;
    }

    return false;
}

//            ret.routeLine.width = VsmMapConstants::Theme::Day::RouteLine::Width;
//            ret.routeLine.fillColor = VsmMapConstants::Theme::Day::RouteLine::FillColor;
//            ret.routeLine.strokeColor = VsmMapConstants::Theme::Day::RouteLine::StrokeColor;
//            ret.routeLine.passedFillColor = VsmMapConstants::Theme::Day::RouteLine::PassedFillColor;
//            ret.routeLine.passedStrokeColor = VsmMapConstants::Theme::Day::RouteLine::PassedStrokeColor;

//            struct Internal {
//                static QString decRGBA2Hex(qint32 rgba) {
//                    return QString("#%1").arg((uint)rgba, 8, 16, QLatin1Char('0'));
//                }
//            };

//            QJsonObject root;
//            QJsonObject routeLine;
//            routeLine["width"] = VsmMapConstants::Theme::Day::RouteLine::Width;
//            routeLine["fillColor"] = Internal::decRGBA2Hex(0xff0000ff);
//            routeLine["strokeColor"] = Internal::decRGBA2Hex(theme.routeLine.strokeColor);
//            routeLine["passedFillColor"] = Internal::decRGBA2Hex(theme.routeLine.passedFillColor);
//            routeLine["passedStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.passedStrokeColor);

//            QJsonObject turnPoint;
//            turnPoint["width"] = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::Width;
//            turnPoint["floatGap"] = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FloatGap;
//            turnPoint["fillColor"] = Internal::decRGBA2Hex(VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FillColor);
//            turnPoint["strokeColor"] = Internal::decRGBA2Hex(VsmMapConstants::Theme::Day::RouteLine::TurnPoint::StrokeColor);
//            routeLine["turnPoint"] = turnPoint;

//            root["routeLine"] = routeLine;

//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, QJsonDocument(root).toJson(QJsonDocument::Compact).data());
