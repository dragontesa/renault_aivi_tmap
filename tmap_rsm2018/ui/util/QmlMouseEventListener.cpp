#include "QmlMouseEventListener.h"
#include <QQuickWindow>

using namespace SKT;

QmlMouseEventListener::QmlMouseEventListener( QQuickItem* parent )
    : QQuickItem( parent )
{
    setFiltersChildMouseEvents( true );
    setAcceptedMouseButtons( Qt::LeftButton );
}

bool QmlMouseEventListener::childMouseEventFilter( QQuickItem* item, QEvent* event )
{
    if ( !isVisible() || !isEnabled() ) {
        return QQuickItem::childMouseEventFilter( item, event );
    }

    switch ( event->type() ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        handleMouseEvent( item, static_cast<QMouseEvent *>( event ));
    default:
        break;
    }

    return QQuickItem::childMouseEventFilter( item, event );
}

void QmlMouseEventListener::handleMouseEvent( QQuickItem* item, QMouseEvent* event )
{
    if ( !( event->button() & acceptedMouseButtons() ) ) {
        return;
    }

    QPointF localPos = mapFromScene(event->windowPos());

    if ( contains( localPos ) ) {
        switch ( event->type() ) {
        case QEvent::MouseMove:
            break;
        case QEvent::MouseButtonPress:
            emit pressed( localPos );
            break;
        case QEvent::MouseButtonRelease:
            emit released( localPos );
            break;
        default:
            break;
        }
    }
}

