#ifndef TCONTROLLER_H
#define TCONTROLLER_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TController
    : public QObject
    , public IController
    , public TShareable<TController>
{
public:
    TController();

    virtual ~TController();

    void setView( IView* view );

public:
    bool executeCommand( const INotificationPtr& note );

    void registerCommand( int noteId, const QMetaObject* commandClassRef );

    bool hasCommand( int noteId );

    void removeCommand( int noteId );

protected:
    IView* mView;
    QMap<int, const QMetaObject*> mCommandMap;
};

}}

#endif // TCONTROLLER_H

