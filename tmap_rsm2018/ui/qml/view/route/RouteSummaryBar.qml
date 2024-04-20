import QtQuick 2.0
import "../../component"
import "../util/Formatter.js" as Formatter
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property int distance: 0
    property int remainTime: 0
    property int fee: 0

    function retranslationUi() {
        remainTimeLoader.sourceComponent = undefined
        feeLoader.sourceComponent = undefined

        remainTimeLoader.sourceComponent = textComp
        feeLoader.sourceComponent = textComp
    }

    function updateValues(distance, remainTime, fee) {
        root.distance = distance
        root.remainTime = remainTime
        root.fee = fee

        distanceLoader.sourceComponent = textComp
        remainTimeLoader.sourceComponent = textComp
        feeLoader.sourceComponent = textComp
    }

    Component {
        id: textComp
        Text {
            height: 26
            font.family: GlobalConstants.kFontFamily
            font.pixelSize: Resource.pixelToDp(28)
            color: "#ffffff"
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle { // background
        color: "#222226"
        opacity: 0.85
        anchors.fill: parent

        Item {
            width: parent.width
            height: parent.height

            scale: layout.scale
            x: -(layout.summaryBarWidth * (1.0 - layout.scale) * 0.5)

            // Distance
            Image {
                id: distanceImage
                source: Resource.imageUrl('route/route_list_distance.png')
                x: 26
                anchors.verticalCenter: parent.verticalCenter
            }
            Loader {
                id: distanceLoader

                anchors.left: distanceImage.right
                anchors.leftMargin: 10

                height: parent.height

                onLoaded: {
                    item.text = Formatter.getDistanceText(distance)
                    distanceLoader.width = item.paintedWidth
                }
            }

            // RemainTime
            Image {
                id: remaintTimeImage

                source: Resource.imageUrl('route/route_list_time.png')
                anchors.left: distanceLoader.right
                anchors.leftMargin: 44
                anchors.verticalCenter: parent.verticalCenter
            }
            Loader {
                id: remainTimeLoader
                anchors.left: remaintTimeImage.right
                anchors.leftMargin: 10
                height: parent.height

                onLoaded: {
                    item.text = Formatter.getTimeText(remainTime)
                    remainTimeLoader.width = item.paintedWidth
                }
            }

            // fee
            Image {
                id: feeImage

                source: Resource.imageUrl('route/route_list_charge.png')
                anchors.left: remainTimeLoader.right
                anchors.leftMargin: 44
                anchors.verticalCenter: parent.verticalCenter
            }
            Loader {
                id: feeLoader
                anchors.left: feeImage.right
                anchors.leftMargin: 10
                height: parent.height

                onLoaded: {
                    item.text = Formatter.getFeeText(fee, qsTr('원'))
                    feeLoader.width = item.paintedWidth
                }
            }
        }
    }
}
