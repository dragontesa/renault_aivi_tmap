import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

import "../../component"
import '../../view/util'
Item {
    id: root
    property string title
    property int buttonIndex    
    property color borderColor:"black"
    property real borderColorOpacity: 1
    property int   borderWidth:0
    property int   borderName:0 // let has one of borderName 0: visible four direction(left right top bottom)
    property alias color: button.color
    property alias colorOpacity: button.opacity
    property alias textColor: text.color
    property bool isCswFocused: false

    property var borderNameEnum: {
        "All"  : 0,
        "Left" : 1,
        "Top"  : 2,
        "Right" : 3,
        "Bottom" : 4,
        properties: {
            0: {name: "All"  },
            1: {name: "Left" },
            2: {name: "Top"  },
            3: {name: "Right"  },
            4: {name: "Bottom"  }
        }
    }

    signal clicked(int buttonIndex)

    function setButtonBorder()
    {
        button.anchors.leftMargin = 0
        button.anchors.topMargin  = 0
        button.anchors.rightMargin = 0
        button.anchors.bottomMargin = 0
        button.anchors.fill = root
        leftBorder.visible = false
        topBorder.visible = false
        rightBorder.visible = false
        bottomBorder.visible = false

        if (borderWidth > 0)
        {
            switch (borderName)
            {
            case borderNameEnum.All :
               button.anchors.leftMargin =  borderWidth
               button.anchors.topMargin  =   borderWidth
               button.anchors.rightMargin =  borderWidth
               button.anchors.bottomMargin = borderWidth
                leftBorder.visible = true
                topBorder.visible = true
                rightBorder.visible  = true
                bottomBorder.visible = true
                break;
            case borderNameEnum.Left:
                button.anchors.leftmargin = borderWidth
                leftBorder.visible = true
                break;
            case borderNameEnum.Top:
                button.anchors.topMargin   = borderWidth
                topBorder.visible = true
                break;
            case borderNameEnum.Right:
                button.anchors.rightMargin   = borderWidth
                rightBorder.visible = true
                break;
            case borderNameEnum.Bottom:
                button.anchors.bottomMargin   = borderWidth
                bottomBorder.visible = true
                break;
            }
        }
    }

    Rectangle {
        id: button
        anchors.fill: parent

        // 17.11.8 hskim changed MouseArea to TMosueArea so that make zone1 text input lose its focus when a tab page view is clicked
        TMouseArea {
            anchors.fill: parent
            onClicked: {
                root.clicked(buttonIndex)
            }
        }
    }

    Text2 {
        id: text
        text: root.title
        anchors.fill: button
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize)}
        offsetY: 3
    }

    Rectangle {
        id: leftBorder
        anchors.right : button.left
        width : borderWidth
        height: parent.height
        color: borderColor
        opacity: borderColorOpacity
    }

    Rectangle {
        id: topBorder
        anchors.bottom: button.top
        width : parent.width
        height: borderWidth
        color: borderColor
        opacity: borderColorOpacity
    }

    Rectangle {
        id: rightBorder
        anchors.left : button.right
        width : borderWidth
        height: parent.height
        color: borderColor
        opacity: borderColorOpacity
    }

    Rectangle {
        id: bottomBorder
        anchors.top: button.bottom
        width : parent.width
        height: borderWidth
        color: borderColor
        opacity: borderColorOpacity
    }

    // for csw focus
    Rectangle {
        anchors.fill: button
        color: '#ffffff'
        visible: isCswFocused

        Text2 {
            text: root.title
            anchors.fill: parent
            color: '#000000'
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize)}
            offsetY: 3
        }
    }
}
