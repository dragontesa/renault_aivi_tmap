#include "GadgetOffscreenRenderer.h"
#include "view/mediator/widget/WidgetUtils.h"
#include "ApplicationConstants.h"
#include "ImageStreamProvider.h"
#include "qlogging.h"
#include <QOpenGLFunctions_2_1>
#include <QQuickRenderControl>
#include <QTransform>
#include <QtSqlDepends>

using namespace SKT;

GadgetOffscreenRenderer::GadgetOffscreenRenderer( QQuickWindow* window )
    : mWindow( window )
{
    connect(mWindow, &QQuickWindow::beforeRendering, this, &GadgetOffscreenRenderer::onGadgetBeforeRendering, Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(mWindow, SIGNAL(afterRendering()), this, SLOT(onGadgetAfterRendering()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
    connect(mWindow, &QQuickWindow::frameSwapped, this, &GadgetOffscreenRenderer::onFrameSwapped);
}

GadgetOffscreenRenderer::~GadgetOffscreenRenderer()
{
    if ( mFbo ) {
        delete mFbo;
        mFbo = nullptr;

        if ( mPboNonMap.bufferSize ) {
            mGlFunc.glDeleteBuffers( mPboNonMap.count, mPboNonMap.ids );
            mGlFunc.glDeleteBuffers( mPboMap.count, mPboMap.ids );
            mPboNonMap.bufferSize = 0;
        }
    }

    if ( mWindow->renderTarget() != nullptr ) {
        qDebug() << "[GadgetSwitch]~GadgetOffscreenRenderer setRenderTarget NULL";
        mCheckIfWindowHasRendered = true;
        mWindow->setRenderTarget( nullptr );
    }
}

void GadgetOffscreenRenderer::setImageStreamProvider( const ImageStreamProviderPtr& provider )
{
    mProvider = provider;
}

void GadgetOffscreenRenderer::updateActivations( int reason, const QList<UIConstants::GadgetType>& activations )
{
    QMutexLocker lock( &mMutex );

    mActivations = activations;
    mHasMap = false;
    mHasNonMap = false;

    if ( mActivations.count() > 0 ) {
        T_PERF_LOG() << "[Gadget]updateActivations: " << mActivations;
        mHasMap = mActivations.contains( UIConstants::GadgetXXL ) || mActivations.contains( UIConstants::GadgetXL );
        if ( mHasMap ) {
            auto index = mActivations.indexOf( UIConstants::GadgetXXL );
            if ( index == -1 ) {
                index = mActivations.indexOf( UIConstants::GadgetXL );
            }
            mMapType = mActivations[index];
        }
        if ( mActivations.indexOf( UIConstants::GadgetLarge ) > -1 ||
            mActivations.indexOf( UIConstants::GadgetMedium ) > -1 ||
             mActivations.indexOf( UIConstants::GadgetSmall ) > -1 ) {
            mHasNonMap = true;
        }
    }
    else {
        T_PERF_LOG() << "[Gadget]updateActivations: empty reason: " << reason;
    }
}

void GadgetOffscreenRenderer::updateImmediately( UIConstants::GadgetType type )
{
    QMutexLocker lock( &mMutex );

    mImmediateUpdates.append( type );
}

void GadgetOffscreenRenderer::start()
{
    QMutexLocker lock( &mMutex );

    mRunning = true;
    mTotalFrames = 0;

    //qDebug() << "[Gadget]start!";
}

void GadgetOffscreenRenderer::stop()
{
    //qDebug() << "[Gadget]stop";

    QMutexLocker lock( &mMutex );

    mRunning = false;

    if ( mWindow->renderTarget() == nullptr ) {
        mCheckIfWindowHasRendered = true;
        emit stopped();
    }
}

void GadgetOffscreenRenderer::onGadgetBeforeRendering()
{
    //qDebug() << "[Gadget]onGadgetBeforeRendering before";

    QMutexLocker lock( &mMutex );

    if ( mRunning ) {
        prepareBuffer();

        if ( mWindow->renderTarget() == nullptr ) {
            qDebug() << "[GadgetSwitch]onGadgetBeforeRendering setRenderTarget FBO";
            mWindow->setRenderTarget( mFbo );
        }
    }
    else {
        if ( mFbo ) {
            delete mFbo;
            mFbo = nullptr;

            mGlFunc.glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            mGlFunc.glClear( GL_COLOR_BUFFER_BIT );
            mGlFunc.glDeleteBuffers( mPboNonMap.count, mPboNonMap.ids );
            mGlFunc.glDeleteBuffers( mPboMap.count, mPboMap.ids );
            mPboNonMap.bufferSize = 0;

            if ( mWindow->renderTarget() != nullptr ) {
                qDebug() << "[GadgetSwitch]onGadgetBeforeRendering setRenderTarget NULL";
                mWindow->setRenderTarget( nullptr );
            }

            mCheckIfWindowHasRendered = true;

            emit stopped();
        }
    }

    //qDebug() << "[Gadget]onGadgetBeforeRendering after";
}

void GadgetOffscreenRenderer::onGadgetAfterRendering()
{
    //qDebug() << "[Gadget]onGadgetAfterRendering before";

    QMutexLocker lock( &mMutex );    

    if ( mActivations.count() == 0 ) {
        return;
    }

    if ( !mImmediateUpdates.isEmpty() ) {
        if ( mRunning ) {
            if ( mWindow->renderTarget() != nullptr ) {
                mImmediateUpdate = mImmediateUpdates.first();

                mMapUpdate = mNonMapUpdate = false;
                auto hasMap = mImmediateUpdate == UIConstants::GadgetXXL || mImmediateUpdate == UIConstants::GadgetXL;
                hasMap ? mMapUpdate = true : mNonMapUpdate = true;

                sync();

                mImmediateUpdates.removeFirst();
                mImmediateUpdate = UIConstants::GadgetNone;
            }
        }
    }
    else {
        bool update = false;
        bool mapUpdate = false;
        bool nonMapUpdate = false;

        if ( mHasMap ) {
            if ( !mMapUpdateTimer.isValid() ) {
                mapUpdate = true;
            }
            else {
                auto elapsed = mMapUpdateTimer.elapsed();
                if ( elapsed > 1000 / MAP_FPS ) {
                    mapUpdate = true;
                }
            }
        }

        if ( mHasNonMap ) {
            if ( !mNonMapUpdateTimer.isValid() ) {
                nonMapUpdate = true;
            }
            else {
                auto elapsed = mNonMapUpdateTimer.elapsed();
                if ( elapsed > 1000 / NONMAP_FPS ) {
                    nonMapUpdate = true;
                }
            }
        }

        if ( nonMapUpdate ) {
            mapUpdate = false;
        }

        mMapUpdate = mapUpdate;
        mNonMapUpdate = nonMapUpdate;
        update = mMapUpdate || mNonMapUpdate;

        //qDebug() << "[Gadget]onGadgetAfterRendering after";

        if ( update ) {
            if ( mRunning ) {
                if ( mWindow->renderTarget() != nullptr ) {
                    sync();

                    if ( mMapUpdate ) {
                        //qDebug() << "[Gadget]Map Update activations: " << mActivations;
                        mMapUpdateTimer.restart();
                    }

                    if ( mNonMapUpdate ) {
                        //qDebug() << "[Gadget]Non Map Update activations: " << mActivations;
                        mNonMapUpdateTimer.restart();
                    }
                }
            }
        }

        //qDebug() << "[Gadget]onGadgetAfterRendering after";
    }
}

void GadgetOffscreenRenderer::onFrameSwapped()
{
    QMutexLocker lock( &mMutex );

    //qDebug() << "[GadgetFBO]onFrameSwapped";
    if ( mCheckIfWindowHasRendered ) {
        qDebug() << "[GadgetSwitch]onFrameSwapped";
        mCheckIfWindowHasRendered = false;
        emit windowHasRendered();
    }
}

void GadgetOffscreenRenderer::prepareBuffer()
{
    //qDebug() << "[Gadget]prepareBuffer";

    if ( mFbo == nullptr ) {
        mTotalFrames = 0;

        if ( mActivations.count() == 0 ) {
            return;
        }

        auto size = UIConstants::gadgetSize( (UIConstants::GadgetType)mActivations[0] );

        mPboNonMap.size = QSize(820, 390);
        mPboNonMap.bufferSize = mPboNonMap.size.width() * mPboNonMap.size.height() * sizeof(float);
        mPboMap.size = UIConstants::gadgetSize( UIConstants::GadgetXXL );
        mPboMap.bufferSize = mPboMap.size.width() * mPboMap.size.height() * sizeof(float);

        if (mFbo == nullptr || mFbo->size() != size) {
            if ( mFbo != nullptr ) {
                mGlFunc.glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
                mGlFunc.glClear( GL_COLOR_BUFFER_BIT );
                mGlFunc.glDeleteBuffers( mPboNonMap.count, mPboNonMap.ids );
                mGlFunc.glDeleteBuffers( mPboMap.count, mPboMap.ids );
                delete mFbo;
                mFbo = nullptr;
            }
        }

        auto glCtx = mWindow->openglContext();
        bool isValid = glCtx->isValid();
        qDebug() << "[Gadget]prepareBuffer glCtx isValid: " << isValid;
        mGlFunc = QOpenGLExtraFunctions(glCtx);
        mGlFunc.initializeOpenGLFunctions();

        struct Internal {
            static void genBuffer( QOpenGLExtraFunctions& glFunc, Pbo* pbo ) {
                glFunc.glGenBuffers( pbo->count, pbo->ids );
                auto error = glFunc.glGetError();
                if ( error != GL_NO_ERROR ) {
                    //qDebug() << "[Gadget]glGenBuffers Error #1";
                }
                for (int i = 0; i < pbo->count; ++i) {                    
                    glFunc.glBindBuffer( GL_PIXEL_PACK_BUFFER, pbo->ids[i] );
                    error = glFunc.glGetError();
                    if ( error != GL_NO_ERROR ) {
                        //qDebug() << "[Gadget]glGenBuffers Error #2";
                    }
                    glFunc.glBufferData( GL_PIXEL_PACK_BUFFER, pbo->bufferSize, 0, GL_DYNAMIC_READ );
                    if ( error != GL_NO_ERROR ) {
                        //qDebug() << "[Gadget]glGenBuffers Error #3";
                    }
                }

                //qDebug() << "[Gadget]glGenBuffers count: " << pbo->count << " size: " << pbo->size << " bufferSize: " << pbo->bufferSize;
            }
        };

        Internal::genBuffer( mGlFunc, &mPboNonMap );
        Internal::genBuffer( mGlFunc, &mPboMap );
    }

    if ( mWindow->renderTarget() == nullptr ) {
        QOpenGLFramebufferObjectFormat format;
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(0);
        mFbo = new QOpenGLFramebufferObject(QSize(mWindow->width(), mWindow->height()), format);
        qDebug() << "[GadgetSwitch]prepareBuffer setRenderTarget FBO";
        mWindow->setRenderTarget(mFbo);
    }
}

void GadgetOffscreenRenderer::sync()
{
    //qDebug << "[Gadget]sync before";

    if ( mActivations.count() == 0 ) {
        //qDebug() << "[Gadget]sync #1";
        return;
    }

    mSync = mGlFunc.glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    GLint value = 0;
    GLsizei length = 0;
    mGlFunc.glGetSynciv(mSync, GL_SYNC_CONDITION, sizeof(value), &length, &value);
    //qDebug() << "[Gadget]sync length: " << length;
    if ( length == 1 ) {
        if ( value == GL_SYNC_GPU_COMMANDS_COMPLETE ){
            rasterize();
        }
    }
    mGlFunc.glDeleteSync(mSync);

    //qDebug() << "[Gadget]sync after";
}

void GadgetOffscreenRenderer::rasterize()
{
    if ( mImmediateUpdate != UIConstants::GadgetNone ) {
        auto hasMap = mImmediateUpdate == UIConstants::GadgetXXL || mImmediateUpdate == UIConstants::GadgetXL;
        if ( hasMap ) {
            rasterizeMap();
        }
        else {
            rasterizeNonMapImmediately();
        }
    }
    else {
        if ( mNonMapUpdate ) {
            rasterizeNonMap();
        }
        else if ( mMapUpdate ) {
            rasterizeMap();
        }
    }
}

#ifndef GL_BGRA
#define GL_BGRA					0x80E1
#define GL_RGBA					0x1908
#define GL_RGBA8				0x8058
#endif

void GadgetOffscreenRenderer::rasterizeMap()
{
    static int pboIndex = 0;
    int nextPboIndex = 0;

    GLenum status;
    GLenum error;
    auto mapSize = UIConstants::gadgetSize( mMapType );
    void *pboBuf = 0;
    bool isBlank = false;
    unsigned int* tBuf = 0;

    Pbo* pbo = nullptr;
    pbo = &mPboMap;

//    qDebug() << "[Gadget]rasterize before";

    pboIndex = (pboIndex + 1) % pbo->count;
    nextPboIndex = (pboIndex + 1 ) % pbo->count;

    status = mGlFunc.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( status != GL_FRAMEBUFFER_COMPLETE ) {
        //qDebug() << "[Gadget]rasterize skipped status: " << status;
        return;
    }

    //qDebug() << "[Gadget]rasterizeMap pbo id: " << pbo->ids[pboIndex];
    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo->ids[pboIndex]);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

//    qDebug() << "[Gadget]rasterize glReadPixels: before";
    mGlFunc.glReadPixels(0, 0, mapSize.width(), mapSize.height(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //qDebug() << "[Gadget]rasterize glReadPixels: after";
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo->ids[nextPboIndex]);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, pbo->ids[pboIndex]);
    pboBuf = mGlFunc.glMapBufferRange(GL_ARRAY_BUFFER, 0, pbo->bufferSize, GL_MAP_READ_BIT);
//    error = mGlFunc.glGetError();
//    if ( error != GL_NO_ERROR ) {
//        goto BUFFER_ERROR;
//    }

    if ( pboBuf == nullptr ) {
        goto BUFFER_ERROR;
    }

//    qDebug() << "[Gadget]rasterize pboData: " << pboBuf << " " << this;

    // it prevents showing the intermediate stage due to the resizing the map view when it enters into gadget
    tBuf = (unsigned int*)pboBuf;
    if (*tBuf == 0 && *(tBuf + 1) == 0 && *(tBuf + 2) == 0 && *(tBuf + 3) == 0 &&
        *(tBuf + 4) == 0 && *(tBuf + 5) == 0 && *(tBuf + 6) == 0 && *(tBuf + 7) == 0) {
        isBlank = true;
    }

    if ( !isBlank && mRunning && !mProvider.isNull() ){
        // write into hmi video buffer directly
        QImage image;
        auto fboFmt = mFbo->format().internalTextureFormat();
        bool hasAlpha = fboFmt == GL_RGBA || fboFmt == GL_BGRA|| fboFmt == GL_RGBA8;

//        qDebug() << "[Gadget]rasterize memcpy before fboFmt: " << fboFmt;

        if (fboFmt == GL_BGRA) {
            image = QImage((uchar*)pboBuf, mapSize.width(), mapSize.height(), hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);
        }
        else if ((fboFmt == GL_RGBA) || (fboFmt == GL_RGBA8)) {
            image = QImage((uchar*)pboBuf, mapSize.width(), mapSize.height(), hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGBX8888);
        }
        else {
            //qDebug() << "[Gadget]rasterize OpenGlOffscreenSurface::grabFramebuffer() - Unsupported framebuffer format" << fboFmt << "!";
        }

//        qDebug() << "[Gadget]rasterize memcpy after";

#ifdef QT_DEBUG
//        static int tick = 0;
//        if ( tick > 9 ) tick = 0;
//        image.save( UIConstants::gadgetPath( UIConstants::GadgetTest, tick++ ) );
#endif

        mTotalFrames++;

        T_PERF_LOG() << "[GadgetTest]rasterizeMap onGadgetRendered type: " << mMapType << " size: " << mapSize;
        setLastRenderedImage( mMapType, image );
        pushWithFocus( mMapType );
        if ( mMapType == mImmediateUpdate ) {
            qDebug() << "[3539]immediate update: " << mImmediateUpdate;
        }
    }

//    qDebug() << "[Gadget]rasterize after";

BUFFER_ERROR:
    if ( error != GL_NO_ERROR ) {
        //qDebug() << "[Gadget]rasterize error: " << error << " " << this;
    }

    mGlFunc.glUnmapBuffer(GL_ARRAY_BUFFER);
    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GadgetOffscreenRenderer::rasterizeNonMap()
{   
    static int pboIndex = 0;
    int nextPboIndex = 0;
    void *pboBuf = 0;
    bool isBlank = false;
    unsigned int* tBuf = 0;
    int xxlHeight = 0;

    pboIndex = (pboIndex + 1) % mPboNonMap.count;
    nextPboIndex = (pboIndex + 1 ) % mPboNonMap.count;

    auto status = mGlFunc.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( status != GL_FRAMEBUFFER_COMPLETE ) {
        //qDebug() << "[Gadget]rasterize skipped status: " << status;
        return;
    }

    T_PERF_LOG() << "[GadgetTest]rasterizeNonMap pbo id: " << mPboNonMap.ids[pboIndex];

    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboNonMap.ids[pboIndex]);
    auto error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    T_PERF_LOG() << "[Gadget]rasterize glReadPixels: before";
    xxlHeight = UIConstants::gadgetSize( UIConstants::GadgetXXL ).height();


    GLint prevAlignment, prevRowLength, prevSkipRows, prevSkipPixels;
    mGlFunc.glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
    mGlFunc.glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prevRowLength);
    mGlFunc.glGetIntegerv(GL_UNPACK_SKIP_ROWS, &prevSkipRows);
    mGlFunc.glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &prevSkipPixels);

    mGlFunc.glPixelStorei(GL_UNPACK_ROW_LENGTH, mWindow->width());
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_ROWS, xxlHeight);

    mGlFunc.glReadPixels(0, xxlHeight, mPboNonMap.size.width(), mPboNonMap.size.height(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboNonMap.ids[nextPboIndex]);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, mPboNonMap.ids[pboIndex]);
    pboBuf = mGlFunc.glMapBufferRange(GL_ARRAY_BUFFER, 0, mPboNonMap.bufferSize, GL_MAP_READ_BIT);
    if ( pboBuf == nullptr ) {
        goto BUFFER_ERROR;
    }

    T_PERF_LOG() << "[Gadget]rasterize pboData: " << pboBuf << " " << this;

    // it prevents showing the intermediate stage due to the resizing the map view when it enters into gadget
    tBuf = (unsigned int*)pboBuf;
