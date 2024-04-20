#include "RgDataVo.h"
#include "RgProtocol.h"

#include "StringHelper.h"

using namespace SKT;

RgDataVo::RgDataVo()
    : mSummaryTbtListUpdated( false ),
    mHighwayTbtListUpdated( false ),
    mHighwayDisplay( false ),
    mIsEnableToPassVia( false ),
    mIsSimul( false )
{
}

RgDataVo::RgDataVo( const RGDataPtr& rgData )
    : mSummaryTbtListUpdated( false ),
    mHighwayTbtListUpdated( false ),
    mHighwayDisplay( false ),
    mIsEnableToPassVia( false ),
    mRgData( rgData ),
    mIsSimul( false )
{
}

RgDataVo::RgDataVo( const RgDataVo& other )
    : mSummaryTbtListUpdated( other.mSummaryTbtListUpdated ),
    mHighwayTbtListUpdated( other.mHighwayTbtListUpdated ),
    mHighwayDisplay( other.mHighwayDisplay ),
    mSummaryTbtList( other.mSummaryTbtList ),
    mHighwayTbtList( other.mHighwayTbtList ),
    mIsEnableToPassVia( other.mIsEnableToPassVia ),
    mRgData( other.mRgData ),
    mIsSimul( other.mIsSimul )
{
}

RgDataVo& RgDataVo::operator=( const RgDataVo& other )
{
    mSummaryTbtListUpdated = other.mSummaryTbtListUpdated;
    mHighwayTbtListUpdated = other.mHighwayTbtListUpdated;
    mHighwayDisplay = other.mHighwayDisplay;
    mSummaryTbtList = other.mSummaryTbtList;
    mHighwayTbtList = other.mHighwayTbtList;
    mIsEnableToPassVia = other.mIsEnableToPassVia;
    mRgData = other.mRgData;
    mIsSimul = other.mIsSimul;

    return *this;
}

RGDataPtr RgDataVo::rgdata() const
{
    return mRgData;
}

bool RgDataVo::isSimul() const
{
    return mIsSimul;
}

void RgDataVo::setIsSimul(bool value)
{
    mIsSimul = value;
}

// via
bool RgDataVo::isEnableToPassVia() const
{
    return mIsEnableToPassVia;
}

void RgDataVo::setEnableToPassVia(bool value)
{
    mIsEnableToPassVia = value;
}

bool RgDataVo::bSdi() const
{
    return mRgData->bSDI;
}

int RgDataVo::nSdiDist() const
{
    return mRgData->stSDI[0].nSdiDist;
}

int RgDataVo::nSdiBlockDist() const
{
    return mRgData->stSDI[0].nSdiBlockDist;
}

int RgDataVo::nSdiBlockSpeed() const
{
    return mRgData->stSDI[0].nSdiBlockSpeed;
}

int RgDataVo::nSdiBlockAverageSpeed() const
{
    return mRgData->stSDI[0].nSdiBlockAverageSpeed;
}

int RgDataVo::nSdiBlockTime() const
{
    return mRgData->stSDI[0].nSdiBlockTime;
}

int RgDataVo::nSdiSpeedLimit() const
{
    return mRgData->stSDI[0].nSdiSpeedLimit;
}

int RgDataVo::nSdiBlockEndpointX() const
{
    return mRgData->stSDI[0].vpSdiBlockEndPoint.x;
}

int RgDataVo::nSdiBlockEndpointY() const
{
    return mRgData->stSDI[0].vpSdiBlockEndPoint.y;
}

int RgDataVo::nSdiDrivingBusLine() const
{
    return mRgData->stSDI[0].nDrivingBusLine;
}

bool RgDataVo::bSdiPlus() const
{
    return mRgData->bSDIPlus;
}

int RgDataVo::nSdiType() const
{
    return mRgData->stSDI[0].nSdiType;
}

int RgDataVo::vpSdiPointX() const
{
    return mRgData->stSDI[0].vpSdiPoint.x;
}

int RgDataVo::vpSdiPointY() const
{
    return mRgData->stSDI[0].vpSdiPoint.y;
}

int RgDataVo::vpSdiBlockEndpointX() const
{
    return mRgData->stSDI[0].vpSdiBlockEndPoint.x;
}

int RgDataVo::vpSdiBlockEndpointY() const
{
    return mRgData->stSDI[0].vpSdiBlockEndPoint.y;
}

void RgDataVo::set_nSdiType( int value )
{
    mRgData->stSDI[0].nSdiType = value;
}

void RgDataVo::set_nSdiSpeedLimit( int value )
{
    mRgData->stSDI[0].nSdiSpeedLimit = value;
}

void RgDataVo::set_nSdiDist( int value )
{
    mRgData->stSDI[0].nSdiDist = value;
}

