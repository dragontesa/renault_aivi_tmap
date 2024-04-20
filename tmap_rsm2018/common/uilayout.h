#ifndef UILAYOUT_H
#define UILAYOUT_H

#include <QSize>

namespace SKT
{

class UILayout {
public:
    enum Layout {
        L800x480 = 0,
        L820x1024,
        L1920x720
    };

    static QSize layoutSize( Layout layout) {
        int width = 0;
        int height = 0;

        switch ( layout ) {
        case L800x480:
            width = 800;
            height = 480;
            break;
        case L820x1024:
            width = 820;
            height = 1024;
            break;
        case L1920x720:
            width = 1920;
            height = 720;
            break;
        }

        return QSize(width, height);
    }
};

}

#endif
