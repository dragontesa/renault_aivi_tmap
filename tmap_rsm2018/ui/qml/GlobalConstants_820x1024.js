.pragma library

/*
 *  Screen Size
 */
var kScreenWidth = 820
var kScreenHeight = 1024
var kScreenZone1Height = 136
var kScreenHardKeyHeight = 92
var kScreenContentHeight = kScreenHeight - kScreenZone1Height - kScreenHardKeyHeight
var kScreenZone3Height = 83
var kScreenZone3Y = kScreenZone1Height + kScreenContentHeight - kScreenZone3Height

/*!
SDI
*/
var kSdiY = 125 + 82
var kSpeedMeterY = kSdiY + 164

/*
 *  List Size
 */
var kListBackgroundWidth = 790
var kListBackgroundLeftMargin = 10
var kListBackgroundRightMargin = 20
var kListLineLeftMargin = 10
var kListLineRightMargin = 20
var kScrollBarWidth = 2
var kScrollBarLeftMarin = 10
var kScrollBarRightMargin = 9
var kListTextHeight = 32
var kListTextLeftMargin = 20
var kListTextRightMargin = 40
var kListSubTextHeight = 28
var kListSubTextLeftMargin = 20
var kListSubTextRightMargin = 40
var kListButtonRightMargin = 40
var kListTextItemHeight = 84
var kListTwoTextItemHeight = 115
var kListViewFooterHeight = kScreenZone3Height + 41

/*
 *  Color
 */
var kBasicBackgoundColor = "#000000"
var kBasicLineColor = "#404040"
var kListNormalColor = "#000000"
var kListPressColor = "#0091ff"
var kListFocusColor = "#ffffff"
var kListLineColor = "#404040"
var kListTextColor = "#ffffff"
var kListTextFocusColor = "#000000"
var kListSubTextColor = "#8a8a99"
var kScrollBarFrontColor = "#0091ff"
var kScrollBarBackColor = "#4c4c59"
var kPressedColor = "#0091ff"
var kNormalColor = "#222226"
var kFocusedColor = "#ffffff"

/*
 *  Font
 */
//var kFontFamily = "Read Condensed"
//var kFontFamily = "Read"
var kFontFamily = "Read Condensed JK"
var kFontFamilySKP = "Read Condensed JK"//"SKPGo B"
var kListTextFontSize = 28
var kListSubTextFontSize = 24
var kGeneralMessageFontSize = 35
var kDialogMessageFontSize = 22

var kOneLineListTextFontSize = 35
var kSecondLineListTextFontSize = 29
var kToggleBTextFontSize = 35

/*
 *  Toast
 */
var kToastUp = 0
var kToastDown = 1
var kToastCenter = 2

/*!
Dialog
*/
var kDialogContentWidth = 380
var kDialogContentHeight = 240


/*
 *list item/menu press color 노출시간 ms
*/
var kPressedColorExposureTime = 200
var kLongPressedColorExposureTime = 300

/*
 * CSW
 */
var kCswFocusColor = '#ffffff'

/*
*
*/
var kGadgetDarkBgColor = Qt.hsva(0, 0, 0.7, 0.3)
var kGadgetBrightBgColor = Qt.hsva(0, 0, 1, 0.3)
