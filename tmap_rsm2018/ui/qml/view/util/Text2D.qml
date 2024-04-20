import QtQuick 2.0

Item {
    id: itemRoot

    // item property
    // anchors
    // width, height
    // y
    // opacity
    // visible
    // scale - ???
    // clip - ???
    // enabled

    // text property
    property alias font: internalItem.font
    property alias text: internalItem.text
    property alias color: internalItem.color
    property alias maximumLineCount: internalItem.maximumLineCount
    property alias lineHeight: internalItem.lineHeight
    property alias lineHeightMode: internalItem.lineHeightMode
    property alias wrapMode: internalItem.wrapMode
    property alias elide: internalItem.elide
    property alias contentWidth: internalItem.contentWidth
    property alias contentHeight: internalItem.contentHeight

    // text2 property
    property int offsetY: 0
    property bool guideVisible: true
    property bool itemGuideVisible: true
    property int verticalAlignment: -1
    property int horizontalAlignment: -1

    onVerticalAlignmentChanged: {
        if (verticalAlignment == Text.AlignVCenter) {
            internalItem.anchors.verticalCenter = dummyItem.verticalCenter
            internalItem.verticalAlignment = verticalAlignment
            textGuide.anchors.verticalCenter = itemRoot.verticalCenter
        }
    }
    onHorizontalAlignmentChanged: {
        if (horizontalAlignment == Text.AlignHCenter) {
            internalItem.anchors.horizontalCenter = dummyItem.horizontalCenter
            internalItem.horizontalAlignment = horizontalAlignment
            textGuide.anchors.horizontalCenter = itemRoot.horizontalCenter
        }
    }

    Item {
        id: dummyItem
        y: offsetY
        width: parent.width; height: parent.height

        Text {
            id: internalItem
            width: parent.width; height: contentHeight
            Rectangle {
                id: contentGuide
                visible: guideVisible
                width: internalItem.contentWidth; height: internalItem.contentHeight

                color: 'transparent'; border.color: 'green'
            }
        }

        Rectangle {
            visible: guideVisible
            width: internalItem.contentWidth; height: 1; color: 'red';
            anchors.top: internalItem.baseline; anchors.left: internalItem.left
        }
    }
    Rectangle {
        visible: itemGuideVisible
        anchors.fill: parent; color: 'transparent'; border.color: 'white'
    }
    MouseArea {
        anchors.fill: parent
        onClicked: offsetY += 1
        onDoubleClicked: offsetY = 0
        onPressAndHold: {
            if (guideVisible) {
                guideVisible = false
            } else if (itemGuideVisible) {
                itemGuideVisible = false
            } else  {
                guideVisible = true
                itemGuideVisible = true
            }
        }
    }
    Text {
        visible: guideVisible
        anchors.baseline: parent.top
        color: 'yellow'
        text: 'y:' + offsetY
    }
    Rectangle {
        id: textGuide
        visible: guideVisible
        width: internalItem.width; height: internalItem.height
        color: 'transparent'; border.color: 'orange'
    }
}
