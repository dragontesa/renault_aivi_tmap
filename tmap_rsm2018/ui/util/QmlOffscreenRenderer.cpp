#include "QmlOffscreenRenderer.h"
#include <QQuickRenderControl>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include "ImageStreamProvider.h"
#include "MapInClusterGmemTest.h"

namespace SKT
{

static const QEvent::Type kINIT = QEvent::Type(QEvent::User + 1);
static const QEvent::Type kRENDER = QEvent::Type(QEvent::User + 2);
static const QEvent::Type kRESIZE = QEvent::Type(QEvent::User + 3);
static const QEvent::Type kSTOP = QEvent::Type(QEvent::User + 4);
static const QEvent::Type kUPDATE = QEvent::Type(QEvent::User + 5);
static const qreal kDevicePixelRatio = 1.0f;

class FboResizeEvent : public QEvent
{
public:
    FboResizeEvent(QEvent::Type type, const QSize& size)
        : QEvent(type)
        , m_size(size) {}
    const QSize m_size;
};

QuickFboRenderer::QuickFboRenderer(int type)
    : mContext(0),
      mSurface(0),
      mFbo(0),
      mQuickWindow(0),
      mRenderControl(0),
      mQuit(false),
      mType(type)
{
}

void QuickFboRenderer::postInit()
{
    QCoreApplication::postEvent(this, new QEvent(kINIT));
}

void QuickFboRenderer::postRender()
{
    QCoreApplication::postEvent(this, new QEvent(kRENDER));
}

void QuickFboRenderer::postResize(const QSize& size)
{
    QCoreApplication::postEvent(this, new FboResizeEvent(kRESIZE, size));
}

void QuickFboRenderer::postStop()
{
    QCoreApplication::postEvent(this, new QEvent(kSTOP));
}

bool QuickFboRenderer::event(QEvent *e)
{
    QMutexLocker lock(&mMutex);

    switch (int(e->type())) {
    case kINIT:
        init();
        return true;
    case kRENDER:
        syncAndRender(&lock);
        return true;
    case kRESIZE: {
        mSize = reinterpret_cast<FboResizeEvent*>(e)->m_size;
        return true;
    }
    case kSTOP:
        cleanup();
        return true;
    default:
        return QObject::event(e);
    }
}

void QuickFboRenderer::init()
{
    mContext->makeCurrent(mSurface);
    mRenderControl->initialize(mContext);
}

void QuickFboRenderer::cleanup()
{
    mContext->makeCurrent(mSurface);

    mRenderControl->invalidate();

    delete mFbo;
    mFbo = 0;

    mContext->doneCurrent();
    mContext->moveToThread(QCoreApplication::instance()->thread());

    mCond.wakeOne();
}

void QuickFboRenderer::syncFbo()
{
    if (mFbo && mFbo->size() != mSize * kDevicePixelRatio) {
        delete mFbo;
        mFbo = 0;
    }

    if (!mFbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);        
        auto size = mSize;        

        format.setSamples(0);

#ifdef TMAP_CLUSTER_GMEM_TEST_REDUCE_FBO_SIZE
        size /= 2;
#endif

        mFbo = new QOpenGLFramebufferObject(size * kDevicePixelRatio, format);        
        mQuickWindow->setRenderTarget(mFbo);
    }
}

void QuickFboRenderer::syncAndRender(QMutexLocker *lock)
{
    if (!mContext->makeCurrent(mSurface)) {
        qWarning("Failed to make the context current in the current thread");
        return;
    }

    syncFbo();

    mRenderControl->sync();
    mCond.wakeOne();
    lock->unlock();

    mRenderControl->render();    

    mContext->functions()->glFlush();

    QMutexLocker quitLock(&mQuitMutex);
    if (!mQuit) {
        if (mType == 0) {
            emit rendered( mFbo->toImage() );
        }
    }
}

class QuickFboRenderControl : public QQuickRenderControl
{
public:
    QuickFboRenderControl() {}

private:

};

QmlOffscreenRenderer::QmlOffscreenRenderer(QQmlEngine* qmlEngine, const QString& qmlPath, const QSize& size, int fps)
{    
    mQmlPath = qmlPath;
    mSize = size;
    mFps = fps;
    mQuickInitialized = false;

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setSamples(0);

    mContext = new QOpenGLContext;
    mContext->setFormat(format);
    mContext->create();

    mOffscreenSurface = new QOffscreenSurface;
    mOffscreenSurface->setFormat(mContext->format());
    mOffscreenSurface->create();

    mRenderControl = new QuickFboRenderControl();

    mQuickWindow = new QQuickWindow(mRenderControl);

    mQmlEngine = qmlEngine;

    //mQuickRenderer = new QuickFboRenderer(mQmlPath.indexOf("Gadget") > -1 ? 1 : 0);
    mQuickRenderer = new QuickFboRenderer(0);
    mQuickRenderer->setContext(mContext);

    mQuickRenderer->setSurface(mOffscreenSurface);
    mQuickRenderer->setSize(size);
    mQuickRenderer->setQuickWindow(mQuickWindow);
    mQuickRenderer->setRenderControl(mRenderControl);

    mQuickRendererThread = new QThread;

    mRenderControl->prepareThread(mQuickRendererThread);

    mContext->moveToThread(mQuickRendererThread);
    mQuickRenderer->moveToThread(mQuickRendererThread);

    connect(mQuickRenderer, &QuickFboRenderer::rendered, this, &QmlOffscreenRenderer::transferImage);

    mQuickRendererThread->start();

    mUpdateTimer.setInterval(1000/mFps);
    connect(&mUpdateTimer, &QTimer::timeout, this, &QmlOffscreenRenderer::requestUpdate);
    mUpdateTimer.start();

    //connect(mRenderControl, &QQuickRenderControl::renderRequested, this, &QmlOffscreenRenderer::requestUpdateByRenderRequested);
    //connect(mRenderControl, &QQuickRenderControl::sceneChanged, this, &QmlOffscreenRenderer::requestUpdateBySceneGraphChanged);
}

