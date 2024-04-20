#ifndef INTEGRATEDSEARCHPROXY_H
#define INTEGRATEDSEARCHPROXY_H

#include "AbstractProxy.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRunnable>
#include <QTimer>

//Http,Json
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QSslCipher>
#include <QSslKey>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>


#include "../common/navigationcontroller.h"


namespace SKT {

T_SHAREABLE( IntegratedSearchProxy );
T_SHAREABLE( RequestCommand );
T_SHAREABLE( IntegratedRequest );
T_SHAREABLE( IntegratedResponseDto );

class NetworkAccessManagerWrapper:public QNetworkAccessManager {
    Q_OBJECT
public:
    NetworkAccessManagerWrapper(QObject *parent):
    QNetworkAccessManager(parent){
        mReply = nullptr;
    }

    NetworkAccessManagerWrapper(QObject *parent, const QVariantMap &reqData):
    QNetworkAccessManager(parent){
        mReqData = reqData;
        mReply = nullptr;
    }

    QVariantMap setReqData(const QVariantMap &reqData){
        mReqData = reqData;
    }

    QVariantMap getReqData(){
        return mReqData;
    }

    void setReply(QNetworkReply* reply)  {
       mReply = reply;
    }

    QNetworkReply* getReply() {
        return mReply;
    }

    void closeRely() {
        if (mReply &&
            mReply->isRunning()) {
            mReply->close();
        }
    }

    void setTimer(QNetworkReply* reply, const int timeout) {
       setReply(reply);
       killTimer();
       connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
       mTimer.start(timeout);
    }

    void killTimer() {
        if(mTimer.isActive()) {
            mTimer.stop();
        }
    }
protected:
    QVariantMap mReqData;
    QNetworkReply* mReply;
    QTimer mTimer;

private slots:
  void timeout() {
      closeRely();
  }

};

class IntegratedSearchProxy : public AbstractProxy
{
    Q_OBJECT
public:
    IntegratedSearchProxy(QObject* controller);
    ~IntegratedSearchProxy();

    void requestCommand(const RequestCommandPtr& req);
    bool getOnlineState() const;

    void setGadgetSearchMode(bool b);
    bool isGadgetSearchMode() const;
    void setCurrentGpsTime(QDateTime currentTime);
    QDateTime getCurrentGpsTime() const;
signals:
    void queueChanged(int count);
private slots:
    void Response();
    void onlineChanged(bool online);
    void onResultSearchBySisulNamePrediction(QNetworkReply *reply);
    void onResultSearchAQI(QNetworkReply *reply);
    void onResultSearchAddress(QNetworkReply *reply);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError err);
    void slotSslErrors(QList<QSslError> list);
    void onQueueChanged(int count);
    void onCurrentStreetChanged(RGDataPtr value);
    void onChannelServerPolicyFinished(bool success);

public:
    void onResultSearchBySisulNamePrediction2(const QVariantMap& reqData, const QByteArray& result);
    void onResultSearchAddress2(const QVariantMap& reqData, const QByteArray& result);
    void onResultSearchAQI2(const QVariantMap& reqData, const QByteArray& result);

private:
    void onSearchFinished( const RequestCommandPtr& req);
    void searchBySisulNamePrediction(QVariantMap reqData);
    void searchAQI(QVariantMap reqData);
    void searchAddress(QVariantMap reqData);
    void searchBySisulNamePredictionViaNddsBundle(QVariantMap reqData);
    void searchAQIViaNddsBundle(QVariantMap reqData);
    void searchAddressViaNddsBundle(QVariantMap reqData);
    QString takeUrlSearchBySisulNamePrediction(QVariantMap reqData);
    QString takeUrlSearchAQI(QVariantMap reqData);
    QString takeSearchAddress(QVariantMap reqData);
    void SendReply(int notificationId, int resultCount, QVariant result, QVariantMap reqData);
    void cleanManager(QList<NetworkAccessManagerWrapper*> &managerArr);
    void removeOneManager(QList<NetworkAccessManagerWrapper*> &managerArr, NetworkAccessManagerWrapper *pManager);
    void searchComplexImage(QVariantMap reqData);

private:
    NavigationController* mController;
    bool mIsOnline;
    bool mIsGadgetSearch = false;
    bool mServerPolicySuccess = false;
    QList<NetworkAccessManagerWrapper*> mManagerArrSisul;
    QList<NetworkAccessManagerWrapper*> mManagerArrAddr;
    QList<NetworkAccessManagerWrapper*> mManagerArrAQI;
    void (IntegratedSearchProxy::*mSearchSisulnamPredictionFntr)(QVariantMap);
    void (IntegratedSearchProxy::*mSearchAddressFntr)(QVariantMap);
    void (IntegratedSearchProxy::*mSearchAqiFntr)(QVariantMap);
    QDateTime mCurrentTime;

};

}

#endif // INTEGRATEDSEARCHPROXY_H

