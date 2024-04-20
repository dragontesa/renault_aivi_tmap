import QtQuick 2.1
import QtQuick.Window 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view"

Item {
	id: root
	readonly property string infoTitleText: qsTr("지도 데이터가 없습니다.")
	readonly property string info1Text: qsTr("지도데이터가 없어,\n네비게이션을 사용하실 수 없습니다.")
	readonly property string info2Text: qsTr("USB 지도 업데이트를 통해 지도를 설치하시거나,\n 통신 환경에서 재실행 해주세요.")
    property int screenLayout: LayoutConstants.L800x480
    property int icTopMargin: 10
    property int icBottomMargin: 10
    property int subTextMargin1: 10
    property int subTextMargin2: 30
    property int iconHeight: 56

    width: (parent) ? parent.width : undefined
    height: (parent) ? parent.height : undefined

    onScreenLayoutChanged: {
        if ( screenLayout === LayoutConstants.L820x1024 ) {
            icTopMargin = 160
            icBottomMargin = 40
            subTextMargin1 = 26
            subTextMargin2 = 58
            iconHeight = 102
        }
        else if (screenLayout === LayoutConstants.L800x480 ) {
            icTopMargin = 10
            icBottomMargin = 10
            subTextMargin1 = 10
            subTextMargin2 = 30
            iconHeight = 56
        }
    }


	Loader {
		id: boxLoader
		anchors.fill: parent
		sourceComponent: messageBox
	}

    Component {
        id: messageBox
        Column {
//            anchors.fill: parent
            anchors.centerIn: parent
            Item {
                id: icTop
                height: icTopMargin
            }

            Item {
                width: parent.width
                height: iconHeight
                Image {
                    width: parent.height
                    height: parent.height
                    source : Resource.imageUrl("etc_ic_info.png")
                    anchors.centerIn: parent
                }
            }

            Item {
                id: icBottom
                width: parent.width
                height: icBottomMargin
            }
            Text {
                    id: infoTitle
                    width: parent.width
                    height: 32
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize /* 0.5*/}
                    color: GlobalConstants.kListTextColor
                    text : infoTitleText
            }
            Item {
                id: subLineSpace
                width: parent.width
                height: subTextMargin1
            }
            Text {
                id: info1
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize /* 0.5*/}
                color: GlobalConstants.kListSubTextColor
                text: info1Text
            }
            Item {
                id: subLine2Space
                width: parent.width
                height: subTextMargin1
            }
            Text {
                id: info2
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize /* 0.5*/}
                color: GlobalConstants.kListSubTextColor
                text: info2Text
            }
        }
    }
}
