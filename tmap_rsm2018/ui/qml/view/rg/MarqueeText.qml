import QtQuick 2.0

Item {
    id: marqueeRoot
    height: scrollingText.height
    clip: true

    property alias scrollingText: scrollingText
    property string text
    property alias textAlign: scrollingText.horizontalAlignment
    property alias font: scrollingText.font
    property alias textColor: scrollingText.color
    property bool running: true
    property bool scrolling: false

    signal marqueeStarted()
    signal marqueeFinished()
    signal textMoved()
    signal textStopped()

    onRunningChanged: {
        if ( running ) {
            restartMarquee()
        }
        else {
            stopMarquee()
        }
    }

    onTextChanged: {
        seqAnim.stop()
        numAnim.stop()
        marqueeRoot.scrolling = false

        scrollingText.text = text
        scrollingText.x = 0

        if ( scrollingText.contentWidth > marqueeRoot.width ) {
            restartMarquee()
        }
    }

    function restartMarquee() {
        scrollingText.x = 0
        seqAnim.restart()
    }

    function stopMarquee() {
        seqAnim.stop()
        marqueeRoot.scrolling = false
    }

    Text {
        id: scrollingText
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignLeft
        onContentWidthChanged: {
            if ( !marqueeRoot.scrolling ) {
                if ( scrollingText.contentWidth > marqueeRoot.width ) {
                    restartMarquee()
                }
            }
        }
    }

    onMarqueeStarted: {
        scrollingText.x = 0
    }

    onTextMoved: {
        marqueeRoot.scrolling = true
    }

    onTextStopped: {
        marqueeRoot.scrolling = false
    }

    onMarqueeFinished: {
        seqAnim.restart()
    }

    SequentialAnimation {
        id: seqAnim
        running: false

        ScriptAction { script: marqueeStarted() }
        PauseAnimation { duration: 2000 }
        ScriptAction { script: textMoved() }
        NumberAnimation {
            id: numAnim
            from: 0
            to: {
                if ( scrollingText.contentWidth > 0 ) {
                    return -(scrollingText.contentWidth - marqueeRoot.width)
                }
                return 0
            }
            duration: {
                var d = (scrollingText.contentWidth - marqueeRoot.width)*20
                return d <= 0 ? 0 : d
            }
            target: scrollingText
            properties: 'x'
        }
        ScriptAction { script: textStopped() }
        PauseAnimation { duration: 2000 }
        ScriptAction { script: marqueeFinished() }
    }
}