QmlOffscreenRenderer::~QmlOffscreenRenderer()
{
    qDebug() << "QmlOffscreenRenderer::~QmlOffscreenRenderer #1";

    mQuickRenderer->mutex()->lock();
    mQuickRenderer->postStop();
    mQuickRenderer->cond()->wait(mQuickRenderer->mutex());
    mQuickRenderer->mutex()->unlock();

    mQuickRendererThread->quit();
    mQuickRendererThread->wait();

    delete mRenderControl;
    delete mQmlComponent;
    delete mQuickWindow;
    delete mQmlEngine;

    delete mOffscreenSurface;
    delete mContext;
}

void QmlOffscreenRenderer::setImageStreamProvider( const ImageStreamProviderPtr& provider )
{
    mProvider = provider;
}

void QmlOffscreenRenderer::setExpectedFps(int fps)
{
    mFps = fps;

    mUpdateTimer.stop();
    mUpdateTimer.setInterval(1000/mFps);
    mUpdateTimer.start();
}

void QmlOffscreenRenderer::start()
{
	qDebug() << "QmlOffscreenRenderer::start #1";
    if (!mQuickInitialized) {
		qDebug() << "QmlOffscreenRenderer::start #2";
        startQuick(mQmlPath);
    }
}

void QmlOffscreenRenderer::resize(const QSize& size)
{
    if (mRootItem) {
        mSize = size;
        syncSizes();
        mQuickRenderer->postResize(size);
        polishAndRender();
    }
}

void QmlOffscreenRenderer::requestUpdateBySceneGraphChanged()
{
    //qDebug() << "[Perf][requestUpdateBySceneGraphChanged]#1";
    requestUpdate();
}

void QmlOffscreenRenderer::requestUpdateByRenderRequested()
{
    //qDebug() << "[Perf][requestUpdateByRenderRequested]#1";
    requestUpdate();
}

void QmlOffscreenRenderer::requestUpdate()
{
    if (mQuickInitialized && !mPsrRequested) {
        mPsrRequested = true;
        QCoreApplication::postEvent(this, new QEvent(kUPDATE));
    }
}

bool QmlOffscreenRenderer::event(QEvent *e)
{
    if (e->type() == kUPDATE) {
        polishAndRender();
        mPsrRequested = false;
        return true;
    }

    return QObject::event(e);
}

void QmlOffscreenRenderer::polishAndRender()
{
    //qDebug() << "[Perf]polishAndRender";

    mRenderControl->polishItems();

    QMutexLocker lock(mQuickRenderer->mutex());

    mQuickRenderer->postRender();

    mQuickRenderer->cond()->wait(mQuickRenderer->mutex());
}

void QmlOffscreenRenderer::run()
{
    qDebug() << "QmlOffscreenRenderer::run #1";

    qDebug() << "QmlOffscreenRenderer::run #2";

    if (mQmlComponent->isError()) {
        QList<QQmlError> errors = mQmlComponent->errors();
        for ( auto error : errors ) {
            qDebug() << error.toString();
        }

        qDebug() << "QmlOffscreenRenderer::run #3";

        return;
    }

    qDebug() << "QmlOffscreenRenderer::run #4";

    QObject *rootObject = mQmlComponent->create();
    if (mQmlComponent->isError()) {
        auto errors = mQmlComponent->errors();
        for ( auto error : errors ) {
            qDebug() << error.toString();
        }

        qDebug() << "QmlOffscreenRenderer::run #5";

        return;
    }

    qDebug() << "QmlOffscreenRenderer::run #6";

    mRootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!mRootItem) {
        delete rootObject;

        qDebug() << "QmlOffscreenRenderer::run #7";

        return;
    }

    qDebug() << "QmlOffscreenRenderer::run #8";

    mRootItem->setParentItem(mQuickWindow->contentItem());

    qDebug() << "QmlOffscreenRenderer::run #9";

    syncSizes();

    qDebug() << "QmlOffscreenRenderer::run #10";

    mQuickInitialized = true;

    qDebug() << "QmlOffscreenRenderer::run #11";

    mQuickRenderer->postInit();

    qDebug() << "QmlOffscreenRenderer::run #12";

    polishAndRender();

    qDebug() << "QmlOffscreenRenderer::run #13";

    emit loaded(mQmlEngine, mRootItem);

    qDebug() << "QmlOffscreenRenderer::run #14";
}

void QmlOffscreenRenderer::syncSizes()
{
    mRootItem->setWidth(mSize.width());
    mRootItem->setHeight(mSize.height());

    mQuickWindow->setGeometry(0, 0, mSize.width(), mSize.height());
}

void QmlOffscreenRenderer::startQuick(const QString &filename)
{
	qDebug() << "QmlOffscreenRenderer::startQuick #1";
	
    mQmlComponent = new QQmlComponent(mQmlEngine, QUrl(filename),QQmlComponent::PreferSynchronous);
    if (mQmlComponent->isLoading()) {
		qDebug() << "QmlOffscreenRenderer::startQuick #2";
        connect(mQmlComponent, &QQmlComponent::statusChanged, this, &QmlOffscreenRenderer::run);
    }
    else {
		qDebug() << "QmlOffscreenRenderer::startQuick #3";
        run();
    }
}

void QmlOffscreenRenderer::transferImage( const QImage& img )
{
    mProvider->push( 0, img );
}

}
