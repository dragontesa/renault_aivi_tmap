#ifndef HARDKEY_H
#define HARDKEY_H

namespace SKT
{

/*
 * the below enummerations were named based on the sections on AIVI Touch Simulation
 * Itc: ITCommander
 * Hk: HARD KEYS
 * Sw: Steering Wheel
 */

struct HardKey {
    enum {
        NoKey = 0,
        ItcJogDialUp = 960,
        ItcJogDialDown = 961,
        ItcJogDialLeft = 962,
        ItcJogDialRight = 963,
        ItcJogDialSelect = 968,
        ItcJogDialHome = 971,
        ItcJogDialOption = 970,
        ItcJogDialBack = 972,
        ItcJogDialSpinCcw = 1000, // temporary value for testing in Ubuntu
        ItcJogDialSpinCw = 1001, // temporary value for testing in Ubuntu
        HkVolumeUp = 790,
        HkVolumeDown = 791,

        HARDKEYCODE_CSW_ROTARY_ENCODER_BUTTON = 3,
        HARDKEYCODE_SWC_ROTARY_ENCODER_BUTTON = 9,

        FORWARDING_KEY_OFFSET = 810,
    };
    enum {
        Released = 0,
        Pressed,
        AcceptKeyState = Released
    };
};

}

#endif
