#include "IconGadgetProvider.h"
#include "../common/navigationcontroller.h"
#include "QmlAppHelper.h"
#include "ResourceResolver.h"
#include <QPainter>
#include <QElapsedTimer>

using namespace SKT;

IconGadgetProvider::IconGadgetProvider( QQuickWindow* window, NavigationController* controller )
    : mWindow( window ),
      mController( controller )
{
}

IconGadgetProvider::~IconGadgetProvider()
{
    QMutexLocker lock( &mMutex );

    mActiveProviders.clear();
    mProviders.clear();

    disconnect(mWindow, SIGNAL(afterRendering()), this, SLOT(onWindowAfterRendering()));
}

#define DRAW_ICON_GADGET_OUTLINE  // 아이콘 가젯 외곽선 그리기 기능 활성화

void IconGadgetProvider::onWindowAfterRendering()
{
    QMutexLocker lock( &mMutex );

    struct Internal {
        inline static void changeImage( NavigationController* controller, QList<ProviderPtr>& activeProviders, int index ) {
            if (activeProviders.count() <= index ) {
                return;
            }
            auto p = activeProviders[index];

#ifdef DRAW_ICON_GADGET_OUTLINE
            if (p->update) {
                if (p->focus) {
                    controller->changeIconGadgetImage( p->focusImage, p->type );
                } else {
                    controller->changeIconGadgetImage( p->image, p->type );
                }
                if (p->exchange == false) {
                    p->exchange = true;
                } else {
                    p->update = false;
                    p->exchange = false;
                }
                return;
            }
#endif

            auto elapsed = p->elapsedTimer.elapsed();
            if ( p->tick < 10) {
                if ( elapsed > 1000 / IconGadgetProvider::FPS ) {
                    if (p->focus) {
                        controller->changeIconGadgetImage( p->focusImage, p->type );
                    } else {
                        controller->changeIconGadgetImage( p->image, p->type );
                    }

                    p->elapsedTimer.restart();
                    p->tick++;
                }
            }
            else {
#ifdef DRAW_ICON_GADGET_OUTLINE
                //activeProviders.removeAt( index );
#else
                activeProviders.removeAt( index );
#endif
            }
        }
    };

    if ( mActiveProviders.count() == 0 ) {
        disconnect(mWindow, SIGNAL(afterRendering()), this, SLOT(onWindowAfterRendering()));
    }
    else {
        if ( mActiveProviders.count() == 1 ) {
            Internal::changeImage( mController, mActiveProviders, 0 );
        }
        else if ( mActiveProviders.count() == 2 ) {
            Internal::changeImage( mController, mActiveProviders, 0 );
            Internal::changeImage( mController, mActiveProviders, 1 );
        }
        else if ( mActiveProviders.count() == 3 ) {
            Internal::changeImage( mController, mActiveProviders, 0 );
            Internal::changeImage( mController, mActiveProviders, 1 );
            Internal::changeImage( mController, mActiveProviders, 2 );
        }
    }
}

IconGadgetProvider::ProviderPtr IconGadgetProvider::provider( int type )
{
    ProviderPtr ret;

    if ( !mProviders.contains( type ) ) {
        ret = t_new_shared<Provider>();
        ret->type = type;
        mProviders[type] = ret;
    }

    ret = mProviders[type];

    return ret;
}

void IconGadgetProvider::Provider::reset()
{
    tick = 0;
    active = false;
    focus = false;
    update = false;
    exchange = false;
    image = QImage();
    focusImage = QImage();
    elapsedTimer = QElapsedTimer();
}

void IconGadgetProvider::start(int type, const QString& imgFile )
{
    QMutexLocker lock( &mMutex );

    qDebug() << "[Gadget][IconGadgetProvider]start type: " << type;

    connect(mWindow, SIGNAL(afterRendering()), this, SLOT(onWindowAfterRendering()), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

    auto p = provider( type );
    p->reset();

    p->active = true;
    p->tick = 0;
    auto imgPath = ResourceResolver::instance()->imagePath( QStringLiteral("gadget"), imgFile );
    if ( !p->image.load( imgPath ) ) {
        p->active = false;
        qDebug() << QString( "Could not find the image with the name of '%1'" ).arg( imgPath );
    }
    else {
        struct Internal {
            static void convertToBgraPremulti(QImage& image) {
                int h = image.height();
                int w = image.width();
                for (int y = 0; y < h; y++) {
                    auto pixels = reinterpret_cast<QRgb*>(image.scanLine(y));
                    for (int x = 0; x < w; x++) {
                        auto r = qRed(pixels[x]);
                        auto g = qGreen(pixels[x]);
                        auto b = qBlue(pixels[x]);
                        auto a = qAlpha(pixels[x]);
                        r = int(r * a / 255);
                        g = int(g * a / 255);
                        b = int(b * a / 255);

                        pixels[x] = qRgba(b,g,r,a);
                    }
                }
            }
        };

        Internal::convertToBgraPremulti(p->image);

        p->elapsedTimer.restart();
        mActiveProviders.removeOne( p );
        mActiveProviders.append( p );
        qDebug() << QString( "[Gadget][IconGadgetProvider]start actives count: %1" ).arg( mActiveProviders.count() );
    }
}

void IconGadgetProvider::stop( int type )
{
    QMutexLocker lock( &mMutex );

    auto p = provider( type );
    p->active = false;

    mActiveProviders.removeOne( p );

    int count = mActiveProviders.count();
    qDebug() << QString( "[Gadget][IconGadgetProvider]stop actives count: %1" ).arg( mActiveProviders.count() );
}

void IconGadgetProvider::update(int type, bool focus, bool pressed, const QColor &outlineColor, int width)
{
    QMutexLocker lock( &mMutex );

    auto p = provider( type );

    if (p->active) {
        auto updateColor = false;

        if (p->focusImage.isNull()) {
            p->focusImage = p->image;
            updateColor = true;
        }
        auto colorChanged = p->outlineColor != outlineColor;
        auto dontUpdate = !focus && !pressed;

        if (updateColor || (colorChanged && !dontUpdate)) {
            p->outlineColor = outlineColor;
            auto w = p->image.width();
            auto h = p->image.height();
            if (pressed) {
                p->focusImage = p->image;
            }
            QPainter painter(&p->focusImage);
            QPen pen(outlineColor);
            pen.setWidth(width);
            painter.setPen(pen);

            QBrush brush(outlineColor);
            painter.setBrush(brush);

            auto thickness = width - 1;

            painter.drawRect(0, 0, w, thickness);
            painter.drawRect(0, 0, thickness, h);
            painter.drawRect(w-thickness, 0, w, h);
            painter.drawRect(0, h-thickness, w, h);
        }

        p->focus = focus;
        p->update = true;
        p->exchange = false;
    }
}
