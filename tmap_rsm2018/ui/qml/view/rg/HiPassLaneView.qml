import QtQuick 2.1
import HiPassLaneViewEvent 1.0
import "../../component"
import "../../view"

View {
    id: root
    property int logoHeight: 50
    property int itemWidth: 48
    property int itemHeight: 48
    property int iconMargin: 0
    property font tollFeeTextFont: Qt.font({
        pixelSize: Resource.pixelToDp(34)
    });
    property font tollFeeNumberFont: Qt.font({
        pixelSize: Resource.pixelToDp(38)
    });
    property color tollFeeTextColor: '#ffffff'
    property int tollFeeBottomMargin: 174
    property int tollFeeWidth: 240
    property int tollFeeHeight: 70
    property int tollFeeX: 28
    property int tollFeeTextLeftMargin: 8
    property int tollFeeTextRightMargin: 4
    property int tollFeeUnitTextRightMargin: 28

    width: laneRow.width
    height: logoHeight + iconMargin + itemHeight
    visible: false

    property int maxLanes : 0
    property var laneNumbers: []
    property var laneBgs: []
    property alias tollFeeVisible: tollFeeImage.visible
    property string tollFee
    property alias laneVisible: laneRow.visible

    onMediated: {
        sendEvent( HiPassLaneViewEvent.Mediated )
    }

    onVisibleChanged: {
        sendEvent( HiPassLaneViewEvent.VisibleChanged, {visible: visible} )
    }

    onLaneNumbersChanged: {
        maxLanes = laneNumbers.length
        var laneRepeaterItemAt = laneRepeater.itemAt
        for ( var i = 0; i < laneNumbers.length; i++ ) {
            laneRepeaterItemAt( i ).numSource = laneNumbers[i]
        }
    }
	
    onLaneBgsChanged: {
        maxLanes = laneBgs.length
        var laneRepeaterItemAt = laneRepeater.itemAt
        for ( var i = 0; i < laneBgs.length; i++ ) {
            laneRepeaterItemAt( i ).bgSource = laneBgs[i]
        }
    }

    function reset() {
        maxLanes = 0
        laneNumbers = []
        laneBgs = []
        visible = false

        var laneRepeaterItemAt = laneRepeater.itemAt
        for ( var i = 0; i < laneRepeater.count; i++ ) {
            laneRepeaterItemAt( i ).numSource = ''
            laneRepeaterItemAt( i ).bgSource = ''
        }
    }

    onMaxLanesChanged: {
        if ( maxLanes >= 10 ) {
            icon.anchors.left = laneRow.left
            icon.anchors.right = undefined
            icon.anchors.rightMargin = undefined
            icon.anchors.top = undefined
            icon.anchors.bottom = laneRow.top
        }
        else {
            icon.anchors.left = undefined
            icon.anchors.right = laneRow.left
            icon.anchors.rightMargin = 18
            icon.anchors.top = laneRow.top
            icon.anchors.bottom = undefined
        }
    }

    Image {
        id: tollFeeImage
        anchors.bottom: parent.bottom
        anchors.bottomMargin: tollFeeBottomMargin
        anchors.horizontalCenter: laneRow.horizontalCenter
        width: tollFeeWidth
        height: tollFeeHeight
        visible: root.tollFeeVisible

        source: Resource.imageUrl( 'hipass', 'hipass_popup_bg.png')

        Text {
            id: tollFeeTitleText
            x: tollFeeX
            anchors.verticalCenter: parent.verticalCenter
            font: tollFeeTextFont
            color: tollFeeTextColor
            text:  qsTr('요금') + Tr.d
        }
        Text {
            id: tollFeeText
            anchors.left: tollFeeTitleText.right
            anchors.leftMargin: tollFeeTextLeftMargin
            anchors.right: tollFeeUnitText.left
            anchors.rightMargin: tollFeeTextRightMargin
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
            font: tollFeeNumberFont
            color: tollFeeTextColor
            //text:  qsTr('19,200')
            text: root.tollFee
        }
        Text {
            id: tollFeeUnitText
            anchors.right: parent.right
            anchors.rightMargin: tollFeeUnitTextRightMargin
            anchors.verticalCenter: parent.verticalCenter
            font: tollFeeTextFont
            color: tollFeeTextColor
            text:  qsTr('원') + Tr.d
        }
    }

    Image {
        id: icon
        source: Resource.imageUrl( 'hipass', 'hipass_logo.png')
        height: logoHeight
        visible: tollFeeImage.visible || laneRow.visible
    }

    Row {
        id: laneRow
        anchors.bottom: parent.bottom
        width: childrenRect.width
        height: itemHeight
        visible: root.visible
        Repeater {
            id: laneRepeater
            model: maxLanes
            delegate: Item {
                width: itemWidth
                height: itemHeight
                property string numSource
                property string bgSource
                Image {
                    source: parent.bgSource
                }
                Image {
                    source: parent.numSource
                }
                Rectangle {
                    width: 1
                    height: parent.height
                    color: '#325299'
                    visible: index > 0 && index < maxLanes
                }
            }
        }
    }
}
