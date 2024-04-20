#ifndef HARDKEYFILTER_H
#define HARDKEYFILTER_H

#include "guicontroltype.h"

namespace SKT {

class HardKeyFilter
{
public:
    HardKeyFilter();
    bool acceptable(const GCKeyInfo &info);

private:
    GCKeyInfo mKeyInfo;
    quint32 mRepeat = 0;
};

}

#endif // HARDKEYFILTER_H
