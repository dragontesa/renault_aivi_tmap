#ifndef ZONEDATASERVICETYPE_H
#define ZONEDATASERVICETYPE_H

#include <QDBusArgument>

/**
 * @brief The Zone1Type enum
 */
enum Zone1Type {
    HOME_PAGE = 0,                  // 0
    TITLE,                          // 1
    TITLE_PHONE,                    // 2
    Zone1Type_SEARCH_BUTTON,        // 3
    SEARCH_BUTTON_PHONE,            // 4
    SEARCH_BUTTON_ABC,              // 5
    SEARCH_BUTTON_ABC_PHONE,        // 6
    SEARCH_BAR,                     // 7
    SEARCH_TEXT_1,                  // 8
    SEARCH_TEXT_2,                  // 9
    NAVIGATION_FEEDBACK,            // 10
    AUDIO_FEEDBACK,                 // 11
    HOME_MENU_EDIT_1,               // 12
    HOME_MENU_EDIT_2,               // 13
    Zone1Type_TOGGLE,               // 14
    VOICE_REC,                      // 15
    ONLY_CLOCK,                     // 16
    NAV_DEBUG_MENU,                 // 17
    NAV_SEARCH_BAR,                 // 18
    Zone1Type_NO_DATA,              // 19
    ABC_SEARCHLIST_002,             // 20
    SEARCH_TEXT_ONLINE_OFFLINE_001, // 21
    CLOCK_TEXT_TITLE,               // 22
    SEARCH_TEXT_ONLINE_OFFLINE_002, // 23
    TITLE_COMPOSITE,                // 24
    MANAGED_APPS,                   // 25
    SEARCH_BUTTON_ABC_MULTI_PHONE,  // 26
    CLOCK_TEMPERTATUR,              // 27
    Zone1Type_EMPTY,                // 28
    TITLE_TCU_NETWORK_STATUS,       // 29
    ZONE1TYPES_END                  // 30
};


/**
 * @brief The Zone3Type enum
 */
enum Zone3Type {
    BASIC = 100,                    // 100
    BUTTONS_1,                      // 101
    BUTTONS_2,                      // 102
    BUTTONS_2X2,                    // 103
    TABS_2,                         // 104
    TABS_3,                         // 105
    TABS_4,                         // 106
    TABS_5,                         // 107
    VIDEO_PLAYER,                   // 108
    VIDEO_PLAYER_2,                 // 109
    PHOTO_PLAYER,                   // 110
    NEXT_PREVIOUS,                  // 111
    PHONE_CALLS,                    // 112
    Zone3Type_SLIDER,               // 113
    Zone3Type_PAGE_INDICATOR,       // 114
    HOMEPAGESLIDER,                 // 115
    NAVI_NCS,                       // 116
    BUTTONS_2X2_NAVI,               // 117
    BUTTONS_2X2_NAVI_119,           // 118
    VOLUME,                         // 119
    VR_DESAMBIGUATE,                // 120
    BASIC_VR,                       // 121
    BASIC_VR_C,                     // 122
    HFP_GUIDANCE,                   // 123
    ONLY_TEXT,                      // 124
    BUTTONS_3,                      // 125
    KEYBOARD_FREEMODE,              // 126
    KEYBOARD_MATCHMODE,             // 127
    Zone3Type_NO_DATA,              // 128
    TABS_3_ADAS,                    // 129
    KEYBOARD_FREEMODE_ZONE2,        // 130
    OPTIONS_BUTTON_1,               // 131
    BUTTONS_1_MEDIUM,               // 132
    BUTTONS_1_LARGE,                // 133
    ICONTEXT_TABS3,                 // 134
    ZONE3TYPES_END                  // 135
};

enum ZoneVisibilityType {
    Zone1Visibility = 2,
    Zone3Visibility
};

/**
 * @brief The enButtonIndex enum
 */
enum enButtonIndex {
    BTN_Z1_SEARCH = 0,
    BTN_Z1_SEARCH_ABC,
    BTN_Z1_SEARCH_NUMBER,
    BTN_Z1_SEARCH_KEYBOARD,
    BTN_Z1_TRASH,
    BTN_Z1_TRASHBIG,
    BTN_Z1_TOGGLE,
    BTN_Z3_SETTING_ENABLE,
    BTN_Z3_SETTING_DISABLE,
    BTN_Z3_BUTTON1,
    BTN_Z3_BUTTON2,

