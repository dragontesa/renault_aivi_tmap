#ifndef TMAPPROPERTY_H
#define TMAPPROPERTY_H

#include "navigationproperty.h"
#include <QReadWriteLock>
namespace SKT {

class TmapController;

class TmapProperty : public QObject, public NavigationProperty
{
    Q_OBJECT
    Q_INTERFACES(SKT::NavigationProperty)
    Q_PROPERTY(QString TmapAppVersion READ getTmapAppVersion CONSTANT)
    Q_PROPERTY(QString TmapMapVersion READ getTmapMapVersion WRITE setTmapMapVersion NOTIFY TmapMapVersionChanged )
    Q_PROPERTY(QString TmapSDIVersion READ getTmapSDIVersion WRITE setTmapSDIVersion NOTIFY TmapSDIVersionChanged )
    Q_PROPERTY(QString TmapLocalMapVersion READ getTmapLocalMapVersion WRITE setTmapLocalMapVersion NOTIFY TmapLocalMapVersionChanged)
    Q_PROPERTY(QString TmapLocalNetworkVersion READ getTmapLocalNetworkVersion WRITE setTmapLocalNetworkVersion NOTIFY TmapLocalNetworkVersionChanged)
    Q_PROPERTY(QString TmapLocalPoiVersion READ getTmapLocalPoiVersion WRITE setTmapLocalPoiVersion NOTIFY TmapLocalPoiVersionChanged)
    Q_PROPERTY(int TmapNaviStartStatus READ getTmapNaviStartStatus WRITE setTmapNaviStartStatus NOTIFY TmapNaviStartStatusChanged )
    Q_PROPERTY(int TmapNaviStatus READ getTmapNaviStatus WRITE setTmapNaviStatus NOTIFY TmapNaviStatusChanged )
    Q_PROPERTY(RGDataPtr TmapRGDataInfo READ getTmapRGDataInfo WRITE setTmapRGDataInfo NOTIFY TmapRGDataInfoChanged )
    Q_PROPERTY(SatelliteDataPtr TmapSatelliteInfo READ getTmapSatelliteInfo WRITE setTmapSatelliteInfo NOTIFY TmapSatelliteInfoChanged )
    Q_PROPERTY(ViewLevelInfo TmapViewLevel READ getTmapViewLevel WRITE setTmapViewLevel NOTIFY TmapViewLevelChanged )
    Q_PROPERTY(int TmapMapMoveMode READ getTmapMapMoveMode WRITE setTmapMapMoveMode NOTIFY TmapMapMoveModeChanged )
    Q_PROPERTY(int TmapViewMode READ getTmapViewMode WRITE setTmapViewMode NOTIFY TmapViewModeChanged )
    Q_PROPERTY(int TmapVoiceGuidanceType READ getTmapVoiceGuidanceType WRITE setTmapVoiceGuidanceType NOTIFY TmapVoiceGuidanceTypeChanged )
    Q_PROPERTY(bool TmapOverSpeedAlertActive READ getTmapOverSpeedAlertActive WRITE setTmapOverSpeedAlertActive NOTIFY TmapOverSpeedAlertActiveChanged )
    Q_PROPERTY(int TmapVolume READ getTmapVolume WRITE setTmapVolume NOTIFY TmapVolumeChanged )
    Q_PROPERTY(bool TmapIsMute READ getTmapIsMute WRITE setTmapIsMute NOTIFY TmapIsMuteChanged )
    Q_PROPERTY(TS_ROUTE_POS TmapWorkLocation READ getTmapWorkLocation WRITE setTmapWorkLocation NOTIFY TmapWorkLocationChanged )
    Q_PROPERTY(TS_ROUTE_POS TmapHomeLocation READ getTmapHomeLocation WRITE setTmapHomeLocation NOTIFY TmapHomeLocationChanged )
    Q_PROPERTY(QRoutePosListPtr TmapWaypointList READ getTmapWaypointList WRITE setTmapWaypointList NOTIFY TmapWaypointListChanged )
    Q_PROPERTY(QList<int> WaypointOperationStatusList READ getWaypointOperationStatusList WRITE setWaypointOperationStatusList NOTIFY TmapWaypointOperationStatusChanged )
    Q_PROPERTY(int RouteCalculateDuration READ getRouteCalculateDuration WRITE setRouteCalculateDuration NOTIFY TmapRouteCalculateDurationChanged )
    // Q_PROPERTY(int DestinationReachedStatus READ getDestinationReachedStatus WRITE setDestinationReachedStatus NOTIFY TmapDestinationStatusChanged )
    Q_PROPERTY(QRoutePosListPtr TmapActiveDestinationInfo READ getTmapActiveDestinationInfo WRITE setTmapActiveDestinationInfo NOTIFY TmapActiveDestinationInfoChanged )
    Q_PROPERTY(bool TmapFavoriteChange READ getTmapFavoriteChange WRITE setTmapFavoriteChange NOTIFY TmapFavoriteChanged )
    Q_PROPERTY(bool TmapRecentDestinationChange READ getTmapRecentDestinationChange WRITE setTmapRecentDestinationChange NOTIFY TmapRecentDestinationChanged )
    Q_PROPERTY(bool TmapSearchWordChange READ getTmapSearchWordChange WRITE setTmapSearchWordChange NOTIFY TmapSearchWordChanged )
    Q_PROPERTY(bool TmapUserProfileChange READ getTmapUserProfileChange WRITE setTmapUserProfileChange NOTIFY TmapUserProfileChanged )
    Q_PROPERTY(bool TmapRouteOptionChange READ getTmapRouteOptionChange WRITE setTmapRouteOptionChange NOTIFY TmapRouteOptionChanged )
    Q_PROPERTY(bool TmapNightModeChange READ getTmapNightModeChange WRITE setTmapNightModeChange NOTIFY TmapNightModeChanged )
    Q_PROPERTY(int TmapScreenID READ TmapScreenID WRITE setTmapScreenID NOTIFY TmapScreenIDChanged)

public:
    explicit TmapProperty(QObject *parent);
    virtual ~TmapProperty();

