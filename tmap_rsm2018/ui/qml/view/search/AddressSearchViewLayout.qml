import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    id: root

    // 800x400

    property int listWidthMargin: -30
    property int listHeightMargin: -30
    property int categoryBarArrowImageFillMode: Image.Stretch
    property int categoryBarCenterArrowLeftMargin: -30
    property int categoryBarCenterArrowWidthMargin: 60
    property int categoryBarItemWidth: GlobalConstants.kScreenWidth / 3
    property int categoryBarItemCenterWidth: categoryBarItemWidth + categoryBarCenterArrowWidthMargin
    property int barHeight: 70    
    property int shortcutTopMargin: 10
    property int shortcutRightMargin: 20
    property int shortcutButtonSpacing: 4
    property int shortcutButtonWidth: 32
    property int shortcutButtonHeight: 32
    property int shortcutWidth: shortcutButtonWidth * 2 + shortcutButtonSpacing
    property int shortcutButtonFontSize: 40
    property color shortcutButtonColor: '#3a3a40'
    property color shortcutButtonPressedColor: '#2d2d31'
    property font shortcutButtonFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(shortcutButtonFontSize),
        bold: false
    });

    property int inputKeyboardHeight: 419
    property color inputLabelTextColor: '#8a8a99'
    property color inputLabelCswTextColor: '#000000'
    property color inputBgColor: '#3a3a40'
    property int inputLabelFontSize: 35
    property color inputLabelColor: '#8a8a99'
    property color inputTextColor: 'white'
    property int inputDivideLineWidth: 1
    property int inputDivideLineLeftMargin: 28
    property color inputDivideLineColor: '#000000'
    property color inputBottomDivideLineColor: '#404040'
    property int inputTopMargin: 7
    property int inputLeftRightMargin: 10
    property int inputCheckButtonImgWidth: 63
    property int inputCheckButtonImgHeight: 63
    property int inputWrapWidth: 800
    property int inputWrapHeight: 70

    property int lotInputLotLabelLeftMargin: 28
    property int lotInputLotLabelWidth: 108
    property int lotInputInputLeftMargin: 28
    property int lotInputInputWidth: 270
    property int lotInputSanLabelLeftMargin: 30
    property int lotInputSanLabelWidth: 24
    property int lotInputSanCheckLeftMargin: 8
    property int lotInputAptDivideLeftMargin: 20
    property int lotInputAptLabelLeftMargin: 28
    property int lotInputAptLabelWidth: 72
    property int lotInputAptCheckLeftMargin: 8    

    property real bldListLabelFontPointSizeRatio: 0.5
    property int bldListItemDividierHeight: 1
    property int bldInputLotLabelLeftMargin: 28
    property int bldInputLotLabelWidth: 108
    property int bldInputInputLeftMargin: 28
    property int bldInputInputWidth: 577
    property int bldInputKeyboardHeight: 419

    property bool listItemImageFill: false
    property int listWidth: GlobalConstants.kScreenWidth + listWidthMargin
    property int listHeight: 261 + listHeightMargin
    property int listItemWidthMargin: -10
    property int listItemHeightMargin: -10
    property int listLeftMargin: 20
    property int listTopMargin: 30
    property int listItemWidth: 200
    property int listItemHeight: 120

    property real roadListLabelFontPointSizeRatio: 0.5

    Component.onCompleted: {
    }

    function shortcutHeight( parentHeight ) {
        return parentHeight - 8
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L800x480 ) {
            listItemImageFill = true
            shortcutTopMargin = 5
        }
        else if ( layout === LayoutConstants.L820x1024 ) {
            roadListLabelFontPointSizeRatio = 1.0
            bldListLabelFontPointSizeRatio = 1.0
            barHeight = 84
            listWidth = GlobalConstants.kScreenWidth - listLeftMargin
            listHeight = 629 - listTopMargin
            listItemImageFill = false
            shortcutButtonWidth = 84
            shortcutButtonHeight = 83
            categoryBarItemWidth = 290
            categoryBarItemCenterWidth = 304
            categoryBarCenterArrowLeftMargin = 0
            categoryBarArrowImageFillMode = 0
        }
    }
}


