#ifndef QMLOFFSCREENRENDERER_H
#define QMLOFFSCREENRENDERER_H

#include <QWindow>
#include <QMatrix4x4>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QSize>
#include "OffscreenRenderer.h"

class QOffscreenSurface;

namespace SKT {

class QuickFboRenderer : public QObject
{
    Q_OBJECT

public:
    QuickFboRenderer(int type);

    void postInit();
    void postRender();
    void postResize(const QSize& size);
    void postStop();

    QWaitCondition *cond() { return &mCond; }
    QMutex *mutex() { return &mMutex; }

    void setContext(QOpenGLContext *ctx) { mContext = ctx; }
    void setSurface(QOffscreenSurface *s) { mSurface = s; }
    void setSize(const QSize& size) { mSize = size; }
    void setQuickWindow(QQuickWindow *w) { mQuickWindow = w; }
    void setRenderControl(QQuickRenderControl *r) { mRenderControl = r; }

signals:
    void rendered( const QImage& img );

private:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void init();
    void cleanup();
    void syncFbo();
    void syncAndRender(QMutexLocker *lock);

    QWaitCondition mCond;
    QMutex mMutex;
    QOpenGLContext* mContext = nullptr;
    QOffscreenSurface* mSurface = nullptr;
    QOpenGLFramebufferObject* mFbo = nullptr;
    QSize mSize;
    QQuickWindow* mQuickWindow = nullptr;
    QQuickRenderControl* mRenderControl = nullptr;
    QMutex mQuitMutex;
    bool mQuit = false;
    int mType = 0;
};

class QmlOffscreenRenderer : public OffscreenRenderer
{
    Q_OBJECT
public:
    QmlOffscreenRenderer(QQmlEngine* qmlEngine, const QString& qmlPath, const QSize& size, int fps);
    ~QmlOffscreenRenderer();

    void setImageStreamProvider( const ImageStreamProviderPtr& provider );

    void setExpectedFps(int fps);

    void start();

public slots:
    void resize(const QSize& size);
    void polishAndRender();

protected:
    bool event(QEvent *e);

signals:
    void loaded( QQmlEngine* qmlEngine, QObject* rootItem );

private slots:
    void run();
    void requestUpdateBySceneGraphChanged();
    void requestUpdateByRenderRequested();
    void requestUpdate();
    void transferImage( const QImage& img );

private:
    void startQuick(const QString &filename);
    void syncSizes();

    ImageStreamProviderPtr mProvider;
    QuickFboRenderer* mQuickRenderer;
    QThread* mQuickRendererThread = nullptr;

    QOpenGLContext* mContext = nullptr;
    QOffscreenSurface* mOffscreenSurface = nullptr;
    QQuickRenderControl* mRenderControl = nullptr;
    QQuickWindow* mQuickWindow = nullptr;
    QQmlEngine* mQmlEngine = nullptr;
    QQmlComponent* mQmlComponent = nullptr;
    QQuickItem* mRootItem = nullptr;
    bool mQuickInitialized = false;
    bool mPsrRequested = false;
    QSize mSize;
    QString mQmlPath;
    QTimer mUpdateTimer;
    int mFps = 0;
};

}

#endif // QMLOFFSCREENRENDERER_H
