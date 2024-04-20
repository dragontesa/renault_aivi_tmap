import QtQuick 2.0

Item {
    id: root

    property int time: 10 // default
    property string numSources: "loading_num_"
    property bool focused: false

    function start() {
        anim.start()
    }
    function stop() {
        anim.stop()
    }

    signal timeout()

    property real r0 : (root.width / 2) // outer
    property real r1 : r0 - 4           // inner
    property color color: '#ffffff'

    Image {
        id: number
        width: 28
        height: 28
        anchors.centerIn: parent
        source: {
            var numText = (time - parseInt(ring.rotation / 360)).toString()
            var prefix = numText.length < 2 ? '0' : ''
            return Resource.imageUrl(numSources + prefix + numText + ".png")
        }
    }
    Image {
        id: ring
        anchors.centerIn: parent
        source: color === '#ffffff' ? Resource.imageUrl('loading_img_f.png') : Resource.imageUrl('loading_img.png')

        RotationAnimation on rotation {
            id: anim

            running: false
            from: 0
            to: 360 * time
            duration: 1000 * time
        }
        onRotationChanged: {
            if (anim.running && rotation == 360 * time) {
                root.timeout()
                anim.running = false
            }
        }
    }
}