//    if (*tBuf == 0 && *(tBuf + 1) == 0 && *(tBuf + 2) == 0 && *(tBuf + 3) == 0 &&
//        *(tBuf + 4) == 0 && *(tBuf + 5) == 0 && *(tBuf + 6) == 0 && *(tBuf + 7) == 0) {
//        isBlank = true;
//    }

    T_PERF_LOG() << "[Gadget]rasterize tBuf: " << *(tBuf + 0) << " " << *(tBuf + 1) << " " << *(tBuf + 2) << " " << *(tBuf + 3);

    if ( !isBlank && mRunning && !mProvider.isNull() ){
        // write into hmi video buffer directly
        QImage image;
        auto fboFmt = mFbo->format().internalTextureFormat();
        bool hasAlpha = fboFmt == GL_RGBA || fboFmt == GL_BGRA|| fboFmt == GL_RGBA8;

        T_PERF_LOG() << "[Gadget]rasterize memcpy before fboFmt: " << fboFmt;

        if (fboFmt == GL_BGRA) {
            image = QImage((uchar*)pboBuf, mPboNonMap.size.width(), mPboNonMap.size.height(), hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);
        }
        else if ((fboFmt == GL_RGBA) || (fboFmt == GL_RGBA8)) {
            image = QImage((uchar*)pboBuf, mPboNonMap.size.width(), mPboNonMap.size.height(), hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGBX8888);
        }
        else {
            //qDebug() << "[Gadget]rasterize OpenGlOffscreenSurface::grabFramebuffer() - Unsupported framebuffer format" << fboFmt << "!";
        }

//        qDebug() << "[Gadget]rasterize memcpy after";

        T_PERF_LOG() << "[GadgetTest]rasterize onGadgetRendered type: " << mActivations;

#ifdef QT_DEBUG
        //mProvider->push( UIConstants::GadgetTest, image );
        //image.save( UIConstants::gadgetPath( UIConstants::GadgetTest ) );
#endif

        for ( auto i = 0, count = mActivations.count(); i < count; i++ ) {
            auto viewType = mActivations[i];
            mTotalFrames++;

            QImage eachImage;

            // Cropping

            // 1. QImage copy

            auto viewSize = UIConstants::gadgetSize( viewType );

            switch ( viewType ) {
            case UIConstants::GadgetLarge:
                {
                    eachImage = image.copy(0, 0, viewSize.width(), viewSize.height() );
                }
                break;
            case UIConstants::GadgetMedium:
                {
                    auto lSize = UIConstants::gadgetSize( UIConstants::GadgetLarge );
                    eachImage = image.copy(lSize.width(), 0, viewSize.width(), viewSize.height() );
                }
                break;
            case UIConstants::GadgetSmall:
                {
                    auto lSize = UIConstants::gadgetSize( UIConstants::GadgetLarge );
                    auto mSize = UIConstants::gadgetSize( UIConstants::GadgetMedium );
                    eachImage = image.copy(lSize.width(), mSize.height(), viewSize.width(), viewSize.height() );
                }
                break;
            }

            T_PERF_LOG() << "[GadgetTest]rasterizeNonMap onGadgetRendered type: " << viewType;

            if ( !eachImage.isNull() ) {
                setLastRenderedImage( viewType, eachImage );
                pushWithFocus( viewType );
            }

//#ifdef QT_DEBUG
//            static int tick = 0;
//            if ( tick > 9 ) tick = 0;
//            image.save( UIConstants::gadgetPath( viewType, tick++ ) );
//#endif
        }
    }

