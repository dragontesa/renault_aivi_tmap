import QtQuick 2.0

Item {
    id: root

    function setRemainTime(h, m) {
        internal.setTime(h, m)
    }

    width: row.childrenRect.width
    height: row.childrenRect.height

    Item {
        id: internal

        property int hour
        property int min

        function numberImageSource(num) {
            return "zone/drive_num_" + num + ".png"
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
            _remainMinText.visible = true
        }

        Row {
            id: row
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                id: _10h
            }
            Image {
                id: _1h
            }
            Image {
                id: _remainHourText
                source: lang == 'ko' ? Resource.imageUrl("zone/drive_num_hour.png")
                                     : Resource.imageUrl("zone/drive_num_hour_eng.png")
                visible: false
            }
            Image {
                id: _10m
            }
            Image {
                id: _1m
            }
            Image {
                id: _remainMinText
                source: lang == 'ko' ? Resource.imageUrl("zone/drive_num_minute.png")
                                     : Resource.imageUrl("zone/drive_num_minute_eng.png")
                visible: false
            }
        }
    }
}