    QObject* qobject() override { return this; }
    QString getTmapAppVersion() const override;

    QString getTmapMapVersion() const override;
    void setTmapMapVersion(const QString &value) override;

    QString getTmapSDIVersion() const override;
    void setTmapSDIVersion(const QString &value) override;

    QString getTmapLocalMapVersion() const;
    void setTmapLocalMapVersion(const QString &value);
    QString getTmapLocalNetworkVersion() const;
    void setTmapLocalNetworkVersion(const QString &value);
    QString getTmapLocalPoiVersion() const;
    void setTmapLocalPoiVersion(const QString &value);

    /** navi start status
     *  0: failed to start navigation
     *  1: The Client of NavigationExtService ready to communication and handle the data from system
     *  2: Third party navigation ready and wrapper can get information from navigation, such as dataset/software version, etc...
     *  3: Third party navigation stop and ready to power off.
    */
    int getTmapNaviStartStatus() override;
    void setTmapNaviStartStatus(int value) override;

    int getTmapNaviStatus() override;
    void setTmapNaviStatus(int value) override;

    RGDataPtr getTmapRGDataInfo() override;
    void setTmapRGDataInfo(RGDataPtr value) override;

    SatelliteDataPtr getTmapSatelliteInfo() override;
    void setTmapSatelliteInfo(SatelliteDataPtr value) override;

    ViewLevelInfo getTmapViewLevel() override;
    void setTmapViewLevel(ViewLevelInfo value) override;

    int getTmapMapMoveMode() override;
    void setTmapMapMoveMode(int value) override;

    int getTmapViewMode() override;
    void setTmapViewMode(int value) override;

    int getTmapVoiceGuidanceType() override;
    void setTmapVoiceGuidanceType(int value) override;
    
    bool getTmapOverSpeedAlertActive() override;
    void setTmapOverSpeedAlertActive(bool value) override;

    int getTmapVolume() override;
    void setTmapVolume(int value) override;

    bool getTmapIsMute() override;
    void setTmapIsMute(bool value) override;

