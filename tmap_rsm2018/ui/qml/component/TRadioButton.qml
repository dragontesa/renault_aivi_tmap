import QtQuick 2.1

Item {
    id: btn
    property var exclusiveGroup : null
    property string   imgNormal:  ""
    property string   imgPressed: ""
    property int      checked: 0
    property alias img : img
    implicitWidth: 16
    implicitHeight: 16

    opacity: (!enabled) ? 0.4 : 1.0 // effect dimming

    signal buttonClicked()
    onCheckedChanged: {
        if(checked)
            exclusiveGroup.selected = btn
        checked = 0
    }

    Image {
        id: img
        anchors.centerIn: parent        
        source:  exclusiveGroup.selected === btn ? imgPressed : imgNormal
    }

    MouseArea {
        id: btnMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            exclusiveGroup.selected = btn
            buttonClicked()
        }
        onDoubleClicked: {}
    }
}


