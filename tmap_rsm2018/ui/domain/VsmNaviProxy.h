#ifndef VSMNAVIPROXY_H
#define VSMNAVIPROXY_H

#include "AbstractProxy.h"
#include <QMap>
#include <QList>
#include <vector>
#include "RouteSummaryVo.h"

#include "../common/navigationcontroller.h"

struct VSM_stTmapPosInfo;

namespace SKT
{

T_SHAREABLE(VsmNaviProxy);
T_SHAREABLE(SettingDto); // to setup cavatar

class VsmNaviProxy : public AbstractProxy
{
    Q_OBJECT
public:
    VsmNaviProxy(QObject* controller);

    void setGlobalMapView(QObject* mapView);

    // 17.12.8 hsim apply multi map view
    void attachMapView( QObject* mapView, VSM_ViewType type = VSM_VIEW_TYPE_NORMAL );
    void detachMapView( QObject* mapView );

    //QPoint centerPosition() const;
    QPoint locationPoint() const;
    QPoint currentPosition() const;
    int currentSpeed() const;

    void applySelectRoute(QObject* mapView, quint32 routeId);
    void cancelRoute();
    void cacheRemainDistance(int remainDistance);
    void cacheSummary(const QVariant &v);
    void fillCachedSummary(QVariantMap &m);
    void cacheWeather(const QVariant &);
    void fillCachedWeather(QVariantMap &);
    void setDrawGasStationInfo(QObject* mapView);
    bool setTvasData(const QStringList &filenames, const QList<quint32> &options, bool islocal = false);
    bool setTvasDataWithColorTable(const QStringList &filenames, const QList<quint32> &options, const QStringList &colors, bool isLocal = false);
    bool setTvasDataRerouted(bool isLocal = false, int routeOption = 1);
    void selectRoute(int index);
    int getSelectedRoute() const;
    bool resumeRoute(bool isLocal = false);
    bool isSetTvas() const;

    void blockReroute(bool block);

    void setDrawRoute(bool draw);
    void drawRouteAll(bool visible=true);

    void setAutoZoom(bool b);
    void setFuelType(int fuel);
    int fuelType() const;
    void setChargeType(int type);
    int chargeType() const;

    QVariant summary(int routeId, const QString &description = QString());
    QVariant getDetailRoute(int routeId);
    QVariant trafficGuidance() const;
    QVariant getGasStationInfo() const;
    QVariant getEStationInfo() const;
    void SetUserOilType(QObject* mapView);

    void soundPlayScenario(int sid);
    void soundPlayVolume();

    void updateCavatarIcon(QObject* mapView);
    void setCavatarIcon(QObject* mapView, int type);

    void showGasStationInfo(QObject* mapView, bool show);

    void setTvasVersion(int ver);
    bool isRoute() const;
    bool isSoundPlaying() const;
    void enableSimulatorStopBySpeed(bool b);

    void setChannelPolicyReceived(bool received);

private slots:
    void onlineChanged(bool online);

private:
    QObject* chooseMapView(QObject* mapView) const;
    void* chooseMapViewHandle(QObject* mapView) const;

    void registPos(std::vector<VSM_stTmapPosInfo> &posInfo);
    bool checkValidPos(int wx, int wy);
    void initTurnTable();
    void setupCavatar(void* mapViewHandle, int userCavatar, const QList<SettingDtoPtr>* cavataSet);    

private:
    bool mOnline;
    bool mChannelPolicyReceived = false;
    // Route summary cache data    
    int mSelectedRoute;
    int mDistance;
    int mFee;
    int mRemainTime;
    int mRemainDistance = 0;
    int mTvasVer; // 6: 5.0, 7:5.1, 8:5.2, 9: 5.3, it is only used to be for server 2018.4.12
    QString mSkyCode;
    int mTemp;
    int mFuelType;
    int mChargeType = 1; // 1,2,3
    NavigationController* mController;
    QString mTtsPath;

    std::vector<VSM_UInt32> mGasStationInfo;
    VSM_UInt32 mLowestPricePoiID;
    QObject* mGlobalMapView = nullptr;
    void* mGlobalMapViewHandle = nullptr;
    bool mIsSetTvas = false;
    bool mEnableStopSimulatorBySpeed = true;
};

}

#endif // VSMNAVIPROXY_H
