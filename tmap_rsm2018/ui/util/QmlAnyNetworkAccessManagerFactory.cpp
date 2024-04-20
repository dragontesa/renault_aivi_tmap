#include "QmlAnyNetworkAccessManagerFactory.h"
#include <QtNetwork/qsslsocket.h>

using namespace SKT;

namespace SKT {

class AnyNetworkAccessManager : public QNetworkAccessManager {
public:
    AnyNetworkAccessManager(QObject* parent) : QNetworkAccessManager(parent) {
    }

protected:
    QNetworkReply* createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request,
        QIODevice *outgoingData = Q_NULLPTR) {

        qDebug() << QString("AnyNetworkAccessManager #1 createRequest url: %1").arg(request.url().toString());

        QNetworkRequest req = request;
        QSslConfiguration conf(QSslConfiguration::defaultConfiguration());
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        req.setSslConfiguration(conf);

        QNetworkReply *reply = QNetworkAccessManager::createRequest(op, req, outgoingData);
        qDebug() << req.url();
        return reply;
    }
};

}

QmlAnyNetworkAccessManagerFactory::QmlAnyNetworkAccessManagerFactory()
{
}

QmlAnyNetworkAccessManagerFactory::~QmlAnyNetworkAccessManagerFactory()
{
}

QNetworkAccessManager* QmlAnyNetworkAccessManagerFactory::create(QObject* parent)
{
    QNetworkAccessManager* nam = new AnyNetworkAccessManager(parent);
    QObject::connect(nam, SIGNAL(sslErrors(QNetworkReply*, QList<QSslError>)),
        this, SLOT(onIgnoreSslErrors(QNetworkReply*,QList<QSslError>))
        );

    return nam;
}

void QmlAnyNetworkAccessManagerFactory::onIgnoreSslErrors(QNetworkReply* reply, QList<QSslError> errors)
{
    QNetworkRequest req = reply->request();
    reply->ignoreSslErrors(errors);
}
