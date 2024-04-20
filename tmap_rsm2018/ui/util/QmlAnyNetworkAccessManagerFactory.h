#ifndef QMLANYNETWORKACCESSMANAGERFACTORY_H
#define QMLANYNETWORKACCESSMANAGERFACTORY_H

#include <QSslCertificate>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace SKT {

class QmlAnyNetworkAccessManagerFactory : public QObject, public QQmlNetworkAccessManagerFactory {
Q_OBJECT
private:
    QList<QSslError> mExpectedSslErrors;
public:
    explicit QmlAnyNetworkAccessManagerFactory();
    ~QmlAnyNetworkAccessManagerFactory();

    virtual QNetworkAccessManager* create(QObject* parent);

public slots:
    void onIgnoreSslErrors(QNetworkReply* reply, QList<QSslError> errors);
};

}

#endif
