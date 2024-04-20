#include "TView.h"
#include "TObserver.h"

using namespace SKT;
using namespace SKT::mvc;

TView::TView()
{
}

TView::~TView()
{
}

void TView::registerObserver ( int noteId, const IObserverPtr& observer )
{
    mObserverMap[ noteId ].append( observer );
}

#define TMAP_RSM_UI_ENABLE_MVC_FAST_NOTIFICATION

namespace SKT {
    namespace Unroll {
        template <size_t N> struct Count{ };

        template <size_t N, typename Function, typename Index>
        inline void loop(const Index& index, const Function& function, Count<N>) {
            loop(index, function, Count<N-1>());
            function(index + N);
        }

        template <typename Function, typename Index>
        inline void loop(const Index& index, const Function& function, Count<0>) {
            function(index);
        }
    }
}

void TView::notifyObservers( const INotificationPtr& note )
{
    if( mObserverMap.contains( note->getId() ) ) {

        auto observers_ref = mObserverMap[ note->getId() ];

        static int maxCount = -1;
        if ( observers_ref.count() > maxCount) {
            maxCount = observers_ref.count();
            T_PERF_LOG() << "[Perf][TView]notifyObservers id: " << note->getId() << " maxCount: " << maxCount;
        }

        auto count = observers_ref.count();

        T_PERF_LOG() << "[Perf][TView]notifyObservers count: " << count << " id: " << note->getId() << " #begin";

#ifdef TMAP_RSM_UI_ENABLE_MVC_FAST_NOTIFICATION
        auto iterator = [&](const int& i) {
            observers_ref[i]->notifyObserver( note );
        };

#define TMAP_TVIEW_NOTIFY(COUNT, FUNC) \
    case COUNT : { \
        int i = 0; \
        SKT::Unroll::loop(i, FUNC, Unroll::Count<COUNT - 1>() ); \
    } \
    break;

        switch ( count ) {
            case 0: break;
            TMAP_TVIEW_NOTIFY(1, iterator)
            TMAP_TVIEW_NOTIFY(2, iterator)
            TMAP_TVIEW_NOTIFY(3, iterator)
            TMAP_TVIEW_NOTIFY(4, iterator)
            TMAP_TVIEW_NOTIFY(5, iterator)
            TMAP_TVIEW_NOTIFY(6, iterator)
            TMAP_TVIEW_NOTIFY(7, iterator)
            TMAP_TVIEW_NOTIFY(8, iterator)
            TMAP_TVIEW_NOTIFY(9, iterator)
            TMAP_TVIEW_NOTIFY(10, iterator)
            TMAP_TVIEW_NOTIFY(11, iterator)
            TMAP_TVIEW_NOTIFY(12, iterator)
            TMAP_TVIEW_NOTIFY(13, iterator)
            TMAP_TVIEW_NOTIFY(14, iterator)
            TMAP_TVIEW_NOTIFY(15, iterator)
            TMAP_TVIEW_NOTIFY(16, iterator)
            TMAP_TVIEW_NOTIFY(17, iterator)
            TMAP_TVIEW_NOTIFY(18, iterator)
            TMAP_TVIEW_NOTIFY(19, iterator)
            TMAP_TVIEW_NOTIFY(20, iterator)
            TMAP_TVIEW_NOTIFY(21, iterator)
            TMAP_TVIEW_NOTIFY(22, iterator)
            TMAP_TVIEW_NOTIFY(23, iterator)
            TMAP_TVIEW_NOTIFY(24, iterator)
            TMAP_TVIEW_NOTIFY(25, iterator)
            TMAP_TVIEW_NOTIFY(26, iterator)
            TMAP_TVIEW_NOTIFY(27, iterator)
            TMAP_TVIEW_NOTIFY(28, iterator)
            TMAP_TVIEW_NOTIFY(29, iterator)
            TMAP_TVIEW_NOTIFY(30, iterator)
            TMAP_TVIEW_NOTIFY(31, iterator)
            TMAP_TVIEW_NOTIFY(32, iterator)
            default: {
                T_PERF_LOG() << "[Perf][TView]notifyObservers count: " << count;

                QMutableListIterator<IObserverPtr> it( observers_ref );
                while ( it.hasNext() ) {
                    auto& observer = it.next();
                    observer->notifyObserver( note );
                }
            }
        }
#else
        // QList는 shallow copy 방식이므로 deep copy로 working array를 만들어 준다
        QList<IObserverPtr> observers;
        foreach( auto& observer, observers_ref ) {
            observers.append( observer );
        }

        foreach( auto& observer, observers ) {
            observer->notifyObserver( note );
        }
#endif

        T_PERF_LOG() << "[Perf][TView]notifyObservers count: " << count << " id: " << note->getId() << " #end";
    }
    else {
#ifdef TMAP_RSM_UI_ENABLE_MVC_LOG
        QString srcName;
        auto source = note->getSource();
        if ( source ) {
            QString objName = source->objectName();
            if ( !objName.isEmpty() ) {
                srcName = objName;
            }
            else {
                srcName = source->metaObject()->className();
            }
        }
        QJsonObject noteJson{
            {"id",note->getType()},
            {"body",note->getBody().isNull() ? "null" : note->getBody().toString()}
        };
        T_DEBUG( QJsonObject({
            {"type","Notification"},
            {"error",QString("Could not find match with '%1'").arg( note->getType() )},
            {"note",noteJson},
            {"from",srcName}
        }) );
#endif
    }
}

