// SoftkeyArrivalTime에서 이미지만 고속도로용으로 교체.
import QtQuick 2.0

Item {
    id: root

    width: row.childrenRect.width
    height: row.childrenRect.height

    property string format
    property bool isAmPm: (0 <= format.indexOf('A')) || (0 <= format.indexOf('AP'))
    property bool is24: (0 <= format.indexOf('H'))
    property bool displayZeroHour: (0 <= format.indexOf('hh')) || (0 <= format.indexOf('HH'))
    property bool displayZeroMin: (0 <= format.indexOf('mm'))
    property bool langKo: (lang !== undefined && lang === 'ko')
    property int cachedHour: 0
    onLangKoChanged: {
        internal.updateAmPm(cachedHour)
    }

    function setArrivalTime(h, m) {
        internal.setTime(h, m)
        cachedHour = h
    }
    function getHourText(h) {
        var ret

        if (!is24 && h > 12)
            h = h - 12

        var h2 = h.toString()
        if (h2.length === 2)
            return h2

        if (displayZeroHour)
            return "0" + h2
        else
            return h2
    }

    Item {
        id: internal

        function setTime(h, m) {
            var hourText = getHourText(h)

            if (hourText.length === 2) {
                _10h.source = Resource.imageUrl(numberImageSource(hourText[0]))
                _10h.visible = true
                _1h.source = Resource.imageUrl(numberImageSource(hourText[1]))
            } else {
                _10h.visible = false
                _1h.source = Resource.imageUrl(numberImageSource(hourText[0]))
            }

            var minText = m.toString()
            if (minText.length === 2) {
                _10m.source = Resource.imageUrl(numberImageSource(minText[0]))
                _1m.source = Resource.imageUrl(numberImageSource(minText[1]))
            } else {
                _10m.source = Resource.imageUrl(numberImageSource('0'))
                _1m.source = Resource.imageUrl(numberImageSource(minText[0]))
            }

            updateAmPm(h)
        }
        function updateAmPm(h) {
            if (isAmPm) {
                if (h < 12) {
                    _dayNightText.source = langKo ? Resource.imageUrl("highway/highway_num_day.png")
                                                        : Resource.imageUrl("highway/highway_num_day_eng.png")
                } else {
                    _dayNightText.source = langKo ? Resource.imageUrl("highway/highway_num_night.png")
                                                        : Resource.imageUrl("highway/highway_num_night_eng.png")
                }
            }
        }

        function numberImageSource(num) {
            return "highway/highway_num_" + num + ".png"
        }

        Row {
            id: row

            Image {
                id: _dayNightText
                visible: isAmPm && langKo
            }
            Image {
                id: _10h
            }
            Image {
                id: _1h
            }
            Image {
                source: Resource.imageUrl("highway/highway_num_colon.png")
            }
            Image {
                id: _10m
            }
            Image {
                id: _1m
            }
            Image {
                id: _dayNightTextEn
                visible: isAmPm && !langKo
                source: _dayNightText.source
            }
        }
    }
}
