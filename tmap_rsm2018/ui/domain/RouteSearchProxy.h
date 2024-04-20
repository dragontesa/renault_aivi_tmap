#ifndef ROUTESEARCHPROXY_H
#define ROUTESEARCHPROXY_H

#include "AbstractProxy.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRunnable>
#include "../common/rprequest.h"
#include "RouteManager.h"
#include "navigationproperty.h"

namespace SKT {

T_SHAREABLE( RouteSearchProxy );
T_SHAREABLE( RouteRequest );
T_SHAREABLE( RouteResponseDto );
T_SHAREABLE( RequestCommand );

//class RPnSearch;

class RouteSearchProxy : public AbstractProxy
{
    Q_OBJECT
public:
private :
    //RPnSearch *rpHelper;
public:
    RouteSearchProxy();

    void setGlobalMapView(QObject* mapView);
    // 17.12.8 hsim apply multi map view
    void attachMapView(QObject* mapView);
    void detachMapView(QObject* mapView);

    void requestCommand(const RequestCommandPtr& req);

public:
    void setOnline(bool online);
    bool isOnline() const;
    void setChannelPolicyReceived(bool received);
    bool isChannelPolicyReceived() const;
    void setHipass(bool hipass);
    bool isRpServerOnline() const;

    int addVia(const RouteManager::RouteItem &item);
    bool insertVia(const RouteManager::RouteItem &item);
    void deleteVia(int begin, int end);
    bool hasEmptyVia() const;

    void clearVia();
    void setViaSlot(int);
    int viaSlot() const;
    void setStartSearchCode(int);
    void moveDown(QObject* mapView, int index);
    void moveUp(QObject* mapView, int index);
    void removeVia(QObject* mapView, int type);
    void skipNextVia(QObject* mapView);
    void fillViaInfo(QVariantMap &v, bool skipDeparture=false);
    void fillDepartureInfo(QVariantMap &m, bool reroute=false);
    void setVia(int type, const RouteManager::RouteItem& item);
    void setViaAddress(int type, const QString &address);
    void replaceVia(int type, int x, int y);
    QString viaAddress(int type) const;
    QPoint viaWorldPos(int type) const;
    QString targetAddress() const;
    QPoint targetPos() const;
    RouteManager::RouteItem targetItem() const;
    void showVia(QObject* mapView);
    void hideVia(QObject* mapView);
    void backupVia();
    void restoreVia();
    void addSearchMarker(QObject* mapView, int id, int focusIdx, const QString& text, int x, int y);
    void removeSearchMarker(QObject* mapView, int id, int x, int y);
    void updatePoi();
    bool hasVia() const;
    bool hasVia(int index) const;

    // 시작시 경로 안내를 이어서 할 건지 표시하는 플래그.
    void setRerouteByContinue(bool byContinue);
    void setRouteOption(int routeOption);
    bool isRerouteByContinue() const;
    int routeOption() const;
    void setTvasValid(bool ok);
    bool isTvasValid() const;
    void rerouteByOption(int option);
    void showRerouteToast(int option);
    bool isOfflineOption(int option) const;

    void requestRoute(QRoutePosListPtr plist, int option, bool apply);

    void saveViaToPreference();
    void restoreViaFromPreference(const QStringList &via, bool skipDeparture=false);

    void sendWaypointProperties();
    void sendWaypointList();
    void sendWaypointOperationStatus(int status);
    void sendWaypointOperationStatus(QList<int> status);
    void sendActiveDestinationInfo();

    bool byVr() const;
    void setByVr(bool);
    bool applyTimer() const;
    void setApplyTimer(bool);

    void updateViaCurrentPosition();
    void setCurrentPosition(int x, int y);
    void setCurrentAddress(const QString&);
    void setLastUserSelectPosition(int x, int y, const QString &address);
    void insertWaypoint(int slot, const QPoint &pt);
    void insertWaypointFromSelectInMap(int slot);
    bool canInsert(int slot) const;    

    void setTvasVersion(int ver);

    CheckViaResult checkViaIsNear();
    bool isArrivalBelowDeparture() const;

    void relocationViaInfo();
    void fillRerouteData(QVariantMap &m);
    void fillLocalRerouteData(QVariantMap &m);
    void fillGpsTraceData(bool isPeriodicReroute, QVariantMap &m);
    void fillLastGoalData(QVariantMap &m);
    void fillAngleAndSpeed(QVariantMap &m);

    // NDDS 지난 위치 정보 저장. 다음 경로 탐색 시 사용.
    void setSessionId(const QString &sessionId);
    QString sessionId() const;
    void saveLastGoalData(const QString &sessionId, const QDateTime &currentTime, const QPoint goalPos = QPoint());
    void resetLastGoalData();
    void setUpdateRpDebugText(bool);

    void setRpContinue(bool);
    bool isRpContinue() const;
    void setNddsPrimaryDomain(const QString &domain);

private slots:
    void Response();

public slots:
    void sendViaInfo();
    void removeMarker(int);

private:
    void onRequestRouteFinished( const RequestCommandPtr& req);
    void swapViaMarker(QObject* mapView, int index0, int index1);

private:
    QObject* chooseMapView(QObject* mapView) const;
    void* chooseMapViewHandle(QObject* mapView) const;
    void debugLog(const RequestCommandPtr& req);
    QString getTvasVersionString(int ver) const;
    void saveRgPreference(const QString &section, const QString &key, const QVariant &value);

private:
    RouteManager mRouteManager;
    bool mIsValidTvas;
    bool mRerouteByContinue;
    bool mOnline;
    bool mChannelPolicyReceived = false;
    bool mHipass = false;
    bool mByVr;
    bool mApplyTimer;
    int mRouteOption;
    int mViaSlot;
    int mTvasVer; // 6: 5.0, 7:5.1, 8:5.2, 9: 5.3, it is only used to be for server  2018.4.12
    bool mIsDeartureAuto = true;
    int mStartSearchCode = 0x0; //gps
    bool mUpdateRpDebugText = false;
    QPoint mCurrentPosition;
    QString mCurrentAddress;
    QObject* mGlobalMapView = nullptr;
    void* mGlobalMapViewHandle = nullptr;
    QPoint mLastUserSelectPosition;
    QString mLastUserSelectAddress;
    QPoint mLastGoalPos;
    QDateTime mLastGoalTime;
    QString mLastGoalSessionId;
    QString mSessionId;
    bool mIsRpContinue = false; // 경로 이어 하기 모드
};

}

#endif // ROUTESEARCHPROXY_H
