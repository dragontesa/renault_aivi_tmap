#ifndef NAVIGATIONPROPERTYPROTOCOL_H
#define NAVIGATIONPROPERTYPROTOCOL_H

namespace SKT {

class NavigationPropertyProtocol {
public:
    enum ProtocolK {
        setCurrentStreet,
        TmapMapVersion,
        TmapNaviStatus,
        TmapRGDataInfo,
        TmapSatelliteInfo,
        TmapViewLevel,
        TmapMapMoveMode,
        TmapViewMode,
        TmapPOICategory,
        TmapVoiceGuidanceType,
        TmapVolume,
        TmapIsMute,
        TmapWorkLocation,
        TmapHomeLocation,
        TmapWaypointList,
        TmapActiveDestinationInfo,
        TmapRouteCalculateDuration,
        TmapRouteRecalculationReason,
        TmapFavoriteChange,
        TmapRecentDestinationChange,
        TmapSearchWordChange,
        TmapUserProfileChanged,
        TmapRouteOptionChanged,
        TmapScreenIDChanged,
        TmapWaypointOperationStatusChanged,
        TmapOverSpeedAlertActive,
    };
};

}

#endif // NAVIGATIONPROPERTYPROTOCOL_H
