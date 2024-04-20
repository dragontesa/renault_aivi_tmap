#ifndef ZONE3HELPER_H
#define ZONE3HELPER_H

#include "hmiZonetype.h"

namespace SKT
{

class Zone3Helper
{
public:
    static void setupForShowKeyboard(ContentInfo& ci, const QString& text, int keyboardType=1);
    static void setupForHideKeyboard(ContentInfo& ci);
};

}

#endif // ZONE3HELPER_H
