#include "HiPassLaneViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SettingConstants.h"
#include "TestConstants.h"
#include "ResourceResolver.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "RgProtocol.h"
#include "SimulatorConstants.h"
#include "RgUtils.h"

using namespace SKT;

HiPassLaneViewMediator::HiPassLaneViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurrMapViewMode( MapViewMode::Normal )
{
}

HiPassLaneViewMediator::~HiPassLaneViewMediator()
{

}

QList<int> HiPassLaneViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        //RgConstants::UpdateHighwayRouteGuidance,
        MapConstants::ApplyMapViewMode,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RgConstants::StartRouteGuidance
    };

    return notes;
}

bool HiPassLaneViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = QStringLiteral("NightMode");
            sendNotification( SettingConstants::RequestSetting, body );

            sendNotification(MapConstants::NotifyMapViewMode);

            sendNotification(RgConstants::RequestUiState);
        }
        break;
    case VisibleChanged:
        {
            auto m = data.toMap();
            auto visible = m[QStringLiteral("visible")].toBool();

            qDebug() << "[RG][ViewSwitch][LaneView]visible: " << visible;

            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::LaneView)},
                    {QStringLiteral("item"), RgConstants::HiPassLaneVisible},
                    {CommonConstants::Value, visible}
                }
            );
        }
        break;
    default:
        return false;
    }

    return true;
}

bool HiPassLaneViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateRouteGuidance:
        {
            auto rgDataVo = note->getBody().value<RgDataVo>();
            applyRgData( rgDataVo );
        }
        break;
//    case RgConstants::UpdateHighwayRouteGuidance:
//        {
//            auto rgDataVo = note->getBody().value<RgDataVo>();
//            applyRgData( rgDataVo );
//        }
//        break;
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == QStringLiteral("NightMode") ) {
            }
        }
        break;
    case SettingConstants::ApplySetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == QStringLiteral("NightMode") ) {
            }
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurrMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
        }
        break;
    case RgConstants::StartRouteGuidance:
        {
            T_DEBUG( QString( "StartRouteGuidance #1" ) );
            QMetaObject::invokeMethod(mViewComponent, "reset");
        }
        break;
    default:
        return false;
    }

    return true;
}

//#define TMAP_RSM_UI_ENALBE_TEST_HIPASSLANE

//        a. 요금 정보
//        - 차기 TBT가 톨게이트이고, 남은 거리가 2km일 때부터 표시

//        b. 하이패스 차선정보
//        - 요금정보 표시될 때 함께 표시 됨

static const int HIPASSLANE_VISIBLE_DISTANCE = 2000;

void HiPassLaneViewMediator::applyRgData( const RgDataVo& rgDataVo )
{    
    if ( rgDataVo.isNull() ) {
        return;
    }

    bool curVisible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
    bool newVisible = true;

    if ( !MapViewMode::isInRouteGuidance( mCurrMapViewMode ) ) {
        newVisible = false;
    }

    int tbtType = rgDataVo.stGuidePoint_nTBTTurnType();
    int tollFee = rgDataVo.stGuidePoint_nTollFee();
    int dist = rgDataVo.stGuidePoint_nTBTDist();
    bool isTollgate = tbtType == RgTbtCode::TOL_EXP
            || tbtType == RgTbtCode::TOL_NORMAL
            || tbtType == RgTbtCode::TOLLGATE;
    bool isInVisibleDistance = dist <= HIPASSLANE_VISIBLE_DISTANCE;

    bool tollFeeVisible = false;

    if ( newVisible ) {
        // 차기 TBT가 톨게이트이고
        if ( isTollgate ) {
            QString tollFeeStr;
            // 요금이 0보다 크고
            if ( tollFee > 0 ) {
                tollFeeStr = QLocale().toString( tollFee );
                // 남은 거리가 2km일 때부터 요금정보 표시
                if ( isInVisibleDistance ) {
                    QQmlProperty::write( mViewComponent, QStringLiteral("tollFee"), tollFeeStr );
                    tollFeeVisible = true;
                }
            }
        }
    }

    bool laneVisible = true;

    if ( rgDataVo.nHiPassLaneCount() <= 0 || !isInVisibleDistance ) {
        laneVisible = false;
    }

#ifdef TMAP_RSM_UI_ENALBE_TEST_HIPASSLANE
    newVisible = true;
#endif

    //qDebug() << QString("[HiPassLane]isTollgate: %1 fee: %2 dist: %3 lanes: %4 laneVis: %5").arg(isTollgate).arg(tollFee).arg(dist).arg(rgDataVo.nHiPassLaneCount()).arg(laneVisible);

    if ( laneVisible ) {
        QStringList numList;
        QStringList bgList;
        int lastLaneNum = 0;
        QString laneImg;

#ifdef TMAP_RSM_UI_ENALBE_TEST_HIPASSLANE
        int tempLaneInfo[] = {1,2,3,4,11,12,13,14};
        //int tempLaneInfo[] = {1,2,3,4,11,12,13,14,15};
        //int tempLaneInfo[] = {1,2,3,4,11,12,13,14,15,16};
        for ( int i = 0, count = sizeof(tempLaneInfo)/sizeof(int); i < count; i++ ) {
            auto laneNum = tempLaneInfo[i];
#else
        for ( int i = 0, count = rgDataVo.nHiPassLaneCount(); i < count; i++ ) {
            auto laneNum = rgDataVo.nHiPassArray( i );
#endif

            //qDebug() << QString( "lane[%1] %2", i, laneNum );
            if ( i != 0 && ( laneNum - lastLaneNum ) > 1 ) {
                bgList.append( ResourceResolver::instance()->imageUrl( QStringLiteral("hipass"), QString( QStringLiteral( "hipass_lane_more.png" ) ) ) );
                numList.append( QString() );
            }

            if ( i == 0 ) {
                laneImg = QStringLiteral( "hipass_lane_l_bg.png" );
            }
            else if ( i == count - 1 ) {
                laneImg = QStringLiteral( "hipass_lane_r_bg.png" );
            }
            else {
                laneImg = QStringLiteral( "hipass_lane_m_bg.png" );
            }

            bgList.append( ResourceResolver::instance()->imageUrl( QStringLiteral("hipass"), laneImg ) );
            numList.append( ResourceResolver::instance()->imageUrl( QStringLiteral("hipass"), QString("hipass_lane_num_%1.png").arg( laneNum ) ) );
            lastLaneNum = laneNum;
        }

        QQmlProperty::write( mViewComponent, QStringLiteral("laneNumbers"), numList );
        QQmlProperty::write( mViewComponent, QStringLiteral("laneBgs"), bgList );

        mOldRgDataVo = rgDataVo;
    }    

    newVisible = tollFeeVisible || laneVisible;

    QQmlProperty::write( mViewComponent, QStringLiteral("tollFeeVisible"), tollFeeVisible );
    QQmlProperty::write( mViewComponent, QStringLiteral("laneVisible"), laneVisible );
    QQmlProperty::write( mViewComponent, CommonConstants::Visible, newVisible );
}

