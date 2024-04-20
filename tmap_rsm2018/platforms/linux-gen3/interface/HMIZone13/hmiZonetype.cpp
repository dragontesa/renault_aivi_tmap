#include <QMetaType>
#include <QDBusMetaType>
#include "hmiZonetype.h"

void HMIZonetype::registerMetaTypes()
{

    qDBusRegisterMetaType<textInfo>();
    qDBusRegisterMetaType<textInfoList>();
    qDBusRegisterMetaType<SliderData>();
    qDBusRegisterMetaType<Content>();
    qDBusRegisterMetaType<ContentInfo>();
	qDBusRegisterMetaType<Zone1InfoHolder>();
	qDBusRegisterMetaType<Zone3InfoHolder>();
    qDBusRegisterMetaType<NodeBg>();
    qDBusRegisterMetaType<RestrictionInfo>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const NodeBg &nb)
{
    argument.beginStructure();
    argument << nb.m_bgBasePath;
    argument << nb.m_bgColor;
    argument << nb.m_textColor;
    argument << nb.m_textEffect;
    argument << nb.m_bgType;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NodeBg &nb)
{
    argument.beginStructure();
    argument >> nb.m_bgBasePath;
    argument >> nb.m_bgColor;
    argument >> nb.m_textColor;
    argument >> nb.m_textEffect;
    argument >> nb.m_bgType;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const RestrictionInfo &ri)
{
    argument.beginStructure();
    argument << ri.m_speedRestrictionCategory;
    argument << ri.m_speedRestrictionBehavior;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, RestrictionInfo &ri)
{
    argument.beginStructure();
    argument >> ri.m_speedRestrictionCategory;
    argument >> ri.m_speedRestrictionBehavior;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const SliderData &sd)
{
    argument.beginStructure();
    argument << sd.m_currentSliderValue;
    argument << sd.m_sliderText;
    argument << sd.m_maximumSliderValue;
    argument << sd.m_minimumSliderValue;
    argument << sd.m_sliderEnableStatus;
    argument << sd.m_sliderSwipeEnable;
    argument << sd.m_sliderMarkerMomement;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SliderData &sd)
{
    argument.beginStructure();
    argument >> sd.m_currentSliderValue;
    argument >> sd.m_sliderText;
    argument >> sd.m_maximumSliderValue;
    argument >> sd.m_minimumSliderValue;
    argument >> sd.m_sliderEnableStatus;
    argument >> sd.m_sliderSwipeEnable;
    argument >> sd.m_sliderMarkerMomement;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Content &ci)
{
    argument.beginStructure();
    argument << ci.m_nodeIndex;
    argument << ci.m_bgInfo;
    argument << ci.m_nodeType;
    argument << ci.m_iconPath;
    argument << ci.m_buttonState;
    argument << ci.m_textData;
    argument << ci.m_speedRestriction;
    argument << ci.m_editfieldMode;
    argument << ci.m_sdsType;
    argument << ci.m_floatValue;
    argument << ci.m_nodeLayoutType;
    argument << ci.m_sliderInfo;
    argument << ci.m_keypadFormat;
    argument << ci.m_inwardExitAnimationStatus;
    argument << ci.m_outwardExitAnimationStatus;
    argument << ci.m_inwardEntryAnimationStatus;
    argument << ci.m_outwardEntryAnimationStatus;
    argument << ci.m_defaultFocusOrder;
    argument << ci.m_focusOrder;
    argument << ci.m_focusable;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Content &ci)
{
    argument.beginStructure();
    argument >> ci.m_nodeIndex;
    argument >> ci.m_bgInfo;
    argument >> ci.m_nodeType;
    argument >> ci.m_iconPath;
    argument >> ci.m_buttonState;
    argument >> ci.m_textData;
    argument >> ci.m_speedRestriction;
    argument >> ci.m_editfieldMode;
    argument >> ci.m_sdsType;
    argument >> ci.m_floatValue;
    argument >> ci.m_nodeLayoutType;
    argument >> ci.m_sliderInfo;
    argument >> ci.m_keypadFormat;
    argument >> ci.m_inwardExitAnimationStatus;
    argument >> ci.m_outwardExitAnimationStatus;
    argument >> ci.m_inwardEntryAnimationStatus;
    argument >> ci.m_outwardEntryAnimationStatus;
    argument >> ci.m_defaultFocusOrder;
    argument >> ci.m_focusOrder;
    argument >> ci.m_focusable;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Zone1InfoHolder &z1h)
{
    argument.beginStructure();
    argument << z1h.m_zone1Type;
    argument << z1h.m_nodeInfo;
    argument << z1h.m_updateRegion;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Zone1InfoHolder &z1h)
{
    argument.beginStructure();
    argument >> z1h.m_zone1Type;
    argument >> z1h.m_nodeInfo;
    argument >> z1h.m_updateRegion;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Zone3InfoHolder &z3h)
{
    argument.beginStructure();
    argument << z3h.m_zone3Type;
    argument << z3h.m_nodeInfo;
    argument << z3h.m_updateRegion;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Zone3InfoHolder &z3h)
{
    argument.beginStructure();
    argument >> z3h.m_zone3Type;
    argument >> z3h.m_nodeInfo;
    argument >> z3h.m_updateRegion;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const textInfo &data)
{
    argument.beginStructure();
    argument << data.m_textId;
    argument << data.m_textStr;
    argument << data.m_type;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, textInfo &data)
{
    argument.beginStructure();
    argument >> data.m_textId;
    argument >> data.m_textStr;
    argument >> data.m_type;
    argument.endStructure();

    return argument;
}
