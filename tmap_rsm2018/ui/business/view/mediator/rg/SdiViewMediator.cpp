#include "SdiViewMediator.h"
#include "Constants.h"
#include "mediator/app/MapViewMode.h"
#include "RgProtocol.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "TestConstants.h"
#include "StringHelper.h"
#include "ResourceResolver.h"
#include "ViewRegistry.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "navigationdata.h"
#include "CommonConstants.h"
#include "RgUtils.h"
#include "SimulatorConstants.h"

#ifdef QT_DEBUG
//#define TMAP_RSM_UI_TEST_SDI
//#define TMAP_RSM_UI_TEST_SDI_NORMAL
//#define TMAP_RSM_UI_TEST_SDI_ALERT
//#define TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_FIXED
//#define TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_CHANGEABLE
//#define TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_CHANGEABLE_BUT_SPEEDLIMIT_NA
#endif

using namespace SKT;

SdiViewMediator::SdiViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    // 제한속도구간 영역
    mSectionSpeedWrap = viewComponent->findChild<QObject*>(QStringLiteral("sectionSpeedWrap"));

    // 제한속도표식 영역
    mSignWrap = viewComponent->findChild<QObject*>(QStringLiteral("signWrap"));

    // 표식
    mSignImage = viewComponent->findChild<QObject*>(QStringLiteral("signImage"));

    // 남은거리
    mRemainDistWrap = viewComponent->findChild<QObject*>(QStringLiteral("remainDistWrap"));
    mRemainDistText = viewComponent->findChild<QObject*>(QStringLiteral("remainDistText"));
}

SdiViewMediator::~SdiViewMediator()
{
    qDebug() << "[FullscreenAlertView]HideView";
    sendNotification(Constants::HideView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::FullscreenAlertView)}});
}

QList<int> SdiViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
        TestConstants::RunTest,
        RgConstants::RequestSdiStateUpdate,
        RgConstants::StartRouteGuidance,
        TestConstants::LogSdiData,
        RgConstants::StopRouteGuidance
    };

    return notes;
}

bool SdiViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            sendNotification(MapConstants::NotifyMapViewMode);
        }
        break;
    case VisibleChanged:
        {
        }
        break;
    default:
        return false;
    }

    return true;
}

