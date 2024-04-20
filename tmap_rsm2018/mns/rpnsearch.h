#ifndef RPNSEARCH_H
#define RPNSEARCH_H

#include <QObject>
#include <QVariantMap>
#include <QMutex>
#include "MNS_Interface.h"
#include "NDDS_Interface.h"
#include "rpmanagerinterface.h"
#include "tvasparser.h"

#define APPLY_DUAL_CHANNEL    // 듀얼 채널 적용

namespace SKT {

class RPnSearchServer;
class TmapController;

typedef struct tagResult
{
    QByteArray* resultData;
    void* dataAuto;
    int resultCnt;
}T_RESULT;

class RequestExecutor;

class RPReqObject : public QObject, public RPRequestHandle
{
    Q_OBJECT
    Q_INTERFACES(SKT::RPRequestHandle)
public:
    RPReqObject(RPRequestPtr req, QObject *parent = nullptr) : QObject(parent), m_req(req) {}
    virtual ~RPReqObject() {}

    RPRequestPtr request() override { return m_req; }
    QObject* qobject() override { return this; }

signals:
    void finished();

private:
    RPRequestPtr m_req;
};

class RPnSearch : public QObject, public RPManagerInterface
{
public :
    enum RpOption {
        RpOptionOPTIMUM		= 0x0001,	///< 추천 경로 우선
        RpOptionEXPRESSWAY  = 0x0002,	///< 고속도로 우선
        RpOptionFREEWAY		= 0x0004,	///< 일반도로 우선
        RpOptionSHORTESTWAY	= 0x0008,	///< 최단 경로 우선

        //will deprecate
        RpOptionPosibleAll = RpOptionOPTIMUM
            | RpOptionEXPRESSWAY
            | RpOptionFREEWAY
            | RpOptionSHORTESTWAY,
    };

    enum RpPositionType {
        RpPositionScreen = 1,
        RpPositionSelectedPOI,
    };

    enum RpRouteItemType {
        RpRouteNONE = -2,
        RpRouteALL = RpRouteNONE,
        RpRouteAdd,
        RpRouteDeparture,
        RpRouteDestination,
        RpRouteVia1,
        RpRouteVia2,
        RpRouteVia3,
    };

    Q_OBJECT
    Q_INTERFACES(SKT::RPManagerInterface)
public:
    explicit RPnSearch();
    virtual ~RPnSearch();
    // not thread safe
    static RPnSearch* sharedInstance(void);

    void init(
        TmapController* controller, 
        const QString &rootPath, 
        const QString& serverURL,
        int serverPort=9443,
        bool commerce=true,
        bool avnext=false);

    void destroy();

public slots:
    bool isSync() const override;

    RPRequestHandle* createRequest(RPRequestPtr req) override;
    void requestCommand(RPRequestHandle* req) override;
    void onChannelServerPolicyFinished(bool);

private slots:
    void onMapDataChanged();

public:
    // API Type ()
    void setActiveRequest(RPRequestPtr req);
    void setDeviceId(QString deviceID);

    void setVia(RpRouteItemType type, RoutePosPtr pos);
    RoutePosPtr getVia(RpRouteItemType type);

    void requestRouteSummary(RPRequestPtr& req);
    void requestRoute(RPRequestPtr& req);
    void requestReroute(RPRequestPtr& req);
    void cancelRoute(RPRequestPtr& req);
    bool canOnlineRequest(RPRequestPtr& req);
    bool isCommerceServer() const { return m_commerce; }
    bool avnextServer() const { return m_avnext; }
    bool isOutOfDistance(RPRequestPtr req);
    void setPrimaryNddsDomain(const QString &domain);

    static int callbackStatus(int nMsg, long lParam1, long lParam2, void *lpParam);
    static int callbackNddsStatus(int nMsg, int nSessionID, long lParam1, long lParam2, void *lpParam);

signals:
    void Init(bool success);
    void nddsMessageReceived(int msg, long lParam1, long lParam2, void *lpParam);

protected:
    RPRequestPtr activeRequest;
    TvasParse tvasParse;

    int positionCount;
    TS_ROUTE_POS routePosition[5];
    bool mServerPolicySuccess = false;

protected:

    void makeRouteRequest_DefaultData();
    void makeRouteRequest_Option(int option);
    void makeRouteRequest_Departure();
    void makeRouteRequest_Via1();
    void makeRouteRequest_Via2();
    void makeRouteRequest_Via3();
    void makeRouteRequest_Destination();
    void makeRouteRequest_TvasVersion(const QString& version);
    void makeRouteRequest_Hipass(bool bHipass);
    void makeRouteRequest_AngleAndSpeed(RPRequestPtr& req);
    void requestClearData();

    void makeRouteResponse_DefaultData();
    void makeRouteResponse_Data(QString tvasFileName);

    bool MakeRouteRequest_CheckPosition(TS_ROUTE_POS &data);
    void checkRouteRequestDeparture();
    void changeDeparture_currentPosition();
    bool isValidRouteRequestPosition(TS_ROUTE_POS &pos);
    void clearRouteRequestPosition(TS_ROUTE_POS & pos);

    QString primaryNddsDomain() const;
    QString secondaryNddsDomain() const;

private:
    static RPnSearch* s_instance;

    bool m_init;

    // options
    bool m_isSync;

    // server type
    bool m_commerce;
    int m_avnext;

    // helper
    RequestExecutor *executor;

    // device id
    QString m_deviceID;

    // RP
    TS_ROUTE_REQ req1_test;
    TS_ROUTE_RES res1_test;

    TmapController *m_controller;

    QMutex m_mapDataLock;
    QString m_safeDBPath;
    QString m_safeDBIndexPath;
    QString mPrimaryNddsDomain;
    QString mSecondaryNddsDomain;

    static QMutex s_mnsMutex;
    static QMutex s_nddsMutex;
};

}
#endif // RPNSEARCH_H
