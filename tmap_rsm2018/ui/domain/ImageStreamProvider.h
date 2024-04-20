#ifndef IMAGESTREAMPROVIDER_H
#define IMAGESTREAMPROVIDER_H

namespace SKT
{

class NavigationController;

class ImageStreamProvider : public QObject
{
public:
    virtual ~ImageStreamProvider() {}

    virtual void push( int type, const QImage& image ) = 0;
};

}

#endif // IMAGESTREAMPROVIDER_H
