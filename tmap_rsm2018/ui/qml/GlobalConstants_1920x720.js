.pragma library

/*
 *  Screen Size
 */
var kFullScreenWidth = 1920
var kFullScreenHeight = 720
var kScreenZone1Height = 136
var kScreenHardKeyHeight = 92
var kScreenWidth = kFullScreenWidth - 173 * 2
var kScreenHeight = kFullScreenHeight - kScreenZone1Height - kScreenHardKeyHeight
var kScreenContentHeight = kScreenHeight - kScreenZone1Height - kScreenHardKeyHeight
var kScreenZone3Height = 83
var kScreenZone3Y = kScreenZone1Height + kScreenHeight - kScreenZone3Height

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
var kListSubTextColor = "#8a8a99"
var kScrollBarFrontColor = "#0091ff"
var kScrollBarBackColor = "#4c4c59"
var kPressedColor = "#0091ff"
var kNormalColor = "#222226"
var kFocusedColor = "#ffffff"

/*
 *  Font
 */
var kFontFamily = "Read Condensed"
var kFontFamilySKP = "Read Condensed"//"SKPGo B"
var kListTextFontSize = 28
var kListSubTextFontSize = 24

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
