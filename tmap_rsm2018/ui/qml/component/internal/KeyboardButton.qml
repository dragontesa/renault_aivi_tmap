import QtQuick 2.1
import CommonConstants 1.0

Item{
    id: root

    width:   imgN.width  == 0 ?  53 : imgN.width;
    height:  imgN.height == 0 ?  43 : imgN.height;

    //[ID]
    property alias root      : root
    property alias box       : box
    property alias imgN      : imgN
    property alias imgP      : imgP
    property alias txt       : txt
    property alias mouseArea : mouseArea

    //[속성]
    property alias change    : root.state

    //[시그널]
    signal buttonPressed()
    signal buttonReleased()
    signal buttonClicked()
    signal buttonLongClicked()

    //[상태]
    state: "normal"
    states: [
        State {
            name: "normal"
            PropertyChanges { target: box;  color : box.colorN }
            PropertyChanges { target: imgN; source: imgN.sourceN }
            PropertyChanges { target: txt;  color : txt.colorN }
        },
        State {
            name: "pressed"
            PropertyChanges { target: box;  color : box.colorP }
            PropertyChanges { target: imgP; source: imgP.sourceP }
            PropertyChanges { target: imgN; source: imgN.sourceN }
            PropertyChanges { target: txt;  color : txt.colorP }
        }
    ]


    Rectangle {
        id: box
        anchors.fill : parent
        property string colorN   : CommonConstants.Transparent
        property string colorP   : CommonConstants.Transparent
    }
    Image {
        id: imgP
        anchors.centerIn: parent
        property string sourceP
    }
    Image {
        id: imgN
        anchors.centerIn: parent
        property string sourceN
    }
    Text {
        id:txt
        anchors.fill: parent
        horizontalAlignment     : Qt.AlignHCenter
        verticalAlignment       : Qt.AlignVCenter
        property string colorN  : "white"
        property string colorP  : "white"
    }
    MouseArea {
        id: mouseArea
        anchors.fill: root
        onPressed: {
            buttonPressed()
            root.state = "pressed"
        }
        onReleased: {
            root.state = "normal"
            buttonReleased()
        }
        onPressAndHold: buttonLongClicked()
        onClicked:  buttonClicked()
    }
}
