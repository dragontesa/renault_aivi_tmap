#include <QImage>
#include <QDebug>

#include "MarkerManager.h"
#include "VSM_Marker.h"
#include <QPainter>
#include "ResourceResolver.h"

#define GRAY_ICON   0
#define BLUE_ICON   1

namespace SKT {

//=============================================================================
//  MarkerManager (refactoring.)
//=============================================================================
class MarkerManagerPrivate
{
public:
    MarkerManagerPrivate();
    MarkerManagerPrivate(QObject* mapView);

    enum {
        ViaMarkerIdBase = 0x1000,
    };

    bool initialize();
    bool loadImages();
    bool installViaMarkers();
    void setVia(int id, const QPoint &pos);
    void addSearchMarker(int id, int focusIdx, const QString& text, int x, int y);
    void removeSearchMarker(int id, int x, int y);

    int viaMarkerId(int index) const;
    QString resourcePath(const QString &imagePath) const;

    QImage mImages[7];
    QImage mLargeNumberImages[2][10];
    QImage mSmallNumberImages[2][10];
    VSMMarkerStyle mMarkerStyle;
    qreal mScale;
    bool mInitialized;    
    void* mMapViewHandle = nullptr;
};

MarkerManagerPrivate::MarkerManagerPrivate(QObject* mapView)
    : mInitialized(false)
    , mScale(1.0)//0.6)
{    
    mMapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));

    if (initialize()) {
        mInitialized = true;//installViaMarkers();
    }

    Q_ASSERT(mInitialized == true);
}

bool MarkerManagerPrivate::initialize()
{
    memset(&mMarkerStyle, 0, sizeof(VSMMarkerStyle));

    mMarkerStyle.fillColor = { 0xff, 0xff, 0xff, 0xff };
    mMarkerStyle.lineWidth = 0;
    mMarkerStyle.fontSize = 0;
    mMarkerStyle.showPriority = 0;
    mMarkerStyle.iconAnchorX = 0.5;
    mMarkerStyle.iconAnchorY = 1.0;
    mMarkerStyle.iconDispWidth = 0;
    mMarkerStyle.iconDispHeight = 0;
    mMarkerStyle.renderType = VSM_POINT_MAKER_RENDER_SYMBOL_ONLY;

    return loadImages();
}

static void rgbSwap(QImage& image) { image = image.rgbSwapped(); }
static bool loadNumberImage(const QString& path, QImage& image) {
    if (image.load(path)) {
        image = image.rgbSwapped();
        return true;
    }
    return false;
}
bool MarkerManagerPrivate::loadImages()
{
    bool ok = true;
    const char *sources[] = {
        "poi_map_departure.png",
        "poi_map_passby1.png",
        "poi_map_passby2.png",
        "poi_map_passby3.png",
        "poi_map_arrival.png",
        "poi_map_gray.png",
        "poi_map_blue.png"
    };
    for (int i=0; i<7; i++) {
        QString source = sources[i];

        auto imgPath = ResourceResolver::instance()->imagePath( source );

        bool loaded = mImages[i].load(imgPath);
        if (!loaded) {
            qDebug() << QString("MarkerManager::initialize load failed -> %1").arg(source);
            ok = false;
        } else {
            rgbSwap(mImages[i]);
        }
    }
    for (int i=0; i<10; i++) {
        // Gray
        auto source = QString("poi_num_%1_s.png").arg(i);
        auto sourcePath = ResourceResolver::instance()->imagePath(source);
        if (!loadNumberImage(sourcePath, mLargeNumberImages[GRAY_ICON][i])) {
            ok = false;
        }
        source = QString("poi_num_%1_s2.png").arg(i);
        sourcePath = ResourceResolver::instance()->imagePath(source);
        if (!loadNumberImage(sourcePath, mSmallNumberImages[GRAY_ICON][i])) {
            ok = false;
        }
        // Blue
        source = QString("poi_num_%1_b.png").arg(i);
        sourcePath = ResourceResolver::instance()->imagePath(source);
        if (!loadNumberImage(sourcePath, mLargeNumberImages[BLUE_ICON][i])) {
            ok = false;
        }
        source = QString("poi_num_%1_b2.png").arg(i);
        sourcePath = ResourceResolver::instance()->imagePath(source);
        if (!loadNumberImage(sourcePath, mSmallNumberImages[BLUE_ICON][i])) {
            ok = false;
        }
    }
    return ok;
}

bool MarkerManagerPrivate::installViaMarkers()
{
    VSM_Bool res = VSM_TRUE;
    for (auto i=0; i<7; i++) {
        auto &image = mImages[i];
        VSMImage vsmImage = {
            (VSM_UInt16)image.width(),
            (VSM_UInt16)image.height(),
            (VSM_UInt16)image.bytesPerLine(),
            (VSM_UInt16)image.depth(),
            (VSM_UInt8*)image.bits(),
            (VSM_UInt32)image.byteCount()
        };

        mMarkerStyle.iconDispWidth = image.width();
        mMarkerStyle.iconDispHeight= image.height();

        auto id = viaMarkerId(i);
        VSMPoint2i pos = { 0, 0 };

        QString markerId = QString::number(id);
        res = VSMMap_Marker_AddPoint(mMapViewHandle, markerId.toUtf8().data(), pos, nullptr, 0, &mMarkerStyle, &vsmImage);

        if (res == VSM_FALSE)
            break;
    }

    return res == VSM_TRUE;
}