    BTN_Z3_BUTTON3, // = 11
    BTN_Z3_BUTTON4,
    BTN_Z3_BUTTON5,
    BTN_Z3_TAB_BTN1,
    BTN_Z3_TAB_BTN2,
    BTN_Z3_TAB_BTN3,
    BTN_Z3_TAB_BTN4,
    BTN_Z3_TAB_BTN5,
    BTN_Z3_BACK,
    BTN_PHONECALL_FEEDBACK_S34,

    BTN_Z3_BUTTON_2X2_1, // = 21
    BTN_Z3_BUTTON_2X2_2,
    BTN_Z3_BUTTON_2X2_3,
    BTN_Z3_BUTTON_2X2_4,
    BTN_SPVR_FEEDBACK,
    BTN_Z3_MENU,
    BTN_Z3_PROFILE,
    BTN_Z3_PAGEINDICATOR1,
    BTN_Z3_PAGEINDICATOR2,
    BTN_Z3_PAGEINDICATOR3,

    BTN_Z3_PHONECALL, // = 31
    BTN_Z3_VR_PREVIOUS,
    BTN_Z3_VR_NEXT,
    BTN_Z3_VR_HELP,
    BTN_AUDIO_FEEDBACK,
    BTN_NAVI_FEEDBACK,
    BTN_HANGUP,
    BTN_CLOCK,
    BTN_PHONECALL2_FEEDBACK,
    BTN_PHONECALL2_HOLD,

    BTN_TITLE, // = 41
    BTN_NOTIFICATION,
    BTN_PHONEINFO,
    BTN_ZONE4_STANDBY,
    BTN_ZONE4_MENU,
    BTN_ZONE4_HOME,
    BTN_ZONE4_VEHICLE_SETTING,
    BTN_ZONE4_VOLUME_SETTING,
    BTN_Z3_AMBIENTMINUS,
    BTN_Z3_AMBIENTPLUS,

    BTN_Z1_SOURCES, // = 51
    BTN_HANGUP_TCU_XCALL,
    BTN_TCU_XCALL_FEEDBACK,
    BTN_Z3_SPELLER_OPTION,
    BTN_Z1_SEARCH_TEXT_AREA,
    BTN_Z1_SEARCH_TEXT_002_FREE_MODE,
    BTN_Z1_SEARCH_TEXT_001_MATCH_MODE,
    BTN_Z1_SEARCH_TEXT_FREE_MODE_002,
    BTN_Z1_SEARCH_TEXT_MATCH_MODE_002,
    BTN_Z1_HOME_PAGE_PROFILE,

    BTN_Z1_PAGEINDICATOR1, // = 61
    BTN_Z1_PAGEINDICATOR2,
    BTN_Z1_PAGEINDICATOR3,
    BTN_Z1_PAGEINDICATOR4,
    BTN_Z1_PAGEINDICATOR5,
    BTN_Z1_PAGEINDICATOR6,
    BTN_Z1_PAGEINDICATOR7,
    BTN_Z1_PAGEINDICATOR8,
    BTN_Z3_PREVIOUS,
    BTN_Z3_NEXT,
    BTN_CLOSE_ONOFF_POUPUP
};

/**
 * @brief The ZoneButtonState enum
 */
enum ZoneButtonState {
    ACTIVE = 0,         // 0
    NORMAL,             // 1
    PRESSED,            // 2
    DISABLED,           // 3
    FOCUSED_NORMAL,     // 4
    INACTIVE,           // 5
    INVISIBLE,          // 6
//    AVAILABLE           // 6      deprecated since FW 19.0V10
};

/**
 * @brief The ZoneColorTransparency enum
 */
enum ZoneColorTransparency {
    ZONECOLOR_DEFAULT = 0,
    ZONECOLOR_BLACK,
    ZONECOLOR_TRANSPARENT,
};

/**
 * @brief The NotificationType enum
 */
