#ifndef ROUTEMANAGER_H
#define ROUTEMANAGER_H

#include <QObject>
#include <vector>
#include "MNS_Define.h"
#include "MarkerManager.h"

namespace SKT {

class AbstractMediator;

enum RouteItemType {
    RouteItem_Start = 0,
    RouteItem_Via1,
    RouteItem_Via2,
    RouteItem_Via3,
    RouteItem_End,

    RouteItem_Num,
    RouteItem_AddVia
};
enum CheckViaResult {
    CheckViaResultOk = 0,
    CheckViaResultFail_DepartuerAndVia,     // 출발지, 경유지 동일
    CheckViaResultFail_DepartureAndArrival, // 출발지, 도착지 동일
    CheckViaResultFail_Via,                 // 경유지 동일
    CheckViaResultFail_ViaAndArrival,       // 경유지, 도착지 동일일
    CheckViaResultFail_All,                 // 출발지, 경유지, 도착지 동일
};

class RouteManager : public QObject
{
    Q_OBJECT

public:
    static RouteManager* sharedInstance();
    //17.12.11 hskim not used
    //static void destroyInstance();

    struct RouteItem {
        int wx = 0;
        int wy = 0;
        int poiId = 0;
        int poleCate = 0;

        unsigned char rpFlag = 0; //Unknown
        int navSeq = 0;
        bool dirty = false;
        int status = 1; // DELETED
        int searchFlag = 0xff;

        QString name;
        QString address;
        QPoint toPoint() const {
            return QPoint(wx, wy);
        }
        void reset() {
            wx = 0;
            wy = 0;
            poiId = -1;
            poleCate = -1;

            rpFlag = 0x63;
            navSeq = 0;
            dirty = false;
            name.clear();
            address.clear();
            status = 1;
            searchFlag = 0xff;
        }
        bool operator==(const RouteItem &other) const;
        RouteItem &operator=(const RouteItem &other);

        void setTsRoutePos(TS_ROUTE_POS* p);
        void set(TS_ROUTE_POS &pos);
        bool isNull() const;
    };

    RouteManager();

    void initialize(QObject *addressManager);
    void clear();

    // 17.12.8 hskim apply multi map view
    void attachMapView(QObject* mapView);
    void detachMapView(QObject* mapView);

    bool hasVia() const;
    bool hasVia(int index) const;
    bool hasEmptyVia() const;
    int addVia(const RouteItem& item);
    void backup();
    void restore();
    void restoreFromStringList(const QStringList& sl, bool skipDeparture);

    int moveDown(int index);
    int moveUp(int index);
    bool remove(int index);

    QString startName() const;
    QString endName() const;
    QString viaName(int index) const;
    QString name(RouteItemType type) const;
    QString targetAddress() const;
    QPoint worldPos(RouteItemType type) const;
    QPoint targetWorldPos() const;

    void requestAddress(int wx, int wy);
    RouteItem getItem(RouteItemType type) const;
    void setItem(RouteItemType type, const RouteItem &item);
    bool insertItem(RouteItemType type, const RouteItem &item, bool overwriteIfItemsFull=false);

    RouteItem targetItem() const;

    void addViaMarker(QObject* mapView, int id, const QPoint &pos);
    void removeViaMarker(QObject* mapView, int id);
    void clearViaMarker(QObject* mapView);

    void addSearchMarker(QObject* mapView, int id, int focusIdx, const QString &text, int x, int y);
    void removeSearchMarker(QObject* mapView, int id, int x, int y);

    QStringList getViaStringList() const;
    bool checkVia(int type, const QPoint& currentPos);
    CheckViaResult checkDepartureAndViaDistance(const QPoint& currentPos); // 출발지-나머지 검사
    bool checkViaAndArrivalDistance() const;    // 경유지-도착지 검사
    bool checkViaDistance() const;  // 경유지들만 검사
    void relocation();
    bool canInsert(int slot) const;

signals:
    void updated(int index);
    void updated(int wx, int wy, QString address);
    void removeMarker(int index);

private slots:
    void updateAddress(int wx, int wy, QString address);
    void routeRequested(TS_POINT*);

private:
    bool isValidPosition(int wx, int wy) const;
    void swap(int index0, int index1);

    RouteItem mItems[RouteItem_Num];
    RouteItem mBackupItems[RouteItem_Num];
    QObject *addressManager;
    //QObject *markerManager;
    MarkerManager mMarkerManager;
    static RouteManager *sInstance;
};

class RouteRequestAddress
{
public:
    RouteRequestAddress(QObject *receiver = NULL);
    void operator()(int wx, int wy);

private:
    QObject *mReceiver;
};

void strcpyNavSeqFromInt(char *szNavSeq, int navSeq);

} // SKT

#define gRouteManager   SKT::RouteManager::sharedInstance()

#endif // ROUTEMANAGER_H
