#ifndef ZONE1HELPER_H
#define ZONE1HELPER_H

#include "hmiZonetype.h"

namespace SKT
{

class Zone1Helper
{
public:
    static void setupForEditBox(ContentInfo& ci);
    static void setupForKeyboard(ContentInfo& ci);
    static void setupForKeyboardPulldown(ContentInfo& ci, const QString& text);
    static void setupForEditBoxWithCountNumber(ContentInfo& ci, const QString& text, int searchCount);
    static void setupForTitle(ContentInfo& ci, const QString& title);
};

}

#endif // ZONE1HELPER_H
