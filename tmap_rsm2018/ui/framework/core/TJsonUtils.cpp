#include "TJsonUtils.h"
#include <QJsonDocument>
#include <QJsonObject>

using namespace SKT;

TJsonUtils::TJsonUtils()
{

}

QString TJsonUtils::toJsonString( const QVariantMap& varMap )
{
    QString ret;

    //QVariantMap varMap{{"type","Notification"}};
    auto jsonObj = QJsonObject::fromVariantMap( varMap );
    QJsonDocument doc(jsonObj);
    ret = doc.toJson(QJsonDocument::Indented);

    return ret;
}
