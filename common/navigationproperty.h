#ifndef NAVIGATIONPROPERTY_H
#define NAVIGATIONPROPERTY_H
#include <QObject>
#include <QList>
#include "navigationdata.h"
#include "satellitedataptr.h"

namespace SKT {

typedef QSharedPointer<QList<TS_ROUTE_POS>> QRoutePosListPtr;
typedef QSharedPointer<QList<POICateInfo>> QPOICateInfoListPtr;
typedef QSharedPointer<QList<QString>> QSearchWordListPtr;

class NavigationProperty
{
    // level 1
public:
    virtual ~NavigationProperty() {}

    virtual QObject* qobject() = 0;

    virtual QString getTmapAppVersion() const = 0;

    virtual QString getTmapMapVersion() const = 0;
    virtual void setTmapMapVersion(const QString &value) = 0;

    virtual QString getTmapSDIVersion() const = 0;
    virtual void setTmapSDIVersion(const QString &value) = 0;

    virtual QString getTmapLocalMapVersion() const = 0;
    virtual void setTmapLocalMapVersion(const QString &value) = 0;
    virtual QString getTmapLocalNetworkVersion() const = 0;
    virtual void setTmapLocalNetworkVersion(const QString &value) = 0;
    virtual QString getTmapLocalPoiVersion() const = 0;
    virtual void setTmapLocalPoiVersion(const QString &value) = 0;

    virtual int getTmapNaviStartStatus() = 0;
    virtual void setTmapNaviStartStatus(int value) = 0;

    virtual int getTmapNaviStatus() = 0;
    virtual void setTmapNaviStatus(int value) = 0;

    virtual RGDataPtr getTmapRGDataInfo() = 0;
    virtual void setTmapRGDataInfo(RGDataPtr value) = 0;

    virtual SatelliteDataPtr getTmapSatelliteInfo() = 0;
    virtual void setTmapSatelliteInfo(SatelliteDataPtr value) = 0;

    virtual ViewLevelInfo getTmapViewLevel() = 0;
    virtual void setTmapViewLevel(ViewLevelInfo value) = 0;

    virtual int getTmapMapMoveMode() = 0;
    virtual void setTmapMapMoveMode(int value) = 0;

    virtual int getTmapViewMode() = 0;
    virtual void setTmapViewMode(int value) = 0;

    virtual int getTmapVoiceGuidanceType() = 0;
    virtual void setTmapVoiceGuidanceType(int value) = 0;

    virtual bool getTmapOverSpeedAlertActive() = 0;
    virtual void setTmapOverSpeedAlertActive(bool value) = 0;

    virtual int getTmapVolume() = 0;
    virtual void setTmapVolume(int value) = 0;

    virtual bool getTmapIsMute() = 0;
    virtual void setTmapIsMute(bool value) = 0;

    virtual TS_ROUTE_POS getTmapWorkLocation() = 0;
    virtual void setTmapWorkLocation(TS_ROUTE_POS value) = 0;

    virtual TS_ROUTE_POS getTmapHomeLocation() = 0;
    virtual void setTmapHomeLocation(TS_ROUTE_POS value) = 0;

    virtual QRoutePosListPtr getTmapWaypointList() = 0;
    virtual void setTmapWaypointList(QRoutePosListPtr value) = 0;

    virtual QRoutePosListPtr getTmapActiveDestinationInfo() = 0;
    virtual void setTmapActiveDestinationInfo(QRoutePosListPtr value) = 0;

    virtual bool getTmapFavoriteChange() = 0;
    virtual void setTmapFavoriteChange(bool value) = 0;

    virtual bool getTmapRecentDestinationChange() = 0;
    virtual void setTmapRecentDestinationChange(bool value) = 0;

    virtual bool getTmapSearchWordChange() = 0;
    virtual void setTmapSearchWordChange(bool value) = 0;

    virtual bool getTmapUserProfileChange() = 0;
    virtual void setTmapUserProfileChange(int value) = 0;

