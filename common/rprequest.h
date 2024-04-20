#ifndef RPREQUEST_H
#define RPREQUEST_H

#include <QObject>
#include <QVariantMap>
#include <QSharedPointer>
#include <QPoint>

#include "MNS_Define.h"
#include "NDDS_Define.h"

namespace SKT {

// Request Key
#define RPREQ_KEY_RequestType "RequestType"
#define RPREQ_KEY_Online "isOnline"
#define RPREQ_KEY_State "State"
#define RPREQ_KEY_RPOptions "RPOptions"
#define RPREQ_KEY_RPCalcOption "RPCalcOption"
#define RPREQ_KEY_TvasFileName "TvasFileName"
#define RPREQ_KEY_VMSResID "VMSResID"
#define RPREQ_KEY_TvasVersion "TvasVer"
#define RPREQ_KEY_Hipass "Hipass"
#define RPREQ_KEY_RequestId "RequestId"
#define RPREQ_KEY_PrimaryServer "PrimaryServer"
#define RPREQ_KEY_DepartureReplaced "DepartureReplaced"
#define RPREQ_KEY_StartSearchCode "StartSearchCode"
#define RPREQ_KEY_DestSearchCode "DestSearchCode"
#define RPREQ_KEY_LastTollgateId "LastTollgateId"
#define RPREQ_KEY_PreTollgateIds "PreTollgateIds"
#define RPREQ_KEY_RadiusInfoType "RadiusInfoType"
#define RPREQ_KEY_RadiusEntryTurnType "RadiusEntryTurnType"
#define RPREQ_KEY_RadiusLinkId "RadiusLinkId"
#define RPREQ_KEY_RadiusMeshCode "RadiusMeshCode"
#define RPREQ_KEY_RadiusLinkDirection "RadiusLinkDirection"
#define RPREQ_KEY_RadiusLeaveTurnType "RadiusLeaveTurnType"
#define RPREQ_KEY_RadiusXPos "RadiusXPos"
#define RPREQ_KEY_RadiusYPos "RadiusYPos"
#define RPREQ_KEY_LastRid "LastRid"
#define RPREQ_KEY_PreMapVersion "PreMapVersion"
#define RPREQ_KEY_ReRids "ReRids"
#define RPREQ_KEY_PreSecs "PreSecs"
#define RPREQ_KEY_InitSrchLength "InitSrchLength"
#define RPREQ_KEY_InitSrchSessionId "InitSrchSessionId"
#define RPREQ_KEY_CongestRid "congestRid"
#define RPREQ_KEY_GpsTraceData "gpsTraceData"
#define RPREQ_KEY_LastGoalSessionId "lastGoalSessionId"
#define RPREQ_KEY_LastGoalTime "lastGoalTime"
#define RPREQ_KEY_LastGoalPos "lastGoalPos"
#define RPREQ_KEY_RD4_Filename "RD4"
#define RPREQ_KEY_Angle "Angle"
#define RPREQ_KEY_Speed "Speed"

// Response Key
#define RPREQ_KEY_Response "Response"
#define RPREQ_KEY_Answer "Answer" // will delete
#define RPREQ_KEY_AnswerCode "AnswerCode"
#define RPREQ_KEY_ErrorCode "ErrorCode"
#define RPREQ_KEY_WeatherInfo "WeatherInfo"
#define RPREQ_KEY_RouteSummaryInfo "RouteSummaryInfo"
#define RPREQ_KEY_VMSGuideInfo "VMSGuideInfo"
#define RPREQ_KEY_SwitchToLocal "SwitchToLocal"
#define RPREQ_KEY_SessionId "SessionId"
#define RPREQ_KEY_ServerUrgentMode "ServerUrgentMode"
#define RPREQ_KEY_RerouteForSDI "RerouteForSDI"

// Answer Value  // will change Numeric type
#define RPREQ_Error_Success "Done"
#define RPREQ_Error_Cancel "Cancel"
#define RPREQ_Error_NotImplement "Not Implement"
#define RPREQ_Error_NotSupport "Not Support"
#define RPREQ_Error_NotReady "Not Ready"
// ErroCode Value
#define RPREQUEST_NULL_VALUE 404
// MNS_ErrorCode

#define RPREQ_KEY_Command      QStringLiteral("Command")
#define RPREQ_KEY_ByVR         QStringLiteral("byVr")
#define REREQ_KEY_ByProfileChange QStringLiteral("byProfileChange")
#define RPREQ_KEY_Apply        QStringLiteral("apply")

class RPRequest;

typedef QSharedPointer<TS_ROUTE_POS> RoutePosPtr;
typedef QSharedPointer<QVariantMap> QVariantMapPtr;
typedef QSharedPointer<QStringList> QStringListPtr;
typedef QSharedPointer<QList<TeRouteOption>> QRouteOptionListPtr;
typedef QSharedPointer<DS_WEATHERINFO_RES> QWeatherInfo;
typedef QSharedPointer<DS_VMSGUIDEINFO> QVMSGuideInfoPtr;

struct RouteSummaryInfo{
    int		nTotalDist;			// 총 거리(단위, m)
    int		nTotalTime;			// 총 소요 시간(단위, 초)
};
typedef QSharedPointer<RouteSummaryInfo> QRouteSummaryInfo;

class RPRequest
{
public:
    enum RequestType{
        RequestNone = -1,
        RequestCancel,
        // Route Planing
        RequestRoute,
        RequestRouteSummary,
        RequestReRouteForce,
        RequestReRouteByAuto,
        RequestReRouteByBreakaway,
        // Route Guidance Function
        RequestVMSInfo,
    };

