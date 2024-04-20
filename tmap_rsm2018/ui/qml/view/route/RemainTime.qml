import QtQuick 2.0

Item {
    id: itemRoot

    function setRemainTime(h, m) {
        internal.setTime(h, m)
    }

    property string hourTextSource
    property string minTextSource
    property string numSources
    property string postfix: "_n"
    property int hour
    property int min

    visible: false
    width: row.childrenRect.width
    height: row.childrenRect.height

    Component.onCompleted: {
        setRemainTime(hour, min)
    }
    onPostfixChanged: {
        setRemainTime(hour, min)
    }

    state: "normal"
    states: [
        State {
            name: "normal"
            PropertyChanges { target: itemRoot; postfix: "_n" }
        },
        State {
            name: "selected"
            PropertyChanges { target: itemRoot; postfix: "_s" }
        },
        State {
            name: "csw-focus"
            PropertyChanges { target: itemRoot; postfix: "_f" }
        }
    ]

    Item {
        id: internal

        function numberImageSource(num) {
            return numSources + "_" + num + postfix + ".png"
        }

        function setTime(h, m) {
            hour = h
            min = m

            var hourText = h.toString()

            if (h >= 10) {
                _10h.source = Resource.imageUrl(numberImageSource(hourText[0]))
                _1h.source = Resource.imageUrl(numberImageSource(hourText[1]))
            } else {
                _1h.source = Resource.imageUrl(numberImageSource(hourText[0]))
            }

            var minText = m.toString()
            if (m >= 10) {
                _10m.source = Resource.imageUrl(numberImageSource(minText[0]))
                _1m.source = Resource.imageUrl(numberImageSource(minText[1]))
            } else {
                _1m.source = Resource.imageUrl(numberImageSource(minText[0]))
            }

            _10h.visible = h >= 10
            _1h.visible = _remainHourText.visible = h > 0
            _10m.visible = m >= 10
        }

        Row {
            id: row

            Image {
                id: _10h
            }
            Image {
                id: _1h
            }
            Image {
                id: _remainHourText
                source: Resource.imageUrl(hourTextSource + postfix + ".png")
            }
            Image {
                id: _10m
            }
            Image {
                id: _1m
            }
            Image {
                id: _remainMinText
                source: Resource.imageUrl(minTextSource + postfix + ".png")
            }
        }
    }
}
