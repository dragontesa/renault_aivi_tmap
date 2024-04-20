#ifndef INOTIFICATION_H
#define INOTIFICATION_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT INotification
{
public:
    virtual ~INotification() {}

    virtual int getId() = 0;

    virtual void setBody( const QVariant& body ) = 0;
    virtual QVariant getBody() = 0;

    virtual void setType( const QString& type ) = 0;
    virtual QString getType() = 0;    

    virtual QObjectPtr getSource() = 0;
    virtual void setSource( const QObjectPtr& obj ) = 0;

    virtual QObjectPtr getDestination() = 0;
    virtual void setDestination( const QObjectPtr& obj ) = 0;

    virtual QVariant getObserverId() = 0;
    virtual void setObserverId( const QVariant& id ) = 0;
};

}}

#endif // INOTIFICATION_H

