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
    property alias lineCount: internalItem.lineCount
    property alias lineHeight: internalItem.lineHeight
    property alias lineHeightMode: internalItem.lineHeightMode
    property alias wrapMode: internalItem.wrapMode
    property alias elide: internalItem.elide
    property alias contentWidth: internalItem.contentWidth
    property alias contentHeight: internalItem.contentHeight
    property alias truncated: internalItem.truncated

    // text2 property
    property int offsetY: 0
    property int verticalAlignment: -1
    property int horizontalAlignment: -1

    onVerticalAlignmentChanged: {
        if (verticalAlignment < 0)
            return
        if (verticalAlignment == Text.AlignVCenter) {
            internalItem.anchors.verticalCenter = dummyItem.verticalCenter
        }
        internalItem.verticalAlignment = verticalAlignment
    }
    onHorizontalAlignmentChanged: {
        if (horizontalAlignment < 0)
            return
        if (horizontalAlignment == Text.AlignHCenter) {
            internalItem.anchors.horizontalCenter = dummyItem.horizontalCenter
        }
        internalItem.horizontalAlignment = horizontalAlignment
    }

    Item {
        id: dummyItem
        y: offsetY
        width: parent.width; height: parent.height

        Text {
            id: internalItem
            width: parent.width; height: parent.height
        }
    }
}
