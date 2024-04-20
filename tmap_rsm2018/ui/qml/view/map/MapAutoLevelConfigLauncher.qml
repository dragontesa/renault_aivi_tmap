import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: rootItem
    objectName: "mapAutoConfigGui"
    width: 60
    height: 20
    x: 2
    y: 158
    visible: false
    property Item configGui : null

    function showGui() {
        if (configGui != null) {
            configGui.anchors.top = btnShow.top
            configGui.anchors.topMargin = 0
            configGui.anchors.left = rootItem.right
            configGui.anchors.leftMargin = 2
            configGui.load()
        }
        width = 60
        height = 20
        visible = true
    }

    function hideGui() {
        if (configGui != null)
           configGui.quit()

        width = 0
        height = 0
        visible = true // button always appears on advanced mode
    }

    function quitGui() {
        if (configGui != null)
           configGui.quit()
        visible = false
    }


    Rectangle {
        anchors.fill: btnShow
        color: "#523141"
        radius: 4.0
        opacity: 0.8
    }


    Text {
        id: btnShow
        width: 60
        height:20
        anchors.top: parent.top
        anchors.topMargin: - 20
        anchors.left: parent.left
        anchors.leftMargin: 0
        font { family: GlobalConstants. kFontFamily; pixelSize: 16;}
        color:"#fafafa"
        text: "Show"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (rootItem.configGui != null) {
                    if (rootItem.configGui.visible) {
                        rootItem.hideGui()
                         btnShow.text = "Show"
                    }
                    else {
                        rootItem.showGui()
                        btnShow.text = "Hide"
                    }
                }
            }
        }
    }

}