//    qDebug() << "[Gadget]rasterize after";

BUFFER_ERROR:
    if ( error != GL_NO_ERROR ) {
        //qDebug() << "[GadgetTest]rasterize error: " << error << " " << this;
    }

    mGlFunc.glUnmapBuffer(GL_ARRAY_BUFFER);
    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, 0);

    mGlFunc.glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
    mGlFunc.glPixelStorei(GL_UNPACK_ROW_LENGTH, prevRowLength);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_ROWS, prevSkipRows);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_PIXELS, prevSkipPixels);
}

void GadgetOffscreenRenderer::rasterizeNonMapImmediately()
{
    static int pboIndex = 0;
    int nextPboIndex = 0;
    void *pboBuf = 0;
    bool isBlank = false;
    unsigned int* tBuf = 0;
    int skipRows = 0;
    int skipPixels = 0;
    auto size = UIConstants::gadgetSize( mImmediateUpdate );

    pboIndex = (pboIndex + 1) % mPboNonMap.count;
    nextPboIndex = (pboIndex + 1 ) % mPboNonMap.count;

    auto status = mGlFunc.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( status != GL_FRAMEBUFFER_COMPLETE ) {
        //qDebug() << "[Gadget]rasterize skipped status: " << status;
        return;
    }

    T_PERF_LOG() << "[GadgetTest]rasterizeNonMapImmediately pbo id: " << mPboNonMap.ids[pboIndex];

    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboNonMap.ids[pboIndex]);
    auto error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    T_PERF_LOG() << "[Gadget]rasterize glReadPixels: before";

    switch( mImmediateUpdate ) {
    case UIConstants::GadgetLarge:
        skipRows = UIConstants::gadgetSize( UIConstants::GadgetXXL ).height();
        skipPixels = 0;
        break;
    case UIConstants::GadgetMedium:
        skipRows = UIConstants::gadgetSize( UIConstants::GadgetXXL ).height();
        skipPixels = UIConstants::gadgetSize( UIConstants::GadgetLarge ).width();
        break;
    case UIConstants::GadgetSmall:
        skipRows = UIConstants::gadgetSize( UIConstants::GadgetXXL ).height();
        skipRows += UIConstants::gadgetSize( UIConstants::GadgetMedium ).height();
        skipPixels = UIConstants::gadgetSize( UIConstants::GadgetLarge ).width();
        break;
    }

    GLint prevAlignment, prevRowLength, prevSkipRows, prevSkipPixels;
    mGlFunc.glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
    mGlFunc.glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prevRowLength);
    mGlFunc.glGetIntegerv(GL_UNPACK_SKIP_ROWS, &prevSkipRows);
    mGlFunc.glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &prevSkipPixels);

    mGlFunc.glPixelStorei(GL_UNPACK_ROW_LENGTH, mWindow->width());
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_PIXELS, skipPixels);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_ROWS, skipRows);

    mGlFunc.glReadPixels(skipPixels, skipRows, size.width(), size.height(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboNonMap.ids[nextPboIndex]);
    error = mGlFunc.glGetError();
    if ( error != GL_NO_ERROR ) {
        goto BUFFER_ERROR;
    }

    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, mPboNonMap.ids[pboIndex]);
    pboBuf = mGlFunc.glMapBufferRange(GL_ARRAY_BUFFER, 0, mPboNonMap.bufferSize, GL_MAP_READ_BIT);
    if ( pboBuf == nullptr ) {
        goto BUFFER_ERROR;
    }

    T_PERF_LOG() << "[Gadget]rasterize pboData: " << pboBuf << " " << this;

    // it prevents showing the intermediate stage due to the resizing the map view when it enters into gadget
    tBuf = (unsigned int*)pboBuf;

    if ( !isBlank && mRunning && !mProvider.isNull() ){
        // write into hmi video buffer directly
        QImage image;
        auto fboFmt = mFbo->format().internalTextureFormat();
        bool hasAlpha = fboFmt == GL_RGBA || fboFmt == GL_BGRA|| fboFmt == GL_RGBA8;

//        qDebug() << "[Gadget]rasterize memcpy before fboFmt: " << fboFmt;

        if (fboFmt == GL_BGRA) {
            image = QImage((uchar*)pboBuf, size.width(), size.height(), hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);
        }
        else if ((fboFmt == GL_RGBA) || (fboFmt == GL_RGBA8)) {
            image = QImage((uchar*)pboBuf, size.width(), size.height(), hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGBX8888);
        }
        else {
            //qDebug() << "[Gadget]rasterize OpenGlOffscreenSurface::grabFramebuffer() - Unsupported framebuffer format" << fboFmt << "!";
        }

        mTotalFrames++;

        T_PERF_LOG() << "[GadgetTest]rasterizeMap onGadgetRendered type: " << mMapType << " size: " << size;
        setLastRenderedImage( mImmediateUpdate, image );
        pushWithFocus( mImmediateUpdate );
        qDebug() << "[3539]immediate update: " << mImmediateUpdate;

//#ifdef QT_DEBUG
//        static int tick = 0;
//        if ( tick > 9 ) tick = 0;
//        image.save( UIConstants::gadgetPath( mImmediateUpdate, tick++ ) );
//#endif
    }

