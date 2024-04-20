#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <QPoint>

struct VSMMarkerStyle;

namespace SKT {

class MarkerManagerPrivate;

T_SHAREABLE( MarkerManagerPrivate )

class MarkerManager : public QObject
{
    Q_OBJECT

public:
    // 17.12.11 hskim not used
//    static MarkerManager *sharedInstance();
//    static void destroyInstance();

    MarkerManager();
    virtual ~MarkerManager();
    Q_DISABLE_COPY(MarkerManager)

    // 17.12.8 hsim apply multi map view
    void attachMapView(QObject* mapView);
    void detachMapView(QObject* mapView);

    bool initialize();

public slots:
    void clear(QObject* mapView);
    void setVia(QObject* mapView, int viaId, const QPoint &pos);
    void removeVia(QObject* mapView, int viaId);
    void addSearchMarker(QObject* mapView, int id, int focusIdx, const QString& text, int x, int y);
    void removeSearchMarker(QObject* mapView, int id, int x, int y);

private:
    //MarkerManagerPrivate *d;
    QMap<QObject*,MarkerManagerPrivatePtr> mDataMap;
    static MarkerManager *sInstance;
};

}

#endif // MARKERMANAGER_H