void RgDataVo::set_bSdi( bool value )
{
    mRgData->bSDI = value;
}

void RgDataVo::set_vpSdiPointX( int value )
{
    mRgData->stSDI[0].vpSdiPoint.x = value;
}

void RgDataVo::set_vpSdiPointY( int value )
{
    mRgData->stSDI[0].vpSdiPoint.y = value;
}

void RgDataVo::set_nSdiBlockDist( int value )
{
    mRgData->stSDI[0].nSdiBlockDist = value;
}

void RgDataVo::set_nSdiBlockTime( int value )
{
    mRgData->stSDI[0].nSdiBlockTime = value;
}

void RgDataVo::set_nSdiBlockSpeed( int value )
{
    mRgData->stSDI[0].nSdiBlockSpeed = value;
}

int RgDataVo::set_nSdiBlockAverageSpeed( int value )
{
    mRgData->stSDI[0].nSdiBlockAverageSpeed = value;
}

// rgData.nSdiPlusType 확인 필요

int RgDataVo::nSdiPlusType() const
{
    return mRgData->stSDIPlus[0].nSdiType;
}

int RgDataVo::nSdiPlusSpeedLimit() const
{
    return mRgData->stSDIPlus[0].nSdiSpeedLimit;
}

int RgDataVo::nSdiPlusDist() const
{
    return mRgData->stSDIPlus[0].nSdiDist;
}

bool RgDataVo::bSDIPlus() const
{
    return mRgData->bSDIPlus;
}

int RgDataVo::vpSdiPlusPointX() const
{
    return mRgData->stSDIPlus[0].vpSdiPoint.x;
}

int RgDataVo::vpSdiPlusPointY() const
{
    return mRgData->stSDIPlus[0].vpSdiPoint.y;
}

int RgDataVo::nSdiPlusBlockDist() const
{
    return mRgData->stSDIPlus[0].vpSdiPoint.x;
}

int RgDataVo::nSdiPlusBlockTime() const
{
    return mRgData->stSDIPlus[0].nSdiBlockTime;
}

int RgDataVo::nSdiPlusBlockSpeed() const
{
    return mRgData->stSDIPlus[0].nSdiBlockSpeed;
}

bool RgDataVo::bIsChangeableSpeedType() const
{
    return mRgData->stSDI[0].bIsChangeableSpeedType;
}

void RgDataVo::set_bIsChangeableSpeedType( bool value )
{
    mRgData->stSDI[0].bIsChangeableSpeedType = value;
}

int RgDataVo::nCurSpeed() const
{
    return mRgData->stCurrent.nPosSpeed;
}

void RgDataVo::set_nCurSpeed(int value)
{
     mRgData->stCurrent.nPosSpeed = value;
}

int RgDataVo::nTotalRemainDist() const
{
    return mRgData->nTotalDist;
}

int RgDataVo::nTotalRemainTime() const
{
    return mRgData->nTotalTime;
}

QString RgDataVo::stCurrent_szPosRoadName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stCurrent.szPosRoadName ).trimmed();
}

QPoint RgDataVo::stCurrent_position() const
{
    return QPoint { mRgData->stCurrent.vpPosPoint.x, mRgData->stCurrent.vpPosPoint. y };
}

QString RgDataVo::stCurrent_szPosAddrName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stCurrent.szPosAddrName ).trimmed();
}

int RgDataVo::stGuidePoint_nTBTTurnType() const
{
    return mRgData->stGuidePoint.nTBTTurnType;
}

int RgDataVo::stGuidePoint_nTBTDist() const
{
    return mRgData->stGuidePoint.nTBTDist;
}

int RgDataVo::stGuidePoint_nTollFee() const
{
    return mRgData->stGuidePoint.nTollFee;
}

QString RgDataVo::stGuidePoint_szRoadName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szRoadName ).trimmed();
}

QString RgDataVo::stGuidePoint_szCrossName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szCrossName ).trimmed();
}

QString RgDataVo::stGuidePoint_szNearDirName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szNearDirName ).trimmed();
}

QString RgDataVo::stGuidePoint_szMidDirName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szMidDirName ).trimmed();
}

QString RgDataVo::stGuidePoint_szFarDirName() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szFarDirName ).trimmed();
}

QString RgDataVo::stGuidePoint_szTBTMainText() const
{
    return StringHelper::instance()->fromCp949( mRgData->stGuidePoint.szTBTMainText ).trimmed();
}

int RgDataVo::stGuidePointNext_nTBTTurnType() const
{
    return mRgData->stGuidePointNext.nTBTTurnType;
}

int RgDataVo::stGuidePointNext_nSvcLinkDist() const
{
    return mRgData->stGuidePointNext.nSvcLinkDist;
}

