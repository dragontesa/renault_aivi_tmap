#ifndef INOTIFIER_H
#define INOTIFIER_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT INotifier
{
public:
    virtual ~INotifier() {}

    virtual void sendNotification( int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() ) = 0;
};

}}

#endif // INOTIFIER_H