BUFFER_ERROR:
    if ( error != GL_NO_ERROR ) {
        //qDebug() << "[GadgetTest]rasterize error: " << error << " " << this;
    }

    mGlFunc.glUnmapBuffer(GL_ARRAY_BUFFER);
    mGlFunc.glBindBuffer(GL_ARRAY_BUFFER, 0);

    mGlFunc.glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
    mGlFunc.glPixelStorei(GL_UNPACK_ROW_LENGTH, prevRowLength);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_ROWS, prevSkipRows);
    mGlFunc.glPixelStorei(GL_UNPACK_SKIP_PIXELS, prevSkipPixels);
}

int GadgetOffscreenRenderer::lastRenderedImageIndex( UIConstants::GadgetType type )
{
    int ret = -1;

    switch( type ) {
    case UIConstants::GadgetXXL:
        ret = 0;
        break;
    case UIConstants::GadgetXL:
        ret = 1;
        break;
    case UIConstants::GadgetLarge:
        ret = 2;
        break;
    case UIConstants::GadgetMedium:
        ret = 3;
        break;
    case UIConstants::GadgetSmall:
        ret = 4;
        break;
    }

    return ret;
}

void GadgetOffscreenRenderer::setLastRenderedImage( UIConstants::GadgetType type, const QImage& image )
{
    QImage img = image.copy();

    switch( type ) {
    case UIConstants::GadgetXXL:
        mLastRenderedImages[0] = img;
        mLastFocusedRenderedImages[0] = QImage();
        break;
    case UIConstants::GadgetXL:
        mLastRenderedImages[1] = img;
        mLastFocusedRenderedImages[1] = QImage();
        break;
    case UIConstants::GadgetLarge:
        mLastRenderedImages[2] = img;
        mLastFocusedRenderedImages[2] = QImage();
        break;
    case UIConstants::GadgetMedium:
        mLastRenderedImages[3] = img;
        mLastFocusedRenderedImages[3] = QImage();
        break;
    case UIConstants::GadgetSmall:
        mLastRenderedImages[4] = img;
        mLastFocusedRenderedImages[4] = QImage();
        break;
    }
}

