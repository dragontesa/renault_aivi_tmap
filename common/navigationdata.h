#ifndef NAVIGATIONDATA_H
#define NAVIGATIONDATA_H

#include <QObject>
#include <QString>
#include "rgdataptr.h"
#include "MNS_Define.h"

namespace SKT {

enum TmapNaviStatus{
    TMAPNAVISTATUS_UNKNOWN = 0,
    TMAPNAVISTATUS_BUSY,  // POI, RP REQUESTING
    TMAPNAVISTATUS_MAP_UPDATING, // MAP UPDATE
    TMAPNAVISTATUS_MAP_RECOVERY, // NOT SUPPORT
    TMAPNAVISTATUS_ANDO,  // IDLE
    TMAPNAVISTATUS_PLAYROUTE, // ROUTE GUIDANCE_ACTIVE
    TMAPNAVISTATUS_CALCULATING_ROUTE, // RP
    TMAPNAVISTATUS_STANDBY, // NOT SUPPORT
};

struct ViewLevelInfo {
 int mLevel;
 int mSubLevel;
};

struct POICateInfo {
 QString m_categoryName;
 uint m_poiCategoryId;
};

}

Q_DECLARE_METATYPE(SKT::ViewLevelInfo)
Q_DECLARE_METATYPE(TS_ROUTE_POS)
#endif // NAVIGATIONDATA_H