enum NotificationType {
    AUDIO_TITTLE = 0,
    AUDIO_WITHOUT_TITTLE,
    DIGITAL_TV,
    INCOMING_OUTGOING_CALL,
    DURING_CALL,
    DURING_CATCH_ARVIAL,
    DURING_CATCH_CALL,
    DURING_CALL_HANDSET,
    ON_HOLD,
    SPVR,
    PHONECALL_ONGOING_CALL1,
    PHONECALL_ONGOING_CALL2,
    END_CALL,
    NAVIGATION,
    CARPLAY_AA,
    NotificationType_INVALID = 255,
};

/**
 * @brief The SDSType enum
 */
enum SDSType {
    LISTENING = 0,
    THINKING,
    SPEAKING,
};

/**
 * @brief The SRCategory enum
 */
enum SRCategory {
    VIDEO_PLAYING = 1,
    PICTURE_SLIDESHOW,
    SMS_TEXT_INPUT,
    FEATURE_TUTORIAL,
    SYS_USER_HANDBOOK,
    TEXT_INPUT,
    PHONE_DIAL_INPUT,
    NAVI_DEST_INPUT,
    MUSIC_RADIO_COVER_DISPLAY,
    GENERAL_HMI,
    SEATS,
    SERVICE_ACTIVATION,
    DISPLAYED_TEXT_LENGTH,
    HMI_ANIMATION,
    TYRE_PRESSURE_RESET,
    STATIONARY_PICTURE,
    NONMUSIC_LIST_SCROLL,
    SMOOTH_MAP_SCROLL,
    DRAG_OPERATION,
    PINCH_OPERATION,
    USER_PROFILE,
    HOMEMENU_EDITION,
    CAMERA_SETTINGS,
    MAP_ROTATION,
    PICTURE_TEXT_BG_DISPLAY,
    NISSAN_5_OPERATION,
    NISSAN_8_OPERATION,
    RENAULT_5_OPERATION,
    RENAULT_8_OPERATION,
    AUDIO_PLAY_AUTOMATIC,
    SYSTEM_UPDATE,
    SXM_STOCK,
    MUSIC_LIST_SCROLL,
    LOW_FIDELITY,
    SOFTKEYBOARD,
    SOFTPHONEKEYPAD,
    NONMUSICLIST,
    MUSICLIST,
    JAPANMAP,
    NO_VIDEO_PLAYBACK,
    NO_TEXT_INPUT,
    NO_VOICE_INPUT,
    NO_SETUP_CONFIG,
    DISPLAYED_MSG_LENGTH,
    MAP_SCROLL_BY_HOLD,
    PREVENT_WRONG_OPERATION,
    SRCATEGORY_INVALID = 255,
};

/**
 * @brief The EditfieldMode enum
 */
enum EditfieldMode {
    FREEMODE = 0,
    MATCHMODE,
};

/**
 * @brief The NodeTextColor enum
 */
enum NodeTextColor {
    WHITE = 0,
    BLACK,
};

/**
 * @brief The NodeBgColor enum
 */
enum NodeBgColor {
    NodeBgColor_DEFAULT = 0,
    AMBIENTCOLOR,
    APPLICATIONCOLOR,
    TRANSPARENT,
    NAVIGATION_COLOR,
    TUNER_COLOR,
    MEDIA_COLOR,
    PHONE_COLOR
};

/**
 * @brief The NodeTextEffect enum
 */
enum NodeTextEffect {
    NOSHADOW = 0,
    SHADOW
};

/**
 * @brief The NodeType enum
 */
