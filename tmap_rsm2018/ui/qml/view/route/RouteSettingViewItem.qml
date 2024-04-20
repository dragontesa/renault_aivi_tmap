import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import "../../component"

Item {
    id: root

    property bool isCurrent: false
    property string address
    property int addressType: -1//getAddressType(visualIndex)
    property int visualIndex: -1
    property string text // not use
    property alias bgVisible: bg.visible
    property alias delEnabled: delButton.enabled
    property alias delVisible: delButton.visible
    property bool showHorizontalLine: true
    property int focusPos: -1
    property color focusTextColor: '#000000'
    property var registNotices: [
        qsTr("출발지 추가 (등록해 주세요)"),
        qsTr("경유지 추가 (등록해 주세요)"),
        qsTr("목적지 추가 (등록해 주세요)")
    ]
    property var titleStrings: [
        qsTr("출발지"),
        qsTr("경유지 1"),
        qsTr("경유지 2"),
        qsTr("경유지 3"),
        qsTr("목적지")
    ]

    function getTitle(index) {
        switch (index) {
        case 0:
            return qsTr("출발지")
        case 1:
        case 2:
        case 3:
            return qsTr("경유지 %1").arg(index)
        case 4:
            return qsTr("목적지")

        default:
            return ""
        }
    }
    function getAddressType(index) {
        switch (index) {
        case 0:
            return 0
        case 1:
        case 2:
        case 3:
            return 1
        case 4:
            return 2
        default:
            return 0
        }
    }

    function getAddress(address, type) {
        var ret
        if (address.length > 0)
            ret = address
        else
            ret = registNotices[type]

        return ret
    }

    function getSource(index) {
        switch (index) {
        case 0: return "route/route_rg_bk_23.png"
        case 1: return "route/route_rg_passby1.png"
        case 2: return "route/route_rg_passby2.png"
        case 3: return "route/route_rg_passby3.png"
        case 4: return "route/route_rg_bk_24.png"
        }
        console.log("ERROR !!! RouteSettingViewItem.qml.getSource("+index+")")
        return ""
    }
    function setFocusToMain() {
        focusPos = 1
        root.state = "focused"
        delButton.state = "otherFocused"
        updateFocusBg()
    }
    function setFocusToDelButton() {
        root.state = ""
        delButton.state = "focused"
    }
    function setLostFocus() {
        root.state = ""
        delButton.state = "normal"
        focusPos = -1
        updateFocusBg()
    }
    function moveToLeft() {
        if (focusPos == 1) {
            if (address.length == 0) {
                return
            }
        }
        if (0 < focusPos)
            focusPos -= 1
    }
    function moveToRight() {
        if (focusPos == 1) {
            if (address.length == 0) {
                return
            }
            if (!delButton.enabled) {
                return
            }
        }
        if (focusPos < 2)
            focusPos += 1
    }

    function isMainFocused() {
        return root.state === "focused"
    }
    function isDelButtonFocused() {
        return delButton.state === "focused"
    }
    function isEditButtonFocused() {
        return focusPos === 0
    }
    function setStatePressed() {
        focusPos = -1
        updateFocusBg()
        root.state = 'pressed'
    }

    function updatePressedBg() {
        focusPos = -1
        bg.anchors.left = root.left
        bg.anchors.right = root.right
        bg.anchors.leftMargin = 10
        bg.anchors.rightMargin= 20
    }

    function updateFocusBg() {
        switch (focusPos) {
        case -1:
            bg.anchors.left = root.left
            bg.anchors.right = root.right
            bg.anchors.leftMargin = 10
            bg.anchors.rightMargin= 20
            root.state = ''
            delButton.state = 'normal'
            break
        case 0:
            bg.anchors.left = titleLabel.left
            bg.anchors.leftMargin = 10
            bg.anchors.right= titleLabel.right
            bg.anchors.rightMargin = 0
            root.state = 'focused'
            delButton.state = 'normal'
            focusTextColor = address.length > 0 ? GlobalConstants.kListTextColor : GlobalConstants.kListSubTextColor
            break
        case 1:
            bg.anchors.left = bg.parent.left
            bg.anchors.leftMargin = titleLabel.width
            bg.anchors.right = bg.parent.right
            bg.anchors.rightMargin = 20
            root.state = 'focused'
            delButton.state = 'otherFocused'
            focusTextColor = '#000000'
            break
        case 2:
            root.state = ''
            delButton.state = 'focused'
            focusTextColor = address.length > 0 ? GlobalConstants.kListTextColor : GlobalConstants.kListSubTextColor
            break
        }
    }

    onFocusPosChanged: {
        updateFocusBg()
    }

    onAddressTypeChanged: {
        addressText.text = getAddress(address, addressType) + Tr.d
    }
    onAddressChanged: {
        addressText.text = getAddress(address, addressType) + Tr.d
    }
    onVisualIndexChanged: {
        if (0 <= visualIndex) {
            switch (visualIndex) {
            case 0: addressType = 0
                break
            case 4: addressType = 2
                break
            default: addressType = 1
                break
            }
        }
    }

    state: ""
    states: [
        State {
            name: ""
            PropertyChanges {
                target: bg
                visible: false
            }
            PropertyChanges {
                target: addressText
                color: address.length > 0 ? GlobalConstants.kListTextColor : GlobalConstants.kListSubTextColor
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: bg
                visible: true
                color: MultiSenseCoordinator.kAmbientColor
            }
            PropertyChanges {
                target: addressText
                color: "#ffffff"
            }
            PropertyChanges {
                target: delButton
                state: 'lpressed'
                visible: !listViewController.visible
            }
            StateChangeScript {
                script: updatePressedBg()
            }
        },
        State {
            name: "focused"
            PropertyChanges {
                target: bg
                visible: true
                color: GlobalConstants.kListFocusColor
            }
            PropertyChanges {
                target: addressText
                color: focusTextColor
            }
        }
    ]

    signal titleClicked
    signal delClicked
    signal addressAreaClicked

    onEnabledChanged: {
        label.color = enabled ? "darkgray" : "lightgray"
    }

    Rectangle {
        id: bg;
        anchors { top: parent.top; bottom: parent.bottom; bottomMargin: 1 }
        visible: false;
        color: MultiSenseCoordinator.kAmbientColor
    }

    Item {
        id: titleLabel
        width: layout.leftItemWidth
        height: parent.height

        RouteSettingViewDragger {
            id: dragger
            anchors.left: parent.left
            anchors.leftMargin: layout.draggerLeftMargin
            anchors.verticalCenter: parent.verticalCenter
            focused: focusPos == 0

            visible: addressType != 0
            enabled: address.length > 0
        }

        Image {
            id: icon
            width: layout.iconWidth
            height: layout.iconHeight
            anchors.left: dragger.right
            anchors.verticalCenter: parent.verticalCenter
            source: Resource.imageUrl(getSource(visualIndex))
        }

        MouseArea {
            anchors.left: dragger.right
            anchors.right: parent.right
            height: parent.height

            onClicked: {
                if (visualIndex != 0)
                    root.titleClicked()
            }
        }

        Rectangle {
            width: layout.verticalLineWidth;
            height: layout.verticalLineHeight;
            color: "#ffffff"; opacity: 0.3
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Rectangle {
        visible: showHorizontalLine
        width: layout.horizontalLineWidth
        height: layout.horizontalLineHeight
        color: "#404040"
        anchors.left: parent.left
        anchors.leftMargin: layout.horizontalLineLeftMargin
        anchors.rightMargin: layout.horizontalLineRightMargin
        anchors.bottom: parent.bottom
    }
    Item {
        anchors.left: titleLabel.right
        anchors.leftMargin: layout.textLeftMargin
        width: delButton.visible ? layout.textWidth - (layout.delButtonWidth + layout.textRightMargin)
                                 : layout.textWidth
        height: parent.height
        MouseArea {
            anchors.fill: parent
            onClicked: root.addressAreaClicked()
        }

        Text {
            id: addressText

            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            text: getAddress(address, addressType)
            color: (address.length > 0) ? GlobalConstants.kListTextColor : GlobalConstants.kListSubTextColor
            elide: Text.ElideRight
            font.family: GlobalConstants.kFontFamily
            font.pixelSize: Resource.pixelToDp(layout.fontSize)
        }
    }
    Item {
        id: delButton
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.verticalCenter: parent.verticalCenter
        visible: address.length > 0

        width: childrenRect.width
        height: childrenRect.height
        state: 'normal'

        // RouteSettingView 에서만 사용되는 Component
        Button {
            state: delButton.state

            y: layout.delButtonY
            width: layout.delButtonWidth
            height: layout.delButtonHeight

            text: qsTr("삭제")
            onClicked: { root.delClicked(); delButton.state = 'normal' }
            onPressed: delButton.state = 'pressed'
            onReleased: delButton.state = 'normal'
        }
    }
} // delegate
