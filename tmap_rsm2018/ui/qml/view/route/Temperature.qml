import QtQuick 2.0

Item {
    id: root

    width: row.childrenRect.width
    height: row.childrenRect.height
    visible: false

    function setTemperature(temperature) {
        var abs = Math.abs(temperature)
        var absStr = abs.toString()
        var len = absStr.length
        _10.visible = abs >= 10
        if (len >= 2) {
            _10.source = Resource.imageUrl(internal.numberImageSource(absStr[len-2]))
        }
        _1.source = Resource.imageUrl(internal.numberImageSource(absStr[len-1]))
        _minus.visible = temperature < 0

        root.visible = true
    }

    property string numberSources
    property string dotIconSource
    property string minusSource

    Item {
        id: internal

        function numberImageSource(num) {
            return numberSources + "_" + num + ".png"
        }

        Row {
            id: row
            Image {
                id: _minus
                visible: false
            }
            Image {
                id: _10
            }
            Image {
                id: _1
            }
            Image {
                source: dotIconSource
            }
        }
    }
}
