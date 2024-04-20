#ifndef QMLMOUSEEVENTLISTENER_H
#define QMLMOUSEEVENTLISTENER_H

#include <QQuickItem>
#include <QMouseEvent>

namespace SKT {

class QmlMouseEventListener : public QQuickItem
{
    Q_OBJECT

public:
    explicit QmlMouseEventListener( QQuickItem* parent = 0 );

signals:
    void pressed( const QPointF& pos );
    void released( const QPointF& pos );

protected:
    virtual bool childMouseEventFilter( QQuickItem* item, QEvent* event );

private:
    void handleMouseEvent( QQuickItem* item, QMouseEvent* event );
};

}

#endif // QMLMOUSEEVENTLISTENER_H