bool SdiViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            applySdiData( vo );
        }
        break;
    case RgConstants::RequestSdiStateUpdate:
        {
            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSignType},
                {QStringLiteral("value"),mLastState.signType}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSignImage},
                {QStringLiteral("value"),mLastState.signImage}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiAlert},
                {QStringLiteral("value"),mAlertState}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedVisible},
                {QStringLiteral("value"),mLastState.sectionSpeedVisible}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSignVisible},
                {QStringLiteral("value"),mLastState.signVisible}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiDistanceVisible},
                {QStringLiteral("value"),mLastState.remainDistVisible}
            });

            auto signType = QQmlProperty::read( mViewComponent, QStringLiteral("signType") ).toInt();
            auto updateDetail = signType == RgConstants::SdiCircledNumber ? RgConstants::SdiSignNumber : RgConstants::SdiSignSmallNumber;

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),updateDetail},
                {QStringLiteral("value"),mLastState.speedLimit}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedAverageNumber},
                {QStringLiteral("numberType"),mLastState.sectionSpeedAverageNumberType},
                {QStringLiteral("value"),mLastState.sectionSpeedAverage}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiDistance},
                {QStringLiteral("value"),mLastState.remainDist}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedTime},
                {QStringLiteral("numberType"),mLastState.sectionSpeedAverageNumberType},
                {QStringLiteral("value"),mLastState.sectionSpeedTime}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedToggleAverageAndTime},
                {QStringLiteral("numberType"),mLastState.sectionSpeedAverageNumberType},
                {QStringLiteral("value"),mLastState.sectionSpeedToggleAverageAndTime}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedAverageVisible},
                {QStringLiteral("numberType"),mLastState.sectionSpeedAverageNumberType},
                {QStringLiteral("value"),mLastState.sectionSpeedAverageVisible}
            });

            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),RgConstants::SdiSectionSpeedTimeVisible},
                {QStringLiteral("numberType"),mLastState.sectionSpeedAverageNumberType},
                {QStringLiteral("value"),mLastState.sectionSpeedTimeVisible}
            });

            // TODO 복수이면 위젯이 요청하면 mUpdate를 여러벌로( widget id )
            mUpdate = NotifiableUpdate();
        }
        break;

    case RgConstants::StartRouteGuidance:
    case RgConstants::StopRouteGuidance:
        {
            reset();
        }
        break;
    case TestConstants::RunTest:
        {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();
            if ( data.contains("SdiLog") ) {
                mSdiLog = true;
            }
        }
        break;
    case TestConstants::LogSdiData:
        {
            auto data = note->getBody().value<QVariantMap>();
            if ( data.contains("enable") ) {
                mSdiLog = data["enable"].toBool();
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

void SdiViewMediator::applySdiData(RgDataVo& rgDataVo)
{
    bool bSdi = false;

    if ( rgDataVo.isNull() ) {
        return;
    }

#ifdef TMAP_RSM_UI_TEST_SDI
    #include "SdiViewMediatorTest.cpp"
    SdiViewMediatorTest();
#endif

    if ( rgDataVo.bSdi() && rgDataVo.nSdiType() == RgSdiCode::SPEED_ACCIDENT_POS && rgDataVo.nSdiSpeedLimit() == 0 ) {
        qDebug( "[SDI]Detected an invalid sdi information. bSdi: %d, nSdiType: %d nSdiSpeedLimit: %d", rgDataVo.bSdi(), rgDataVo.nSdiType(), rgDataVo.nSdiSpeedLimit() );
    }
	
    bool oldSpeedLimitSignVisible = QQmlProperty::read( mSignWrap, CommonConstants::Visible ).toBool();

    QString signImage;
    int speedLimit = rgDataVo.nSdiSpeedLimit();
    bool updateSign = false;
    int vpSdiPointX = 0;
    int vpSdiPointY = 0;

    //
    bool oldAlertState = mAlertState;        
    bool isSectionSpeed = false;
    bool sectionSpeedVisible = mLastState.sectionSpeedVisible;
    bool signVisible = mLastState.signVisible;
    bool remainDistVisible = mLastState.remainDistVisible;

    auto index = RgSdiCode::staticMetaObject.indexOfEnumerator( "RgSdiCodeK" );
    auto typeName = RgSdiCode::staticMetaObject.enumerator(index).valueToKey( rgDataVo.nSdiType() );

    if ( mSdiLog ) {
        qInfo( "[SDI]bSdi: %d nDist: %d bChg: %d Type: %d Limit: %d xPos: %d yPos: %d plusXPos: %d plusYPos: %d blkDist: %d blkSpd: %d blkAvgSpd: %d blkTime: %d %s",
            rgDataVo.bSdi(), rgDataVo.nSdiDist(),
            rgDataVo.bIsChangeableSpeedType(),
            rgDataVo.nSdiType(), rgDataVo.nSdiSpeedLimit(),
            rgDataVo.vpSdiPointX(), rgDataVo.vpSdiPointY(),
            rgDataVo.vpSdiPlusPointX(), rgDataVo.vpSdiPlusPointY(),
            rgDataVo.nSdiBlockDist(), rgDataVo.nSdiBlockSpeed(), rgDataVo.nSdiBlockAverageSpeed(),
            rgDataVo.nSdiBlockTime(),
            typeName
            );
    }
	
    if (rgDataVo.bSdi()) {
        bSdi = true;
    } else if (rgDataVo.bSdiPlus()) {
        //qDebug( "[SDI]bSdiPlus == true" );
        rgDataVo.set_nSdiType( rgDataVo.nSdiPlusType() );
        rgDataVo.set_nSdiSpeedLimit( rgDataVo.nSdiPlusSpeedLimit() );
        rgDataVo.set_nSdiDist( rgDataVo.nSdiPlusDist() );
        rgDataVo.set_bSdi( rgDataVo.bSDIPlus() );
        rgDataVo.set_vpSdiPointX( rgDataVo.vpSdiPlusPointX() );
        rgDataVo.set_vpSdiPointY( rgDataVo.vpSdiPlusPointY() );
        rgDataVo.set_nSdiBlockDist( rgDataVo.nSdiPlusBlockDist() );
        rgDataVo.set_nSdiBlockTime( rgDataVo.nSdiPlusBlockTime() );
        rgDataVo.set_nSdiBlockSpeed( rgDataVo.nSdiPlusBlockSpeed() );
        bSdi = true;
    } else {
        mLastSdiType = RgSdiCode::UNKNOWN;
        mLastSdiPointX = 0;
        mLastSdiPointY = 0;
    }

#ifdef TMAP_RSM_UI_TEST_SDI_NORMAL
    SdiViewMediatorTest_Normal();
#endif
#ifdef TMAP_RSM_UI_TEST_SDI_ALERT
    SdiViewMediatorTest_Alert();
#endif
#ifdef TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_FIXED
    SdiViewMediatorTest_FixedSectionSpeed();
#endif
#ifdef TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_CHANGEABLE
    SdiViewMediatorTest_ChangeableSectionSpeed();
#endif
#ifdef TMAP_RSM_UI_TEST_SDI_SECTION_SPEED_CHANGEABLE_BUT_SPEEDLIMIT_NA
    SdiViewMediatorTest_ChangeableSectionSpeedButSpeedLimitNa();
#endif

    bool bBus = false;

    if ( !bSdi ) {
        reset();
        isSectionSpeed = false;
        sectionSpeedVisible = false;
        signVisible = false;
        remainDistVisible = false;
    }
    else {
        if(rgDataVo.vpSdiPointX() > 0 && rgDataVo.vpSdiPointY() > 0) {
            vpSdiPointX = rgDataVo.vpSdiPointX();
            vpSdiPointY = rgDataVo.vpSdiPointY();
        }
        else if(rgDataVo.nSdiBlockEndpointX() > 0 && rgDataVo.nSdiBlockEndpointY() > 0) {
            vpSdiPointX = rgDataVo.nSdiBlockEndpointX();
            vpSdiPointY = rgDataVo.nSdiBlockEndpointY();
        }

        auto nSdiType = rgDataVo.nSdiType();
        if(mLastSdiType != nSdiType || mLastSdiPointX != vpSdiPointX || mLastSdiPointY != vpSdiPointY) {
            updateSign = true;
        }

        if ( nSdiType == RgSdiCode::SPEED_BLOCK_START_POS ||
            nSdiType == RgSdiCode::SPEED_BLOCK_MID_POS ||
            nSdiType == RgSdiCode::SPEED_BLOCK_END_POS ) {
            if ( rgDataVo.nSdiBlockAverageSpeed() != 0 ) {
                updateSign = true;
            }
        }

        if(updateSign) {
            // 17.10.16 hskim so as not to display limit block when it's not in limit block
            isSectionSpeed = false;

            RgConstants::SdiSignTypeK signType = RgConstants::SdiSymbol;

            switch (nSdiType) {
            case RgSdiCode::BUS_LANE: // 버스전용무카-고정식
                // 17.9.19 hskim 버스라인 주행 가능한 경우 SDI를 띄우지 않는다
                if ( rgDataVo.nSdiDrivingBusLine() != 0 ) {
                    qDebug() << "[SDI]rgDataVo.nSdiDrivingBusLine() != 0";
                    signVisible = false;
                    bBus = true;
                }
                else {
                    signImage = QStringLiteral("c_06.png");
                }
                break;
            case RgSdiCode::PARKING_CONTROL_POS: // 주차단속무카-고정식
                signImage = QStringLiteral("c_07.png");
                break;
            case RgSdiCode::CHANGEROAD_POS: // 17. 가변 차로 단속
                signImage = QStringLiteral("c_14.png");
                break;
            case RgSdiCode::ROAD_CONTROL_POS: // 갓길감시무카-고정식
                signImage = QStringLiteral("c_08.png");
                break;
            case RgSdiCode::TRAFFICINFO_COLLECT_POS: // 교통수집카메라-고정식
                signImage = QStringLiteral("c_04.png");
                break;
            case RgSdiCode::ACCIDEDNT_AREA: // 사고다발지역
                signImage = QStringLiteral("s_02.png");
                break;
            case RgSdiCode::SHARP_CURVE_AREA: // 급커브지역
                signImage = QStringLiteral("s_01.png");
                break;
            case RgSdiCode::NEW_CURVE_AREA: // 178. 급커브구간1 (신규)
                signImage = QStringLiteral("s_01.png");
                break;
            case RgSdiCode::FOG_AREA: // 안개지역
                signImage = QStringLiteral("s_03.png");
                break;
            case RgSdiCode::SERVICE_AREA: // 고속도로 휴게소
                signImage = QStringLiteral("p_01.png");
                break;
            case RgSdiCode::TUNNEL_AREA: // 터널
                signImage = QStringLiteral("p_02.png");
                break;
            case RgSdiCode::LPG_STATION: // 충전소
                signImage = QStringLiteral("p_03.png");
                break;
            case RgSdiCode::TOLLGATE: // 톨게이트
                signImage = QStringLiteral("p_04.png");
                break;
            case RgSdiCode::HAZARDOUS_AREA: // 유해물질지역
                //[TMCRSM-33 leejaeho] uid에 존재하지 않는 이미지 노출 제거함.
//                signImage = QStringLiteral("s_04.png");
                break;
            case RgSdiCode::SPEEDBUMP: // 18: 과속방지턱
                signImage = QStringLiteral("s_05.png");
                break;
            case RgSdiCode::SCHOOL_ZONE_START: // 19: 어린이보호구역
                signImage = QStringLiteral("s_06.png");
                break;
            case RgSdiCode::RAILWAY_CROSSING: // 20: 철길건널목
                signImage = QStringLiteral("s_07.png");
                break;
            case RgSdiCode::INTRUDER_AREA: // 21: 끼어들기금지
                signImage = QStringLiteral("c_12.png");
                break;
            case RgSdiCode::LOAD_BAD_CONTROL_POS: // 24: 적재불량단속
                signImage = QStringLiteral("c_13.png");
                break;
            case RgSdiCode::CCTV_AREA: // 182. 방범용CCTV(신규)
            case RgSdiCode::BOX_SPEED_LIMIT_POS: // 181. 박스형이동식(신규)
                signImage = QStringLiteral("c_16.png");
                break;
            case RgSdiCode::VISUAL_RIGHT_AREA: // 171. 우측시야불량지점(신규)
            case RgSdiCode::VISUAL_FRONT_AREA: // 170. 시야불량지점(신규)
            case RgSdiCode::VISUAL_LEFT_AREA: // 172. 좌측시야불량지점(신규)
                signImage = QStringLiteral("s_08.png");
                break;
            case RgSdiCode::TRAFFIC_CONGEST_AREA: //175. 교통혼잡지역(신규)
            case RgSdiCode::DIRECTION_LANE: //176. 방향별차로선택지점(신규)
            case RgSdiCode::SPEED_DRIVING_AREA: //174. 과속운행다발구간(신규)
            case RgSdiCode::ONE_WAY_AREA: //179.  일방통행도로 (신규)
            case RgSdiCode::ROAD_ACCIDEDNT_AREA: //191. 갓길 사고 다발 지점 (신규)
            case RgSdiCode::SPEED_ACCIDEDNT_AREA: //192. 과속 사발 다발 지점 (신규)
            case RgSdiCode::SLEEP_ACCIDEDNT_AREA: //193. 졸음 사고 다발 지점 (신규)
            case RgSdiCode::HWA03: //제동장치사고다발지점
            case RgSdiCode::HWA06: //중앙선침범사고다발지점
            case RgSdiCode::HWA09: //통행위반사고다발지점
                signImage = QStringLiteral("s_12.png");
                break;
            case RgSdiCode::SIGNAL_VIOLATION_AREA: // 173. 신호위반다발구간(신규)
                signImage = QStringLiteral("s_11.png");
                break;
            case RgSdiCode::WALK_CROSS_AREA: // 177. 무단횡단사고다발지점(신규)
                signImage = QStringLiteral("s_13.png");
                break;
            case RgSdiCode::SLOPE_AREA: // 180. 급경사구간 (신규)
                signImage = QStringLiteral("s_15.png");
                break;
            case RgSdiCode::PEDESTRIAN_ACCIDENT: //  보행자 사고다발지역
                signImage = QStringLiteral("s_20.png");
                break;
            case RgSdiCode::REST_PLACE:
                signImage = QStringLiteral("p_05.png");
                break;
            case RgSdiCode::OVERLOAD_DANGEROUS_AREA: // 과적무카-고정식
                signImage = QStringLiteral("c_09.png");
                break;
            case RgSdiCode::SIGNAL_ACCIDENT_POS: // 신호무카-고정식
                signImage = QStringLiteral("c_03.png");
                break;
            case RgSdiCode::SPEED_ACCIDENT_POS: // 신호과속무카-고정식
                signImage = QStringLiteral("c_02.png");
                signType = RgConstants::SdiNumberWithSymbol;
                break;
            case RgSdiCode::SPEED_LIMIT_POS: // 과속무카-고정식
                signImage = QStringLiteral("tmap_speed_symbol.png");
                signType = RgConstants::SdiCircledNumber;
                break;
            case RgSdiCode::SPEED_LIMIT_DANGEROUS_AREA: // 과속무카-이동식
                signImage = QStringLiteral("c_05.png");
                signType = RgConstants::SdiNumberWithSymbol;
                break;
            case RgSdiCode::SPEED_BLOCK_START_POS: // 구간단속시점
                isSectionSpeed = true;
                break;
            case RgSdiCode::SPEED_BLOCK_MID_POS: // 구간단속중                
                isSectionSpeed = true;
                break;
            case RgSdiCode::SPEED_BLOCK_END_POS: // 구간단속종점
                //[TMCRSM-213 leejaeho] 이미지와 제한속도표시가 겹치는 문제 수정
                isSectionSpeed = true;
                break;
            case RgSdiCode::UNKNOWN:
            default:
                signVisible = false;
                break;
            }

            // 속도 제한 구간
            if ( isSectionSpeed ) {
                auto speedLimit = rgDataVo.nSdiSpeedLimit();
                sectionSpeedVisible = setSectionSpeed(signType, signImage, rgDataVo.bIsChangeableSpeedType(), rgDataVo.nSdiBlockSpeed(), rgDataVo.nSdiBlockTime(), rgDataVo.nSdiBlockAverageSpeed(), speedLimit);
            }
            else {
                // 제한 속도
                setSignNumber( signType, speedLimit );
            }
			
            // 표식 유형
            //qDebug() << QString( "[SDI]signType: %1" ).arg( signType );
            QQmlProperty::write( mViewComponent, QStringLiteral("signType"), signType );
			
            // 표식 이미지
            if ( signImage.isEmpty() ) {
                signVisible = false;
                qDebug() << QString( "Could not find signImage for sdiType: %1" ).arg( nSdiType );
            }
            else {
                auto source = ResourceResolver::instance()->imageUrl( "sdi", signImage );
                if ( source.isEmpty() ) {
                    signVisible = false;
                    qDebug() << QString( "Could not find image resoruce for signImage: %1" ).arg( signImage );
                }
                else {
                    signVisible = true;
                    // TMCRSM-882
                    auto signSectionSpeedToggle = QQmlProperty::read( mViewComponent, QStringLiteral("signSectionSpeedToggle") ).toBool();
                    if ( !signSectionSpeedToggle ) {
                        QQmlProperty::write( mSignImage, QStringLiteral("source"), source );
                    }
                }
            }

            mLastSdiType = rgDataVo.nSdiType();
            mLastSdiPointX = vpSdiPointX;
            mLastSdiPointY = vpSdiPointY;
			
            mLastState.signType = signType;
            mLastState.signImage = signImage;
        }

        // 남은거리
        setRemainDistanceNumber(remainDistVisible, rgDataVo.nSdiDist(), rgDataVo.nSdiBlockDist(), rgDataVo.nSdiType());
    }
	
    QQmlProperty::write( mSectionSpeedWrap, CommonConstants::Visible, sectionSpeedVisible );

    // 알람 표시 관련 >>>
    if ( !bSdi ) {
        mAlertState = false;
    }
    else {
        int nCurSpeed = rgDataVo.nCurSpeed();
        int nSdiSpeedLimit = rgDataVo.nSdiSpeedLimit();

        if ( nSdiSpeedLimit > 0 ) {
            mAlertState = nCurSpeed > nSdiSpeedLimit;
        }

        if ( oldSpeedLimitSignVisible != signVisible ) {
            if ( signVisible ) {
                // hskim 17.7.20 external interface
//                QVariantMap body;
//                body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapNaviStatus );
//                body.insert( QStringLiteral("propertyValue"),QVariant::fromValue((int)TMAPNAVISTATUS_ANDO));
//                sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
            }

            if ( !signVisible ) {
                mAlertState = false;
            }
        }
    }

    if ( mAlertState != oldAlertState ) {
        if ( mAlertState ) {
            sendNotification(Constants::CreateView,QVariantMap{
                {CommonConstants::Url,ViewRegistry::url(ViewRegistry::FullscreenAlertView)},
                {"parentObjectName","alertWrap"}
            });
            qDebug() << "[FullscreenAlertView]ShowView";
            sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::FullscreenAlertView)}});
        }
        else {
            qDebug() << "[FullscreenAlertView]HideView";
            sendNotification(Constants::HideView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::FullscreenAlertView)}});
        }
    }
	
	// <<< 알람 표시 관련

    // 위젯 관련 >>>
	
    if ( mUpdate.signImage != mLastState.signImage ) {
        mUpdate.signImage = mLastState.signImage;
		sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSignImage},
            {QStringLiteral("value"),mUpdate.signImage}
		});
    }
	
    if ( mUpdate.signType != mLastState.signType ) {
        mUpdate.signType = mLastState.signType;
		sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSignType},
            {QStringLiteral("value"),mUpdate.signType}
		});
    }

    mLastState.sectionSpeedVisible = sectionSpeedVisible;
    if ( mUpdate.sectionSpeedVisible != sectionSpeedVisible ) {
        mUpdate.sectionSpeedVisible = sectionSpeedVisible;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedVisible},
            {QStringLiteral("value"),mUpdate.sectionSpeedVisible}
        });
    }

    if ( !signVisible ) {
        mLastState.signImage = "";
        remainDistVisible = false;
    }    

    QQmlProperty::write( mSignWrap, CommonConstants::Visible, signVisible );
    mLastState.signVisible = signVisible;
    if ( mUpdate.signVisible != signVisible ) {
        mUpdate.signVisible = signVisible;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSignVisible},
            {QStringLiteral("value"),mUpdate.signVisible}
        });
    }

    QQmlProperty::write( mRemainDistWrap, CommonConstants::Visible, remainDistVisible );
    mLastState.remainDistVisible = remainDistVisible;
    if ( mUpdate.remainDistVisible != remainDistVisible ) {
        mUpdate.remainDistVisible = remainDistVisible;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiDistanceVisible},
            {QStringLiteral("value"),mUpdate.remainDistVisible}
        });
    }

    if ( mAlertState != oldAlertState ) {
        // widget
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiAlert},
            {QStringLiteral("value"),mAlertState}
        });
	}
	
	// <<< 위젯 관련
}

