#ifndef GADGETOFFSCREENRENDERER_H
#define GADGETOFFSCREENRENDERER_H

#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <QMutex>
#include <QElapsedTimer>
#include "OffscreenRenderer.h"
#include "uiconstants.h"
#include <QPainter>

namespace SKT {

struct GadgetActivation
{
    UIConstants::GadgetType type;
    enum Reason {
        Activation,
        Focus
    };
    Reason reason;
};

class GadgetOffscreenRenderer : public OffscreenRenderer
{
    Q_OBJECT
public:
    GadgetOffscreenRenderer( QQuickWindow* win );
    ~GadgetOffscreenRenderer();

    void setImageStreamProvider( const ImageStreamProviderPtr& provider );

    void start();    
    void stop();

    void updateActivations( int tag, const QList<UIConstants::GadgetType>& activations );
    void updateImmediately( UIConstants::GadgetType type );
    void updateFocus( UIConstants::GadgetType type, bool focused, const QColor &outlineColor, int width );

signals:
    void stopped();
    void windowHasRendered();

private slots:
    void onGadgetBeforeRendering();
    void onGadgetAfterRendering();
    void onFrameSwapped();

private:
    void prepareBuffer();
    void sync();

    void rasterize();
    void rasterizeMap();
    void rasterizeNonMap();
    void rasterizeNonMapImmediately();

    bool hasMap( UIConstants::GadgetType type );

    int lastRenderedImageIndex( UIConstants::GadgetType type );
    void setLastRenderedImage( UIConstants::GadgetType type, const QImage& image );

    void pushWithFocus( UIConstants::GadgetType type );

    ImageStreamProviderPtr mProvider;
    QQuickWindow* mWindow = nullptr;
    QOpenGLFramebufferObject* mFbo = nullptr;

    struct Pbo {
        int count = 2;
        QSize size;
        GLuint ids[2];
        int bufferSize = 0;
    };

    Pbo mPboMap;
    Pbo mPboNonMap;    

    QOpenGLExtraFunctions mGlFunc;
    bool mIsGlFuncInitiated = false;
    GLsync mSync;

    QMutex mMutex;
    bool mRunning = false;
    int mTotalFrames = 0;

    const int MAP_FPS = 15;
    const int NONMAP_FPS = 1;

    bool mHasMap = false;
    bool mHasNonMap = false;    
    UIConstants::GadgetType mMapType = UIConstants::GadgetNone;
    bool mMapUpdate = false;
    bool mNonMapUpdate = false;

    QElapsedTimer mMapUpdateTimer;
    QElapsedTimer mNonMapUpdateTimer;

    QList<UIConstants::GadgetType> mActivations;
    bool mCheckIfWindowHasRendered = false;

    QList<UIConstants::GadgetType> mImmediateUpdates;
    UIConstants::GadgetType mImmediateUpdate = UIConstants::GadgetNone;

    QImage mLastRenderedImages[5];
    QImage mLastFocusedRenderedImages[5];
    QColor mFocusColor;
    bool mFocusStates[5] = {false,};
    int mFocusWidth = 4;
};

}

#endif // QMLOFFSCREENRENDERER_H
