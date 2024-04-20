#ifndef SIMPLECOMMAND_H
#define SIMPLECOMMAND_H

namespace SKT {

class SimpleCommand : public QObject, public mvc::TSimpleCommand
{
public:
    SimpleCommand( QObject* parent = 0 );

    virtual bool execute( const mvc::INotificationPtr& note );

    virtual void sendNotification( int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() );

};

}

#endif // SIMPLECOMMAND_H