void GadgetOffscreenRenderer::updateFocus( UIConstants::GadgetType type, bool focused, const QColor &outlineColor, int width )
{
    QMutexLocker lock( &mMutex );

    auto index = lastRenderedImageIndex( type );
    if ( index == -1 ) {
        qInfo() << "[Gadget]updateFocus there should be a rendered image corresponding to it before it gets focused";
        return;
    }

    mFocusStates[index] = focused;
    if (mFocusColor != outlineColor) {
        mFocusColor = outlineColor;
        mLastFocusedRenderedImages[index] = QImage{};
    }
    mFocusWidth = width;

    pushWithFocus( type );
}

void GadgetOffscreenRenderer::pushWithFocus( UIConstants::GadgetType type )
{
    auto index = lastRenderedImageIndex( type );
    if ( index == -1 ) {
        qInfo() << "[Gadget]pushWithFocus #1 a wrong gadget type has been sent. type: " << (int)type;
        return;
    }
    QImage image = mLastRenderedImages[index];
    if ( image.isNull() ) {
        qInfo() << "[Gadget]pushWithFocus #2 there should be a rendered image corresponding to it before it gets focused. type: " << (int)type;
        return;
    }
    QImage focusedImage = mLastFocusedRenderedImages[index];
    auto focused = mFocusStates[index];
    auto thickness = mFocusWidth - 1;

    if ( focused ) {
        if ( focusedImage.isNull() ) {
            focusedImage = image;
            auto w = focusedImage.width();
            auto h = focusedImage.height();
            QPainter painter(&focusedImage);
            QPen pen(mFocusColor);
            pen.setWidth(2);
            painter.setPen(pen);

            QBrush brush(mFocusColor);
            painter.setBrush(brush);

            painter.drawRect(0, 0, w, thickness);
            painter.drawRect(0, 0, thickness, h);
            painter.drawRect(w-thickness, 0, w, h);
            painter.drawRect(0, h-thickness, w, h);

            mLastFocusedRenderedImages[index] = focusedImage;
        }

        image = focusedImage;
    }

    mProvider->push( type, image );

//    static int tick = 0;

//    if ( focused ) {
//        qDebug() << "[3539]push #1 type: " << type << " focused";
//        image.save( UIConstants::gadgetPath( type, "focused", tick++ ) );
//    }
//    else {
//        qDebug() << "[3539]push #2 type: " << type << " not focused";
//        image.save( UIConstants::gadgetPath( type, "not_focused", tick++ ) );
//    }
}
