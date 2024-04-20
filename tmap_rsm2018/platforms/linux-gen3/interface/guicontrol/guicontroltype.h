#ifndef GUICONTROLTYPE_H
#define GUICONTROLTYPE_H

#include <QDBusArgument>
#define TOAST_POPUP_3           "Toastpopup3"
#define TOAST_POPUP_10          "Toastpopup10"
#define MAP_UPDATE_POPUP        "MapUpdatePopup"
#define NAVI_POPUP              "NaviPopup"

typedef QList<int> InstanceIds;

/**
 * @brief The View ID enum
 */
enum VIEW_ID {
    MAP_MAIN = 304,
    NAVIGATION_SERVICE = 334
};

/**
 * @brief The ContextState enum
 */
enum ContextState {
    CONTEXT_STATE_ACCEPTED = 0,
    CONTEXT_STATE_REJECTED = 1,
    CONTEXT_STATE_COMPLETED =2,
    CONTEXT_STATE_INVALID =3
};

/**
 * @brief The ClientStatus enum
 */
enum ClientStatus {
    ClientStatus_CLIENT_STATUS_READY = 0,
    ClientStatus_CLIENT_STATUS_GUI_INITED,
    ClientStatus_CLIENT_STATUS_GUI_SENSE_READY
};

/**
 * @brief The HMISurfaceState enum
 */
enum HMISurfaceState {
    HMISurfaceState_HMI_SURFACE_FG = 0,
    HMISurfaceState_HMI_SURFACE_BG
};

/**
 * @brief The GenericResults enum
 */
enum GenericResults {
    GenericResults_GENERIC_RESULT_OK = 0,
    GenericResults_GENERIC_RESULT_FAIL
};

/**
 * @brief The GenericResults enum
 * "BEEPTYPE_CLICK" shall be used for Key board.
 */
enum BeepType {
    BEEPTYPE_ERROR = 1,
    BEEPTYPE_ROGER = 2,
    BEEPTYPE_WARN = 3,
    BEEPTYPE_CLICK = 4,
    BEEPTYPE_POI = 5,
    BEEPTYPE_SDS_END = 6,
    BEEPTYPE_SMS_INCOMING = 7,
    BEEPTYPE_SMS_SEND = 8,
    BEEPTYPE_URGENT = 9,
    BEEPTYPE_SPEED_WARN = 10,
    BEEPTYPE_CURVE_WARN = 11,
    BEEPTYPE_AVM_OPERATION = 12,
    BEEPTYPE_AVM_ERROR = 13,
    BEEPTYPE_SPEED_HINT = 14,
    BEEPTYPE_CURVE_HINT = 15,
    BEEPTYPE_IPA_NOSOUND = 16,
    BEEPTYPE_IPA_3 = 17,
    BEEPTYPE_IPA_7 = 18,
    BEEPTYPE_IPA_8 = 19,
    BEEPTYPE_IPA_9 = 20,
    BEEPTYPE_IPA_10 = 21,
    BEEPTYPE_IPA_11 = 22,
    BEEPTYPE_IPA_STOP_SOUND = 23,
    BEEPTYPE_SDS_START = 24,
    BEEPTYPE_IPA_4 = 25,
    BEEPTYPE_IPA_5 = 26,
    BEEPTYPE_IPA_6 = 27,
    BEEPTYPE_GST_01 = 65,
    BEEPTYPE_GST_02 = 66,
    BEEPTYPE_GST_03 = 67,
    BEEPTYPE_GST_04 = 68,
    BEEPTYPE_GST_05 = 69,
    BEEPTYPE_GST_06 = 70,
    BEEPTYPE_GST_07 = 71,
    BEEPTYPE_GST_08 = 72,
    BEEPTYPE_GST_09 = 73,
    BEEPTYPE_GST_10 = 74,
    BEEPTYPE_GST_11 = 75,
    BEEPTYPE_GST_12 = 76,
    BEEPTYPE_GST_13 = 77,
    BEEPTYPE_GST_14 = 78,
    BEEPTYPE_WARN_NOT_DEF = 255
};

/**
 * @brief The GCNaviGadgetFocusInfo struct
 */
struct GCNaviGadgetFocusInfo {
    GCNaviGadgetFocusInfo() : m_GadgetInstanceId(0), m_FocusStatus(0) { }
    uint m_GadgetInstanceId;
    bool m_FocusStatus;
};

QDBusArgument &operator<<(QDBusArgument &argument, const GCNaviGadgetFocusInfo& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, GCNaviGadgetFocusInfo &data);

/**
 * @brief The GCKeyInfo struct
 */
struct GCKeyInfo {
    GCKeyInfo() : m_key(0), m_state(0) { }
    uint m_key;
    uchar m_state;
};

QDBusArgument &operator<<(QDBusArgument &argument, const GCKeyInfo& keyInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, GCKeyInfo &keyInfo);

/**
 * @brief The popupProperty struct
 */
struct PopupProperty {
    PopupProperty() : m_LayerId(0), m_SurfaceId(0), m_Width(0), m_Height(0) { }
    uint m_LayerId;
    uint m_SurfaceId;
    uint m_Width;
    uint m_Height;
};
QDBusArgument &operator<<(QDBusArgument &argument, const PopupProperty& popupProperty);
const QDBusArgument &operator>>(const QDBusArgument &argument, PopupProperty &popupProperty);

/**
 * @brief The touchInfo struct
 */
struct TouchInfo {
    TouchInfo() : touchState(0), xPos(0), yPos(0), timeStamp(0) { }
    uint8_t touchState;
    uint xPos;
    uint yPos;
    uint timeStamp;
};
QDBusArgument &operator<<(QDBusArgument &argument, const TouchInfo& touchInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, TouchInfo &touchInfo);

/**
 * @brief The FocusInteractionInfo struct
 */
struct FocusInteractionInfo {
    FocusInteractionInfo() : instanceId(0), interactionType(0) { }
    uint instanceId;
    uint interactionType;
};
QDBusArgument &operator<<(QDBusArgument &argument, const FocusInteractionInfo& focusInteractionInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, FocusInteractionInfo &focusInteractionInfo);

/**
 * @brief The EnavHMIProperty struct
 */
struct EnavHMIProperty {
    EnavHMIProperty() : hmiPropertyType(0), hmiPropertyValue(0) { }
    uint hmiPropertyType;
    uint hmiPropertyValue;
};
QDBusArgument &operator<<(QDBusArgument &argument, const EnavHMIProperty& property);
const QDBusArgument &operator>>(const QDBusArgument &argument, EnavHMIProperty &property);

Q_DECLARE_METATYPE(GCKeyInfo)
Q_DECLARE_METATYPE(PopupProperty)
Q_DECLARE_METATYPE(TouchInfo)
Q_DECLARE_METATYPE(EnavHMIProperty)
Q_DECLARE_METATYPE(GCNaviGadgetFocusInfo)
Q_DECLARE_METATYPE(FocusInteractionInfo)

/**
 * @brief The GuiControlType class
 */
class GuiControlType
{
public:
    static void registerMetaTypes();
};



#endif // GUICONTROLTYPE_H