enum NodeType {
    NodeType_DEFAULT = 0,               // 0
    PULLUP,                             // 1
    ROOT,                               // 2
    BACK,                               // 3
    CLOCK,                              // 4
    SEARCHBAR,                          // 5
    SEARCH_TEXT,                        // 6
    SPELLER,                            // 7
    TEMPERATURE,                        // 8
    PHONEINFO,                          // 9
    Z1_DEFAULT,                         // 10
    Z3_DEFAULT,                         // 11
    WCBS,                               // 12
    SUGGESTION,                         // 13
    CONNECTIVITY,                       // 14
    RES,                                // 15
    NodeType_TOGGLE,                    // 16
    TALKING_HEAD,                       // 17
    NodeType_PAGE_INDICATOR,            // 18
    DEFAULT_GRP_Z3_BTN_1,               // 19
    DEFAULT_GRP_Z3_BTN_2,               // 20
    DEFAULT_GRP_Z3_BTN_3,               // 21
    DEFAULT_GRP_Z3_BTN_2X2,             // 22
    DEFAULT_GRP_Z3_BTN_2X2_NAVI,        // 23
    DEFAULT_GRP_Z3_BTN_2X2_NAVI_119,    // 24
    DEFAULT_GRP_Z3_TAB_2,               // 25
    DEFAULT_GRP_Z3_TAB_3,               // 26
    DEFAULT_GRP_Z3_TAB_4,               // 27
    DEFAULT_GRP_Z3_TAB_5,               // 28
    DEFAULT_GRP_Z3_VIDEO_PLAYER_2,      // 29
    DEFAULT_GRP_Z3_NEXT_PREVIOUS,       // 30
    DEFAULT_GRP_Z3_PHONE_CALL,          // 31
    DEFAULT_GRP_Z3_SLIDER,              // 32
    DEFAULT_GRP_Z3_PAGE_INDICATOR,      // 33
    DEFAULT_GRP_Z3_HOMEPAGESLIDER,      // 34
    DEFAULT_GRP_Z3_NAVI_NCS,            // 35
    DEFAULT_GRP_Z3_VOLUME,              // 36
    DEFAULT_GRP_Z3_VR_DESAMBIGUATE,     // 37
    DEFAULT_GRP_Z3_HFP_GUIDANCE,        // 38
    DEFAULT_GRP_Z3_ONLY_TEXT,           // 39
    DEFAULT_GRP_Z3_TABS_3_ADAS,         // 40
    Z3_BACKGROUND,                      // 41
    HELP,                               // 42
    ABCSEARCH,                          // 43
    HEADERTITLE,                        // 44
    EDITHOMEMENU,                       // 45
    NodeType_SEARCH_BUTTON,             // 46
    TCUNETWORKDATA,                     // 47
    SUGGESTIONCOUNT,                    // 48
    HEADER_TITLE_GRP_TITLE_1,           // 49
    HEADER_TITLE_GRP_TITLE_2,           // 50
    HEADER_TITLE_GRP_TITLE_3,           // 51
    NodeType_SLIDER,                    // 52
    NodeType_INVALID,                   // 53
    DEFAULT_GRP_Z3_BASIC_VR_C,          // 54
    DEFAULT_GRP_Z3_ICONTEXT_TABS_3,     // 55
};

/**
 * @brief The NodeBgType enum
 */
enum NodeBgType {
    NodeBgType_DEFAULT = 0,
    NodeBgType_SEPARATOR
};

/**
 * @brief The KeypadFormat enum
 */
enum KeypadFormat {
    BASIC_QWERTY = 1,
    BASIC_ABC,
    BASIC_qwerty,
    BASIC_abc,
    NUMERIC,
    SPECIAL
};

/**
 * @brief The NodeLayoutType enum
 */
enum NodeLayoutType {
    FLAT = 0,
    OVAL,
    NodeLayoutType_INVALID
};

/**
 * @brief The PullUpListStatus enum
 */
enum PullUpListStatus {
    CLOSED = 0,
    OPEN,
};


/**
 * @brief The textInfo struct
 */
struct textInfo {
    textInfo() : m_textId(0), m_type(false) { }
    uint m_textId;
    QString m_textStr;
    bool m_type;
};
typedef QList<textInfo> textInfoList;

QDBusArgument &operator<<(QDBusArgument &argument, const textInfo &data);
const QDBusArgument &operator>>(const QDBusArgument &argument, textInfo &data);

/**
 * @brief The NodeBg struct
 */
struct NodeBg {
    NodeBg() : m_bgColor(0), m_textColor(0), m_textEffect(0), m_bgType(0) {}
    QString m_bgBasePath;
    uint m_bgColor;
    uint m_textColor;
    uint m_textEffect;
    uint m_bgType;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NodeBg &nb);
const QDBusArgument &operator>>(const QDBusArgument &argument, NodeBg &nb);

/**
 * @brief The RestrictionInfo struct
 */
struct RestrictionInfo {
    RestrictionInfo() : m_speedRestrictionCategory(0), m_speedRestrictionBehavior(0) {}
    uint m_speedRestrictionCategory;
    uint8_t m_speedRestrictionBehavior;
};