// lane first

bool RgDataVo::bLane( int lane ) const
{
    return lane == 0 ? mRgData->bLane1st : mRgData->bLane2nd;
}

void RgDataVo::set_bLane( int lane, bool value )
{
    lane == 0 ? mRgData->bLane1st = value : mRgData->bLane2nd = value;
}

int RgDataVo::nLaneDist( int lane ) const
{
    return lane == 0 ? mRgData->stLane1st.nLaneDist : mRgData->stLane2nd.nLaneDist;
}

int RgDataVo::nLaneCount( int lane ) const
{
    return lane == 0 ? mRgData->stLane1st.nLaneCount : mRgData->stLane2nd.nLaneCount;
}

int RgDataVo::nLaneTurnCode( int lane ) const
{
    return lane == 0 ? mRgData->stLane1st.nLaneTurnCode : mRgData->stLane2nd.nLaneTurnCode;
}

int RgDataVo::nLaneTurnInfo( int lane, int index ) const
{
    return lane == 0 ? mRgData->stLane1st.nLaneTurnInfo[index] : mRgData->stLane2nd.nLaneTurnInfo[index];
}

int RgDataVo::nLaneEtcInfo( int lane, int index ) const
{
    return lane == 0 ? mRgData->stLane1st.nLaneEtcInfo[index] : mRgData->stLane2nd.nLaneEtcInfo[index];
}

int RgDataVo::nLaneAvailable( int lane, int index ) const
{
    int ret = lane == 0 ? mRgData->stLane1st.nLaneAvailable[index] : mRgData->stLane2nd.nLaneAvailable[index];

    if ( ret == 0 ) {
        //T_DEBUGV( "lane: %d index: %d available: %d", lane, index, ret );
    }

    return ret;
}

bool RgDataVo::bLanePlay( int lane ) const
{
    return lane == 0 ? mRgData->stLane1st.bLanePlay : mRgData->stLane2nd.bLanePlay;
}

// highway

int RgDataVo::nShowHighway() const
{
    return mRgData->stTBTListPos.nShowHighway;
}

int RgDataVo::nTBTIndexHW() const
{
    return mRgData->stTBTListPos.nTBTIndexHW;
}

QString RgDataVo::szGoPosName() const
{
    return StringHelper::instance()->fromCp949( mRgData->szGoPosName ).trimmed();
}

int RgDataVo::nGroupID() const
{
    return mRgData->stTBTListPos.nGroupID;
}

int RgDataVo::nAccDist() const
{
    return mRgData->stTBTListPos.nAccDist;
}

bool RgDataVo::bExtcImage() const
{
    return mRgData->bExtcImage;
}

int RgDataVo::nExtcImageShowDist() const
{
    return mRgData->nExtcImageShowDist;
}

int RgDataVo::nExtcImageCode() const
{
    return mRgData->nExtcImageCode;
}

QString RgDataVo::szImageBaseUrl() const
{
    return StringHelper::instance()->fromCp949( mRgData->szImageBaseUrl ).trimmed();
}

QString RgDataVo::szImageDayUri() const
{
    return StringHelper::instance()->fromCp949( mRgData->szImageDayUri ).trimmed();
}

QString RgDataVo::szImageNightUri() const
{
    return StringHelper::instance()->fromCp949( mRgData->szImageNightUri ).trimmed();
}

int RgDataVo::nDirImageCode() const
{
    return mRgData->nDirImageCode;
}

int RgDataVo::nHiPassLaneCount() const
{
    return mRgData->nHiPassLaneCount;
}

int RgDataVo::nHiPassArray( int index ) const
{
    return mRgData->nHiPassArry[index];
}

bool RgDataVo::isHipassLaneChanged( const RgDataVo& old )
{
    return memcmp( old.mRgData->nHiPassArry, mRgData->nHiPassArry, sizeof( mRgData->nHiPassArry ) ) != 0;
}

int RgDataVo::congestRid() const
{
    return mRgData->stDRGInfo.congestRid;
}

int RgDataVo::nTBTIndexSummary() const
{
    return mRgData->stTBTListPos.nTBTIndexSummary;
}

// summary tbt list

void RgDataVo::setSummaryTbtList( const QVector<VSM_OsTBTList>& tbtList )
{
    mSummaryTbtList = tbtList;
}

void RgDataVo::setSummaryTbtListUpdated( bool updated )
{
    mSummaryTbtListUpdated = updated;
}

bool RgDataVo::summaryTbtListUpdated() const
{
    return mSummaryTbtListUpdated;
}

int RgDataVo::summaryTbtList_length() const
{
    return mSummaryTbtList.count();
}

