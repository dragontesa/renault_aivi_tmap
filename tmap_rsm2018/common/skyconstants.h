#ifndef SKYCONSTANTS_H
#define SKYCONSTANTS_H

namespace SKT {

class SkyConstants {
public:
    enum SkyState {
        SkySunset = 3,
        SkySunny = 2,
        SkySunrise = 1,
        SkyNight = 4,
        SkyRain = 5,
        SkySnow = 6,
        SkyCluster = 7
    };
};

}

#endif
