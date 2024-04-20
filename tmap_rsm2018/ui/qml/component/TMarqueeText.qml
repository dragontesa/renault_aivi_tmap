import QtQuick 2.0

Item {
	id:marqueeText
	height: scrollingText.height
	clip: true
	property int tempX: 0
    property alias marqueeTimer : marqueeTimer
    property alias scrollingText: scrollingText
    property alias text: scrollingText.text
    property alias textAlign: scrollingText.horizontalAlignment
    property alias font: scrollingText.font
    property alias textColor: scrollingText.color
    property bool running: true
    property bool scrolling: false

    onRunningChanged: {
        if ( running ) {
            restartMarquee()
        }
        else {
            stopMarquee()
        }
    }

    function restartMarquee() {
        scrollingText.x = 0
        marqueeText.tempX = 0
        marqueeTimer.running = true
        pauseTimer.running = false
    }

    function stopMarquee() {
        marqueeTimer.running = false
        pauseTimer.running = false
    }

	Text {
        id:scrollingText
        width: parent.width
		anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
        property string truncatedText

        onTextChanged: {
            var isTruncating = scrollingText.elide == Text.ElideNone

            if ( truncatedText !== text ) {
                isTruncating = false
                width = parent.width
                marqueeText.tempX = 0
                scrollingText.x = 0
                scrollingText.elide = Text.ElideRight
            }

            if ( !isTruncating ) {
                marqueeTimer.running = false
                pauseTimer.running = false
            }

            marqueeText.scrolling = isTruncating
        }

        onTruncatedChanged: {
            if ( truncated ) {
                truncatedText = text
                width = undefined
                scrollingText.elide = Text.ElideNone
                restartMarquee()
            }
        }
	}

	Timer {
        id:marqueeTimer
        interval: 200; running: false; repeat: true

        onTriggered:{
            tempX = tempX + 1
            scrollingText.x = -tempX;

            if( tempX + marqueeText.width > scrollingText.width ) {
                marqueeTimer.running = false
                pauseTimer.running = true
            }
		}
	}

	Timer {
		id:pauseTimer
		interval: 500; running: false; repeat: false
		onTriggered: {
            restartMarquee()
		}
	}
}
