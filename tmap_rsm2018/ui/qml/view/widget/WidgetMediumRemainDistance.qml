// SoftkeyRemainDistance에서 이미지 리소스만 고속도로용으로 교체
import QtQuick 2.0

Item {
    id: root
    property string distance
    property var numSources: []
    property bool isKilometer: false

    //visible: false

    width: childrenRect.width
    height: childrenRect.height

    function isOver1km(str) {
        return str.indexOf("km") >= 0
    }

    onDistanceChanged: {
        var str = distance
        var i = str.indexOf("km")
        isKilometer = i >= 0

        if (isKilometer) {
            str = str.substring(0, i)
        } else {
            i = str.indexOf("m")
            if (i >= 0)
                str = str.substring(0, i)
        }

        model.clear()
        for (var index in str) {
            model.append({c : str[index]})
        }
    }

    function numberImageSource(c) {
        return (c !== '.')
                ? "highway/highway_num_" + c + ".png"
                : "highway/highway_num_dot.png"
    }

    ListModel { id: model }

    Row {
        Repeater {
            id: repeater
            model: model
            delegate: Image {
                source: Resource.imageUrl(numberImageSource(c))
            }
        }
        Image {
            source: root.isKilometer
                    ? Resource.imageUrl("highway/highway_num_km.png")
                    : Resource.imageUrl("highway/highway_num_m.png")
        }
    }
}
