#include "MapConfigDebugUiControl.h"
#include "VSM_Map_Settings.h"
#include "VSM.h"
#include "VSM_Map.h"
// #include <QQmlComponent>
// #include <QQmlContext>
#include <QtCore/qglobal.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "NavigationControllerProxy.h"
#include <QSettings>
#include <QFile>
#include <QRegularExpression>

namespace SKT {

#ifdef PLATFORM_GEN3
QString vsmIniPath = QStringLiteral("/var/opt/bosch/dynamic/navigation_kor/shared_prefs/vsm_app.ini");
#elif defined(PLATFORM_QNX)
#else
QString vsmIniPath = QString(qgetenv("RSM2018_PREFIX")) + QStringLiteral("/tmap_rsm2018/dynamic/navigation_kor/shared_prefs/vsm_app.ini");
#endif


VSMColor decRGBA2VsmColor(qint32 rgba)
{
  VSMColor color;
  color.r = rgba >> 24 & 0xff;
  color.g = rgba >> 16 & 0xff;
  color.b = rgba >> 8  & 0xff;
  color.a = rgba       & 0xff;

  return color;
}

VSMColor decARGB2VsmColor(qint32 argb)
{
  VSMColor color;
  color.r = argb >> 16 & 0xff;
  color.g = argb >> 8  & 0xff;
  color.b = argb       & 0xff;
  color.a = argb >> 24 & 0xff;

  return color;
}

static QString decRGBA2Hex(qint32 rgba)
{
    return QString("#%1").arg((uint)rgba, 8, 16, QLatin1Char('0'));
}

qint32 hex2RGBA(const QString& hex)
{
    QRegularExpression re("([[:xdigit:]]{4}){1,3}+");
    auto matched = re.match(hex);
    QString captured = matched.captured();
    if (captured.isEmpty())
        return 0;
    QByteArray ba = QByteArray::fromHex(captured.toLatin1());
    qint32 rgba = 0;
    rgba |= ba.mid(0,1).toHex().toUShort(0,16) << 24;
    rgba |= ba.mid(1,1).toHex().toUShort(0,16) << 16;
    rgba |= ba.mid(2,1).toHex().toUShort(0,16) << 8;
    rgba |= ba.mid(3,1).toHex().toUShort(0,16);
    return rgba;

 }

qint32 vsmColor2ARGB(VSMColor& color)
{
  return (color.a<<24) | (color.r<<16) | (color.g<<8) | color.b;
}

extern void setVsmRouteTbtDefaultStyle(void*);
void applyVsmMapCustomConfig()
{
    const int defDensitydpi = 160;
    const int defTileDiskCache = 2;
    const int defBuildingFilter = 2;
    const float defFovy = 38.0;
    const float defNaviPoiScale = 1.0;
    const float defRouteWidth = 3.0;
    const bool defCopycatBuilding = true;
    const bool defShowPoi = true;
    const bool defPoiScaleForce = true;
    const bool defPoiOcclusion = true;
    const bool defRenderSubwayline = true;
    const bool defRouteTi = true;
    const bool defLandmark = true;
    const bool defOutline3d = false;
    const bool defOutline2d = true;
    const bool defOnly2d = false;

    int customDensitydpi = defDensitydpi;
    int customTileDiskCache = defTileDiskCache;
    int customBuildingFilter = defBuildingFilter;
    float customFovy = defFovy;
    float customNaviPoiScale = defNaviPoiScale;
    float customRouteWidth = defRouteWidth;
    bool customCopycatBuilding = defCopycatBuilding;
    bool customShowPoi = defShowPoi;
    bool customPoiScaleForce = defPoiScaleForce;
    bool customPoiOcclusion = defPoiOcclusion;
    bool customRenderSubwayline = defRenderSubwayline;
    bool customRouteTi = defRouteTi;
    bool customLandmark = defLandmark;
    bool customOutline3d = defOutline3d;
    bool customOutline2d = defOutline2d;
    bool customOnly2d = defOnly2d;

    QByteArray customDaylightStyle;
    QByteArray customNightStyle;


    // setup custom
    QFile vsmIniFile( vsmIniPath );
    if (vsmIniFile.exists()) {
        QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
        vsmIni.beginGroup((QStringLiteral("VsmConfig")));

        auto densitydpi = vsmIni.value(QStringLiteral("densitydpi"));
        if (densitydpi.isValid())
            customDensitydpi = (VSM_UInt16)densitydpi.toInt();

        auto tileDiskCache = vsmIni.value(QStringLiteral("tileSidkCache"));
        if (tileDiskCache.isValid())
            customTileDiskCache = tileDiskCache.toInt();

        auto buildingFilter = vsmIni.value(QStringLiteral("buildingFilter"));
        if (buildingFilter.isValid())
            customBuildingFilter = buildingFilter.toInt();

        auto fovy = vsmIni.value(QStringLiteral("fovy"));
        if (fovy.isValid())
            customFovy = fovy.toFloat();

        auto copycatBuilding = vsmIni.value(QStringLiteral("copycatBuilding"));
        if (copycatBuilding.isValid())
            customCopycatBuilding = copycatBuilding.toBool();

        auto showPoi = vsmIni.value(QStringLiteral("showPoi"));
        if (showPoi.isValid())
            customShowPoi = showPoi.toBool();

        auto naviPoiScale = vsmIni.value(QStringLiteral("naviPoiScale"));
        if (naviPoiScale.isValid())
            customNaviPoiScale = naviPoiScale.toFloat();

        auto poiScaleForce = vsmIni.value(QStringLiteral("poiScaleForce"));
        if (poiScaleForce.isValid())
            customPoiScaleForce = poiScaleForce.toBool();

        auto poiOcclusion = vsmIni.value(QStringLiteral("poiOcclusion"));
        if (poiOcclusion.isValid())
            customPoiOcclusion = poiOcclusion.toBool();

        auto renderSubwayline = vsmIni.value(QStringLiteral("renderSubwayline"));
        if (renderSubwayline.isValid())
            customRenderSubwayline = renderSubwayline.toBool();

        auto routeTi = vsmIni.value(QStringLiteral("routeTi"));
        if (routeTi.isValid())
            customRouteTi = routeTi.toBool();

       // load route style themes
       // daylight
        auto daylight = vsmIni.value(QStringLiteral("routestyle_Daylight"));
        if (daylight.isValid()) {
            customDaylightStyle = daylight.toByteArray();
        }
       // night
        auto night = vsmIni.value(QStringLiteral("routestyle_Night"));
        if (night.isValid()) {
            customNightStyle = night.toByteArray();
        }

         // 3d Landmark
        auto landmark = vsmIni.value(QStringLiteral("landmark"));
        if (landmark.isValid())
            customLandmark = landmark.toBool();

        // building outline
        auto outline3d = vsmIni.value(QStringLiteral("outline3d"));
        if (outline3d.isValid())
            customOutline3d = outline3d.toBool();

        auto outline2d = vsmIni.value(QStringLiteral("outline2d"));
        if (outline2d.isValid())
            customOutline2d = outline2d.toBool();

        auto only2d = vsmIni.value(QStringLiteral("only2d"));
        if (only2d.isValid())
            customOnly2d = only2d.toBool();
    }

    // apply config to every mapviews
    auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    auto mapviews = proxy->getMapviewHandle();
    for (auto mapviewHandle : mapviews) {
    // setup default
    VSMMap_Setting_SetDensityDpi(mapviewHandle, (VSM_UInt16)customDensitydpi);

    // [TMCRSM-2128] VSM_SetTileDiskCachMode is applied globally, so no need to call it whenenver each map view is created
    //VSM_SetTileDiskCachMode(VSM_TileDBMode(customTileDiskCache));

    VSMMap_Setting_SetBuildingFilterMode(mapviewHandle, VSM_BuildingFilterMode(customBuildingFilter));
    VSMMap_Setting_SetFovy(mapviewHandle, (VSM_Float)customFovy);
    VSMMap_Setting_SetEnableBuildingPattern(mapviewHandle, (VSM_Bool)customCopycatBuilding);
    VSMMap_Setting_SetShowPOI(mapviewHandle, (VSM_Bool)customShowPoi);
    VSMMap_Setting_SetPOIScaleForNaviMode(mapviewHandle, (VSM_Float)customNaviPoiScale);
    VSMMap_Setting_SetPOIScaleForcedApply(mapviewHandle, (VSM_Bool)customPoiScaleForce);
    VSMMap_Setting_SetPOIOcclusion(mapviewHandle, (VSM_Bool)customPoiOcclusion);
    VSMMap_Setting_SetShouldRenderSubwayLines(mapviewHandle, (VSM_Bool)customRenderSubwayline);
    VSMMap_Setting_SetRouteTI(mapviewHandle, (VSM_Bool)customRouteTi);
    VSMMap_Setting_SetShowLandmark(mapviewHandle, (VSM_Bool)customLandmark);
    VSMMap_Setting_SetShowBuildingOutline3D(mapviewHandle, (VSM_Bool)customOutline3d);
    VSMMap_Setting_SetShowBuildingOutline2D(mapviewHandle, (VSM_Bool)customOutline2d);
    VSMMap_Setting_SetShowBuilding2DOnly(mapviewHandle, (VSM_Bool)customOnly2d);

#if 0
    VSMMap_Setting_SetRouteWidth(mapviewHandle, 3.0 );
#endif
    setVsmRouteTbtDefaultStyle(mapviewHandle);

    if (!customDaylightStyle.isEmpty()) {
        VSMMap_CreateOrUpdateThemeStyle(mapviewHandle
                                        , VSM_THEME_DAY
                                        , customDaylightStyle.data());
    }

    if (!customNightStyle.isEmpty()) {
        VSMMap_CreateOrUpdateThemeStyle(mapviewHandle
                                        , VSM_THEME_NIGHT
                                        , customNightStyle.data());
    }

    }

}



#if 0
VSMRouteStyle parseRouteStyle(const QString& raw)
{
    QStringList tokens = raw.split(QChar(','));
    bool ok;
    VSMRouteStyle style;
    qint32 argb = 0xff000000;
    
    auto parseHex = [&tokens](int i)->qint32 {
      int ln = tokens.length();
      if (i < ln) {
        const QByteArray hex = QByteArray::fromHex(tokens.at(i).toLatin1());
        if (hex.length() < 4) {
          qDebug() << ">>> failed to parse route style by invalid argb " << hex.constData();
          return 0xff000000;
        }

        qint32 d = 0;
        d |= hex.mid(0,1).toHex().toUShort(0,16) << 24;
        d |= hex.mid(1,1).toHex().toUShort(0,16) << 16;
        d |= hex.mid(2,1).toHex().toUShort(0,16) << 8;
        d |= hex.mid(3,1).toHex().toUShort(0,16);
        qDebug() << tokens.at(i) << " to " << QString("%1").arg(d,0,16);
        return d;
      }
    };

    argb = parseHex(0);
    style.fillColorDay = decARGB2VsmColor(argb);
    argb = parseHex(1);
    style.lineColorDay = decARGB2VsmColor(argb);
    argb = parseHex(2);
    style.fillColorNight = decARGB2VsmColor(argb);
    argb = parseHex(3);
    style.lineColorNight = decARGB2VsmColor(argb);
    
    if (tokens.length() > 4)
        style.lineWidth = tokens.at(4).toUShort();
    if (tokens.length() > 5)
        style.showArrow = (bool)tokens.at(5).toShort();

    return style;
}

VSMRouteTBTStyle parseRoutTbtStyle(const QString& raw)
{
   QStringList tokens = raw.split(QChar(','));
    bool ok;
    VSMRouteTBTStyle style;
    qint32 argb = 0xff000000;
    
    auto parseHex = [&tokens](int i)->qint32 {
      int ln = tokens.length();
      if (i < ln) {
        QByteArray hex = QByteArray::fromHex(tokens.at(i).toLatin1());
        if (hex.length() < 4) {
          qDebug() << "failed to parse route style by invalid argb " << hex.constData();
          return 0xff000000;
        }
        qint32 d = 0;
        d |= hex.mid(0,1).toHex().toUShort(0,16) << 24;
        d |= hex.mid(1,1).toHex().toUShort(0,16) << 16;
        d |= hex.mid(2,1).toHex().toUShort(0,16) << 8;
        d |= hex.mid(3,1).toHex().toUShort(0,16);
        qDebug() << tokens.at(i) << " to " << QString("%1").arg(d,0,16);
        return d;
      }
    };

    argb = parseHex(0);
    style.fillColorDay = decARGB2VsmColor(argb);
    argb = parseHex(1);
    style.lineColorDay = decARGB2VsmColor(argb);
    argb = parseHex(2);
    style.fillColorNight = decARGB2VsmColor(argb);
    argb = parseHex(3);
    style.lineColorNight = decARGB2VsmColor(argb);
    if (tokens.length() > 4)
        style.size = tokens.at(4).toFloat();
    if (tokens.length() > 5)
        style.floatGap = tokens.at(5).toFloat();

    return style;
}
#endif

#define DEFAULT_ROUTE_LINE_WIDTH (22.0f) // sp 단위
#define DEFAULT_ROUTE_TBT_FLOAT (5.0f) // tbt gap
#define DEFAULT_TURN_ARROW_SIZE (24.0f)
// 주행선
#define DEFAULT_ROUTE_BASIC_COLOR                           0x188bf4ff
#define DEFAULT_ROUTE_BASIC_OUTLINE_COLOR                   0x1e5e94ff
#define DEFAULT_ROUTE_BASIC_NIGHT_COLOR                     0x00b4ffff
#define DEFAULT_ROUTE_BASIC_NIGHT_OUTLINE_COLOR             0x036a94ff

// 대안경로선
#define DEFAULT_ROUTE_ALTERNATIVE_COLOR                     0x989aa0ff
#define DEFAULT_ROUTE_ALTERNATIVE_OUTLINE_COLOR             0x7d7f84ff
#define DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR               0x9fa6adff
#define DEFAULT_ROUTE_ALTERNATIVE_NIGHT_OUTLINE_COLOR       0x767d86ff

// 지나온 경로선
#define DEFAULT_ROUTE_PASSED_COLOR                          0xc6c6c6ff
#define DEFAULT_ROUTE_PASSED_OUTLINE_COLOR                  0x747474ff
#define DEFAULT_ROUTE_PASSED_NIGHT_COLOR                    0xc6c6c6ff
#define DEFAULT_ROUTE_PASSED_NIGHT_OUTLINE_COLOR            0x707070ff

// Turn Arrow
#define DEFAULT_TURNARROW_COLOR                             0xffffffff
#define DEFAULT_TURNARROW_OUTLINE_COLOR                     0x787878ff
#define DEFAULT_TURNARROW_NIGHT_COLOR                       0xffffffff
#define DEFAULT_TURNARROW_NIGHT_OUTLINE_COLOR               0x787878ff

// DAY
#define DEFAULT_TRAFFIC_NODATA_COLOR                        0x188BF4FF  // 0x6c6c6cff
#define DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR                0x1E5E94FF  // 0x464646ff
#define DEFAULT_TRAFFIC_GOOD_COLOR                          0x03bc6fff
#define DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR                  0x0c753dff
#define DEFAULT_TRAFFIC_SLOW_COLOR                          0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR                  0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_COLOR                           0xf21616ff // 0xf1522aff
#define DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR                   0x823a2fff

// NIGHT
#define DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR                  0x00B4FFFF  // 0x727272ff
#define DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR          0x036A94FF  // 0x464646ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR                    0x13bc62ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR            0x046f36ff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR                    0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR            0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_NIGHT_COLOR                     0xf21616ff//0xf1522aff
#define DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR             0x823a2fff


void setVsmRouteTbtDefaultStyle(void* hMapviewHandle)
{
    QJsonObject crCongestion;
    crCongestion["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_COLOR);
    crCongestion["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR);
    crCongestion["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_COLOR);
    crCongestion["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR);
    crCongestion["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_COLOR);
    crCongestion["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR);
    crCongestion["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_COLOR);
    crCongestion["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR);

    // just for test
    QJsonObject cr0_style;
    cr0_style["width"] = 6.0f;
    cr0_style["fillColor"] = decRGBA2Hex(0x3ac8deff);//ff543aff);
    cr0_style["strokeColor"] = decRGBA2Hex(0x128ea1ff);//af230fff);
    cr0_style["showCongestion"] = false;
    cr0_style["congestion"] = crCongestion;

    QJsonObject cr0_selected(cr0_style);   // copy
    cr0_selected["width"] = 12.0f;
    cr0_selected["showCongestion"] = true;

    QJsonObject candidateRoute0;
    candidateRoute0["id"] = "routeRed";
    candidateRoute0["default"] = cr0_style;
    candidateRoute0["selected"] = cr0_selected;

    QJsonObject cr1_style(cr0_style);   // copy
    cr1_style["fillColor"]   = decRGBA2Hex(0x188ef8ff);
    cr1_style["strokeColor"] = decRGBA2Hex(0x0468c2ff);

    QJsonObject cr1_selected(cr1_style);   // copy
    cr1_selected["width"] = 12.0f;
    cr1_selected["showCongestion"] = true;

    QJsonObject candidateRoute1;
    candidateRoute1["id"] = "routeBlue";
    candidateRoute1["default"] = cr1_style;
    candidateRoute1["selected"] = cr1_selected;

    QJsonObject cr2_style(cr0_style);   // copy
    cr2_style["fillColor"] = decRGBA2Hex(0x88b900ff);
    cr2_style["strokeColor"] = decRGBA2Hex(0x648800ff);

    QJsonObject cr2_selected(cr2_style);   // copy
    cr2_selected["width"] = 12.0f;
    cr2_selected["showCongestion"] = true;

    QJsonObject candidateRoute2;
    candidateRoute2["id"] = "routeGreen";
    candidateRoute2["default"] = cr2_style;
    candidateRoute2["selected"] = cr2_selected;

    QJsonArray candidates;
    candidates.push_back(candidateRoute0);
    candidates.push_back(candidateRoute1);
    candidates.push_back(candidateRoute2);

    // 경로 요약화면의 최적길 경로선 야간 설정

    QJsonObject cr0_style_night(cr0_style);
    cr0_style_night["fillColor"] = decRGBA2Hex(0x32cae2ff);
    cr0_style_night["strokeColor"] = decRGBA2Hex(0x0a8699ff);

    QJsonObject cr0_selected_night(cr0_style_night);
    cr0_selected_night["width"] = 12.0f;
    cr0_selected_night["showCongestion"] = true;

    QJsonObject candidateRoute0_night;
    candidateRoute0_night["id"] = "routeRed";
    candidateRoute0_night["default"] = cr0_style_night;
    candidateRoute0_night["selected"] = cr0_selected_night;

    QJsonArray candidatesNight;
    candidatesNight.push_back(candidateRoute0_night);
    candidatesNight.push_back(candidateRoute1);
    candidatesNight.push_back(candidateRoute2);

    QJsonObject oilPopup;
    oilPopup["scale"] = 1.6f;

    QJsonObject carvatar;
    carvatar["width"] = 110.0f;

    // for day theme
    {
        QJsonObject routeLine;
        routeLine["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        routeLine["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_COLOR);
        routeLine["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_OUTLINE_COLOR);
        routeLine["passedFillColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_COLOR);
        routeLine["passedStrokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_OUTLINE_COLOR);

        QJsonObject turnPoint;
        turnPoint["width"] = DEFAULT_TURN_ARROW_SIZE;
        turnPoint["floatGap"] = DEFAULT_ROUTE_TBT_FLOAT;
        turnPoint["fillColor"] = decRGBA2Hex(DEFAULT_TURNARROW_COLOR);
        turnPoint["strokeColor"] = decRGBA2Hex(DEFAULT_TURNARROW_OUTLINE_COLOR);
        turnPoint["outlineRatio"] = 0.4; // 2018.12.04 new feature

        QJsonObject congestion;
        congestion["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_COLOR);
        congestion["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR);
        congestion["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_COLOR);
        congestion["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR);
        congestion["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_COLOR);
        congestion["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR);
        congestion["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_COLOR);
        congestion["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR);

        routeLine["turnPoint"] = turnPoint;
        routeLine["congestion"] = congestion;

        QJsonObject alternateRoute;
        alternateRoute["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        alternateRoute["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_COLOR);
        alternateRoute["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_OUTLINE_COLOR);

        QJsonObject routeSelection;
        routeSelection["candidates"] = candidates;

        QJsonObject root;
        root["routeLine"] = routeLine;
        root["alternateRoute"] = alternateRoute;
        root["routeSelection"] = routeSelection;
        root["oilPopup"] = oilPopup;
        root["carvatar"] = carvatar;

        VSMMap_CreateOrUpdateThemeStyle(hMapviewHandle,
                                        VSM_THEME_DAY,
                                        QJsonDocument(root).toJson(QJsonDocument::Compact).data());
    }

    // for night theme
    {
        QJsonObject routeLine;
        routeLine["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        routeLine["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_NIGHT_COLOR);
        routeLine["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_NIGHT_OUTLINE_COLOR);
        routeLine["passedFillColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_NIGHT_COLOR);
        routeLine["passedStrokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_NIGHT_OUTLINE_COLOR);

        QJsonObject turnPoint;
        turnPoint["width"] = DEFAULT_TURN_ARROW_SIZE;
        turnPoint["floatGap"] = DEFAULT_ROUTE_TBT_FLOAT;
        turnPoint["fillColor"] = decRGBA2Hex(DEFAULT_TURNARROW_NIGHT_COLOR);
        turnPoint["strokeColor"] = decRGBA2Hex(DEFAULT_TURNARROW_NIGHT_OUTLINE_COLOR);
        turnPoint["outlineRatio"] = 0.4;

        QJsonObject congestion;
        congestion["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR);
        congestion["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR);
        congestion["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR);
        congestion["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR);
        congestion["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR);
        congestion["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR);
        congestion["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_COLOR);
        congestion["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR);

        routeLine["turnPoint"] = turnPoint;
        routeLine["congestion"] = congestion;

        QJsonObject alternateRoute;
        alternateRoute["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        alternateRoute["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR);
        alternateRoute["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_NIGHT_OUTLINE_COLOR);

        QJsonObject routeSelection;
        routeSelection["candidates"] = candidatesNight;

        QJsonObject root;
        root["routeLine"] = routeLine;
        root["alternateRoute"] = alternateRoute;
        root["routeSelection"] = routeSelection;
        root["oilPopup"] = oilPopup;
        root["carvatar"] = carvatar;

        VSMMap_CreateOrUpdateThemeStyle(hMapviewHandle,
                                        VSM_THEME_NIGHT,
                                        QJsonDocument(root).toJson(QJsonDocument::Compact).data());
    }
}


MapConfigDebugUiControl::MapConfigDebugUiControl(QObject *parent)
    : QObject(parent)
    , mMapViewHandle(nullptr)
    , mSelectTheme(0)

{
    initRouteStyleTheme();
    qint32 rgba = hex2RGBA("#abcdefa9");
    qDebug() << "hex to rgba: " << rgba;
}


MapConfigDebugUiControl::~MapConfigDebugUiControl()
{
	saveConfigSettings();
}

void MapConfigDebugUiControl::initRouteStyleTheme()
{
        // day theme
        mRouteLineStyle["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        mRouteLineStyle["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_COLOR);
        mRouteLineStyle["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_OUTLINE_COLOR);
        mRouteLineStyle["passedFillColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_COLOR);
        mRouteLineStyle["passedStrokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_OUTLINE_COLOR);

        QJsonObject turnPoint;
        turnPoint["width"] = DEFAULT_TURN_ARROW_SIZE;
        turnPoint["floatGap"] = DEFAULT_ROUTE_TBT_FLOAT;
        turnPoint["fillColor"] = decRGBA2Hex(DEFAULT_TURNARROW_COLOR);
        turnPoint["strokeColor"] = decRGBA2Hex(DEFAULT_TURNARROW_OUTLINE_COLOR);
        turnPoint["outlineRatio"] = 0.4; // 2018.12.04 new feature

        QJsonObject congestion;
        congestion["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_COLOR);
        congestion["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR);
        congestion["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_COLOR);
        congestion["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR);
        congestion["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_COLOR);
        congestion["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR);
        congestion["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_COLOR);
        congestion["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR);

        mRouteLineStyle["turnPoint"] = turnPoint;
        mRouteLineStyle["congestion"] = congestion;


        // night theme
        mRouteLineStyle_Night["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        mRouteLineStyle_Night["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_NIGHT_COLOR);
        mRouteLineStyle_Night["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_BASIC_NIGHT_OUTLINE_COLOR);
        mRouteLineStyle_Night["passedFillColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_NIGHT_COLOR);
        mRouteLineStyle_Night["passedStrokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_PASSED_NIGHT_OUTLINE_COLOR);

        QJsonObject turnPoint_Night;
        turnPoint_Night["width"] = DEFAULT_TURN_ARROW_SIZE;
        turnPoint_Night["floatGap"] = DEFAULT_ROUTE_TBT_FLOAT;
        turnPoint_Night["fillColor"] = decRGBA2Hex(DEFAULT_TURNARROW_NIGHT_COLOR);
        turnPoint_Night["strokeColor"] = decRGBA2Hex(DEFAULT_TURNARROW_NIGHT_OUTLINE_COLOR);
        turnPoint_Night["outlineRatio"] = 0.4; // 2018.12.04 new feature

        QJsonObject congestion_Night;
        congestion_Night["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR);
        congestion_Night["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR);
        congestion_Night["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR);
        congestion_Night["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR);
        congestion_Night["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR);
        congestion_Night["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR);
        congestion_Night["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_COLOR);
        congestion_Night["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR);

        mRouteLineStyle_Night["turnPoint"] = turnPoint_Night;
        mRouteLineStyle_Night["congestion"] = congestion_Night;

        QJsonObject cr0_style;
        cr0_style["width"] = 6.0f;
        cr0_style["fillColor"] = decRGBA2Hex(0xff543aff);
        cr0_style["strokeColor"] = decRGBA2Hex(0xaf230fff);
        cr0_style["showCongestion"] = false;
        cr0_style["congestion"] = congestion;

        QJsonObject cr0_selected(cr0_style);   // copy
        cr0_selected["width"] = 12.0f;
        cr0_selected["showCongestion"] = true;

        QJsonObject candidateRoute0;
        candidateRoute0["id"] = "routeRed";
        candidateRoute0["default"] = cr0_style;
        candidateRoute0["selected"] = cr0_selected;

        mRouteCandidateStyle1 = candidateRoute0;
        mRouteCandidateStyle1_Night = candidateRoute0;

        QJsonObject cr1_style(cr0_style);   // copy
        cr1_style["fillColor"]   = decRGBA2Hex(0x188ef8ff);
        cr1_style["strokeColor"] = decRGBA2Hex(0x0468c2ff);

        QJsonObject cr1_selected(cr1_style);   // copy
        cr1_selected["width"] = 12.0f;
        cr1_selected["showCongestion"] = true;

        // 최소시간 길
        QJsonObject candidateRoute1;
        candidateRoute1["id"] = "routeBlue";
        candidateRoute1["default"] = cr1_style;
        candidateRoute1["selected"] = cr1_selected;

        mRouteCandidateStyle2 = candidateRoute1;
        mRouteCandidateStyle2_Night = candidateRoute1;

        QJsonObject cr2_style(cr0_style);   // copy
        cr2_style["fillColor"] = decRGBA2Hex(0x88b900ff);
        cr2_style["strokeColor"] = decRGBA2Hex(0x648800ff);

        QJsonObject cr2_selected(cr2_style);   // copy
        cr2_selected["width"] = 12.0f;
        cr2_selected["showCongestion"] = true;

        // 무료길
        QJsonObject candidateRoute2;
        candidateRoute2["id"] = "routeGreen";
        candidateRoute2["default"] = cr2_style;
        candidateRoute2["selected"] = cr2_selected;

        mRouteCandidateStyle3 = candidateRoute2;
        mRouteCandidateStyle3_Night = candidateRoute2;

        QJsonObject alternateRoute;
        alternateRoute["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        alternateRoute["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR);
        alternateRoute["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_OUTLINE_COLOR);
        mRouteAlternateStyle = alternateRoute;

        QJsonObject alternateRouteNight;
        alternateRouteNight["width"] = DEFAULT_ROUTE_LINE_WIDTH;
        alternateRouteNight["fillColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_NIGHT_COLOR);
        alternateRouteNight["strokeColor"] = decRGBA2Hex(DEFAULT_ROUTE_ALTERNATIVE_NIGHT_OUTLINE_COLOR);
        mRouteAlternateStyle_Night = alternateRouteNight;
}

void MapConfigDebugUiControl::setConfigSettings()
{
     if (!mMapViewHandle) return;

        const int defDensitydpi = 160;
        const int defTileDiskCache = 2;
        const int defBuildingFilter = 2;
        const float defFovy = 38.0;
        const float defNaviPoiScale = 1.0;
        const float defRouteWidth = 3.0;
        const bool defCopycatBuilding = true;
        const bool defShowPoi = true;
        const bool defPoiScaleForce = true;
        const bool defPoiOcclusion = true;
        const bool defRenderSubwayline = true;
        const bool defRouteTi = true;
        const bool defLandmark = true;
        const bool defOutline3d = false;
        const bool defOutline2d = true;
        const bool defOnly2d = false;

        int customDensitydpi = defDensitydpi;
        int customTileDiskCache = defTileDiskCache;
        int customBuildingFilter = defBuildingFilter;
        float customFovy = defFovy;
        float customNaviPoiScale = defNaviPoiScale;
        float customRouteWidth = defRouteWidth;
        bool customCopycatBuilding = defCopycatBuilding;
        bool customShowPoi = defShowPoi;
        bool customPoiScaleForce = defPoiScaleForce;
        bool customPoiOcclusion = defPoiOcclusion;
        bool customRenderSubwayline = defRenderSubwayline;
        bool customRouteTi = defRouteTi;
        bool customLandmark = defLandmark;
        bool customOutline3d = defOutline3d;
        bool customOutline2d = defOutline2d;
        bool customOnly2d = defOnly2d;

        QByteArray customDaylightStyle;
        QByteArray customNightStyle;

        // load default from vsm
        VSMMap_Setting_GetDensityDpi(mMapViewHandle, (VSM_UInt16&)customDensitydpi);
        VSM_GetTileDiskCachMode((VSM_TileDBMode&)customTileDiskCache);
        VSMMap_Setting_GetBuildingFilterMode(mMapViewHandle, (VSM_BuildingFilterMode&)customBuildingFilter);
        VSMMap_Setting_GetFovy(mMapViewHandle,(VSM_Float&)customFovy);
        VSMMap_Setting_GetEnableBuildingPattern(mMapViewHandle, (VSM_Bool&)customCopycatBuilding);
        VSMMap_Setting_GetShowPOI(mMapViewHandle, (VSM_Bool&)customShowPoi);
        VSMMap_Setting_GetPOIScaleForNaviMode(mMapViewHandle, (VSM_Float&)customNaviPoiScale);
        VSMMap_Setting_GetPOIScaleForcedApply(mMapViewHandle, (VSM_Bool&)customPoiScaleForce);
        VSMMap_Setting_GetPOIOcclusion(mMapViewHandle, (VSM_Bool&)customPoiOcclusion);
        VSMMap_Setting_GetShouldRenderSubwayLines(mMapViewHandle, (VSM_Bool&)customRenderSubwayline);
        VSMMap_Setting_GetRouteTI(mMapViewHandle, (VSM_Bool&)customRouteTi);

        VSMMap_Setting_GetShowLandmark(mMapViewHandle, (VSM_Bool&)customLandmark);
        VSMMap_Setting_GetShowBuildingOutline3D(mMapViewHandle, (VSM_Bool&)customOutline3d);
        VSMMap_Setting_GetShowBuildingOutline2D(mMapViewHandle, (VSM_Bool&)customOutline2d);

     
      // load from custom setting

    QFile vsmIniFile( vsmIniPath );
    if (vsmIniFile.exists()) {
        QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
        vsmIni.beginGroup((QStringLiteral("VsmConfig")));

        auto densitydpi = vsmIni.value(QStringLiteral("densitydpi"));
        if (densitydpi.isValid())
            customDensitydpi = (VSM_UInt16)densitydpi.toInt();

        auto tileDiskCache = vsmIni.value(QStringLiteral("tileSidkCache"));
        if (tileDiskCache.isValid())
            customTileDiskCache = tileDiskCache.toInt();

        auto buildingFilter = vsmIni.value(QStringLiteral("buildingFilter"));
        if (buildingFilter.isValid())
            customBuildingFilter = buildingFilter.toInt();

        auto fovy = vsmIni.value(QStringLiteral("fovy"));
        if (fovy.isValid())
            customFovy = fovy.toFloat();

        auto copycatBuilding = vsmIni.value(QStringLiteral("copycatBuilding"));
        if (copycatBuilding.isValid())
            customCopycatBuilding = copycatBuilding.toBool();

        auto showPoi = vsmIni.value(QStringLiteral("showPoi"));
        if (showPoi.isValid())
            customShowPoi = showPoi.toBool();

        auto naviPoiScale = vsmIni.value(QStringLiteral("naviPoiScale"));
        if (naviPoiScale.isValid())
            customNaviPoiScale = naviPoiScale.toFloat();

        auto poiScaleForce = vsmIni.value(QStringLiteral("poiScaleForce"));
        if (poiScaleForce.isValid())
            customPoiScaleForce = poiScaleForce.toBool();

        auto poiOcclusion = vsmIni.value(QStringLiteral("poiOcclusion"));
        if (poiOcclusion.isValid())
            customPoiOcclusion = poiOcclusion.toBool();

        auto renderSubwayline = vsmIni.value(QStringLiteral("renderSubwayline"));
        if (renderSubwayline.isValid())
            customRenderSubwayline = renderSubwayline.toBool();

        auto routeTi = vsmIni.value(QStringLiteral("routeTi"));
        if (routeTi.isValid())
            customRouteTi = routeTi.toBool();

       // load route style themes
       // daylight
        auto daylight = vsmIni.value(QStringLiteral("routestyle_Daylight"));
        if (daylight.isValid()) {
            customDaylightStyle = daylight.toByteArray();
        }
       // night
        auto night = vsmIni.value(QStringLiteral("routestyle_Night"));
        if (night.isValid()) {
            customNightStyle = night.toByteArray();
        }

         // 3d Landmark
        auto landmark = vsmIni.value(QStringLiteral("landmark"));
        if (landmark.isValid())
            customLandmark = landmark.toBool();

        // building outline
        auto outline3d = vsmIni.value(QStringLiteral("outline3d"));
        if (outline3d.isValid())
            customOutline3d = outline3d.toBool();

        auto outline2d = vsmIni.value(QStringLiteral("outline2d"));
        if (outline2d.isValid())
            customOutline2d = outline2d.toBool();

        auto only2d = vsmIni.value(QStringLiteral("only2d"));
        if (only2d.isValid())
            customOnly2d = only2d.toBool();
    }

       // load route style themes
       // daylight
       {
            if (!customDaylightStyle.isEmpty()) {
                QJsonDocument json(QJsonDocument::fromJson(customDaylightStyle));
                QJsonObject root = json.object();
                mRouteAlternateStyle = root["alternateRoute"].toObject();
                mRouteLineStyle      = root["routeLine"].toObject();
                QJsonObject routeSelection = root["routeSelection"].toObject();
                QJsonArray candidates = routeSelection["candidates"].toArray();
                mRouteCandidateStyle1 = candidates.takeAt(0).toObject();
                mRouteCandidateStyle2 = candidates.takeAt(1).toObject();
                mRouteCandidateStyle3 = candidates.takeAt(2).toObject();
                mTrackPointStyle = root["trackPoint"].toObject();
            }
       }
       // night
       {
            if (!customNightStyle.isEmpty()) {
                QJsonDocument json(QJsonDocument::fromJson(customNightStyle));
                QJsonObject root = json.object();
                mRouteAlternateStyle_Night = root["alternateRoute"].toObject();
                mRouteLineStyle_Night      = root["routeLine"].toObject();
                QJsonObject routeSelection = root["routeSelection"].toObject();
                QJsonArray candidates = routeSelection["candidates"].toArray();
                mRouteCandidateStyle1_Night = candidates.takeAt(0).toObject();
                mRouteCandidateStyle2_Night = candidates.takeAt(1).toObject();
                mRouteCandidateStyle3_Night = candidates.takeAt(2).toObject();
                mTrackPointStyle_Night = root["trackPoint"].toObject();
            }
       }

    // emit signal to ui
    {
      emit setMenuData(K_MenuMapDensity, customDensitydpi);
      emit setMenuData(K_MenuMapTileSidkCache,customTileDiskCache);
      emit setMenuData(K_MenuMapBuildingFilterMode, customBuildingFilter);
      emit setMenuData(K_MenuMapFovy, customFovy);
      emit setMenuData(K_MenuMapCopycatBuilding, customCopycatBuilding);
      emit setMenuData(K_MenuMapShowPoi, customShowPoi);
      emit setMenuData(K_MenuMapPoiScale, customNaviPoiScale);
      emit setMenuData(K_MenuMapPoiScaleForce, customPoiScaleForce);
      emit setMenuData(K_MenuMapPoiOcclusion, customPoiOcclusion);
      emit setMenuData(K_MenuMapRenderSubway, customRenderSubwayline);
      emit setMenuData(K_MenuMapRouteTi, customRouteTi);

      emit setMenuData(K_MenuSelectTheme, mSelectTheme);

      emit setMenuData(K_MenuMap3dLandmark, customLandmark);
      emit setMenuData(K_MenuMapBDOutline3d, customOutline3d);
      emit setMenuData(K_MenuMapBDOutline2d, customOutline2d);
      emit setMenuData(K_MenuMapShowOnly2d, customOnly2d);

      setThemeSettings();

    } // emit signal to ui

    emit menuDataFinished();

}


void MapConfigDebugUiControl::saveConfigSettings()
{
	                // apply last setting
	if (!mMapViewHandle) return;


    QFile vsmIniFile( vsmIniPath );
    // create ini file if there are not
    if (!vsmIniFile.exists()) {
       vsmIniFile.open(QFile::WriteOnly);
       vsmIniFile.close();
    }

    {
	QSettings vsmIni(vsmIniPath, QSettings::IniFormat);
	vsmIni.beginGroup((QStringLiteral("VsmConfig")));
	quint16 densitydpi = 0;
	VSMMap_Setting_GetDensityDpi(mMapViewHandle, densitydpi);
	vsmIni.setValue(QStringLiteral("densitydpi"), densitydpi);

	VSM_TileDBMode tileSidkCache;
    VSM_GetTileDiskCachMode(tileSidkCache);
    vsmIni.setValue(QStringLiteral("tileSidkCache"),tileSidkCache);

    VSM_BuildingFilterMode buildingfilter;
    VSMMap_Setting_GetBuildingFilterMode(mMapViewHandle, buildingfilter);
    vsmIni.setValue(QStringLiteral("buildingFilter"),buildingfilter);
    
    VSM_Float fovy = 0;
    VSMMap_Setting_GetFovy(mMapViewHandle,fovy);
    vsmIni.setValue(QStringLiteral("fovy"),fovy);

    VSM_Bool copycatBuilding;
    VSMMap_Setting_GetEnableBuildingPattern(mMapViewHandle, copycatBuilding);
    vsmIni.setValue(QStringLiteral("copycatBuilding"),copycatBuilding);

    VSM_Bool showPoi;
    VSMMap_Setting_GetShowPOI(mMapViewHandle, showPoi);
    vsmIni.setValue(QStringLiteral("showPoi"),showPoi);

    VSM_Float naviPoiScale;
    VSMMap_Setting_GetPOIScaleForNaviMode(mMapViewHandle, naviPoiScale);
    vsmIni.setValue(QStringLiteral("naviPoiScale"),naviPoiScale);

    VSM_Bool poiScaleForce;
    VSMMap_Setting_GetPOIScaleForcedApply(mMapViewHandle, poiScaleForce);
    vsmIni.setValue(QStringLiteral("poiScaleForce"),poiScaleForce);

    
    VSM_Bool poiOcclusion;
    VSMMap_Setting_GetPOIOcclusion(mMapViewHandle, poiOcclusion);
    vsmIni.setValue(QStringLiteral("poiOcclusion"), poiOcclusion);


    VSM_Bool renderSubwayline;
    VSMMap_Setting_GetShouldRenderSubwayLines(mMapViewHandle, renderSubwayline);
    vsmIni.setValue(QStringLiteral("renderSubwayline"), renderSubwayline);

    VSM_Bool routeTi;
    VSMMap_Setting_GetRouteTI(mMapViewHandle, routeTi);
    vsmIni.setValue(QStringLiteral("routeTi"),routeTi);


    // save route style theme
    {
        // day theme
        QJsonObject root;
        root["routeLine"] = mRouteLineStyle;
        root["alternateRoute"] = mRouteAlternateStyle;
        root["trackPoint"] = mTrackPointStyle;
        QJsonArray candidates;
        candidates.push_back(mRouteCandidateStyle1);
        candidates.push_back(mRouteCandidateStyle2);
        candidates.push_back(mRouteCandidateStyle3);
        QJsonObject routeSelection;
        routeSelection["candidates"] = candidates;
        root["routeSelection"] = routeSelection;

        QJsonDocument json(root);
        vsmIni.setValue(QStringLiteral("routestyle_Daylight"),json.toJson(QJsonDocument::Compact));
    }
    {
        // night theme
        QJsonObject root;
        root["routeLine"] = mRouteLineStyle_Night;
        root["alternateRoute"] = mRouteAlternateStyle_Night;
        root["trackPoint"] = mTrackPointStyle_Night;
        QJsonArray candidates;
        candidates.push_back(mRouteCandidateStyle1_Night);
        candidates.push_back(mRouteCandidateStyle2_Night);
        candidates.push_back(mRouteCandidateStyle3_Night);
        QJsonObject routeSelection;
        routeSelection["candidates"] = candidates;
        root["routeSelection"] = routeSelection;

        QJsonDocument json(root);
        vsmIni.setValue(QStringLiteral("routestyle_Night"),json.toJson(QJsonDocument::Compact));
    }


    VSM_Bool landmark;
    VSMMap_Setting_GetShowLandmark(mMapViewHandle, landmark);
    vsmIni.setValue(QStringLiteral("landmark"), landmark);

    VSM_Bool outline3d;
    VSMMap_Setting_GetShowBuildingOutline3D(mMapViewHandle, outline3d);
    vsmIni.setValue(QStringLiteral("outline3d"), outline3d);

    VSM_Bool outline2d;
    VSMMap_Setting_GetShowBuildingOutline2D(mMapViewHandle, outline2d);
    vsmIni.setValue(QStringLiteral("outline2d"), outline2d);

    VSM_Bool only2d;
    VSMMap_Setting_GetShowBuilding2DOnly(mMapViewHandle, only2d);
    vsmIni.setValue(QStringLiteral("only2d"), only2d);
  }

}

void MapConfigDebugUiControl::setThemeSettings()
{
    if (mSelectTheme == 0) {
        // day theme
        {
        // route line base
            if (!mRouteLineStyle["width"].isUndefined()) {
                float width = (float)mRouteLineStyle["width"].toDouble();
                emit setMenuData(K_MenuMapRouteLineStyle, width, 0); 
            }

            // route line turning point 
            if (!mRouteLineStyle["turnPoint"].isUndefined()) {
                QJsonObject tp = mRouteLineStyle["turnPoint"].toObject();
                if (!tp["width"].isUndefined()) {
                    float width = (float)tp["width"].toDouble();
                    emit setMenuData(K_MenuMapRouteLineTurnpointStyle, width, 0);
                }
                if (!tp["floatGap"].isUndefined()) {
                    float gap = (float)tp["floatGap"].toDouble();
                    emit setMenuData(K_MenuMapRouteLineTurnpointStyle, gap, 1);
                }
                if (!tp["outlineRatio"].isUndefined()) {
                    float outlineRatio = tp["outlineRatio"].toDouble();
                    emit setMenuData(K_MenuMapRouteLineTurnpointStyle, outlineRatio, 2);
                }
            }

            // candidate 1 default
            if (!mRouteCandidateStyle1["default"].isUndefined()) {
                QJsonObject candi = mRouteCandidateStyle1["default"].toObject();
                if (!candi["width"].isUndefined()) {
                    float width = (float)candi["width"].toDouble();
                    emit setMenuData(K_MenuMapRouteCandidateDefaultStyle, width, 0);
                }
            }

            // candidate1 selected
            if (!mRouteCandidateStyle1["selected"].isUndefined()) {
                QJsonObject candi = mRouteCandidateStyle1["selected"].toObject();
                if (!candi["width"].isUndefined()) {
                    float width = (float)candi["width"].toDouble();
                    emit setMenuData(K_MenuMapRouteCandidateSelectedStyle, width, 0);
                }
            }


            // alternate route
            if (!mRouteAlternateStyle["width"].isUndefined()) {
                float width = (float)mRouteAlternateStyle["width"].toDouble();
                emit setMenuData(K_MenuMapRouteAlterateStyle, width, 0);
            }

            // trackpoint
            if (!mTrackPointStyle["outlineWidth"].isUndefined()) {
                float width = (float)mTrackPointStyle["outlineWidth"].toDouble();
                emit setMenuData(K_MenuMapTrackPointStyle, width, 0);
            }

        }// day them

    }

    else {
        // night theme
        {
        // route line base
        if (!mRouteLineStyle_Night["width"].isUndefined()) {
            float width = (float)mRouteLineStyle_Night["width"].toDouble();
            emit setMenuData(K_MenuMapRouteLineStyle, width, 0); 
        }

        // route line turning point 
        if (!mRouteLineStyle_Night["turnPoint"].isUndefined()) {
            QJsonObject tp = mRouteLineStyle_Night["turnPoint"].toObject();
            if (!tp["width"].isUndefined()) {
                float width = (float)tp["width"].toDouble();
                emit setMenuData(K_MenuMapRouteLineTurnpointStyle, width, 0);
            }
            if (!tp["floatGap"].isUndefined()) {
                float gap = (float)tp["floatGap"].toDouble();
                emit setMenuData(K_MenuMapRouteLineTurnpointStyle, gap, 1);
            }
            if (!tp["outlineRatio"].isUndefined()) {
                float outlineRatio = tp["outlineRatio"].toDouble();;
                emit setMenuData(K_MenuMapRouteLineTurnpointStyle, outlineRatio, 2);
            }

        }

            // candidate 1 default
            if (!mRouteCandidateStyle1_Night["default"].isUndefined()) {
                QJsonObject candi = mRouteCandidateStyle1_Night["default"].toObject();
                if (!candi["width"].isUndefined()) {
                    float width = (float)candi["width"].toDouble();
                    emit setMenuData(K_MenuMapRouteCandidateDefaultStyle, width, 0);
                }
            }

            // candidate1 selected
            if (!mRouteCandidateStyle1_Night["selected"].isUndefined()) {
                QJsonObject candi = mRouteCandidateStyle1_Night["selected"].toObject();
                if (!candi["width"].isUndefined()) {
                    float width = (float)candi["width"].toDouble();
                    emit setMenuData(K_MenuMapRouteCandidateSelectedStyle, width, 0);
                }
            }

            // alternate route
            if (!mRouteAlternateStyle_Night["width"].isUndefined()) {
                float width = (float)mRouteAlternateStyle_Night["width"].toDouble();
                emit setMenuData(K_MenuMapRouteAlterateStyle, width, 0);
            }

            // trackpoint
            if (!mTrackPointStyle_Night["outlineWidth"].isUndefined()) {
                float width = (float)mTrackPointStyle_Night["outlineWidth"].toDouble();
                emit setMenuData(K_MenuMapTrackPointStyle, width, 0);
            }
        }
    } // night theme
}

void MapConfigDebugUiControl::connectUi(QObject* qmlObj, QObject* mapView)
{
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto mapviews = proxy->getMapviewHandle();
        if (mapviews.size() > 0)
            mMapViewHandle = mapviews.at(0); // only take the active mapview
        
    connect(qmlObj, SIGNAL(changeMenuData(int,const QVariant&, int)), this, SLOT(onMenuDataChanged(int,const QVariant&,int)));
}

void MapConfigDebugUiControl::onMenuDataChanged(int menuId, const QVariant& value, int keyIndex)
{
	switch (kMenuId(menuId) ) {
        case K_MenuMapDensity: {
            int val = (VSM_UInt16)value.toInt();
            VSMMap_Setting_SetDensityDpi(mMapViewHandle, val);
        }
			break;
		case K_MenuMapTileSidkCache: {
			int val = value.toInt();
           VSM_SetTileDiskCachMode(VSM_TileDBMode(val%3));
		}
			break;
		case K_MenuMapBuildingFilterMode: {
			int val = value.toInt();
			VSMMap_Setting_SetBuildingFilterMode(mMapViewHandle, VSM_BuildingFilterMode(val%3));
		}
			break;
		case K_MenuMapFovy: {
			float val = value.toFloat();
			VSMMap_Setting_SetFovy(mMapViewHandle, val);
		}
			break;
		case K_MenuMapCopycatBuilding: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetEnableBuildingPattern(mMapViewHandle, enabled);
		}
			break;
		case K_MenuMapShowPoi: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetShowPOI(mMapViewHandle, enabled);
		}
			break;
		case K_MenuMapPoiScale: {
			float val = value.toFloat();
			VSMMap_Setting_SetPOIScaleForNaviMode(mMapViewHandle, val);
		}
			break;
		case K_MenuMapPoiScaleForce: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetPOIScaleForcedApply(mMapViewHandle, enabled);
		}
			break;
		case K_MenuMapPoiOcclusion: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetPOIOcclusion(mMapViewHandle, enabled);
		}
			break;
		case K_MenuMapRenderSubway: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetShouldRenderSubwayLines(mMapViewHandle, enabled);
		}
			break;
		case K_MenuMapRouteTi: {
			bool enabled = value.toBool();
			VSMMap_Setting_SetRouteTI(mMapViewHandle, enabled);
		}
			break;
        case K_MenuSelectTheme: {
            int selected = value.toInt();
            setSelectTheme(selected);
        }
            break;
        case K_MenuMapRouteLineStyle:
            setRouteLineStyle( keyIndex, value);
            break;
        case K_MenuMapRouteLineTurnpointStyle:
            setRouteLineTurnpointStyle( keyIndex, value);
            break;
        case K_MenuMapRouteCandidateDefaultStyle:
            setRouteCandidateStyle( keyIndex, value );
            break;  
        case K_MenuMapRouteCandidateSelectedStyle:
            setRouteCandidateStyle( keyIndex, value, true );
            break;
        case K_MenuMapRouteAlterateStyle:
            setRouteAlternateStyle( keyIndex, value);
            break;
        case K_MenuMapTrackPointStyle:
            setTrackpointStyle( keyIndex, value);
            break;
        case K_MenuMap3dLandmark: {
          bool enabled = value.toBool();
          VSMMap_Setting_SetShowLandmark(mMapViewHandle, enabled);
        }
          break;
        case K_MenuMapBDOutline3d: {
          bool enabled = value.toBool();
          VSMMap_Setting_SetShowBuildingOutline3D(mMapViewHandle, enabled);
        }
          break;
        case K_MenuMapBDOutline2d: {
          bool enabled = value.toBool();
          VSMMap_Setting_SetShowBuildingOutline2D(mMapViewHandle, enabled);
        }
          break;
        case K_MenuMapShowOnly2d: {
          bool enabled = value.toBool();
          VSMMap_Setting_SetShowBuilding2DOnly(mMapViewHandle, enabled);
        }
          break;
    	default:break;
    	}
}

void MapConfigDebugUiControl::setSelectTheme(int theme)
{
    mSelectTheme = theme;
    setThemeSettings();
    emit setMenuData(K_MenuSelectTheme, mSelectTheme);
    emit updateView();
}

void MapConfigDebugUiControl::setRouteLineStyle(int elementId, const QVariant& value)
{
    bool night = mSelectTheme == 1;
    float styleValue = value.toFloat();
    qint32 rgba = value.toInt();
    QString color;
    if (elementId > 0)
        color = decRGBA2Hex(rgba);

    QJsonObject style;
    if (night)
        style = mRouteLineStyle_Night;
    else
        style =  mRouteLineStyle;

        switch (elementId) {
            case 0:
                style["width"] = styleValue;
                break;
            default:break;
        }

        QJsonObject routeLine;
        routeLine["routeLine"] = style;
        VSMMap_CreateOrUpdateThemeStyle(mMapViewHandle,
                                        (night)?VSM_THEME_NIGHT:VSM_THEME_DAY,
                                        QJsonDocument(routeLine).toJson(QJsonDocument::Compact).data());

        if (night)
            mRouteLineStyle_Night = style;
        else
            mRouteLineStyle = style;

}

void MapConfigDebugUiControl::setRouteLineTurnpointStyle(int elementId, const QVariant& value)
{
    bool night = mSelectTheme == 1;
    float styleValue = value.toFloat();
    qint32 rgba = value.toInt();
    QString color;
    if (elementId > 2)
        color = decRGBA2Hex(rgba);

    QJsonObject style;
    if (night)
        style = mRouteLineStyle_Night["turnPoint"].toObject();
    else
        style = mRouteLineStyle["turnPoint"].toObject();


    switch (elementId) {
        case 0:
            style["width"] = styleValue;
            break;
        case 1:
            style["floatGap"] = styleValue;
            break;
        case 2:
            style["outlineRatio"] = styleValue;
            break;
        default:break;
    }

    QJsonObject turnPoint;
    turnPoint["turnPoint"] = style;
    QJsonObject routeLine;
    routeLine["routeLine"] = turnPoint;
    VSMMap_CreateOrUpdateThemeStyle(mMapViewHandle,
                                        (night)?VSM_THEME_NIGHT:VSM_THEME_DAY,
                                        QJsonDocument(routeLine).toJson(QJsonDocument::Compact).data());

    if (night)
        mRouteLineStyle_Night["turnPoint"] = style;
    else
        mRouteLineStyle["turnPoint"] = style;
}

void MapConfigDebugUiControl::setRouteCandidateStyle(int elementId, const QVariant& value, bool selectedType)
{
    bool night = mSelectTheme == 1;
    float styleValue = value.toFloat();
    qint32 rgba = value.toInt();
    QString color;
    if (elementId > 0)
        color = decRGBA2Hex(rgba);

    QJsonObject style;
    if (night) {
        if (selectedType)
            style = mRouteCandidateStyle1_Night["selected"].toObject();
        else
            style = mRouteCandidateStyle1_Night["default"].toObject();
    }
    else {
        if (selectedType)
            style = mRouteCandidateStyle1["selected"].toObject();
        else
            style = mRouteCandidateStyle1["default"].toObject();

    }

    switch (elementId) {
        case 0:
        style["width"] = styleValue;
        break;
        default:break;
    }

    QJsonObject route0;
    route0["id"] = QStringLiteral("routeRed");
    if (!selectedType)
        route0["default"] = style;
    else
        route0["selected"] = style;

    QJsonArray candidates;
    candidates.push_back(route0);

    QJsonObject routeSelection;
    routeSelection["candidates"] = candidates;

    QJsonObject root;
    root["routeSelection"] = routeSelection;

    VSMMap_CreateOrUpdateThemeStyle(mMapViewHandle,
                                        (night)?VSM_THEME_NIGHT:VSM_THEME_DAY,
                                        QJsonDocument(root).toJson(QJsonDocument::Compact).data());

    if (night) {
        if (selectedType)
            mRouteCandidateStyle1_Night["selected"] = style;
        else
            mRouteCandidateStyle1_Night["default"] = style;
    }
    else {
        if (selectedType)
            mRouteCandidateStyle1["selected"] = style;
        else
            mRouteCandidateStyle1["default"] = style;
    }
}


void MapConfigDebugUiControl::setRouteAlternateStyle(int elementId, const QVariant& value)
{
    bool night = mSelectTheme == 1;
    float width = value.toFloat();
    qint32 rgba = value.toInt();
    QString color = decRGBA2Hex(rgba);
    QJsonObject style;
    if (night)
        style = mRouteAlternateStyle_Night;
    else
        style = mRouteAlternateStyle;

    switch (elementId) {
        case 0:
        style["width"] = width;
        break;
        default:break;
    }

    QJsonObject alter;
    alter["alternateRoute"] = style;
    VSMMap_CreateOrUpdateThemeStyle(mMapViewHandle,
                                        (night)?VSM_THEME_NIGHT:VSM_THEME_DAY,
                                        QJsonDocument(alter).toJson(QJsonDocument::Compact).data());

    if (night)
        mRouteAlternateStyle_Night = style;
    else
        mRouteAlternateStyle = style;
}


void MapConfigDebugUiControl::setTrackpointStyle(int elementId, const QVariant& value)
{
    bool night = mSelectTheme == 1;
    float styleValue = value.toFloat();
    qint32 rgba = value.toInt();
    QString color;
    if (elementId > 0)
        color = decRGBA2Hex(rgba);

    QJsonObject style;
    if (night)
        style = mTrackPointStyle_Night;
    else
        style = mTrackPointStyle;

    switch (elementId) {
        case 0:
        style["outlineWidth"] = styleValue;
            break;
        default:break;
    }

    QJsonObject tp;
    tp["trackPoint"] = style;
    VSMMap_CreateOrUpdateThemeStyle(mMapViewHandle,
                                        (night)?VSM_THEME_NIGHT:VSM_THEME_DAY,
                                        QJsonDocument(tp).toJson(QJsonDocument::Compact).data());

    if (night)
        mTrackPointStyle_Night = style;
    else
        mTrackPointStyle = style;
}

}
