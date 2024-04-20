import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root
    property var layout

    property string address
    onAddressChanged: {
        if (address.length > 0)
            naviAddress.setYah(address)
    }

    anchors.fill: parent

    Zone3NaviAddress {
        id: naviAddress
        layout: root.layout
        anchors.centerIn: parent
    }
}