void SdiViewMediator::setSignNumber(RgConstants::SdiSignTypeK signType, int limit)
{
    auto str = QString().sprintf("%02d",limit);
    QStringList images;
    QString img;

    for(int i = 0; i < str.length(); i++){
        auto ch = str.at(i);
        switch ( signType ) {
        case RgConstants::SdiCircledNumber:
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiSignNumberImage(ch) );
            break;
        case RgConstants::SdiNumberWithSymbol:
        case RgConstants::SdiSectionSpeed:
        case RgConstants::SdiChangeableSectionSpeed:
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiSignNumberSmallImage(ch) );
            break;
        }
        images.append( img );
    }
    QMetaObject::invokeMethod( mViewComponent, "setSignNumberImages", Q_ARG(QVariant,images) );

    if ( limit != 0 ) {
        mLastState.speedLimit = str;
        if ( mUpdate.speedLimit != str ) {
            mUpdate.speedLimit = str;
            auto updateDetail = signType == RgConstants::SdiCircledNumber ? RgConstants::SdiSignNumber : RgConstants::SdiSignSmallNumber;
            sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                {QStringLiteral("item"),updateDetail},
                {QStringLiteral("value"),mUpdate.speedLimit}
            });
        }
    }
}

bool SdiViewMediator::setSectionSpeedAverageNumber(RgConstants::SdiSectionSpeedNumberType numberType, int average)
{
    QString str = QString().sprintf("%02d",average);

    QStringList images;
    QString img;
    for(int i = 0; i < str.length(); i++){
        auto ch = str.at(i);
        img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiSectionSpeedAverageNumberImage( numberType, ch) );
        images.append( img );
    }
    img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiSpeedSectionAverageNumberKmhImage( numberType ) );
    images.append( img );
    QMetaObject::invokeMethod( mViewComponent, "setSectionSpeedAverageNumberImages", Q_ARG(QVariant,images) );

    mLastState.sectionSpeedAverage = str;
    if ( mUpdate.sectionSpeedAverage != str ) {
        mUpdate.sectionSpeedAverage = str;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedAverageNumber},
            {QStringLiteral("numberType"),(int)mLastState.sectionSpeedAverageNumberType},
            {QStringLiteral("value"),mUpdate.sectionSpeedAverage}
        });
    }

    return average != 0;
}

