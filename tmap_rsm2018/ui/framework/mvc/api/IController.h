#ifndef ICONTROLLER_H
#define ICONTROLLER_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IController
{
public:
    virtual ~IController() {}

    virtual void registerCommand( int noteId, const QMetaObject* commandClassRef ) = 0;

    virtual bool executeCommand( const INotificationPtr& note ) = 0;

    virtual void removeCommand( int noteId ) = 0;

    virtual bool hasCommand( int noteId ) = 0;
};

}}

#endif // ICONTROLLER_H

