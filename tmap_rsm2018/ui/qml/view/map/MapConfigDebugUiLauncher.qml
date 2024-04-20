import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: rootItem
    objectName: "mapConfigDebugUi"
    property Item topParent: null
    width: 65
    height: 30
    x: 2
    y: 180
    visible: false
    property Item configGui : null
    
    function showGui() {
        if (configGui != null) {
            configGui.width = 340
            configGui.height = GlobalConstants.kScreenContentHeight
            configGui.x = topParent.width - 340-1
            configGui.y = -40
            configGui.visible = true
        }
        configGui.setConfigSettings();
        width = 0
        height = 0
        visible = true
    }

    function hideGui() {
        if (configGui != null)
           configGui.visible = false

        width = 0
        height = 0
        visible = true
    }

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: 0.5
        radius: 4.0
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
                        configGui.saveConfigSettings();
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