void TView::removeObserver( int noteId, const QObjectPtr& notifyContext )
{
    auto observers = mObserverMap[ noteId ];

    QMutableListIterator<IObserverPtr> it( observers );

    while ( it.hasNext() ) {
        auto& observer = it.next();
        if ( observer->compareNotifyContext( notifyContext ) == true ) {            
            it.remove();
        }
    }

    mObserverMap[ noteId ] = observers;

    if ( observers.length() == 0 ) {
        mObserverMap.remove( noteId );
    }
}

void TView::registerMediator( const IMediatorPtr& mediator )
{
    mediator->onBeforeRegister();

    // 재등록 방지
    if ( mMediatorMap.contains( mediator->getMediatorName() ) ) return;

    mMediatorMap[ mediator->getMediatorName() ] = mediator;

    auto interests = mediator->listNotificationInterests();
    if ( interests.length() > 0)
    {
        auto notifyMethod = std::bind(&IMediator::handleNotification, mediator, std::placeholders::_1);
        auto observer = IObserverPtr( new TObserver( notifyMethod, qSharedPointerDynamicCast<QObject>(mediator) ) );
        observer->setObserverName( mediator->getMediatorName() );
        observer->setObserverId( mediator->getMediatorId() );

        for ( int i = 0;  i<interests.length(); i++ ) {
            registerObserver( interests[i],  observer );
        }
    }

    mediator->onRegister();
}

IMediatorPtr TView::retrieveMediator( const QString& mediatorName )
{
    IMediatorPtr ret;

    if ( mMediatorMap.contains( mediatorName ) ) {
        ret = mMediatorMap[ mediatorName ];
    }

    return ret;
}

IMediatorPtr TView::removeMediator( const QString& mediatorName )
{
    IMediatorPtr mediator;

    if ( mMediatorMap.contains( mediatorName ) ) {
        mediator = mMediatorMap[mediatorName];
        if ( !mediator.isNull() ) {
            auto interests = mediator->listNotificationInterests();
            for ( int i = 0;  i<interests.length(); i++ ) {
                removeObserver( interests[i], qSharedPointerDynamicCast<QObject>(mediator) );
            }

            mediator->onRemove();
        }
        mMediatorMap.remove( mediatorName );
    }

    return mediator;
}

bool TView::hasMediator( const QString& mediatorName )
{
    return mMediatorMap.contains( mediatorName );
}
