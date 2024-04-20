#ifndef ICONGADGETPROVIDER_H
#define ICONGADGETPROVIDER_H

#include "ImageStreamProvider.h"

namespace SKT
{

class NavigationController;

T_SHAREABLE( IconGadgetProvider )

class IconGadgetProvider : public QObject
{
    Q_OBJECT
public:
    IconGadgetProvider( QQuickWindow* window, NavigationController* controller );

    ~IconGadgetProvider();

    void start( int type, const QString& imgFile );
    void stop( int type );
    void update(int type, bool focus, bool pressed, const QColor &outlineColor, int width);

private slots:
    void onWindowAfterRendering();

private:        
    T_SHAREABLE(Provider)
    ProviderPtr provider( int type );

    NavigationController* mController;

    class Provider {
    public:
        Provider() {}
        ~Provider() {}
        void reset();

        int type = 0;
        int tick = 0;
        bool active = false;
        bool focus = false;
        bool update = false;
        bool exchange = false;
        QImage image;
        QColor outlineColor;
        QImage focusImage;
        QElapsedTimer elapsedTimer;      
    };
    QMap<int,ProviderPtr> mProviders;    
    const static int FPS = 2;
    QMutex mMutex;
    QList<ProviderPtr> mActiveProviders;
    QQuickWindow* mWindow = nullptr;
};

}

#endif // ICONGADGETPROVIDER_H
