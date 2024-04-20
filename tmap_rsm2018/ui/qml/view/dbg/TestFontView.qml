import QtQuick 2.0
import QtQuick.Controls 1.0
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    anchors.fill: parent
    property int fontSize: 16
    property int offset: 0
    property int compSize: 32
    property int grid: 16

    Rectangle { anchors.fill: parent; color: 'black' }
    Item {
        anchors.fill: parent
        anchors.topMargin: 10; anchors.leftMargin: 128
        Row {
            width: compSize*4; height: compSize
            Text {
                width: compSize*2; height: compSize
                color: '#ffffff'
                text: 'font :' + fontSize
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '+'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: fontSize += 1
                }
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '-'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: fontSize -= 1
                }
            }
        }
        Row {
            y:compSize; width: compSize*4; height: compSize
            Text {
                width: compSize*2; height: compSize
                color: '#ffffff'
                text: 'offset : ' + offset
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '+'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: offset += 1
                }
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '-'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: offset -= 1
                }
            }
        }
        Row {
            y:compSize*2; width: compSize*4; height: compSize
            Text {
                width: compSize*2; height: compSize
                color: '#ffffff'
                text: 'grid : ' + grid
                verticalAlignment: Text.AlignVCenter
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '+'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: grid += 1
                }
            }
            Rectangle { width: compSize; height: compSize; color: 'transparent'; border.color: 'white'
                Text { anchors.centerIn: parent; text: '-'; color: 'white' }
                MouseArea {
                    anchors.fill: parent
                    onClicked: grid -= 1
                }
            }
        }
    }
    Rectangle { id: hLine; width: parent.width; height: 1; opacity: 0.5; anchors.verticalCenter: parent.verticalCenter }
    Rectangle { id: vLine; width: 1; height: parent.height; opacity: 0.5; anchors.horizontalCenter: parent.horizontalCenter }
    Rectangle { width: parent.width; height: 1; color: 'yellow'; anchors.bottom: hLine.bottom; anchors.bottomMargin: grid }
    Rectangle { width: parent.width; height: 1; color: 'yellow'; anchors.top: hLine.top; anchors.topMargin: grid }
    Item {
        anchors.fill: parent
        anchors.topMargin: offset
        Text {
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(fontSize) }
            color: 'white'
            anchors.centerIn: parent
            text: '맵 업데이트를 설치하시겠습니까?'
        }
    }
}
