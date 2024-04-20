import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import LayoutConstants 1.0

import "../../singleton"
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property alias backgroundOpacity: background.opacity
    property alias color: background.color
    property string nameText
    property string arrivalTimeText
    property color borderColor
    property font font
    property int type: 0 // 1: office, 2: home
    property int rank: -1
    property string iconSource: ""
    property color labelColor
    property color arrivalColor
    property string postfix: '_n.png'

    property int iconTopMargin: 16
    property int noIconTopMargin: 22
    property int textBoxTopMargin: 10
    property int textBoxHeight: 56
    property int subTextTopMargin: 8
    property real textLineHeight: 28 //30.5
    property int boxLeftMargin: 24
    property int fontSize: 28

    property bool isCurrent: false
    onIsCurrentChanged: {
        if (isCurrent) {
            root.state = 'focused'
        } else {
            root.state = 'normal'
        }
    }

    signal pressed()
    signal clicked()

    Component.onCompleted: {
    }

    state: "normal"
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: root
                color: "#222226"
                labelColor: "#ffffff"
                arrivalColor: "#4ca6ff"
                postfix: '_n.png'
                backgroundOpacity: 1.0
            }

        },
        State {
            name: "pressed"
            PropertyChanges {
                target: root
                color: MultiSenseCoordinator.kAmbientColor
                labelColor: "#ffffff"
                arrivalColor: "#ffffff"
                postfix: '_p.png'
                backgroundOpacity: 0.3
            }
        },
        State {
            name: "focused"
            PropertyChanges {
                target: root
                color: "#ffffff"
                labelColor: "#000000"
                arrivalColor: "#000000"
                postfix: '_n.png'
                backgroundOpacity: 0.9
            }
        }
    ]

    onRankChanged: {
        updateIconSource()

        if (layout.screenResolution === LayoutConstants.L800x480) {
            loader.sourceComponent = legacyItemComponent
        } else {
            if (0 <= rank && rank < 3) {
                loader.sourceComponent = topItemComponent
            } else {
                loader.sourceComponent = normalItemComponent
            }
        }
    }
    onPostfixChanged: updateIconSource()

    function updateIconSource() {
        switch (type) {
        case 0: {
                switch(rank) {
                case 0: iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_1" + postfix)}); break;
                case 1: iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_2" + postfix)}); break;
                case 2: iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_3" + postfix)}); break;
                default:
                    break
                }
            }
            break
        case 1:
            iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_office" + postfix)});
            break
        case 2:
            iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_home" + postfix)});
            break
        }
    }

    Component {
        id: topItemComponent
        Item {
            id: topItem
            Image {
                id: icon
                source: iconSource
            }
            Text {
                id: label

                height: textBoxHeight
                text: nameText
                anchors { left: parent.left; leftMargin: boxLeftMargin;
                    right: parent.right; rightMargin: boxLeftMargin;
                    top: icon.bottom; topMargin: textBoxTopMargin }
                color: labelColor
                font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                lineHeightMode: Text.FixedHeight
                lineHeight: textLineHeight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 2
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }
            Text {
                id: arrivalTime
                width: label.width
                anchors { left: label.left; top: label.bottom; topMargin: subTextTopMargin; bottom: parent.bottom; bottomMargin: iconTopMargin}
                text: arrivalTimeText
                color: arrivalColor
                font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Component.onCompleted: {
                if (label.text.length === 0) {
                    iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_default.png")})
                    icon.anchors.centerIn = topItem
                } else {
                    icon.anchors.top = topItem.top
                    icon.anchors.topMargin = iconTopMargin
                    icon.anchors.horizontalCenter = topItem.horizontalCenter
                }
            }
        }
    }

    Component {
        id: normalItemComponent
        Item {
            id: normalItem

            Image {
                id: normalIcon
                source: iconSource
            }
            Text {
                id: normalLabel

                y: 52
                height: textBoxHeight
                text: nameText
                anchors { left: parent.left; leftMargin: boxLeftMargin;
                    right: parent.right; rightMargin: 24; bottom: parent.bottom; bottomMargin: noIconTopMargin
                }
                color: labelColor
                font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                lineHeightMode: Text.FixedHeight
                lineHeight: textLineHeight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 3
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }
            Component.onCompleted: {
                if (normalLabel.text.length === 0) {
                    iconSource = Qt.binding(function(){ return Resource.imageUrl("destination_ic_default.png")})
                    postfix = ''
                    normalIcon.anchors.centerIn = normalItem
                } else {
                    normalIcon.visible = iconSource.length > 0

                    if (normalIcon.visible == false) {
                        normalLabel.anchors.centerIn = normalItem
                        normalLabel.anchors.topMargin = noIconTopMargin
                        normalLabel.anchors.bottomMargin = noIconTopMargin
                        normalLabel.height = normalItem.height - noIconTopMargin - noIconTopMargin
                    }
                    else {
                        normalIcon.anchors.top = normalItem.top
                        normalIcon.anchors.topMargin = (iconSource.length == 0) ? noIconTopMargin:iconTopMargin
                        normalIcon.anchors.horizontalCenter = normalItem.horizontalCenter
                        normalLabel.height = normalItem.height - iconTopMargin - noIconTopMargin
                        normalLabel.anchors.top = normalIcon.bottom
                        normalLabel.anchors.topMargin = textBoxTopMargin
                        normalLabel.verticalAlignment = Text.AlignTop
                    }
                }
            }
        }
    }

    Component {
        id: legacyItemComponent

        Item {
             id: legacyBox

             width: root.width
             height: root.height

             Column {
                 id : top3
                 anchors.fill: parent
                 anchors.margins: 5

                 Image {
                     id: icon
                     width: 26; height: 26
                     fillMode: Image.PreserveAspectFit
                     source : iconSource
                     anchors.horizontalCenter: parent.horizontalCenter
                 }

                 // space holder between icon and label
                 Item {
                     width: parent.width
                     height: textBoxTopMargin
                 }

                 Text {
                     id: topLabelName
                     height: legacyBox.height - icon.height - labelArrivalTime.height-iconTopMargin-textBoxTopMargin-subTextTopMargin
                     //font: root.font
                     font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                     width: parent.width
                     horizontalAlignment: Text.AlignHCenter
                     color:labelColor
                     maximumLineCount: labelArrivalTime.text == "" ? 3 : 2
                     wrapMode: Text.Wrap
                     elide: Text.ElideRight
                     visible: text.length > 0 && rank < 3
                     text: nameText
                 }
                 // space holder between label and arrival time
                 Item {
                     width: parent.width
                     height: subTextTopMargin
                 }
                 Text {
                     id: labelArrivalTime
                     //font: root.font
                     font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                     text: arrivalTimeText
                     width: parent.width
                     height: 20/*paintedHeight*/
                     horizontalAlignment: Text.AlignHCenter
                     visible: text.length > 0 && rank < 3
                     color: arrivalColor
                 }
             }

             // gotta trick for layout like alignment
             Image {
                 id: defaultIcon
                 width: 26
                 height: 26
                 source : Resource.imageUrl("destination_ic_default.png")
                 anchors.centerIn: parent
                 visible: false
             }

             // space holder between icon and label
             Item {
                 id: noarmlLabel
                 anchors.fill: parent
                 anchors.topMargin: textBoxTopMargin*2
                 anchors.bottomMargin: iconTopMargin
                 visible: false

                 // gotta trick for layout like alignment
                 Text {
                     id: normalLabelName
                     //font: root.font
                     font { family: GlobalConstants.kFontFamily ; pixelSize: Resource.pixelToDp(fontSize) }
                     width: parent.width
                     height: parent.height// - iconTopMargin-textBoxTopMargin
                     anchors.verticalCenter:parent.verticalCenter
                     horizontalAlignment: Text.AlignHCenter
                     verticalAlignment: Text.AlignVCenter
                     color:labelColor
                     wrapMode: Text.WordWrap
                     elide: Text.ElideRight
                 }
             }

             Component.onCompleted:
             {
                 // gotta trick for layout like alignment
                 if (nameText == "" || nameText.length == 0) {
                     top3.visible = false
                     defaultIcon.visible = true
                 }
                 else
                 {
                     if (rank > 2)
                     {
                         //top3.visible = false
                         noarmlLabel.visible = true
                         normalLabelName.text = nameText
                     }
                 }
             }
         }
    }

    Rectangle {
        id: background
        anchors.fill: parent
    }

    Loader {
        id: loader
        anchors.fill: parent
    }

    MouseArea {
        enabled: nameText.length > 0
        anchors.fill: loader
        onPressed: { root.state = "pressed"; root.pressed() }
        onReleased: root.state = "normal"
        onCanceled: root.state = "normal"
        onClicked: root.clicked()
    }

    Rectangle {
        id: vline
        width: 1; height: parent.height
        anchors.right: loader.right
        color: borderColor
        opacity: 0.5
        visible: (rank + 1) % 3 !== 0
    }
    Rectangle {
        id: hline
        width: parent.width; height: 1
        anchors.bottom: loader.bottom
        opacity: 0.5
        color: borderColor
    }
}
