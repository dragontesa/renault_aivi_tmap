#ifndef RGDATAPTR_H
#define RGDATAPTR_H

#include <QSharedPointer>

#include "VSM_Navi_RgData.h"

namespace SKT {

typedef QSharedPointer<VSM_stDriveInfo> RGDataPtr;

}

Q_DECLARE_METATYPE(VSM_stDriveInfo)
Q_DECLARE_METATYPE(SKT::RGDataPtr)


#endif // RGDATAPTR_H