int RgDataVo::summaryTbtList_nAccDist( int index ) const
{
    return mSummaryTbtList[index].nAccDist;
}

int RgDataVo::summaryTbtList_nTBTType( int index ) const
{
    return mSummaryTbtList[index].nTBTType;
}

int RgDataVo::summaryTbtList_nTurnCode( int index ) const
{
    return mSummaryTbtList[index].nTurnCode;
}

QString RgDataVo::summaryTbtList_szTBTMainText( int index ) const
{
    return StringHelper::instance()->fromCp949( mSummaryTbtList[index].szTBTMainText ).trimmed();
}

int RgDataVo::summaryTbtList_nCong( int index ) const
{
    return mSummaryTbtList[index].nCong;
}

// highway tbt list

bool RgDataVo::highwayDisplay()
{
    return mHighwayDisplay;
}

void RgDataVo::setHighwayDisplay( bool display )
{
    mHighwayDisplay = display;
}

void RgDataVo::setHighwayTbtList( const QVector<VSM_OsTBTList>& tbtList )
{
    mHighwayTbtList = tbtList;
}

bool RgDataVo::highwayTbtListUpdated() const
{
    return mHighwayTbtListUpdated;
}

void RgDataVo::setHighwayTbtListUpdated( bool updated )
{
    mHighwayTbtListUpdated = updated;
}

int RgDataVo::highwayTbtList_length() const
{
    return mHighwayTbtList.count();
}

int RgDataVo::highwayTbtList_nAccDist( int index ) const
{
    return mHighwayTbtList[index].nAccDist;
}

int RgDataVo::highwayTbtList_nTBTType( int index ) const
{
    return mHighwayTbtList[index].nTBTType;
}

QString RgDataVo::highwayTbtList_szTBTMainText( int index ) const
{
    return StringHelper::instance()->fromCp949( mHighwayTbtList[index].szTBTMainText ).trimmed();
}

bool RgDataVo::highwayTbtList_bHighWayExit( int index ) const
{
    return mHighwayTbtList[index].bHighWayExit;
}

unsigned char RgDataVo::highwayTbtList_byOilFlag( int index ) const
{
    return mHighwayTbtList[index].byOilFlag;
}

int RgDataVo::highwayTbtList_eOilCompany( int index, int arrayIndex ) const
{
    return mHighwayTbtList[index].eOilCompany[arrayIndex];
}

int RgDataVo::highwayTbtList_eGasCompany( int index ) const
{
    // TOCHECK
    return mHighwayTbtList[index].eOilCompany[0];
}

int RgDataVo::highwayTbtList_uOilPrice( int index, int priceIndex ) const
{
    return mHighwayTbtList[index].uOilPrice[priceIndex];
}

int RgDataVo::highwayTbtList_nCong( int index ) const
{
    return mHighwayTbtList[index].nCong;
}

int RgDataVo::highwayTbtList_nTollFee( int index ) const
{
    return mHighwayTbtList[index].nTollFee;
}

int RgDataVo::highwayTbtList_nAccGroupHighWayDist( int index ) const
{
    return mHighwayTbtList[index].nAccGroupHighWayDist;
}

int RgDataVo::highwayTbtList_nGroupHighWayDist( int index ) const
{
    return mHighwayTbtList[index].nGroupHighWayDist;
}

int RgDataVo::highwayTbtList_nDcCHA( int index ) const
{
    return mHighwayTbtList[index].nDcCHA;
}

int RgDataVo::highwayTbtList_nAC3( int index ) const
{
     return mHighwayTbtList[index].nAC3;
}

int RgDataVo::highwayTbtList_nDcCombo( int index ) const
{
    return  mHighwayTbtList[index].nDcCombo;
}

int RgDataVo::highwayTbtList_nSlowCharge( int index ) const
{
    return mHighwayTbtList[index].nSlowCharge;
}

bool RgDataVo::highwayTbtList_Ev_isRapidCharger( int index ) const
{
    return ( mHighwayTbtList[index].nDcCHA || mHighwayTbtList[index].nAC3 || mHighwayTbtList[index].nDcCombo );
}

bool RgDataVo::highwayTbtList_Ev_isSlowCharger( int index ) const
{
    return (mHighwayTbtList[index].nSlowCharge != 0);
}

int RgDataVo::signalStrength() const
{
	int ret = RgSignal::NONE;

    if ( isNull() ) {
        ret = RgSignal::BAD;
    }
    else {
		if ( mRgData->eVirtualGps == 1 ) {
			ret = RgSignal::TUNNEL;
		} 
		else if ( mRgData->eVirtualGps == 2 ) {
			ret = RgSignal::UNDERPASS;		
		} 
		else {
			ret = RgSignal::GOOD;
		}	
	}
	
	return ret;
}
