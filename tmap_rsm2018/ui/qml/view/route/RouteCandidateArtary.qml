import QtQuick 2.0
import "../../GlobalConstants.js" as GConstants
import "../../component"
import "../../view"

Item {
    id: root

    property alias temperature: temperature
    property real internalScale: layout.internalItemScale

    function setArtary(fromText, toText) {
        from.text = fromText
        to.text = toText
    }

    // 날씨 표시는 삭제 됨.
    // 2018.01.04 ymhong.
    function setSkyCode(skyCode) {
//        if (skyCode.length > 0 && skyCode !== '00') {
//            var url = "weather/icon_weather_m" + skyCode + ".png"
//            weatherIcon.source = Resource.imageUrl(url)
//            weatherIcon.visible = true
//            temperature.visible = true
//        } else {
//            weatherIcon.visible = false
//            temperature.visible = false
//        }
    }

    width: layout.artaryWidth
    height: layout.artaryHeight//64 * internalScale

    MouseArea { anchors.fill: parent } // TMCRSM-666.
    Rectangle {
        color: "#000000"
        anchors.fill: parent

        Item {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            scale: internalScale

            Temperature {
                id: temperature

                anchors.right: parent.right
                anchors.rightMargin: 26
                anchors.verticalCenter: parent.verticalCenter
                numberSources: "tbt/tbt_3rd_num"
                dotIconSource: Resource.imageUrl("route/route_weather_dot.png")
                visible: false
            }
            Image {
                id: weatherIcon
                visible: false
                anchors.right: temperature.left
                anchors.verticalCenter: parent.verticalCenter
                //source: Resource.imageUrl("weather/icon_weather_m01.png")
            }
            Item {
                id: destWeatherText
                anchors.right: weatherIcon.left
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                width: 102
                height: childrenRect.height
                Text {
                    font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(25) }//pixelSize: 20 }
                    color: "#d1d1d1"
                    opacity: 0.8
                    horizontalAlignment: Text.AlignRight
                    //text: qsTr("목적지 날씨")
                }
            }

            Row {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                Item {
                    width: 28
                    height: 28
                }

                Item {
                    y: 4
                    width: childrenRect.width//230
                    height: 28
                    Text {
                        id: from
//                        anchors.fill: parent
                        font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) } //pixelSize: 24 }
                        color: "#ffffff"
//                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Image {
                    source: Resource.imageUrl("route/route_pop_arw.png")
                }
                Item {
                    y: 4
                    width: 230
                    height: 28
                    Text {
                        id: to
                        anchors.fill: parent
                        font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }//pixelSize: 24 }
                        color: "#ffffff"
//                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
