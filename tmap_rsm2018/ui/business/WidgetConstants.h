#ifndef WIDGETCONSTANTS_H
#define WIDGETCONSTANTS_H

#include "Constants.h"
#include "uiconstants.h"

namespace SKT {

class WidgetConstants {
    Q_GADGET
public:    
	enum Size {
        SizeNone = 0,
        SizeXXXL = UIConstants::GadgetXXXL,
        SizeXXL = UIConstants::GadgetXXL,
        SizeXL = UIConstants::GadgetXL,
        SizeLarge = UIConstants::GadgetLarge,
        SizeMedium = UIConstants::GadgetMedium,
        SizeSmall = UIConstants::GadgetSmall
	};
	Q_ENUM(Size)

    enum TbtAppearance {
        TbtAll = 0,
        TbtFirst
    };
    Q_ENUM(TbtAppearance)
	
    enum SdiAppearance {
        SdiNone = 0,
        SdiAll,
        SdiAllForLarge,
        SdiSign,
        SdiSignAndRemainDist,
        SdiCurrentSpeed,
        SdiSignOrCurrentSpeed
    };
    Q_ENUM(SdiAppearance)

    enum MapViewExpectedFps {
        FpsOnForeground = 15,
        FpsOnBackground = 1
    };

    WidgetConstants();
};

}

//#define GADGET_BIND_TO_SIMULATOR

#endif
