#ifndef RGDATAVO_H
#define RGDATAVO_H

#include "../../common/rgdataptr.h"

namespace SKT {

class RgDataVo
{
public:
    RgDataVo();    
    RgDataVo( const RgDataVo& other );
    RgDataVo& operator=( const RgDataVo& other );

    RgDataVo( const RGDataPtr& rgData );

    RGDataPtr rgdata() const;

    bool isNull() const;

    bool isSimul() const;
    void setIsSimul(bool value);

    int eRgStatus() const;

    // via
    bool isEnableToPassVia() const;
    void setEnableToPassVia(bool value);

    // sdi

    bool bSdi() const;
    int nSdiDist() const;
    int nSdiBlockDist() const;
    int nSdiBlockSpeed() const;
    int nSdiBlockAverageSpeed() const;
    int nSdiBlockTime() const;
    int nSdiSpeedLimit() const;
    int nSdiType() const;
    int nSdiDrivingBusLine() const;
    int vpSdiPointX() const;
    int vpSdiPointY() const;
    int vpSdiBlockEndpointX() const;
    int vpSdiBlockEndpointY() const;
    bool bSdiPlus() const;    
    void set_nSdiType( int value );
    void set_nSdiSpeedLimit( int value );
    void set_nSdiCurrentSpeed( int value );
    void set_nSdiDist( int value );
    void set_bSdi( bool value );
    void set_vpSdiPointX( int value );
    void set_vpSdiPointY( int value );
    void set_nSdiBlockDist( int value );
    void set_nSdiBlockTime( int value );
    void set_nSdiBlockSpeed( int value );
    int set_nSdiBlockAverageSpeed( int value );

    int nSdiPlusType() const;
    int nSdiPlusSpeedLimit() const;
    int nSdiPlusDist() const;
    bool bSDIPlus() const;
    int vpSdiPlusPointX() const;
    int vpSdiPlusPointY() const;
    int nSdiPlusBlockDist() const;
    int nSdiPlusBlockTime() const;
    int nSdiPlusBlockSpeed() const;
    int nSdiBlockEndpointX() const;
    int nSdiBlockEndpointY() const;

    bool bIsChangeableSpeedType() const;
    void set_bIsChangeableSpeedType( bool value );

    // current

    int nCurSpeed() const;
    void set_nCurSpeed(int value);
    int nTotalRemainDist() const;
    int nTotalRemainTime() const;

    QString stCurrent_szPosAddrName() const;
    QString stCurrent_szPosRoadName() const;
    QPoint stCurrent_position() const;

    // tbt

    int stGuidePoint_nTBTTurnType() const;
    int stGuidePoint_nTBTDist() const;
    int stGuidePoint_nTollFee() const;
    QString stGuidePoint_szRoadName() const;
    QString stGuidePoint_szCrossName() const;
    QString stGuidePoint_szNearDirName() const;
    QString stGuidePoint_szMidDirName() const;
    QString stGuidePoint_szFarDirName() const;
    QString stGuidePoint_szTBTMainText() const;
    int stGuidePointNext_nTBTTurnType() const;
    int stGuidePointNext_nSvcLinkDist() const;

    // lane

    bool bLane( int lane ) const;
    void set_bLane( int lane, bool value );
    int nLaneDist( int lane ) const;
    int nLaneCount( int lane ) const;
    int nLaneTurnCode( int lane ) const;
    int nLaneTurnInfo( int lane, int index ) const;
    int nLaneEtcInfo( int lane, int index ) const;
    int nLaneAvailable( int lane, int index ) const;
    bool bLanePlay( int lane ) const;

    // tbt list

    int nShowHighway() const;
    int nTBTIndexHW() const;
    QString szGoPosName() const;
    int nGroupID() const;
    int nAccDist() const;
    int nTollFee() const;
    int nTBTIndexSummary() const;

    // summary tbt list

    void setSummaryTbtList( const QVector<VSM_OsTBTList>& tbtList );
    bool summaryTbtListUpdated() const;
    void setSummaryTbtListUpdated( bool updated );    
    int summaryTbtList_length() const;
    int summaryTbtList_nAccDist( int index ) const;
    int summaryTbtList_nTBTType( int index ) const;
    int summaryTbtList_nTurnCode( int index ) const;
    QString summaryTbtList_szTBTMainText( int index ) const;
    int summaryTbtList_nCong( int index ) const;

    // highway tbt list

    bool highwayDisplay();
    void setHighwayDisplay( bool display );
    void setHighwayTbtList( const QVector<VSM_OsTBTList>& tbtList );
    bool highwayTbtListUpdated() const;
    void setHighwayTbtListUpdated( bool updated );
    int highwayTbtList_length() const;
    int highwayTbtList_nAccDist( int index ) const;
    int highwayTbtList_nTBTType( int index ) const;
    QString highwayTbtList_szTBTMainText( int index ) const;
    bool highwayTbtList_bHighWayExit( int index ) const;
    unsigned char highwayTbtList_byOilFlag( int index ) const;

    int highwayTbtList_eOilCompany( int index, int arrayIndex = 0 ) const;

    int highwayTbtList_eGasCompany( int index ) const;
    int highwayTbtList_uOilPrice( int index, int priceIndex ) const;
    int highwayTbtList_nCong( int index ) const;
    int highwayTbtList_nTollFee( int index ) const;
    int highwayTbtList_nAccGroupHighWayDist( int index ) const;
    int highwayTbtList_nGroupHighWayDist( int index ) const;    
    int highwayTbtList_nDcCHA( int index ) const;
    int highwayTbtList_nAC3( int index ) const;
    int highwayTbtList_nDcCombo( int index ) const;
    int highwayTbtList_nSlowCharge( int index ) const;
    bool highwayTbtList_Ev_isRapidCharger( int index ) const;
    bool highwayTbtList_Ev_isSlowCharger( int index ) const;

    // cross

    bool bExtcImage() const;
    int nExtcImageShowDist() const;
    int nExtcImageCode() const;

    QString szImageBaseUrl() const;
    QString szImageDayUri() const;
    QString szImageNightUri() const;

    int nDirImageCode() const;

    // hipass

    int nHiPassLaneCount() const;
    int nHiPassArray( int index ) const;
    bool isHipassLaneChanged( const RgDataVo& old );

    // Reroute
    int nDrgVXIdx() const;
    bool bPeriodicReRouteArea() const;
    int congestRid() const;
	
    // signal
    int signalStrength() const;

private:
    RGDataPtr mRgData;
    QVector<VSM_OsTBTList> mSummaryTbtList;
    bool mSummaryTbtListUpdated;
    QVector<VSM_OsTBTList> mHighwayTbtList;
    bool mHighwayTbtListUpdated;
    bool mHighwayDisplay;
    bool mIsEnableToPassVia;    
    bool mIsSimul = false;
};

inline bool RgDataVo::isNull() const
{
    return mRgData.isNull();
}

inline int RgDataVo::eRgStatus() const
{
    return mRgData->eRgStatus;
}

inline int RgDataVo::nDrgVXIdx() const
{
    return mRgData->stDRGInfo.nDrgVXIdx;
}

inline bool RgDataVo::bPeriodicReRouteArea() const
{
    return mRgData->bPeriodicReRouteArea;
}

}

Q_DECLARE_METATYPE(SKT::RgDataVo)

#endif // RGDATAVO_H