    TS_ROUTE_POS getTmapWorkLocation() override;
    void setTmapWorkLocation(TS_ROUTE_POS value) override;

    TS_ROUTE_POS getTmapHomeLocation() override;
    void setTmapHomeLocation(TS_ROUTE_POS value) override;

    QRoutePosListPtr getTmapWaypointList() override;
    void setTmapWaypointList(QRoutePosListPtr value) override;

    QRoutePosListPtr getTmapActiveDestinationInfo() override;
    void setTmapActiveDestinationInfo(QRoutePosListPtr value) override;

    bool getTmapFavoriteChange() override;
    void setTmapFavoriteChange(bool value) override;

    bool getTmapRecentDestinationChange() override;
    void setTmapRecentDestinationChange(bool value) override;

    bool getTmapSearchWordChange() override;
    void setTmapSearchWordChange(bool value) override;

    bool getTmapUserProfileChange() override;
    void setTmapUserProfileChange(int value) override;

    bool getTmapRouteOptionChange() override;
    void setTmapRouteOptionChange(int value) override;

    bool getTmapNightModeChange() override;
    void setTmapNightModeChange(bool value) override;

    void setFavoriteList(QRoutePosListPtr value) override;
    QRoutePosListPtr getFavoriteList() override;
    void setRecentDestinationList(QRoutePosListPtr value) override;
    QRoutePosListPtr getRecentDestinationList() override;
    void setSearchWordList(QSearchWordListPtr value) override;
    QSearchWordListPtr getSearchWordList() override;

    int getRouteRecalculationReason() const override;
    void setRouteRecalculationReason(int reason) override;

    // int getDestinationReachedStatus() const override;
    // void setDestinationReachedStatus(int status);
    QList<int> getWaypointOperationStatusList() const override;
    void setWaypointOperationStatusList(const QList<int>& status);

    int getRouteCalculateDuration() const;
    void setRouteCalculateDuration(int duration);

    virtual int TmapScreenID();

    void setErrorMap(int error)  override;
    int  getErrorMap() const override;


signals:
    void TmapMapVersionChanged(QString value);
    void TmapSDIVersionChanged(QString value);
    void TmapLocalMapVersionChanged(QString value);
    void TmapLocalNetworkVersionChanged(QString value);
    void TmapLocalPoiVersionChanged(QString value);
    void TmapNaviStartStatusChanged(int value);
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
    void TmapWaypointListChanged(QVariant value);
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
    void onTmapRequestProperty_All() override;
    virtual void setTmapScreenID(int TmapScreenID);

private:
    Q_DISABLE_COPY(TmapProperty)
    TmapController * controller;

private:
    QReadWriteLock mRwLocker;
    QString mTmapMapVersion;
    QString mTmapSDIVersion;
    QString mTmapLocalMapVersion;
    QString mTmapLocalNetworkVersion;
    QString mTmapLocalPoiVersion;
    int mTmapNaviStartStatus;
    int mTmapNaviStatus;
    RGDataPtr mTmapRGDataInfo;
    SatelliteDataPtr mTmapSatelliteInfo;
    ViewLevelInfo mTmapViewLevel;
    int mTmapMapMoveMode;
    int mTmapViewMode;
    int mTmapVoiceGuidanceType;
    int mTmapVolume;
    bool mTmapOverSpeedAlertActive;
    bool mTmapIsMute;
    TS_ROUTE_POS mTmapWorkLocation;
    TS_ROUTE_POS mTmapHomeLocation;
    QRoutePosListPtr mTmapWaypointList;
    QRoutePosListPtr mTmapActiveDestinationInfo;
    QRoutePosListPtr mTmapFavoriteList;
    QRoutePosListPtr mTmapRecentDestinationList;
    QSearchWordListPtr mTmapSearchWordList;
    QList<int> mTmapWaypointOperationStatusList;
    int mDestinationReachedStatus;
    int mUserProfileID;
    int mRouteOption;
    int mRouteRecalculationReason;
    int mRouteCalculateDuration;
    bool mNightMode;
    int m_TmapScreenID;
    int mError;
};

}

#endif // TMAPPROPERTY_H
