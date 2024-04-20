#ifndef QMLAPPHELPER_H
#define QMLAPPHELPER_H

#include "TranslatorManager.h"
#include "LayoutManager.h"
#include "MultiSenseManager.h"

namespace SKT {

T_SHAREABLE( TSettings );

class QmlAppHelperPrivate;

class QmlAppHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString d READ d NOTIFY languageChanged)

public:
    static QmlAppHelper* instance();

    void initialize( QQmlApplicationEngine* qmlAppEngine, const QVariantMap& context );

    Q_INVOKABLE void startup( QObject* appView );
    Q_INVOKABLE QString viewUrl( int viewId );

    // log
    Q_INVOKABLE void log( const QString& msg );
    Q_INVOKABLE void debug( const QString& msg );
    Q_INVOKABLE void info( const QString& msg );
    Q_INVOKABLE void warn( const QString& msg );
    Q_INVOKABLE void error( const QString& msg );
    Q_INVOKABLE void perf( const QString& msg );

    // util
    bool hitTest( const QPointF& posOnAppLocal, QObject* item, const QRectF& hitRectOnItemLocal = QRectF() );\
    // DEPRECATED: use qml timer
    //Q_INVOKABLE void singleShot( int msec, QObject* receiver, const QString& slot );

    // language
    Q_INVOKABLE QString language();
    void setLanguage( const QString& lang );
    QString d();

    // layout
    int layout();
    QString layoutName();
    QString layoutResDir();
    QString defaultLayoutResDir();
    Q_INVOKABLE QString layoutGlobalConstantsJs();

    // multisense
    int multiSenseColorState();
    QColor multiSenseColor();
    void setMultiSenseColorState(int colorState);
    void setMultiSenseColorStateFromHiddenMenu(int color);

    void testMouseClick(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay);
    void testMousePress(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay);
    void testMouseRelease(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay);
    void testMouseMove(const QPointF& pos, int delay);

    void testTouchPress(const QList<QPointF>& points, int delay);
    void testTouchRelease(const QList<QPointF>& points, int delay);
    void testTouchMove(const QList<QPointF>& points, int delay);

    void testAddEvent( const QMap<int,QVariant>& args );
    Q_INVOKABLE void testDoNextEvent( bool cont );

    Q_INVOKABLE bool isDebug();
    Q_INVOKABLE bool isTarget();

signals:
    void languageChanged();

private:
    QmlAppHelper();
    ~QmlAppHelper();

    void registerType();
    void registerQmlType();

    void setContextProperty( const QString& name, QObject* obj );

private:
    QScopedPointer<QmlAppHelperPrivate> mPrivate;
};

}

#endif // QMLAPPHELPER_H
