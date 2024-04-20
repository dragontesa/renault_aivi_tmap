import QtQuick 2.1
import QtGraphicalEffects 1.0

import "../singleton"

TFocusItem{
    id: root

    width:   parent.width
    height:  parent.height

    hasFocus: false

    // 17.6.21 hskim
    property alias label: txt.text
    property alias radius: box.radius
    property alias fontPixelSize: txt.font.pixelSize;
    property alias frame: box
    property alias normalBackgroundColor: box.colorN
    property alias pressedBackgroundColor: box.colorP
    property alias labelNormalColor: txt.colorN
    property alias labelPressedColor: txt.colorP

    //[ID]
    property alias root      : root
    property alias box       : box
    property alias outline   : outline
    property alias img       : img
    property alias txt       : txt
    property alias mouseArea : mouseArea

    //[속성]
    property alias change    : root.state

    property bool listbar: false
    property var exclusiveGroup: null
    property int checked     : 0
    property color overlayColor: MultiSenseCoordinator.kAmbientColor
    property bool mutisense  : false
    property bool enabledOpacity: true
    opacity: enabledOpacity ? (!enabled) ? 0.4 : 1.0 : 1.0


    //[시그널]
    signal buttonPressed()
    signal buttonReleased()
    signal buttonClicked()
    signal buttonLongClicked()

    onCheckedChanged: {
        if(checked) {
            if (exclusiveGroup !== null) {
                exclusiveGroup.selected = root
                root.state = "selected"
            }
        }
    }

    //[상태]
    state: "normal"

    states: [
        State {
            name: "normal"
            PropertyChanges { target: box;  opacity : box.opacityN }
            PropertyChanges { target: box;  color : box.colorN }
            PropertyChanges { target: img;  source: img.sourceN }
            PropertyChanges { target: txt;  color : txt.colorN }
        },
        State {
            name: "pressed"
            PropertyChanges { target: box;  opacity : box.opacityP }
            PropertyChanges { target: box;  color : box.colorP }
            PropertyChanges { target: img;  source: img.sourceP }
            PropertyChanges { target: txt;  color : txt.colorP }
        },
        State {
            name: "selected"
            PropertyChanges { target: box;  opacity : (exclusiveGroup === null || exclusiveGroup.selected === root) ? box.opacityP : box.opacityN  }
            PropertyChanges { target: box;  color : (exclusiveGroup === null || exclusiveGroup.selected === root) ? box.colorP : box.colorN  }
            PropertyChanges { target: img;  source: (exclusiveGroup === null || exclusiveGroup.selected === root) ? img.sourceP: img.sourceN }
            PropertyChanges { target: txt;  color : (exclusiveGroup === null || exclusiveGroup.selected === root) ? txt.colorP: txt.colorN }
        }
    ]

    Rectangle {
        id: box
        x: margins
        y: margins
        width: parent.width  - margins
        height: parent.height - margins
        scale: root.state === "pressed" ? 0.85 : 1.0
        property color colorN   : "#8B8B8B"
        property color colorP   : "gray"
        property real opacityN   : 1.0
        property real opacityP   : 1.0
        property real margins    : 0.0
        Image {
            id: img
            property string sourceN
            property string sourceP
            property string sourceF
            scale: root.state === "pressed" ? 0.85 : 1.0
        }
        ColorOverlay {
            anchors.fill: mutisense? img : undefined
            source: mutisense? img : undefined
            cached: true
            color: overlayColor
            scale: !listbar && root.state === "pressed" ? 0.85 : 1.0
        }
    }

    Text {
        id:txt
        property color colorN  : "white"
        property color colorP  : "white"
        property color colorF  : "#000000"
        anchors.centerIn: box
        scale: root.state === "pressed"  ? 0.85 : 1.0
    }

    Rectangle {
        id: outline
        width: parent.width
        height: parent.height
        color: "transparent"
        scale: root.state === "pressed" ? 0.85 : 1.0
    }

    MouseArea {
        id: mouseArea
        anchors.fill: root

        onPressedChanged: {
            if (pressed) {
                buttonPressed()
                root.state = "pressed"
            } else {
                if (exclusiveGroup !== null &&
                    exclusiveGroup.selected === root) {
                    root.state = "selected"
                } else {
                    root.state = "normal"
                }
                buttonReleased()
            }
        }

        onClicked: {
            if (exclusiveGroup !== null) {
                root.state = "selected"
                exclusiveGroup.selected = root
            }
            buttonClicked()
        }
        onDoubleClicked: {}
    }

}
