#include <QMetaType>
#include <QDBusMetaType>
#include "guicontroltype.h"

void GuiControlType::registerMetaTypes()
{
    qDBusRegisterMetaType<GCKeyInfo>();
    qDBusRegisterMetaType<PopupProperty>();
    qDBusRegisterMetaType<TouchInfo>();
    qDBusRegisterMetaType<EnavHMIProperty>();
    qDBusRegisterMetaType<GCNaviGadgetFocusInfo>();
    qDBusRegisterMetaType<FocusInteractionInfo>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const GCKeyInfo& keyInfo)
{
    argument.beginStructure();
    argument << keyInfo.m_key;
    argument << keyInfo.m_state;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GCKeyInfo &keyInfo)
{
    argument.beginStructure();
    argument >> keyInfo.m_key;
    argument >> keyInfo.m_state;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const PopupProperty& popupProperty)
{
    argument.beginStructure();
    argument << popupProperty.m_LayerId;
    argument << popupProperty.m_SurfaceId;
    argument << popupProperty.m_Width;
    argument << popupProperty.m_Height;

    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PopupProperty &popupProperty)
{
    argument.beginStructure();
    argument >> popupProperty.m_LayerId;
    argument >> popupProperty.m_SurfaceId;
    argument >> popupProperty.m_Width;
    argument >> popupProperty.m_Height;

    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const TouchInfo& touchInfo)
{
    argument.beginStructure();
    argument << touchInfo.touchState;
    argument << touchInfo.xPos;
    argument << touchInfo.yPos;
    argument << touchInfo.timeStamp;

    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, TouchInfo &touchInfo)
{
    argument.beginStructure();
    argument >> touchInfo.touchState;
    argument >> touchInfo.xPos;
    argument >> touchInfo.yPos;
    argument >> touchInfo.timeStamp;

    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const EnavHMIProperty& property)
{
    argument.beginStructure();
    argument << property.hmiPropertyType;
    argument << property.hmiPropertyValue;

    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, EnavHMIProperty &property)
{
    argument.beginStructure();
    argument >> property.hmiPropertyType;
    argument >> property.hmiPropertyValue;

    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GCNaviGadgetFocusInfo& forcusInfo)
{
    argument.beginStructure();
    argument << forcusInfo.m_GadgetInstanceId;
    argument << forcusInfo.m_FocusStatus;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GCNaviGadgetFocusInfo &forcusInfo)
{
    argument.beginStructure();
    argument >> forcusInfo.m_GadgetInstanceId;
    argument >> forcusInfo.m_FocusStatus;
    argument.endStructure();

    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const FocusInteractionInfo& interactionInfo)
{
    argument.beginStructure();
    argument << interactionInfo.instanceId;
    argument << interactionInfo.interactionType;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, FocusInteractionInfo &interactionInfo)
{
    argument.beginStructure();
    argument >> interactionInfo.instanceId;
    argument >> interactionInfo.interactionType;
    argument.endStructure();

    return argument;
}

