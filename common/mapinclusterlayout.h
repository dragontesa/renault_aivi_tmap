#ifndef MAPINCLUSTERLAYOUT_H
#define MAPINCLUSTERLAYOUT_H

#include <QSize>

namespace SKT
{

class MapInClusterLayout {
public:
    enum Layout {
        CenteralMap = 0,
        SideMap,
        HudMap
    };

    enum LayoutSize {
        LargeLayoutWidth   = 800,
        LargeLayoutHeight  = 400,
        SmallLayoutWidth   = 640,
        SmallLayoutHeight  = 480,
    };

    static QSize layoutSize( Layout layout ) {
        QSize ret;
		
        switch ( layout ) {
        case CenteralMap:
            ret = QSize( 800, 400 );
            break;
        case SideMap:
            ret = QSize( 640, 480 );
            break;
        case HudMap:
            ret = QSize( 186, 125 );
            break;
        }
		
        return ret;
    }
};

}

#endif