QDBusArgument &operator<<(QDBusArgument &argument, const RestrictionInfo &ri);
const QDBusArgument &operator>>(const QDBusArgument &argument, RestrictionInfo &ri);

/**
 * @brief The SliderData struct
 */
struct SliderData {
    SliderData() : m_currentSliderValue(0), m_maximumSliderValue(0), m_minimumSliderValue(0), m_sliderEnableStatus(0), m_sliderSwipeEnable(false), m_sliderMarkerMomement(0) {}
    double m_currentSliderValue;
    QString m_sliderText;
    double m_maximumSliderValue;
    double m_minimumSliderValue;
    uchar m_sliderEnableStatus;
    bool m_sliderSwipeEnable;
    uint m_sliderMarkerMomement;
};

QDBusArgument &operator<<(QDBusArgument &argument, const RestrictionInfo &ri);
const QDBusArgument &operator>>(const QDBusArgument &argument, RestrictionInfo &ri);


/**
 * @brief The ContentInfo struct
 */
struct Content {
    Content() : m_nodeIndex(0), m_nodeType(NodeType_DEFAULT), m_buttonState(ACTIVE),m_editfieldMode(FREEMODE),m_sdsType(LISTENING), m_floatValue(0), m_nodeLayoutType(FLAT), m_keypadFormat(BASIC_QWERTY),
        m_inwardExitAnimationStatus(false), m_outwardExitAnimationStatus(false), m_inwardEntryAnimationStatus(false), m_outwardEntryAnimationStatus(false),
        m_defaultFocusOrder(0), m_focusOrder(0), m_focusable(false) {}
    uint m_nodeIndex;
    NodeBg m_bgInfo;
    uint m_nodeType;
    QString m_iconPath;
    uint m_buttonState;
    textInfoList m_textData;
    RestrictionInfo m_speedRestriction;
    uint m_editfieldMode;
    uint m_sdsType;
    double m_floatValue;
    uint m_nodeLayoutType;
    SliderData m_sliderInfo;
    uint m_keypadFormat;
    bool m_inwardExitAnimationStatus;
    bool m_outwardExitAnimationStatus;
    bool m_inwardEntryAnimationStatus;
    bool m_outwardEntryAnimationStatus;
    short m_defaultFocusOrder;
    short m_focusOrder;
    bool m_focusable;
};

QDBusArgument &operator<<(QDBusArgument &argument, const Content &ci);
const QDBusArgument &operator>>(const QDBusArgument &argument, Content &ci);

typedef QList<Content> ContentInfo;

/**
 * @brief The Zone1InfoHolder struct
 */
struct Zone1InfoHolder {
    Zone1InfoHolder() : m_zone1Type(0), m_updateRegion(false) {}
    uint m_zone1Type;
    ContentInfo m_nodeInfo;
    bool m_updateRegion;
};

QDBusArgument &operator<<(QDBusArgument &argument, const Zone1InfoHolder &z1h);
const QDBusArgument &operator>>(const QDBusArgument &argument, Zone1InfoHolder &z1h);

/**
 * @brief The Zone3InfoHolder struct
 */
struct Zone3InfoHolder {
    Zone3InfoHolder() : m_zone3Type(0), m_updateRegion(false) {}
    uint m_zone3Type;
    ContentInfo m_nodeInfo;
    bool m_updateRegion;
};

QDBusArgument &operator<<(QDBusArgument &argument, const Zone3InfoHolder &z3h);
const QDBusArgument &operator>>(const QDBusArgument &argument, Zone3InfoHolder &z3h);


Q_DECLARE_METATYPE(textInfo)
Q_DECLARE_METATYPE(textInfoList)
Q_DECLARE_METATYPE(SliderData)
Q_DECLARE_METATYPE(Content)
Q_DECLARE_METATYPE(ContentInfo)
Q_DECLARE_METATYPE(Zone1InfoHolder)
Q_DECLARE_METATYPE(Zone3InfoHolder)
Q_DECLARE_METATYPE(NodeBg)
Q_DECLARE_METATYPE(RestrictionInfo)

/**
 * @brief The HMIZonetype class
 */
class HMIZonetype
{
public:
    static void registerMetaTypes();
};


#endif // ZONEDATASERVICETYPE_H
