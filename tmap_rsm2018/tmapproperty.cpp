#include <QQmlContext>

#include "tmapproperty.h"
#include "tmapcontroller.h"

#include <qdebug.h>

namespace SKT {

TmapProperty::TmapProperty(QObject *parent)
    : QObject(parent)
{
    controller = (TmapController*)parent;

    // initialize
    mTmapNaviStatus = 0;

    memset(&mTmapViewLevel, 0, sizeof(mTmapViewLevel));

    mTmapMapMoveMode = 0;
    mTmapViewMode = 0;
    mTmapVoiceGuidanceType = 0;
    mTmapVolume = 0;
    mTmapIsMute = false;

    memset(&mTmapWorkLocation, 0, sizeof(mTmapWorkLocation));
    memset(&mTmapHomeLocation, 0, sizeof(mTmapHomeLocation));

    mUserProfileID = 0;
    mRouteOption = 0;
    mNightMode = false;
    m_TmapScreenID = 0;
}

TmapProperty::~TmapProperty()
{

}

QString TmapProperty::getTmapAppVersion() const
{
    return controller->appVersion();
}

QString TmapProperty::getTmapMapVersion() const
{
    return mTmapMapVersion;
}
void TmapProperty::setTmapMapVersion(const QString &value)
{
    if(mTmapMapVersion != value){
        mTmapMapVersion = value;
        emit TmapMapVersionChanged(mTmapMapVersion);
    }
}

QString TmapProperty::getTmapSDIVersion() const
{
    return mTmapSDIVersion;
}
void TmapProperty::setTmapSDIVersion(const QString &value)
{
    if(mTmapSDIVersion != value){
        mTmapSDIVersion = value;
        emit TmapSDIVersionChanged(mTmapSDIVersion);
    }
}

QString TmapProperty::getTmapLocalMapVersion() const
{
    return mTmapLocalMapVersion;
}

void TmapProperty::setTmapLocalMapVersion(const QString &value)
{
    if (mTmapLocalMapVersion != value) {
        mTmapLocalMapVersion = value;
        emit TmapLocalMapVersionChanged(value);
    }
}

QString TmapProperty::getTmapLocalNetworkVersion() const
{
    return mTmapLocalNetworkVersion;
}

void TmapProperty::setTmapLocalNetworkVersion(const QString &value)
{
    if (mTmapLocalNetworkVersion != value) {
        mTmapLocalNetworkVersion = value;
        emit TmapLocalNetworkVersionChanged(value);
    }
}

QString TmapProperty::getTmapLocalPoiVersion() const
{
    return mTmapLocalPoiVersion;
}

void TmapProperty::setTmapLocalPoiVersion(const QString &value)
{
    if (mTmapLocalPoiVersion != value) {
        mTmapLocalPoiVersion = value;
        emit TmapLocalPoiVersionChanged(value);
    }
}

int TmapProperty::getTmapNaviStartStatus()
{
    return mTmapNaviStartStatus;
}

void TmapProperty::setTmapNaviStartStatus(int value)
{
    if (mTmapNaviStartStatus != value) {
        mTmapNaviStartStatus = value;
        emit TmapNaviStartStatusChanged(value);
    }
}

int TmapProperty::getTmapNaviStatus()
{
    return mTmapNaviStatus;
}
void TmapProperty::setTmapNaviStatus(int value)
{
    if (value == 1) return; // 20191101 don't provide BUSY status due to cluster issue.
    qDebug() << "tmap property nav status : " << value << ", " << mTmapNaviStatus;
    if(mTmapNaviStatus != value){
        mTmapNaviStatus = value;
        emit TmapNaviStatusChanged(mTmapNaviStatus);
    }
}

RGDataPtr TmapProperty::getTmapRGDataInfo()
{
    return mTmapRGDataInfo;
}
void TmapProperty::setTmapRGDataInfo(RGDataPtr value)
{
    if(mTmapRGDataInfo != value){
        mTmapRGDataInfo = value;
        emit TmapRGDataInfoChanged(mTmapRGDataInfo);
    }
}

SatelliteDataPtr TmapProperty::getTmapSatelliteInfo()
{
    return mTmapSatelliteInfo;
}
void TmapProperty::setTmapSatelliteInfo(SatelliteDataPtr value)
{
//    if(mTmapSatelliteInfo != value){
        mTmapSatelliteInfo = value;
        emit TmapSatelliteInfoChanged(mTmapSatelliteInfo);
//    }
}

ViewLevelInfo TmapProperty::getTmapViewLevel()
{
    QReadLocker lock(&mRwLocker);
    return mTmapViewLevel;
}
void TmapProperty::setTmapViewLevel(ViewLevelInfo value)
{
    if(mTmapViewLevel.mLevel != value.mLevel
            && mTmapViewLevel.mSubLevel != value.mSubLevel ){
        mTmapViewLevel = value;
        emit TmapViewLevelChanged(mTmapViewLevel);
    }
}

int TmapProperty::getTmapMapMoveMode()
{
    return mTmapMapMoveMode;
}
void TmapProperty::setTmapMapMoveMode(int value)
{
    if(mTmapMapMoveMode != value){
        mTmapMapMoveMode = value;
        emit TmapMapMoveModeChanged(mTmapMapMoveMode);
    }
}

int TmapProperty::getTmapViewMode()
{
    return mTmapViewMode;
}
void TmapProperty::setTmapViewMode(int value)
{
    if(mTmapViewMode != value){
        mTmapViewMode = value;
        emit TmapViewModeChanged(mTmapViewMode);
    }
}

int TmapProperty::getTmapVoiceGuidanceType()
{
    return mTmapVoiceGuidanceType;
}
void TmapProperty::setTmapVoiceGuidanceType(int value)
{
    if(mTmapVoiceGuidanceType != value){
        mTmapVoiceGuidanceType = value;
        qDebug() << "TmapProperty::setTmapVoiceGuidanceType(" << value << ")";
        emit TmapVoiceGuidanceTypeChanged(value);
    }
}

bool TmapProperty::getTmapOverSpeedAlertActive()
{
    return mTmapOverSpeedAlertActive;
}

void TmapProperty::setTmapOverSpeedAlertActive(bool value)
{
    if (mTmapOverSpeedAlertActive != value) {
        mTmapOverSpeedAlertActive = value;
        emit TmapOverSpeedAlertActiveChanged(value);
    }
}

int TmapProperty::getTmapVolume()
{
    return mTmapVolume;
}
void TmapProperty::setTmapVolume(int value)
{
    if(mTmapVolume != value){
        mTmapVolume = value;
        emit TmapVolumeChanged(mTmapVolume);
    }
}

bool TmapProperty::getTmapIsMute()
{
    return mTmapIsMute;
}
void TmapProperty::setTmapIsMute(bool value)
{
    qDebug() << "tmap property mute "<< mTmapIsMute << " -> " << value ;
    if(mTmapIsMute != value){
        mTmapIsMute = value;
        emit TmapIsMuteChanged(mTmapIsMute);
    }
}

TS_ROUTE_POS TmapProperty::getTmapWorkLocation()
{
    QReadLocker lock(&mRwLocker);
    return mTmapWorkLocation;
}
void TmapProperty::setTmapWorkLocation(TS_ROUTE_POS value)
{
//    if(mTmapWorkLocation != value){
        mTmapWorkLocation = value;
        emit TmapWorkLocationChanged(mTmapWorkLocation);
//    }
}

TS_ROUTE_POS TmapProperty::getTmapHomeLocation()
{
    QReadLocker lock(&mRwLocker);
    return mTmapHomeLocation;
}
void TmapProperty::setTmapHomeLocation(TS_ROUTE_POS value)
{
//    if(mTmapHomeLocation != value){
        mTmapHomeLocation = value;
        emit TmapHomeLocationChanged(mTmapHomeLocation);
//    }
}

QRoutePosListPtr TmapProperty::getTmapWaypointList()
{
    QReadLocker lock(&mRwLocker);
    return mTmapWaypointList;
}
void TmapProperty::setTmapWaypointList(QRoutePosListPtr value)
{
    if(mTmapWaypointList != value){
        mTmapWaypointList = value;
        emit TmapWaypointListChanged(QVariant::fromValue(mTmapWaypointList));
    }
}

QRoutePosListPtr TmapProperty::getTmapActiveDestinationInfo() 
{
    QReadLocker lock(&mRwLocker);
    return mTmapActiveDestinationInfo;
}

void TmapProperty::setTmapActiveDestinationInfo(QRoutePosListPtr value)
{
        mTmapActiveDestinationInfo = value;
        emit TmapActiveDestinationInfoChanged(QVariant::fromValue(mTmapActiveDestinationInfo));
}

bool TmapProperty::getTmapFavoriteChange()
{
    return false;
}
void TmapProperty::setTmapFavoriteChange(bool value)
{
    emit TmapFavoriteChanged(true);
}

bool TmapProperty::getTmapRecentDestinationChange()
{
    return false;
}
void TmapProperty::setTmapRecentDestinationChange(bool value)
{
    emit TmapRecentDestinationChanged(true);
}

bool TmapProperty::getTmapSearchWordChange()
{
    return false;
}
void TmapProperty::setTmapSearchWordChange(bool value)
{
    emit TmapSearchWordChanged(true);
}

bool TmapProperty::getTmapUserProfileChange()
{
    return mUserProfileID;
}
void TmapProperty::setTmapUserProfileChange(int value)
{
    mUserProfileID = value;
    emit TmapUserProfileChanged(mUserProfileID);
}

bool TmapProperty::getTmapRouteOptionChange()
{
    return mRouteOption;
}
void TmapProperty::setTmapRouteOptionChange(int value)
{
    mRouteOption = value;
    emit TmapRouteOptionChanged(mRouteOption);
}

bool TmapProperty::getTmapNightModeChange()
{
    return mNightMode;
}
void TmapProperty::setTmapNightModeChange(bool value)
{
    mNightMode = value;
    emit TmapNightModeChanged(mNightMode);
}

void TmapProperty::setFavoriteList(QRoutePosListPtr value)
{
    mTmapFavoriteList = value;
    setTmapFavoriteChange(true);
    qDebug() << "tmap property fav changed: " << mTmapFavoriteList->size();
}

QRoutePosListPtr TmapProperty::getFavoriteList()
{
    QReadLocker lock(&mRwLocker);
    return mTmapFavoriteList;
}
void TmapProperty::setRecentDestinationList(QRoutePosListPtr value)
{
    mTmapRecentDestinationList = value;
    setTmapRecentDestinationChange(true);
    qDebug() << "tmap property dest changed: " << mTmapRecentDestinationList->size();
}

QRoutePosListPtr TmapProperty::getRecentDestinationList()
{
    QReadLocker lock(&mRwLocker);
    return mTmapRecentDestinationList;
}
void TmapProperty::setSearchWordList(QSearchWordListPtr value)
{
    mTmapSearchWordList = value;
    setTmapSearchWordChange(true);
}
QSearchWordListPtr TmapProperty::getSearchWordList()
{
    return mTmapSearchWordList;
}

int TmapProperty::TmapScreenID()
{
    return m_TmapScreenID;
}

int TmapProperty::getRouteRecalculationReason() const
{
    return mRouteRecalculationReason;
}

void TmapProperty::setRouteRecalculationReason(int reason)
{
    mRouteRecalculationReason = reason%11; // range is 0 ~ 10
    emit TmapRouteRecalculationReasonChanged(mRouteRecalculationReason);
}


QList<int> TmapProperty::getWaypointOperationStatusList() const
{
    return mTmapWaypointOperationStatusList;
}

void TmapProperty::setWaypointOperationStatusList(const QList<int>& status)
{
     mTmapWaypointOperationStatusList  = status;
     emit TmapWaypointOperationStatusChanged(status);
}

int TmapProperty::getRouteCalculateDuration() const
{
    return mRouteCalculateDuration;
}

void TmapProperty::setRouteCalculateDuration(int duration)
{
    mRouteCalculateDuration = duration;
    emit TmapRouteCalculateDurationChanged(duration);
}

void TmapProperty::setTmapScreenID(int TmapScreenID)
{
    if (m_TmapScreenID == TmapScreenID)
        return;

    m_TmapScreenID = TmapScreenID;
    emit TmapScreenIDChanged(m_TmapScreenID);
}

void TmapProperty::onTmapRequestProperty_All()
{
    qDebug() << "TmapMapVersionChanged(" << mTmapMapVersion << ")";
    emit TmapMapVersionChanged(mTmapMapVersion);
    qDebug() << "TmapSDIVersionChanged(" << mTmapSDIVersion << ")";
    emit TmapSDIVersionChanged(mTmapSDIVersion);
    qDebug() << "TmapMapMoveModeChanged(" << mTmapMapMoveMode << ")";
    emit TmapMapMoveModeChanged(mTmapMapMoveMode);
    qDebug() << "TmapViewModeChanged(" << mTmapViewMode << ")";
    emit TmapViewModeChanged(mTmapViewMode);
    qDebug() << "TmapViewLevelChanged(" << mTmapViewLevel.mLevel << ")";
    emit TmapViewLevelChanged(mTmapViewLevel);
    qDebug() << "TmapVoiceGuidanceTypeChanged(" << mTmapVoiceGuidanceType << ")";
    emit TmapVoiceGuidanceTypeChanged(mTmapVoiceGuidanceType);
    qDebug() << "TmapVolumeChanged(" << mTmapVolume << ")";
    emit TmapVolumeChanged(mTmapVolume);
    qDebug() << "TmapIsMuteChanged(" << mTmapIsMute << ")";
    emit TmapIsMuteChanged(mTmapIsMute);
    qDebug() << "TmapWorkLocationChanged(" << mTmapWorkLocation.tp.x << ", " << mTmapWorkLocation.tp.y << ")";
    emit TmapWorkLocationChanged(mTmapWorkLocation);
    qDebug() << "TmapHomeLocationChanged(" << mTmapHomeLocation.tp.x << ", " << mTmapHomeLocation.tp.y << ")";
    emit TmapHomeLocationChanged(mTmapHomeLocation);
    if(mTmapRGDataInfo.isNull() == false) {
        qDebug() << "TmapRGDataInfoChanged(" << mTmapRGDataInfo << ")";
        emit TmapRGDataInfoChanged(mTmapRGDataInfo);
    }
    qDebug() << "TmapSatelliteInfoChanged(" << mTmapSatelliteInfo << ")";
    emit TmapSatelliteInfoChanged(mTmapSatelliteInfo);
}

void TmapProperty::setErrorMap(int error)
{
    mError = error;
}

int  TmapProperty::getErrorMap() const
{
    return mError;
}


}
