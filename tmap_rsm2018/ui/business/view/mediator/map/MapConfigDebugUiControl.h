#ifndef MAPCONFIGDEBUGUIOVERLAY_H
#define MAPCONFIGDEBUGUIOVERLAY_H

#include <QObject>
#include <QJsonObject>
#include "VSM_Map.h"

namespace SKT {

class MapConfigDebugUiControl
        : public QObject
{
    Q_OBJECT
public:
    explicit MapConfigDebugUiControl(QObject *parent = nullptr);
    ~MapConfigDebugUiControl();
    enum kMenuId {
        K_MenuMapDensity = 0,
        K_MenuMapTileSidkCache,
        K_MenuMapBuildingFilterMode,
        K_MenuMapFovy,
        K_MenuMapCopycatBuilding,
        K_MenuMapShowPoi,
        K_MenuMapPoiScale,
        K_MenuMapPoiScaleForce,
        K_MenuMapPoiOcclusion,
        K_MenuMapRenderSubway,
        K_MenuMapRouteTi,
        K_MenuSelectTheme, // 11
        K_MenuMapRouteLineStyle,
        K_MenuMapRouteLineTurnpointStyle,
        K_MenuMapRouteCandidateDefaultStyle,
        K_MenuMapRouteCandidateSelectedStyle,
        K_MenuMapRouteAlterateStyle,
        K_MenuMapTrackPointStyle,
        K_MenuMap3dLandmark, // 18
        K_MenuMapBDOutline3d,
        K_MenuMapBDOutline2d,
        K_MenuMapShowOnly2d,
        k_MenuMax,
    };

    Q_INVOKABLE void setConfigSettings();
    Q_INVOKABLE void saveConfigSettings();
    Q_INVOKABLE void connectUi(QObject* qmlObj, QObject* mapView);

private:
    void initRouteStyleTheme();
    void setThemeSettings();
    void setSelectTheme(int theme);
    void setRouteLineStyle(int elementId, const QVariant& value);
    void setRouteLineTurnpointStyle(int elementId, const QVariant& value);
    void setRouteCandidateStyle(int elementId, const QVariant& value, bool selectedType=false);
    void setRouteAlternateStyle(int elementId, const QVariant& value);
    void setTrackpointStyle(int elementId, const QVariant& value);

signals:
    void setMenuData(int menuId, const QVariant& data, int keyIndex=0);
    void menuDataFinished();
    void updateView();

public slots:
    void onMenuDataChanged(int menuId, const QVariant& value, int keyIndex=0);

private:
    void* mMapViewHandle;
    int   mSelectTheme; // 0: daylight(default), 1:night
#if 0
    VSMRouteStyle  mMapRouteStyles[3]; // basic, passed, alternative
    VSMRouteTBTStyle mMapTBTStyle;
#else
    QJsonObject mRouteLineStyle;
    QJsonObject mRouteCandidateStyle1;
    QJsonObject mRouteCandidateStyle2;
    QJsonObject mRouteCandidateStyle3;
    QJsonObject mRouteAlternateStyle;
    QJsonObject mTrackPointStyle;

    QJsonObject mRouteLineStyle_Night;
    QJsonObject mRouteCandidateStyle1_Night;
    QJsonObject mRouteCandidateStyle2_Night;
    QJsonObject mRouteCandidateStyle3_Night;
    QJsonObject mRouteAlternateStyle_Night;
    QJsonObject mTrackPointStyle_Night;
#endif
};

}

#endif // MAPCONFIGDEBUGUIOVERLAY_H
