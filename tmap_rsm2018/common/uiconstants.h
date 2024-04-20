#ifndef UICONSTANTS_H
#define UICONSTANTS_H

namespace SKT
{

class UIConstants {
public:
	enum Appearance {
		App,
		Widget
	};
	
    enum GadgetType {
        GadgetNone = 0,
        GadgetXXXL = 83,
        GadgetXXL = 82,
        GadgetXL = 35,
        GadgetLarge = 51,
        GadgetMedium = 52,
        GadgetSmall = 53,
        GadgetIconParking = 54,
        GadgetIconFuel = 55,

        GadgetIconAqi = 214, //FineDust

        GadgetAqiBase = 999,
        GadgetAqiXXXL,
        GadgetAqiXXL,
        GadgetAqiXL,
        GadgetAqiLarge,
        GadgetAqiMedium,
        GadgetAqiSmall,

        GadgetTest = 9999
    };

    static QString gadgetTypeToString(int type) {
        QString ret;

        switch (type) {
        case GadgetXXXL:
            ret = QStringLiteral("XXXL");
            break;
        case GadgetXXL:
            ret = QStringLiteral("XXL");
            break;
        case GadgetXL:
            ret = QStringLiteral("XL");
            break;
        case GadgetLarge:
            ret = QStringLiteral("L");
            break;
        case GadgetMedium:
            ret = QStringLiteral("M");
            break;
        case GadgetSmall:
            ret = QStringLiteral("S");
            break;
        case GadgetTest:
            ret = QStringLiteral("Test");
            break;
        }

        return ret;
    }

    static bool isIconGadget(int type) {
        bool ret = false;

        switch (type) {
        case GadgetIconAqi:
            ret = true;
            break;
        case GadgetIconFuel:
            ret = true;
            break;
        case GadgetIconParking:
            ret = true;
            break;
        }

        return ret;
    }

    static bool isNaviMapGadget(int type) {
        bool ret = false;

        switch (type) {
        case GadgetXXL:
            ret = true;
            break;
        case GadgetXL:
            ret = true;
            break;
        }

        return ret;
    }

    static bool isNaviGadget(int type) {
        bool ret = false;

        switch (type) {
        case GadgetXXL:
            ret = true;
            break;
        case GadgetXL:
            ret = true;
            break;
        case GadgetLarge:
            ret = true;
            break;
        case GadgetMedium:
            ret = true;
            break;
        case GadgetSmall:
            ret = true;
            break;
        }

        return ret;
    }

    static QSize gadgetSize(GadgetType type) {
        int width = 0;
        int height = 0;

        switch (type) {
        case GadgetXXL:
            width = 800;
            height = 390;
            break;
        case GadgetXL:
            width = 598;
            height = 390;
            break;
        case GadgetLarge:
            width = 396;
            height = 390;
            break;
        case GadgetMedium:
            width = 396;
            height = 191;
            break;
        case GadgetSmall:
            width = 194;
            height = 191;
            break;
        case GadgetAqiSmall:
            width = 194;
            height = 191;
            break;
        case GadgetAqiMedium:
            width = 396;
            height = 191;
            break;
        case GadgetAqiLarge:
            width = 396;
            height = 390;
            break;
        case GadgetAqiXL:
            width = 800;
            height = 191;
            break;
        case GadgetTest:
            width = 800;
            height = 1024;
            break;
        }

        return QSize(width, height);
    }

    static QString gadgetPath(GadgetType type, const QString& tag, int tick = 0) {
#ifdef BUILD_TARGET
        auto path = QString("/var/opt/bosch/navdata/log/screenshot/gadget_%1_%2_%3.png").arg(type).arg(tag).arg(tick);
#else
        auto path = QString("/home/thirdpartydev/tmap_rsm2018/dynamic/gadget_%1_%2_%3.png").arg(type).arg(tag).arg(tick);
#endif
        return path;
    }
};

}

#endif
