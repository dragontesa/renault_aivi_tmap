#ifndef ABSTRACTPROXY_H
#define ABSTRACTPROXY_H

namespace SKT {

class AbstractProxy : public mvc::TProxy
{
public:
    AbstractProxy( const QVariant& data = QVariant() );
    virtual ~AbstractProxy();

    void onBeforeRegister();

    void sendNotification( int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() );

};

}

#endif // ABSTRACTPROXY_H
