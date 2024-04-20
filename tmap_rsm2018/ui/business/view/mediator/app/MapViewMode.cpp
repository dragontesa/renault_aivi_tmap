#include "MapViewMode.h"
#include "Constants.h"
#include "MapConstants.h"
#include "CommonConstants.h"

using namespace SKT;

MapViewMode::MapViewMode()
    : mCurMode( ModeK::Normal ),
    mCurReason( ReasonK::None ),
    mLastDriveMode( ModeK::Normal )
{

}

MapViewMode::ModeK MapViewMode::currentMode()
{
    return mCurMode;
}

MapViewMode::ReasonK MapViewMode::currentReason()
{
    return mCurReason;
}

void MapViewMode::setCurrentMode( QObject* caller, ModeK mode, ReasonK reason )
{
    auto lastMode = mCurMode;
    auto lastReason = mCurReason;
    auto changed = mCurMode != mode || mCurReason != reason;
    mCurMode = mode;
    mCurReason = reason;

//    // debugging
//    T_DEBUG( QString( QStringLiteral("<B>Caller: %1 Curr: %2(%3) Last: %4(%5) LastDriveMode: %6</B>") )
//             .arg( caller->metaObject()->className() )
//             .arg( modeAsString( mCurMode ) )
//             .arg( reasonAsString( mCurReason ) )
//             .arg( modeAsString( lastMode ) )
//             .arg( reasonAsString( lastReason ) )
//             .arg( modeAsString( mLastDriveMode ) ) );

    switch( mCurMode ) {
    case MapViewMode::Normal:
        {
            mLastDriveMode = mCurMode;
        }
        break;
    case MapViewMode::MapControl:
        {
        }
        break;
    case MapViewMode::MapInquiry:
        {
        }
        break;
    case MapViewMode::Navi:
        {
            mLastDriveMode = mCurMode;
        }
        break;
    case MapViewMode::MenuNavi:
        {
			// 17.10.10 hskim MenuNavi 모드는 mLastDriveMode에 영향을 주지 않는다.
        }
        break;		
    case MapViewMode::Simul:
        {
        }
        break;
    case MapViewMode::NormalMapControl:
        {
        }
        break;
    }

    if ( changed ) {
        auto msg = QString( QStringLiteral("Curr: %1(%2) Last: %3(%4) LastDriveMode: %5 Caller: %6") )
                    .arg( modeAsString( mCurMode ) )
                    .arg( reasonAsString( mCurReason ) )
                    .arg( modeAsString( lastMode ) )
                    .arg( reasonAsString( lastReason ) )
                    .arg( modeAsString( mLastDriveMode ) )
                    .arg( caller->metaObject()->className() );

        T_DEBUG( msg );

        QVariantMap body;
        body.insert( CommonConstants::Mode, (int)mode );
        body.insert( QStringLiteral("reason"), (int)reason );
        body.insert( QStringLiteral("lastDriveMode"), (int)mLastDriveMode );
        Facade::instance()->sendNotification( this, MapConstants::ApplyMapViewMode, body );
    }
}

QString MapViewMode::modeAsString( ModeK mode )
{
    QString ret;

    switch ( mode ) {
    case Normal:
        ret = QStringLiteral("Normal");
        break;
    case MapControl:
        ret = QStringLiteral("MapControl");
        break;
    case MapInquiry:
        ret = QStringLiteral("MapInquiry");
        break;
    case Navi:
        ret = QStringLiteral("Navi");
        break;
    case MenuNavi:
        ret = QStringLiteral("MenuNavi");
        break;		
    case Simul:
        ret = QStringLiteral("Simul");
        break;
    case NormalMapControl:
        ret = QStringLiteral("NormalMapControl");
        break;
    }

    return ret;
}

QString MapViewMode::reasonAsString( ReasonK reason )
{
    QString ret;

    switch ( reason ) {
    case MapViewMode::None:
        ret = QStringLiteral("None");
        break;
    case MapViewMode::MapViewCreated:
        ret = QStringLiteral("MapViewCreated");
        break;
    case MapViewMode::MapViewActivated:
        ret = QStringLiteral("MapViewActivated");
        break;
    case MapViewMode::ReturnFromNormalMode:
        ret = QStringLiteral("ReturnFromNormalMode");
        break;
    case MapViewMode::ReturnFromMapControlMode:
        ret = QStringLiteral("ReturnFromMapControlMode");
        break;
    case MapViewMode::ReturnFromMapInquiryMode:
        ret = QStringLiteral("ReturnFromMapInquriyMode");
        break;
    case MapViewMode::ReturnFromNaviMode:
        ret = QStringLiteral("ReturnFromNaviMode");
        break;
    case MapViewMode::ReturnFromMenuNaviMode:
		ret = QStringLiteral("ReturnFromMenuNaviMode");
        break;	
    case MapViewMode::ReturnFromSimulMode:
        ret = QStringLiteral("ReturnFromSimulMode");
        break;
    case  MapViewMode::CancelRoute:
        ret = QStringLiteral("CancelRoute");
        break ;
    case MapViewMode::RecentTileGridViewLostFocus:
        ret = QStringLiteral("RecentTileGridViewLostFocus");
        break;
    }

    return ret;
}

MapViewMode::ModeK MapViewMode::lastDriveMode()
{
    return mLastDriveMode;
}

void MapViewMode::notifyCurrentMode( ReasonK reason )
{
    QVariantMap body;
    body.insert( CommonConstants::Mode, (int)mCurMode );
    if ( reason != None ) {
        body.insert( QStringLiteral("reason"), (int)reason );
    }
    else {
        body.insert( QStringLiteral("reason"), (int)mCurReason );
    }

    Facade::instance()->sendNotification( this, MapConstants::ApplyMapViewMode, body );
}

// 맵이(카바타가) 움직이는 상태인지
bool MapViewMode::isInDrive( int mapViewMode )
{
    return mapViewMode == MapViewMode::Navi || mapViewMode == MapViewMode::Simul || mapViewMode == MapViewMode::Normal || mapViewMode == MapViewMode::MenuNavi;
}

// 경로안내를 받고 있는지
bool MapViewMode::isInRouteGuidance( int mapViewMode  )
{
    return mapViewMode == MapViewMode::Navi || mapViewMode == MapViewMode::Simul;
}