bool SdiViewMediator::setSectionSpeedTime(RgConstants::SdiSectionSpeedNumberType numberType, int time)
{
    QString str;

    if ( time < 0 ) {
        time = 0;
    }

    str = QString().sprintf( "%02d:%02d", time / 60, time % 60 );

    QStringList images;
    QString img;
    for(int i = 0; i < str.length(); i++){
        auto ch = str.at(i);
        img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiSectionSpeedTimeImage( numberType, ch) );
        images.append( img );
    }
    QMetaObject::invokeMethod( mViewComponent, "setSectionSpeedTimeImages", Q_ARG(QVariant,images) );

    if ( mUpdate.sectionSpeedTime != str ) {
        mUpdate.sectionSpeedTime = str;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedTime},
            {QStringLiteral("numberType"),(int)mLastState.sectionSpeedAverageNumberType},
            {QStringLiteral("value"),mUpdate.sectionSpeedTime}
        });
    }

    return time != 0;
}

bool SdiViewMediator::setSectionSpeed(RgConstants::SdiSignTypeK& signType, QString& signImage, bool isChangeableSpeedType, int blockSpeed, int blockTime, int averageSpeed, int speedLimit)
{
    bool averageVisible = false;
    bool timeVisible = false;

	// UID 1.2.2. 지도화면 - 구간 단속 / 가변 구간 단속
    RgConstants::SdiSectionSpeedNumberType averageNumberType = averageSpeed > blockSpeed ? RgConstants::SdiExcessive : RgConstants::SdiNormal;

    // TMCRSM-4659
    if ( speedLimit == 0 ) {
        isChangeableSpeedType = true;
    }

	// 구간단속인 경우
    if ( !isChangeableSpeedType ) {
        signType = RgConstants::SdiSectionSpeed;
        signImage = QStringLiteral("c_10.png");

        setSignNumber( signType, speedLimit );

        // 3. 평균속도를 표시할 수 없는 경우
        averageVisible = setSectionSpeedAverageNumber( averageNumberType, averageSpeed );

        //qDebug() << QString( "[SDI][Fixed]speedLimit %1 blockTime: %2 blockSpeed: %3 averageSpeed: %4 averageNumberType: %5" ).arg( speedLimit ).arg( blockTime ).arg( blockSpeed ).arg( averageSpeed ).arg( averageNumberType );
    }
	// 가변 구간단속인 경우
    else {
        // 1. 제한 속도를 받아와 표시할 수 있는 경우
        if ( speedLimit != 0 ) {
            signType = RgConstants::SdiChangeableSectionSpeed;
            signImage = QStringLiteral("c_10.png");

            setSignNumber( signType, speedLimit );
        }
        // 2. 제한 속도를 받아와 표시할 수 없는 경우		
        else {
            signImage = QStringLiteral("c_11.png");
            signType = RgConstants::SdiChangeableSectionSpeedButSpeedLimitNA;
            averageNumberType = RgConstants::SdiWarning;
        }

        // 3. 평균속도를 표시할 수 없는 경우
        averageVisible = setSectionSpeedAverageNumber( averageNumberType, averageSpeed );

        //qDebug() << QString( "[SDI][Changeable]speedLimit: %1 averageSpeed: %2 averageNumberType: %3" )
        //    .arg( speedLimit ).arg( averageSpeed ).arg( averageNumberType );
    }

    timeVisible = setSectionSpeedTime( averageNumberType, blockTime );
    bool toggleSpeedAndTime = averageVisible && timeVisible;

    QQmlProperty::write( mViewComponent, QStringLiteral("sectionSpeedAverageVisible"), averageVisible  );
    QQmlProperty::write( mViewComponent, QStringLiteral("sectionSpeedTimeVisible"), timeVisible );
    QMetaObject::invokeMethod( mViewComponent, "enableToggleSpeedSectionAverageAndTime", Q_ARG(QVariant,toggleSpeedAndTime) );

    mLastState.sectionSpeedAverageNumberType = averageNumberType;
    mLastState.sectionSpeedAverageVisible = averageVisible;
    mLastState.sectionSpeedTimeVisible = timeVisible;

    if ( mUpdate.sectionSpeedToggleAverageAndTime != toggleSpeedAndTime ) {
        mUpdate.sectionSpeedToggleAverageAndTime = toggleSpeedAndTime;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedToggleAverageAndTime},
            {QStringLiteral("numberType"),(int)mLastState.sectionSpeedAverageNumberType},
            {QStringLiteral("value"),mUpdate.sectionSpeedToggleAverageAndTime}
        });
    }

    if ( mUpdate.sectionSpeedAverageVisible != averageVisible ) {
        mUpdate.sectionSpeedAverageVisible = averageVisible;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedAverageVisible},
            {QStringLiteral("numberType"),(int)mLastState.sectionSpeedAverageNumberType},
            {QStringLiteral("value"),mUpdate.sectionSpeedAverageVisible}
        });
    }

    if ( mUpdate.sectionSpeedTimeVisible != timeVisible ) {
        mUpdate.sectionSpeedTimeVisible = timeVisible;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiSectionSpeedTimeVisible},
            {QStringLiteral("numberType"),(int)mLastState.sectionSpeedAverageNumberType},
            {QStringLiteral("value"),mUpdate.sectionSpeedTimeVisible}
        });
    }

    return averageVisible || timeVisible;
}

