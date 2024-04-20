import QtQuick 2.0
import "../route"

Item {
    id: root
    property var menus: ["최신순", "빈도순"] // option menu don't need multiple language TMCRSM-2500
    property int orderType: 1 // 0:최신순(default), 1:빈도순

    visible: false
    width: 96*2; height: 64

    signal byOrderClicked
    signal byHitClicked

    function toggleMenu() {
        if (0 === orderType)
            root.byOrderClicked()
        else
            root.byHitClicked()
        orderType = orderType === 0 ? 1 : 0
    }
    function toggleVisible() {
        root.visible = root.visible ? false : true
    }

    Rectangle {
        anchors.fill: parent
        radius: 4
        color: "white"
        border.color: "black"

        RouteSummaryText {
            anchors.fill: parent
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            text: menus[orderType]
            color: "black"

            MouseArea {
                anchors.fill: parent
                onClicked: toggleMenu()
            }
        }
    }
}
