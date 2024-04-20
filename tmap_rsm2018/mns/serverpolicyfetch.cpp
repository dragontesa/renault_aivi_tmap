#include <QtCore>
#include "serverpolicyfetch.h"
#include "rpnsearch.h"
#include "NDDS_Define.h"
#include "NDDS_Interface.h"
#include "hostnames.h"

namespace SKT {

ServerPolicyFetchCreator::ServerPolicyFetchCreator()
{
}

PolicyFetcher* ServerPolicyFetchCreator::create()
{
    PolicyFetcher* fetcher = new PolicyFetcher();
    RPnSearch* rp = RPnSearch::sharedInstance();
    QObject::connect(rp, &RPnSearch::nddsMessageReceived,
                     fetcher, &PolicyFetcher::handleNddsMessage);

    return fetcher;
}

PolicyFetcher::PolicyFetcher(QObject* parent) :
    QObject(parent),
    mFailCnt(0)
{

}

PolicyFetcher::~PolicyFetcher()
{
}

void PolicyFetcher::fetch()
{
    auto error = NDDS_AuthTmapForSmartCar("", false);
    if (error != DE_ERROR_SUCCESS) {

    }
}

void PolicyFetcher::free()
{
    disconnect();
    deleteLater();
}

void PolicyFetcher::fetchDualcenterPolicy(const QString& policy)
{
   QString domain;
   int portnum;
   if (!policy.isEmpty()) {
      QStringList items = policy.split("|");
      for (QString item : items) {
          if (item.contains("URL=")) {
              domain = item.mid(4);
          }
          else if (item.contains("SSL_PORT=")) {
              bool ok = false;
              portnum = item.mid(8).toInt(&ok);
              if (!ok)
                  portnum = -1;
          }
      }
   }
   emit fetchedDualcenterPolicy(domain,portnum);
}

void PolicyFetcher::fetchReroutePolicy(const QString& policy)
{
    emit fetchedReroutePolicy(policy);
}

void PolicyFetcher::tryFetch()
{
   mFailCnt++;
   qDebug() <<"policy, try fetch #1:"<<mFailCnt;
   if (mFailCnt == 4) {
       emit finished();
       return;
   }
   else if (mFailCnt > 4)
   {
       free();
       return;
   }

   // request dualcenter policy to another server
   const char* deployment[TmapHostNames::HOST_MAX];
   TmapHostNames::eHostDeploymentId deployId = TmapHostNames::getHostDeploymentId();
   TmapHostNames::getHostDeployment(deployment,deployId);
   // current ndds servers
   const char* primary   = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS_INDEX);
   const char* secondary = TmapHostNames::getHostName(TmapHostNames::HOST_NDDS2_INDEX);
   // replace primary with secondary
   deployment[TmapHostNames::HOST_NDDS_INDEX]   = secondary;
   deployment[TmapHostNames::HOST_NDDS2_INDEX]  = primary;

   // set host deployment
   TmapHostNames::setHostDeployment(deployment);


   // call ndds to change server for policy
   int port = TmapHostNames::getHostPort(TmapHostNames::HOST_NDDS2_INDEX);
   // don't need consider port for vnext, by way ndds
   qDebug() <<"policy, try fetch #2 from " << secondary;
#if 0
   if (strstr(secondary, "vnext.renault.com") != 0) {
       port = 443;
   }
#endif
   // must guarantee always this api is gonna get successful
   DeErrorCode success = NDDS_SetTmapHost(secondary,port);
   if (success != DE_ERROR_SUCCESS) {
       qWarning() <<"policy, try fetch fail, error #2-1 =" << success;
   }

   fetch();
}

void PolicyFetcher::handleNddsMessage(
        int msg,
        long lParam1,
        long lParam2,
        void *lpParam)
{
    qDebug() << "policy, msg #1";
    if (msg == DE_MSGTYPE_AUTH_END) {
        qDebug() << "policy, msg #1-1";
        int validateCode = 0;
        DS_AUTHRESULT result;
        memset(&result, 0, sizeof(DS_AUTHRESULT));
        int errCode = NDDS_GetAuthResult(&result);
        if ( errCode == DE_ERROR_SUCCESS) {
            validateCode = result.sAuthInfo.nValidateCode;
            qDebug() << "policy, msg #1-1A =" << validateCode;
            bool hasPolicy = false;
            for (auto i=0; i<result.sAppControlInfo.nControlInfoCount; i++) {
                auto info = result.sAppControlInfo.sControlInfoDetails[i];
                QString  serviceName = QString::fromLatin1(info.szServiceName);
                bool     use = QString::fromLatin1(info.szServiceYn) == "Y";
                QString  serviceItem = QString::fromLatin1(info.szServiceItems);
                qDebug() << "policy, taken " << serviceName << ":" << use << "," << serviceItem;

                if (use) {
                    if (serviceName == "TmapClientBehavior" || serviceName == "TmapCarCommonBehavior") {
                        fetchReroutePolicy(serviceItem);
                    }
                    else if (serviceName == "TmapServiceInfra") {
                        hasPolicy = true;
                        fetchDualcenterPolicy(serviceItem);
                    }
                    else {
                    }
                }
            }

            if (hasPolicy) {
                qDebug() << "policy, msg #1-1-1A Okay policy!";
                emit finished();
                free();
            }
            else {
                qDebug() << "policy, msg #1-1-1B Sorry policy!"
                           << result.szErrorCode
                           << "," << result.sAuthInfo.nValidateCode;
                tryFetch();
            }
        }
        else {
            qDebug() << "policy, msg #1-1B Sorry policy =" << errCode;
            tryFetch();
        }
    }
}


}