void SdiViewMediator::setRemainDistanceNumber(bool& remainDistVisible, int sdiDist, int sdiBlockDist, int sdiType)
{
    int rest = 0;
    if(sdiType == RgSdiCode::SPEED_BLOCK_MID_POS)
        rest = sdiBlockDist;
    else
        rest = sdiDist;

    auto r1 = mRest;
    auto r2 = rest;

    bool visible = true;
    if ( rest <= 0 ) {
        visible = false;
    }
    else {
        if( mRest != rest ) {
            mRest = rest;
            auto restStr = RgUtils::formatDistance(rest);
            QStringList images;
            for(int i = 0; i < restStr.length(); i++){
                auto ch = restStr.at(i);                
                images.append( ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiDistanceNumberImage(ch) ) );
                if ( ch == QChar('k') ) {
                    i++;
                }
            }

            QMetaObject::invokeMethod( mViewComponent, "setRemainDistNumberImages", Q_ARG(QVariant,images) );

            mLastState.remainDist = restStr;
            if ( mUpdate.remainDist != restStr ) {
                mUpdate.remainDist = restStr;
                sendNotification(RgConstants::UpdateSdiState, QVariantMap{
                    {QStringLiteral("item"),RgConstants::SdiDistance},
                    {QStringLiteral("value"),mUpdate.remainDist}
                });
            }
        }
    }

    remainDistVisible = visible;
}

void SdiViewMediator::reset()
{
    mLastSdiType = RgSdiCode::UNKNOWN;
    mLastSdiPointX = 0;
    mLastSdiPointY = 0;
    mLastPosX = 0;
    mLastPosY = 0;
    mCurrentRecommendedSpeed = -1;
    mRest = -1;
    mAlertState = false;
    mLastState = NotifiableUpdate();
    mUpdate = NotifiableUpdate();    
    QMetaObject::invokeMethod(mViewComponent, "reset");
    sendNotification(RgConstants::UpdateSdiState, QVariantMap{
        {QStringLiteral("item"),RgConstants::SdiReset}
    });
    sendNotification(Constants::HideView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::FullscreenAlertView)}});
}