void MarkerManagerPrivate::setVia(int id, const QPoint &pos)
{
    Q_ASSERT(0 <= id && id < 5);

    auto viaId = viaMarkerId(id);
    QString markerId = QString::number(viaId);

    if (pos.isNull()) {
        VSMMap_Marker_Remove(mMapViewHandle, markerId.toUtf8().data());
    } else {
        const QImage &image = mImages[id];

        mMarkerStyle.iconDispWidth = image.width() * mScale;
        mMarkerStyle.iconDispHeight = image.height() * mScale;
        mMarkerStyle.touchable = false;

        VSMImage vsmImage = {
            (VSM_UInt16)image.width(),
            (VSM_UInt16)image.height(),
            (VSM_UInt16)image.bytesPerLine(),
            (VSM_UInt16)image.depth(),
            (VSM_UInt8*)image.bits(),
            (VSM_UInt32)image.byteCount()
        };        

        VSM_Bool result = VSMMap_Marker_AddPoint(mMapViewHandle, markerId.toUtf8().data(), {pos.x(), pos.y()}, nullptr, 0, &mMarkerStyle, &vsmImage);
    }
}

void drawNumber(QPainter& painter, QImage& dest, QImage* numbers, const QString& text, bool focused) {
    QSize imageSize = dest.size();
    QSize numberSize = numbers[0].size();
    auto len = text.length();
    auto textWidth = numberSize.width() * len;
    auto x = (imageSize.width() - textWidth) / 2;
    auto y = focused ? 20 : 12;
    for (auto i=0; i<len; i++) {
        auto index = text.at(i).toLatin1() - '0';
        Q_ASSERT(0<=index && index<10);
        painter.drawImage(x, y, numbers[index]);
        x += numberSize.width();
    }
}

void MarkerManagerPrivate::addSearchMarker(int id, int focusIdx, const QString& text, int x, int y)
{  
    auto focused = id == focusIdx;
    QImage image = focused ? mImages[6] : mImages[5];

    QPainter painter(&image);
    int icon = focused ? BLUE_ICON : GRAY_ICON;
    if (text.length() < 4) {
        drawNumber(painter, image, mLargeNumberImages[icon], text, focused);
    } else {
        drawNumber(painter, image, mSmallNumberImages[icon], text, focused);
    }

    mMarkerStyle.iconDispWidth = image.width() * mScale;
    mMarkerStyle.iconDispHeight = image.height() * mScale;
    mMarkerStyle.renderType =  VSM_PointMarkerRenderType::VSM_POINT_MAKER_RENDER_SYMBOL_ONLY;
    mMarkerStyle.touchable = true;

    VSMImage vsmImage = {
        (VSM_UInt16)image.width(),
        (VSM_UInt16)image.height(),
        (VSM_UInt16)image.bytesPerLine(),
        (VSM_UInt16)image.depth(),
        (VSM_UInt8*)image.bits(),
        (VSM_UInt32)image.byteCount()
    };

    VSMPoint2i pos = {x, y};
    QString markerId = QString::number(id);
    VSMMap_Marker_AddPoint(mMapViewHandle, markerId.toUtf8().data(), pos, nullptr, 0, &mMarkerStyle, &vsmImage);
}

void MarkerManagerPrivate::removeSearchMarker(int id, int x, int y)
{
    QString markerId = QString::number(id);
    VSMMap_Marker_Remove(mMapViewHandle, markerId.toUtf8().data());
}

int MarkerManagerPrivate::viaMarkerId(int index) const
{
    return ViaMarkerIdBase + index;
}

QString MarkerManagerPrivate::resourcePath(const QString &imagePath) const
{
    return ResourceResolver::instance()->imageUrl(imagePath);
}

MarkerManager::MarkerManager()
{
}

MarkerManager::~MarkerManager()
{
}

void MarkerManager::attachMapView(QObject* mapView)
{
    if ( mapView != nullptr ) {
        mDataMap[mapView] = t_new_shared<MarkerManagerPrivate>(mapView);
    }
}

void MarkerManager::detachMapView(QObject* mapView)
{
    mDataMap.remove(mapView);
}

void MarkerManager::clear(QObject* mapView)
{
    auto handle = qvariant_cast<void*>(mapView->property("nativeHandle"));
    VSMMap_Marker_RemoveAll(handle);
}

void MarkerManager::setVia(QObject* mapView, int viaId, const QPoint &pos)
{
    auto d = mDataMap[mapView];
    d->setVia(viaId, pos);
}

void MarkerManager::removeVia(QObject* mapView, int viaId)
{
    auto d = mDataMap[mapView];
    d->setVia(viaId, QPoint{});
}

void MarkerManager::addSearchMarker(QObject* mapView, int id, int focusIdx, const QString& text, int x, int y)
{
    auto d = mDataMap[mapView];
    d->addSearchMarker(id, focusIdx, text, x, y);
}

void MarkerManager::removeSearchMarker(QObject* mapView, int id, int x, int y)
{
    auto d = mDataMap[mapView];
    d->removeSearchMarker(id, x, y);
}


}
