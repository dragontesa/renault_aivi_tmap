import QtQuick 2.0

Item {
    property var parentObj: null
    property var textObj: null
    property bool marquee: (parentObj&&textObj) && parentObj.width<textObj.width
    property alias marqueeAnim: anim

    signal marqueeStarted()
    signal marqueeFinished()
    signal textMoved()
    signal textStopped()

    function init() {
        anim.stop()
        if (textObj)
            textObj.x = 0
    }

    onMarqueeChanged: {
        if (marquee) {
            anim.restart()
        } else {
            anim.stop()
            textObj.x = 0
        }
    }

    function start() {
        textObj.x = 0
        if (marquee) {
            anim.restart()
        }
    }

    function stop() {
        anim.stop()
    }

    SequentialAnimation {
        id: anim

        running: false

        ScriptAction { script: marqueeStarted() }
        PauseAnimation { duration: 2000 }
        ScriptAction { script: textMoved() }
        NumberAnimation {
            from: 0
            to: -(textObj.width - parentObj.width)
            duration: {
                var d = (textObj.width - parentObj.width)*20
                return d <= 0 ? 0 : d
            }
            target: textObj
            properties: 'x'
        }
        ScriptAction { script: textStopped() }
        PauseAnimation { duration: 2000 }
        ScriptAction { script: marqueeFinished() }
    }
}