    virtual bool getTmapRouteOptionChange() = 0;
    virtual void setTmapRouteOptionChange(int value) = 0;

    virtual bool getTmapNightModeChange() = 0;
    virtual void setTmapNightModeChange(bool value) = 0;

    virtual QRoutePosListPtr getFavoriteList() = 0;
    virtual void setFavoriteList(QRoutePosListPtr value) = 0;
    virtual QRoutePosListPtr getRecentDestinationList() = 0;
    virtual void setRecentDestinationList(QRoutePosListPtr value) = 0;
    virtual QSearchWordListPtr getSearchWordList() = 0;
    virtual void setSearchWordList(QSearchWordListPtr value) = 0;

    virtual int getRouteRecalculationReason() const = 0;
    virtual void setRouteRecalculationReason(int reason) = 0;

    // virtual int getDestinationReachedStatus() const = 0;
    // virtual void setDestinationReachedStatus(int status) = 0;
    virtual QList<int> getWaypointOperationStatusList() const = 0;
    virtual void setWaypointOperationStatusList(const QList<int>& status) = 0;

    virtual int getRouteCalculateDuration() const = 0;
    virtual void setRouteCalculateDuration(int duration) = 0;

    virtual int TmapScreenID() = 0;

    virtual int  getErrorMap() const = 0;
    virtual void setErrorMap(int error) = 0;


signals:
    // just hint. You should declare signals in tmapproperty.h
    void TmapMapVersionChanged(QString value);
    void TmapSDIVersionChanged(QString value);
    void TmapLocalMapVersionChanged(QString value);
    void TmapLocalNetworkVersionChanged(QString value);
    void TmapLocalPoiVersionChanged(QString value);
    void TmapNaviStatusChanged(int value);
    void TmapRGDataInfoChanged(RGDataPtr value);
    void TmapSatelliteInfoChanged(SatelliteDataPtr value);
    void TmapViewLevelChanged(ViewLevelInfo value);
    void TmapMapMoveModeChanged(int value);
    void TmapViewModeChanged(int value);
    void TmapVoiceGuidanceTypeChanged(int value);
    void TmapOverSpeedAlertActiveChanged(bool value);
    void TmapVolumeChanged(int value);
    void TmapIsMuteChanged(bool value);
    void TmapWorkLocationChanged(TS_ROUTE_POS value);
    void TmapHomeLocationChanged(TS_ROUTE_POS value);
    void TmapWaypointListChanged(QRoutePosListPtr value);
    void TmapWaypointOperationStatusChanged(const QList<int> statList); // INSERTED, DELETED, [MOVED], REPLACED
    void TmapRouteCalculateDurationChanged(int duration); // msec
    // void TmapDestinationReachedStatusChanged(int status); // UNKNOWN, WAYPOINT, FINAL_DESTINATION, [WAYPOINT_SKIPPED]
    void TmapActiveDestinationInfoChanged(QVariant value);

    void TmapFavoriteChanged(bool value);
    void TmapRecentDestinationChanged(bool value);
    void TmapSearchWordChanged(bool value);
    void TmapUserProfileChanged(int value);
    void TmapRouteOptionChanged(int value);
    void TmapRouteRecalculationReasonChanged(int value);
    void TmapNightModeChanged(bool value);

    void TmapScreenIDChanged(int TmapScreenID);

public slots:
    virtual void onTmapRequestProperty_All() = 0;
    virtual void setTmapScreenID(int TmapScreenID) = 0;
};

}

Q_DECLARE_METATYPE(SKT::QRoutePosListPtr)
Q_DECLARE_METATYPE(SKT::QPOICateInfoListPtr)
Q_DECLARE_METATYPE(SKT::QSearchWordListPtr)

Q_DECLARE_INTERFACE(SKT::NavigationProperty, "com.skt.tmap.NavigationProperty/1.0")

#endif // NAVIGATIONPROPERTY_H
