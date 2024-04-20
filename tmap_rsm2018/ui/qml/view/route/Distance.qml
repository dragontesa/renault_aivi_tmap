import QtQuick 2.0
import "../../view/util/Formatter.js" as Formatter

Item {
    id: root
    property int distance
    //property var numSources: []
    property string dot
    property string m
    property string km
    property var getNumSources
    property string traffic

    Component {
        id: compMiter

        Item {
            width: 68
            height: 29

            Row {
                anchors.centerIn: parent

                Component.onCompleted: {
                    var str = distance.toString()
                    _100.visible = distance >= 100
                    _10.visible = distance >= 10

                    var index
                    var len = str.length
                    if (distance > 100) {
                        index = str[0]
                        _100.source = Resource.imageUrl(getNumSources(index))
                    }
                    if (distance > 10) {
                        index = str[len-2]
                        _10.source = Resource.imageUrl(getNumSources(index))
                    }
                    index = str[len-1]
                    _1.source = Resource.imageUrl(getNumSources(index))
                }
                Image { id: _100; width: 13; height: 29 }
                Image { id: _10; width: 13; height: 29 }
                Image { id: _1; width: 13; height: 29 }
                Image { source: Resource.imageUrl(m) }
            }
        }
    }
    Component {
        id: compKiloMeter

        Item {
            width: 68
            height: 29

            Component.onCompleted: {
                var str = distance.toString()
                var length = str.length
                var index
                if (distance >= 10000) {
                    _dot.visible = false
                    __1.visible = false
                    index = str[0]
                    _10.source = Resource.imageUrl(getNumSources(index))
                    index = str[1]
                    _1.source = Resource.imageUrl(getNumSources(index))
                } else {
                    _10.visible = false
                    index = str[0]
                    _1.source = Resource.imageUrl(getNumSources(index))
                    index = str[1]
                    __1.source = Resource.imageUrl(getNumSources(index))
                    _dot.source = Resource.imageUrl(dot)
                }
            }

            Row {
                anchors.centerIn: parent

                Image { id: _10; width: 13; height: 29 }
                Image { id: _1; width: 13; height: 29 }
                Image { id: _dot; width: 6; height: 29 } // 소수점
                Image { id: __1; width: 13; height: 29 } // 소수
                Image { source: Resource.imageUrl(km) }
            }
        }
    }

    Loader {
        id: loader
        sourceComponent: distance < 1000 ? compMiter : compKiloMeter
    }
}
