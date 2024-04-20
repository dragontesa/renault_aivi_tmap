#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

namespace SKT {

T_SHAREABLE( ImageStreamProvider )

class OffscreenRenderer : public QObject
{
public:
    virtual ~OffscreenRenderer() {}

    virtual void setImageStreamProvider( const ImageStreamProviderPtr& provider ) = 0;

    virtual void start() = 0;

    virtual void stop() {}
};

}

#endif // OFFSCREENRENDERER_H
