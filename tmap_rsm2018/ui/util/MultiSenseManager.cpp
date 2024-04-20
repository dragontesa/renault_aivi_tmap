#include "MultiSenseManager.h"

using namespace SKT;

MultiSenseManager::MultiSenseManager(QObject *parent) : QObject(parent)
{
}

int MultiSenseManager::colorState()
{
    return mColorState;
}

void MultiSenseManager::setColorState(int colorState)
{
    mColorState = colorState;

    emit colorStateChanged();
}

void MultiSenseManager::setColorStateFromHiddenMenu(int color)
{
#if (0)
    int colorState = 0;
    if ( color == 0x75e04b ) {
        colorState = MultiSenseManager::Echo;
    }
    else if ( color == 0x0091ff ) {
        colorState = MultiSenseManager::Comfort;
    }
    else if ( color == 0xadb7c9 ) {
        colorState = MultiSenseManager::Regular;
    }
    else if ( color == 0xe82e15 ) {
        colorState = MultiSenseManager::Sport;
    }
    setColorState(colorState);
#endif
}

QColor MultiSenseManager::color(int colorState) const
{
    QColor color;
    switch (colorState) {
    case Color_Unavailable:
        color.setRgbF(0.0, 0.471, 0.941, 1.0);
        break;
    case Color_1:
        color.setRgbF(0.549, 0.627, 0.784, 1.0);
        break;
    case Color_2:
        color.setRgbF(0.0, 0.471, 0.941, 1.0);
        break;
    case Color_3:
        color.setRgbF(0.471, 0.863, 0.0, 1.0);
        break;
    case Color_4:
        color.setRgbF(1.0, 0.0, 0.0, 1.0);
        break;
    case Color_5:
        color.setRgbF(0.588, 0.0, 1.0, 1.0);
        break;
    case Color_6:
        color.setRgbF(0.0, 0.863, 1.0, 1.0);
        break;
    case Color_7:
        color.setRgbF(1.0, 0.784, 0.0, 1.0);
        break;
    case Color_8:
        color.setRgbF(1.0, 0.353, 0.0, 1.0);
        break;
    case Color_9:
    case Color_10:
    case Color_11:
    case Color_12:
    case Color_13:
    case Color_14:
    case Color_15:
        color.setRgbF(0.0, 0.471, 0.941, 1.0);
        break;
    }
    return color;
}
