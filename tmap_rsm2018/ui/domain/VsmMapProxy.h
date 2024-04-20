#ifndef VSMPROXY_H
#define VSMPROXY_H

#include "AbstractProxy.h"
#include <QPoint>
#include "view/mediator/app/MapViewMode.h"
#include "VSM_Enum.h"
#include "VsmMapSettingAndTheme.h"

namespace SKT {

T_SHAREABLE( VsmMapProxy );

class VsmMapProxy : public AbstractProxy
{
    Q_OBJECT

public:
    VsmMapProxy();

    void setGlobalMapView(QObject* mapView);

    VsmMapSetting setting( VsmMapType type );
    void applySetting( QObject* mapView, const VsmMapSetting& setting);

    VsmMapTheme theme( VSM_ThemeType themeType );
    void applyTheme( QObject* mapView, VSM_ThemeType themeType, const VsmMapTheme& theme);

    void viewLevel(QObject* mapView, int& level, int& subLevel);
    void setViewLevel(QObject* mapView, int level, int subLevel, bool isAnim = false);

    void update(QObject* mapView);
    void drawRouteAll(QObject* mapView, int routeIndex, int w, int h, int flag = 0);
    void showCarvata(QObject* mapView, bool show);
    void removeCalloutPopup(QObject* mapView, bool animate);

    QPoint currentPosition(QObject* mapView, int& x, int& y) const;
    QPoint centerPosition(QObject* mapView) const;
    QSize mapSize(QObject* mapView) const;

    void moveTo(QObject* mapView, const QPoint& pos);
    void moveTo(QObject* mapView, const QPoint& pos, bool anim);
    QPoint screenToWorld(QObject* mapView, int x, int y);

    QRect requestMapRect(QObject* mapView);
    void drawRouteAll(QObject* mapView, bool isArrivalBelowDeparture, bool isSummaryView=false);
    void setShowRoute(QObject* mapView, bool show);

    QString areaName(int wx, int wy) const;

private:
    QRect routeBoundingRect() const;
    QObject* chooseMapView(QObject* mapView) const;
    void* chooseMapViewHandle(QObject* mapView) const;
    QObject* mGlobalMapView = nullptr;
    void* mGlobalMapViewHandle = nullptr;
};

}

#endif // VSMPROXY_H
