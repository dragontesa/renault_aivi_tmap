import QtQuick 2.0

Item {
    id: root
    property string distance
    property var numSources: []
    property bool isKilometer: false

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
                ? "zone/drive_num_" + c + ".png"
                : "zone/drive_num_dot.png"
    }

    ListModel { id: model }

    Row {
        anchors.centerIn: parent
        Repeater {
            id: repeater
            model: model
            delegate: Image {
                source: Resource.imageUrl(numberImageSource(c))
            }
        }
        Image {
            source: root.isKilometer
                    ? Resource.imageUrl("zone/drive_num_km.png")
                    : Resource.imageUrl("zone/drive_num_m.png")
        }
    }
}
