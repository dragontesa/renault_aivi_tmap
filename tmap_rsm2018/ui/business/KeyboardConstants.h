#ifndef KEYBOARDCONSTANTS_H
#define KEYBOARDCONSTANTS_H

#include "Constants.h"

namespace SKT {

class KeyboardConstants {
public:
    enum NotificationK {
        ShowKeyboard = Constants::Keyboard,
        HideKeyboard,
        PullDownKeyboard,
        PullUpKeyboard,
        ProcessKeyboard,
        ProcessSystemKeyboard, // target에서 키보드를 눌렀을 때
        SystemKeyboardPressed, // zone2 keyboard 직접 처리를 위해(OTP, 즐겨찾기 명칭 등에서 사용)
        SystemKeyboardFinished,
        SystemKeyboardBackPressed,
        UpdateKeyboard,
        RegistCommonKeyboardInput,
        RegistKeyboardInput,
        NotifyKeyboardOwner,
        SetText,
        ShowKeyboardForMonkeyTest
    };
};

}

#define SYSTEM_KEYBOARD_HEIGHT 407

#define SYSTEM_SHIFT_KEY        "0x21E7"
#define SYSTEM_BACKSPACE_KEY    "0x232B"

#endif
