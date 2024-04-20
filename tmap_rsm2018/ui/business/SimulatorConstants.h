#ifndef SIMULATORCONSTANTS_H
#define SIMULATORCONSTANTS_H

#include "Constants.h"

namespace SKT {

class SimulatorConstants {
public:
    enum NotificationK {
        ChangeSimulatorTotalDistance = Constants::Simulator,
        RestoreSimulationView,
        StartSimulator,
        StopSimulator,
        UpdateSimulator,
        ShowSimulatorControl,
        HideSimulatorControl,
        CloseSimulationView
    };
};

}

#endif
