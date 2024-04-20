#ifndef TNOTIFICATION_H
#define TNOTIFICATION_H

#include "INotification.h"

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TNotification
    : public INotification
{
public:
    TNotification( QObject* source, int id, const QVariant& body, const QVariant& observerId = QVariant(), const QString& type = QString() );

    virtual ~TNotification();

    int getId();

    void setBody( const QVariant& body );
    QVariant getBody();

    void setType( const QString& type );
    QString getType();

    QObjectPtr getSource();
    void setSource( const QObjectPtr& obj );

    QObjectPtr getDestination();
    void setDestination( const QObjectPtr& obj );

    virtual QVariant getObserverId();
    virtual void setObserverId( const QVariant& observerId );

protected:
    int mId;    
    QString mType;
    QVariant mBody;
    QObjectPtr mSource;
    QObjectPtr mDestination;
    QVariant mObserverId;
};

}}

#endif // TNOTIFICATION_H

