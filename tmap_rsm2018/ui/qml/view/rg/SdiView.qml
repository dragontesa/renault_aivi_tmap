import QtQuick 2.0
import RgConstants 1.0
import SdiViewEvent 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../component"
import "../../view"
import "../util/DebugUtils.js" as DebugUtils

View {
    id: root
    width: 140
    height: 164
    visible: false
    hasFocus: false

    property int sectionSpeedWrapWidth: 234
    property int sectionSpeedWrapHeight: 90

    property int sectionSpeedAverageTextX: 130
    property int sectionSpeedAverageTextY: 22
    property int sectionSpeedAverageTextWidth: 93
    property int sectionSpeedAverageSpeedX: 130
    property int sectionSpeedAverageSpeedY: 22 + 22 + 3
    property int sectionSpeedAverageSpeedWidth: 93
    property int sectionSpeedAverageSpeedHeight: 34
    property int sectionSpeedAverageSpeedMaxNumbers: 0
    property font sectionSpeedAverageFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(20)
    });

    property int sectionSpeedTimeTextX: 130
    property int sectionSpeedTimeTextY: 22
    property int sectionSpeedTimeTextWidth: 93
    property int sectionSpeedTimeX: 130
    property int sectionSpeedTimeY: 22 + 22 + 3
    property int sectionSpeedTimeWidth: 93
    property int sectionSpeedTimeHeight: 34
    property int sectionSpeedTimeMaxNumbers: 0
    property font sectionSpeedTimeFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(20)
    });

    property int sectionSpeedToggleInterval: 2000

    property int signType: RgConstants.SdiCircledNumber
    property int signNumberY: 49
    property int signNumberSpacing: 3
    property int signMaxNumbers: 0
    property int remainDistMaxNumbers: 0

    property bool signSectionSpeedToggle: false
    property bool averageAndTimeSectionSpeedToggle: false
    property bool sectionSpeedAverageVisible: false
    property bool sectionSpeedTimeVisible: false

    function reset() {
        sectionSpeedWrap.visible = false
        sectionSpeedAverageSpeedMaxNumbers = 0
        sectionSpeedTimeMaxNumbers = 0
        remainDistWrap.visible = false
        remainDistMaxNumbers = 0
        signWrap.visible = false
        signMaxNumbers = 0        
    }

    onMediated: {
        root.visible = true
        sendEvent( SdiViewEvent.Mediated )
    }

    onVisibleChanged: {
        sendEvent( SdiViewEvent.VisibleChanged, visible)
    }

    onSignTypeChanged: {
        if ( signType === RgConstants.SdiCircledNumber ) {
            signNumberWrap.visible = true
            signNumberRepeater.visible = true
            signNumberSpacing = 0
            signNumberY = 49
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = false
            sectionSpeedToggleTimer.running = false
        }
        else if ( signType === RgConstants.SdiSymbol ) {
            signNumberWrap.visible = false
            signNumberRepeater.visible = false
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = false
            sectionSpeedToggleTimer.running = false            
        }
        else if ( signType === RgConstants.SdiNumberWithSymbol ) {
            signNumberWrap.visible = true
            signNumberRepeater.visible = true
            signNumberSpacing = 1
            signNumberY = 42
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = false
            sectionSpeedToggleTimer.running = false            
        }
        else if ( signType === RgConstants.SdiSectionSpeed ) {
            signNumberWrap.visible = true
            signNumberRepeater.visible = true
            signNumberSpacing = 1
            signNumberY = 42
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = false
            sectionSpeedToggleTimer.running = false            
        }
        else if ( signType === RgConstants.SdiChangeableSectionSpeed ) {            
            signNumberWrap.visible = true
            signNumberRepeater.visible = true
            signNumberSpacing = 1
            signNumberY = 42
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = false
            sectionSpeedToggleTimer.running = false
        }
        else if ( signType === RgConstants.SdiChangeableSectionSpeedButSpeedLimitNA ) {
            signNumberWrap.visible = false
            signNumberRepeater.visible = false
            signSectionSpeedToggle = false
            sectionSpeedWrap.visible = true
            sectionSpeedToggleTimer.running = true
         }
    }

    Component.onCompleted: {
    }

    function setSignNumberImages( images ) {        
        signMaxNumbers = images.length
        var signNumberRepeaterItemAt = signNumberRepeater.itemAt
        for ( var i = 0; i < signNumberRepeater.count; i++ ) {
            signNumberRepeaterItemAt( i ).source = ''
        }
        for ( i = 0; i < images.length; i++ ) {
            signNumberRepeaterItemAt( i ).source = images[i]
        }
    }

    function setSectionSpeedAverageNumberImages( images ) {
        sectionSpeedAverageSpeedMaxNumbers = images.length
        var sectionSpeedAverageRepeaterItemAt = sectionSpeedAverageRepeater.itemAt
        for ( var i = 0; i < sectionSpeedAverageRepeater.count; i++ ) {
            sectionSpeedAverageRepeaterItemAt( i ).source = ''
        }
        for ( i = 0; i < images.length; i++ ) {
            sectionSpeedAverageRepeaterItemAt( i ).source = images[i]
        }
    }

    function setSectionSpeedTimeImages( images ) {
        sectionSpeedTimeMaxNumbers = images.length
        var sectionSpeedTimeRepeaterItemAt = sectionSpeedTimeRepeater.itemAt
        for ( var i = 0; i < sectionSpeedTimeRepeater.count; i++ ) {
            sectionSpeedTimeRepeaterItemAt( i ).source = ''
        }
        for ( i = 0; i < images.length; i++ ) {
            sectionSpeedTimeRepeaterItemAt( i ).source = images[i]
        }
    }

    function setRemainDistNumberImages( images ) {        
        remainDistMaxNumbers = images.length
        var remainDistNumberRepeaterItemAt = remainDistNumberRepeater.itemAt
        for ( var i = 0; i < images.length; i++ ) {
            remainDistNumberRepeaterItemAt( i ).source = images[i]
        }
    }

    function toggleSpeedSectionSign() {
        signSectionSpeedToggle = !signSectionSpeedToggle
        if ( signType === RgConstants.SdiChangeableSectionSpeed ) {
            if ( signSectionSpeedToggle ) {
                var img = "c_18.png"
                signImage.source = Resource.imageUrl( 'sdi', img )
                signNumberWrap.visible = false
            }
            else {
                img = "c_10.png"
                signImage.source = Resource.imageUrl( 'sdi', img )
                signNumberWrap.visible = true
            }
        }
        else if ( signType === RgConstants.SdiChangeableSectionSpeedButSpeedLimitNA ) {
            img = signSectionSpeedToggle ? "c_18.png" : "c_11.png"
            signImage.source = Resource.imageUrl( 'sdi', img )
            signNumberWrap.visible = false
        }
    }

    function enableToggleSpeedSectionAverageAndTime(enable) {
        sectionSpeedAverageAndTimeToggleTimer.running = enable
    }

    function toggleSpeedSectionAverageAndTime() {
        averageAndTimeSectionSpeedToggle = !averageAndTimeSectionSpeedToggle
    }

    Item {
        id: sectionSpeedWrap
        objectName: "sectionSpeedWrap"
        width: sectionSpeedWrapWidth
        height: sectionSpeedWrapHeight
        visible: sectionSpeedAverageVisible || sectionSpeedTimeVisible

        Image {
            source: Resource.imageUrl( 'sdi', 'tmap_speed_average_bg.png' )
        }

        // average
        Item {
            id: sectionSpeedAverageWrap
            visible: (averageAndTimeSectionSpeedToggle || !sectionSpeedAverageAndTimeToggleTimer.running) && sectionSpeedAverageVisible
            //visible: sectionSpeedAverageVisible

            Text {
                id: sectionSpeedAverageText
                objectName: "sectionSpeedAverageText"
                x: sectionSpeedAverageTextX
                y: sectionSpeedAverageTextY
                width: sectionSpeedAverageTextWidth
                horizontalAlignment: Text.AlignHCenter
                font: sectionSpeedAverageFont
                text: qsTr("평균속도")
                color: "white"
            }

            Item {
                id: sectionSpeedAverageSpeedWrap
                x: sectionSpeedAverageSpeedX
                y: sectionSpeedAverageSpeedY
                width: sectionSpeedAverageSpeedWidth
                height: sectionSpeedAverageSpeedHeight

                Row {
                    id: sectionSpeedAverageSpeed
                    objectName: "sectionSpeedAverageSpeed"
                    width: childrenRect.width
                    height: childrenRect.height
                    //anchors.right: parent.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater {
                        id: sectionSpeedAverageRepeater
                        objectName: 'sectionSpeedAverageRepeater'
                        model: sectionSpeedAverageSpeedMaxNumbers
                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source//:  Resource.imageUrl( 'sdi', 'tmap_speed_remain_num_9.png' )
                            Image {
                                id: sectionSpeedAverageNumberImage
                                source: parent.source
                            }
                        }
                    }
                }
            }
        }

        // time
        Item {
            id: sectionSpeedTimeWrap
            visible: (!averageAndTimeSectionSpeedToggle  || !sectionSpeedAverageAndTimeToggleTimer.running) && sectionSpeedTimeVisible
            //visible: sectionSpeedTimeVisible

            Text {
                id: sectionSpeedTimeText
                objectName: "sectionSpeedTimeText"
                x: sectionSpeedTimeTextX
                y: sectionSpeedTimeTextY
                width: sectionSpeedTimeTextWidth
                horizontalAlignment: Text.AlignHCenter
                font: sectionSpeedTimeFont
                text: qsTr("남은시간")
                color: "white"
            }

            Item {
                id: sectionSpeedTimeImageWrap
                x: sectionSpeedTimeX
                y: sectionSpeedTimeY
                width: sectionSpeedTimeWidth
                height: sectionSpeedTimeHeight

                Row {
                    id: sectionSpeedTime
                    objectName: "sectionSpeedTime"
                    width: childrenRect.width
                    height: childrenRect.height
                    //anchors.right: parent.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater {
                        id: sectionSpeedTimeRepeater
                        objectName: 'sectionSpeedTimeRepeater'
                        model: sectionSpeedTimeMaxNumbers
                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source//:  Resource.imageUrl( 'sdi', 'tmap_speed_remain_num_9.png' )
                            Image {
                                id: sectionSpeedTimeNumberImage
                                source: parent.source
                            }
                        }
                    }
                }
            }
        }
    }    

    Item {
        id: remainDistWrap
        objectName: "remainDistWrap"
        width: remainDistNumberBgImage.width
        height: remainDistNumberBgImage.height
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
        z: 1
        Image {
            id: remainDistNumberBgImage
            source: Resource.imageUrl( 'sdi', 'tmap_speed_sign.png' )
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Repeater {
                id: remainDistNumberRepeater
                objectName: 'remainDistNumberRepeater'
                model: remainDistMaxNumbers
                Image {
                    id: remainDistNumberImage
                }
            }
        }
    }

    Item {
        id: signWrap
        objectName: "signWrap"
        width: signImage.width
        height: signImage.height
        visible: false
        z: 100
        Image {
            id: signImage
            objectName: "signImage"            
            source: Resource.imageUrl( 'sdi', 'tmap_speed_symbol.png' ) //c_02            
            z: 1
        }
        Row {
            id: signNumberWrap
            objectName: "signNumberWrap"
            y: signNumberY
            width: childrenRect.width
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: signNumberSpacing
            z: 100

            Repeater {
                id: signNumberRepeater
                objectName: "signNumberRepeater"
                model: signMaxNumbers
                delegate: Item {
                    width: childrenRect.width
                    height: childrenRect.height
                    property string source: ""
                    Image {
                        id: signNumberImage
                        source: parent.source
                    }
                }
            }
        }
    }

    Timer {
        id: sectionSpeedToggleTimer
        interval: sectionSpeedToggleInterval
        running: false
        repeat: true
        onTriggered: {
            toggleSpeedSectionSign()
        }
    }

    Timer {
        id: sectionSpeedAverageAndTimeToggleTimer
        interval: sectionSpeedToggleInterval
        running: false
        repeat: true
        onTriggered: {
            toggleSpeedSectionAverageAndTime()
        }
    }
}
