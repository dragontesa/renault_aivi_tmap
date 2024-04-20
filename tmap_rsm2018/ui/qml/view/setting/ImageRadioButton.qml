import QtQuick 2.1
import CommonConstants 1.0
import SettingConstants 1.0
import "../../component"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


Item {
    id: btn
    property var exclusiveGroup     : null
    property string   imgNormal     : CommonConstants.Empty
    property string   imgPressed    : CommonConstants.Empty
    property int      checked       : CommonConstants.False
    property alias img              : img

    implicitWidth                   : 16
    implicitHeight                  : 16

    opacity                         : (!enabled) ? 0.4 : 1.0
    state                           : "normal"

    signal buttonClicked()
    onCheckedChanged: {
        if(checked)
            exclusiveGroup.selected = btn
        checked = CommonConstants.False
    }

    Image {
        id: img
        scale: CommonUtils.isEqual(btn.state , "pressed") ? 0.85 : 1.0
        anchors.centerIn: parent        
        source:  CommonUtils.isEqual(exclusiveGroup.selected , btn) ? imgPressed : imgNormal
    }

    MouseArea {
        id: btnMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressedChanged: {
            if (pressed) {
                btn.state = "pressed"
            } else {
                 btn.state = "normal"
            }
        }
        onClicked: {
            exclusiveGroup.selected = btn
            buttonClicked()
        }
    }
}