    enum RequestServerType{
        RequestServerNone = -1,
        RequestServer_Local,
        RequestServer_Server,
    };

    enum RequestState{
        RequestState_Invalid_Cancel = -1,
        RequestState_Valid
    };

    enum ResponseAnswer{
        ResponseAnswer_Done,
        ResponseAnswer_Cancel,
        ResponseAnswer_NotImplement,
        ResponseAnswer_NotSupport,
        ResponseAnswer_NotReady
    };

    RPRequest():req(new QVariantMap()){}
    RPRequest(QVariantMapPtr req):req(req){}
    ~RPRequest(){
    }
    QVariantMapPtr getReq(){return req;}
    QVariantMapPtr getRes(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_Response].isNull() != true){
            return m[RPREQ_KEY_Response].value<QVariantMapPtr>();
        }
        return QVariantMapPtr(NULL);
    }

    int getResponseErrorCode(){
        QVariantMap& m = *(getRes());
        if(m[RPREQ_KEY_ErrorCode].isNull() == false)
            return (int)(m[RPREQ_KEY_ErrorCode].value<MNS_ErrorCode>());
        return RPREQUEST_NULL_VALUE;
    }

    QString getResponseAnswer(){ // will delete
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_Answer].isNull() == false)
            return m[RPREQ_KEY_Answer].value<QString>();
        return RPREQ_Error_NotReady;
    }

    int getResponseAnswerCode(){
        QVariantMap& m = *(getRes());
        if(m[RPREQ_KEY_AnswerCode].isNull() == false)
            return m[RPREQ_KEY_AnswerCode].value<int>();
        return RPREQUEST_NULL_VALUE;
    }

    RPRequest& setRequestType(int type){
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_RequestType,QVariant::fromValue(type));
        return *this;
    }
    int getRequestType(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_RequestType].isNull() == false)
            return m[RPREQ_KEY_RequestType].toInt();
        return RequestNone;
    }

    RPRequest& setOnline(RequestServerType online){
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_Online,QVariant::fromValue((int)online));
        return *this;
    }
    RequestServerType getOnline(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_Online].isNull() == false)
            return (RequestServerType)m[RPREQ_KEY_Online].toInt();
        return RequestServerNone;
    }

    RPRequest& setHipass(bool hipass){
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_Hipass,QVariant::fromValue(hipass));
        return *this;
    }
    bool getHipass(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_Hipass].isNull() == false)
            return (RequestServerType)m[RPREQ_KEY_Hipass].toBool();
        return false;
    }
    bool getDepartureReplaced(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_DepartureReplaced].isNull() == false)
            return (RequestServerType)m[RPREQ_KEY_DepartureReplaced].toBool();
        return false;
    }
    int getStartSearchCode() {
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_StartSearchCode].isNull() == false)
            return m[RPREQ_KEY_StartSearchCode].toInt();
        return 0x0;
    }
    int getDestSearchCode() {
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_DestSearchCode].isNull() == false)
            return m[RPREQ_KEY_DestSearchCode].toInt();
        return 0xFF;
    }
    int getLastTollgateId() {
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_LastTollgateId].isNull() == false)
            return m[RPREQ_KEY_LastTollgateId].toInt();
        return 0;
    }
    QString getLastGoalSessionId() {
        QVariantMap& m = *(req.data());
        QString sessionId;
        if (m.contains(RPREQ_KEY_LastGoalSessionId)) {
            sessionId = m[RPREQ_KEY_LastGoalSessionId].toString();
        }
        return sessionId;
    }
    QPoint getLastGoalPos() {
        QVariantMap& m = *(req.data());
        QPoint goal;
        if (m.contains(RPREQ_KEY_LastGoalPos)) {
            goal = m[RPREQ_KEY_LastGoalPos].toPoint();
        }
        return goal;
    }
    QString getLastGoalTime() {
        QVariantMap& m = *(req.data());
        QString datetime;
        if (m.contains(RPREQ_KEY_LastGoalTime)) {
            datetime = m[RPREQ_KEY_LastGoalTime].toString();
        }
        return datetime;
    }

    RPRequest& setVia(int id, RoutePosPtr via){
        if(via.isNull() == true) return *this;;
        QVariantMap& m = *(req.data());
        QString ids = QString::asprintf("Via%d",id);
        m.insert(ids,QVariant::fromValue(via));
        return *this;
    }
    RoutePosPtr getVia(int id){
        QVariantMap& m = *(req.data());
        QString ids = QString::asprintf("Via%d",id);
        if(m[ids].isNull() == false)
            return m[ids].value<RoutePosPtr>();
        return RoutePosPtr(NULL);
    }

    RPRequest& setRPOptions(QRouteOptionListPtr op){
        if(op.isNull() == true) return *this;;
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_RPOptions,QVariant::fromValue(op));
        return *this;
    }
    QRouteOptionListPtr getRPOptions(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_RPOptions].isNull() == false)
            return m[RPREQ_KEY_RPOptions].value<QRouteOptionListPtr>();
        return QRouteOptionListPtr(NULL);
    }

    RPRequest& setRPCalcOption(bool option){
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_RPCalcOption,QVariant::fromValue(option));
        return *this;
    }
    bool getRPCalcOption(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_RPCalcOption].isNull() == false)
            return m[RPREQ_KEY_RPCalcOption].value<bool>();
        return false;
    }

    RPRequest& setTvasFileName(QStringListPtr tvass){
        if(tvass.isNull() == true) return *this;;
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_TvasFileName,QVariant::fromValue(tvass));
        return *this;
    }
    QStringListPtr getTvasFileName(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_TvasFileName].isNull() == false)
            return m[RPREQ_KEY_TvasFileName].value<QStringListPtr>();
        return QStringListPtr(NULL);
    }

    QWeatherInfo getWeatherInfo(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_WeatherInfo].isNull() == false)
            return m[RPREQ_KEY_WeatherInfo].value<QWeatherInfo>();
        return QWeatherInfo(NULL);
    }

    QRouteSummaryInfo getRouteSummaryInfo(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_RouteSummaryInfo].isNull() == false)
            return m[RPREQ_KEY_RouteSummaryInfo].value<QRouteSummaryInfo>();
        return QRouteSummaryInfo(NULL);
    }

    int getVMSResID(){
        QVariantMap& m = *(req.data());
        if(m[RPREQ_KEY_VMSResID].isNull() == false)
            return m[RPREQ_KEY_VMSResID].value<int>();
        return -1;
    }

    RPRequest& setVMSResID(int resID){
        if(resID < 0) return *this;
        QVariantMap& m = *(req.data());
        m.insert(RPREQ_KEY_VMSResID,QVariant::fromValue(resID));
        return *this;
    }

    QVMSGuideInfoPtr getVMSGuideInfo(){
        QVariantMap& m = *(getRes());
        if(m[RPREQ_KEY_VMSGuideInfo].isNull() == false)
            return m[RPREQ_KEY_VMSGuideInfo].value<QVMSGuideInfoPtr>();
        return QVMSGuideInfoPtr(NULL);
    }

    QString getTvasVersion() {
        QVariantMap& m = *(getReq());
        if (m.contains(RPREQ_KEY_TvasVersion)) {
            return m[RPREQ_KEY_TvasVersion].toString();
        }
        return QString();
    }

private:
    QVariantMapPtr req;
};

typedef QSharedPointer<RPRequest> RPRequestPtr;

}

Q_DECLARE_METATYPE(SKT::RoutePosPtr)
Q_DECLARE_METATYPE(SKT::QVariantMapPtr)
Q_DECLARE_METATYPE(SKT::QStringListPtr)
Q_DECLARE_METATYPE(SKT::QRouteOptionListPtr)
Q_DECLARE_METATYPE(MNS_ErrorCode)
Q_DECLARE_METATYPE(SKT::QWeatherInfo)
Q_DECLARE_METATYPE(SKT::QRouteSummaryInfo)
Q_DECLARE_METATYPE(SKT::QVMSGuideInfoPtr)

#endif // RPREQUEST_H
