import QtQuick 2.0
import WidgetConstants 1.0
import RgConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root    

    // XXXL
    property int signNumberY: 49
    property int signWidth: 140
    property int signHeight: 164
    property int remainDistWidth: 140
    property int remainDistHeight: 44
    property bool currentSpeedNumberVisible: true
    property int currentSpeedNumberTopMargin: 8
    property int signNumberSpacing
    property int appearance: WidgetConstants.SdiNone
    property int widgetSize: WidgetConstants.SizeNone

    property bool sectionSpeedVisible: false
    property int sectionSpeedWidth: 178
    property int sectionSpeedHeight: 90

    property int sectionSpeedAverageTextX: 98
    property int sectionSpeedAverageTextY: 14
    property int sectionSpeedAverageSpeedX: 98
    property int sectionSpeedAverageSpeedY: 14 + 16 + 3
    property int sectionSpeedAverageSpeedWidth: 72
    property int sectionSpeedAverageSpeedHeight: 24    
    property font sectionSpeedAverageFont: Qt.font({        
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp( 16 )
    });

    property int sectionSpeedTimeTextX: 98
    property int sectionSpeedTimeTextY: 14
    property int sectionSpeedTimeTextWidth: 72
    property int sectionSpeedTimeX: 98
    property int sectionSpeedTimeY: 14 + 16 + 3
    property int sectionSpeedTimeWidth: 72
    property int sectionSpeedTimeHeight: 24
    property int sectionSpeedTimeMaxNumbers: 0
    property font sectionSpeedTimeFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp( 16 )
    });

    property bool sectionSpeedToggleAverageAndTime
    property int sectionSpeedToggleInterval: 2000
    property bool sectionSpeedAverageVisible: false
    property bool sectionSpeedTimeVisible: false

    property variant signType: WidgetConstants.CircledNumber
    property bool signVisible
    property string signImageSource
    property var signNumberImageSources
    property bool signSectionSpeedToggle: false
    property var sectionSpeedAverageNumberImageSources
    property var sectionSpeedTimeImageSources
    property bool remainDistVisible
    property string remainDistNumberBgImageSource
    property var remainDistNumberImageSources
    property var currentSpeedNumberImageSources
    property bool isRg: false
    property bool hasContent: signVisible || remainDistVisible
    property bool reset: false

    width: signWidth

    function setToDefault() {
        signNumberY = 49
        signWidth = 140
        signHeight = 164
        remainDistWidth = 140
        remainDistHeight = 44
        currentSpeedNumberTopMargin = 8
        signNumberSpacing = 0

        var appearanceBak = appearance
        appearance = WidgetConstants.SdiNone
        appearance = appearanceBak
    }

    function toggleSpeedSpectionSign() {
        signSectionSpeedToggle = !signSectionSpeedToggle
        if ( signType === RgConstants.SdiChangeableSectionSpeed ) {
            if ( widgetSize == WidgetConstants.SizeXXXL ) {
                var img = signSectionSpeedToggle ? "c_18.png" : "c_10.png"
                signImage.source = Resource.imageUrl( 'sdi', img )
            }
            else {
                img = signSectionSpeedToggle ? "wid_c_18.png" : "wid_c_10.png"
                signImage.source = Resource.imageUrl( 'widgetsdi', img )
            }

            signNumberWrap.visible = !signSectionSpeedToggle
        }
        else if ( signType === RgConstants.SdiChangeableSectionSpeedButSpeedLimitNA ) {
            if ( widgetSize == WidgetConstants.SizeXXXL ) {
                img = signSectionSpeedToggle ? "c_18.png" : "c_11.png"
                signImage.source = Resource.imageUrl( 'sdi', img )
            }
            else {
                img = signSectionSpeedToggle ? "wid_c_18.png" : "wid_c_15.png"
                signImage.source = Resource.imageUrl( 'widgetsdi', img )
            }

            signNumberWrap.visible = false
        }
    }

    onResetChanged: {
        if ( reset ) {
            signWrap.visible = false
            remainDistWrapWrap.visible = false
            currentSpeedNumberWrap.visible = false
        }
    }

    Component.onCompleted: {
        //appearance = WidgetConstants.SdiCurrentSpeed
    }

    onWidgetSizeChanged: {
        //setToDefault()

        if ( widgetSize == WidgetConstants.SizeXXXL ) {
//            signNumberImageSources = [
//                Resource.imageUrl( "sdi", "tmap_speed_sign_num_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_sign_num_9.png" )
//            ]
//            remainDistNumberImageSources = [
//                Resource.imageUrl( "sdi", "tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_remain_num_m.png" )
//            ]
//            currentSpeedNumberImageSources = [
//                Resource.imageUrl( "sdi", "tmap_speed_num_me_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_num_me_9.png" ),
//                Resource.imageUrl( "sdi", "tmap_speed_num_me_9.png" )
//            ]
            //signImageSource = Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_symbol.png' )
            //signImageSource = Resource.imageUrl( 'sdi', 'c_01.png' )

            remainDistNumberBgImageSource = Resource.imageUrl( 'sdi', 'tmap_speed_sign.png' )
        }
        else if ( widgetSize == WidgetConstants.SizeLarge ) {
            remainDistNumberBgImageSource = Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_sign.png' )

            signNumberY = 37
            signWidth = 106
            signHeight = 123
            signNumberSpacing = 0
            remainDistWidth = 106
            remainDistHeight = 33
        }
        else {
//            signNumberImageSources = [
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_sign_num_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_sign_num_9.png" )
//            ]
//            remainDistNumberImageSources = [
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_remain_num_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_remain_num_m.png" )
//            ]
//            currentSpeedNumberImageSources = [
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_num_me_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_num_me_9.png" ),
//                Resource.imageUrl( "widgetsdi", "wid_tmap_speed_num_me_9.png" )
//            ]
//            signImageSource = Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_symbol.png' )

            remainDistNumberBgImageSource = Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_sign.png' )

            signNumberY = 37
            signWidth = 106
            signHeight = 123
            signNumberSpacing = 0
            remainDistWidth = 106
            remainDistHeight = 33
        }
    }

    onAppearanceChanged: {
        if ( appearance == WidgetConstants.SdiAll ) {
            signWrap.visible = true
            remainDistWrapWrap.visible = true            
            currentSpeedNumberWrap.visible = currentSpeedNumberVisible
        }
        if ( appearance == WidgetConstants.SdiAllForLarge ) {
            signWrap.visible = true
            remainDistWrapWrap.visible = true
            currentSpeedNumberWrap.visible = false
        }
        else if ( appearance == WidgetConstants.SdiSign ) {
            signWrap.visible = true
            remainDistWrapWrap.visible = false
            currentSpeedNumberWrap.visible = false
        }
        else if ( appearance == WidgetConstants.SdiSignAndRemainDist ) {
            signWrap.visible = true
            remainDistWrapWrap.visible = true
            currentSpeedNumberWrap.visible = false
        }
        else if ( appearance == WidgetConstants.SdiCurrentSpeed ) {
            signWrap.visible = false
            remainDistWrapWrap.visible = false
            currentSpeedNumberWrap.visible = currentSpeedNumberVisible
        }
        else if ( appearance == WidgetConstants.SdiSignOrCurrentSpeed ) {
            signWrap.visible = true
            remainDistWrapWrap.visible = false
            currentSpeedNumberWrap.visible = currentSpeedNumberVisible

            currentSpeedNumberWrap.anchors.top = undefined
            currentSpeedNumberWrap.anchors.topMargin = undefined
            currentSpeedNumberWrap.y = 34
            currentSpeedNumberWrap.visible = Qt.binding( function() {
                return root.hasContent ? false : currentSpeedNumberVisible
            });
        }
    }

    onSignTypeChanged: {
        //QmlAppHelper.debug( "signType: " + signType )

        if ( widgetSize == WidgetConstants.SizeXXXL ) {            
            sectionSpeedWidth = 234
            sectionSpeedHeight = 90
            sectionSpeedAverageTextX = 130
            sectionSpeedAverageTextY = 16
            sectionSpeedAverageSpeedX = 130
            sectionSpeedAverageSpeedY = 16 + 22 + 2
            sectionSpeedAverageSpeedWidth = 90
            sectionSpeedAverageSpeedHeight = 34
            sectionSpeedAverageFont = Qt.font({
                family: GlobalConstants.kFontFamily,
                pixelSize: Resource.pixelToDp( 20 )
            });

            if ( signType === RgConstants.SdiCircledNumber ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 0
                signNumberY = 49
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = false
            }
            else if ( signType === RgConstants.SdiSymbol ) {
                signNumberWrap.visible = false
                signNumberRepeater.visible = false                
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = false
            }
            else if ( signType === RgConstants.SdiNumberWithSymbol ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 1
                signNumberY = 42
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = false
            }
            else if ( signType === RgConstants.SdiSectionSpeed ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 1
                signNumberY = 42
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = false
            }
            else if ( signType === RgConstants.SdiChangeableSectionSpeed ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 1
                signNumberY = 42
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = true
            }
            else if ( signType === RgConstants.SdiChangeableSectionSpeedButSpeedLimitNA ) {
                signNumberWrap.visible = false
                signNumberRepeater.visible = false
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = true
            }
        }
        else {            
            if ( widgetSize == WidgetConstants.SizeLarge ) {
                if ( isRg ) {
                    sectionSpeedWrap.x = 13
                    sectionSpeedWrap.y = 123
                    sectionSpeedWidth = 79
                    sectionSpeedHeight = 52

                    sectionSpeedAverageTextX = 0
                    sectionSpeedAverageTextY = 8
                    sectionSpeedAverageSpeedX = 0
                    sectionSpeedAverageSpeedY = 8 + 16 + 3
                    sectionSpeedAverageSpeedWidth = 79
                    sectionSpeedAverageSpeedHeight = 24

                    sectionSpeedTimeTextX = 0
                    sectionSpeedTimeTextY = 8
                    sectionSpeedTimeX = 0
                    sectionSpeedTimeY = 8 + 16 + 3
                    sectionSpeedTimeWidth = 79
                    sectionSpeedTimeHeight = 24
                }
                else {
                    sectionSpeedWrap.x = 0
                    sectionSpeedWrap.y = 1
                    sectionSpeedWidth = 178
                    sectionSpeedHeight = 90

                    sectionSpeedAverageTextX = 98
                    sectionSpeedAverageTextY = 12
                    sectionSpeedAverageSpeedX = 98
                    sectionSpeedAverageSpeedY = 12 + 16 + 2
                    sectionSpeedAverageSpeedWidth = 70
                    sectionSpeedAverageSpeedHeight = 24

                    sectionSpeedTimeTextX = 98
                    sectionSpeedTimeTextY = 12
                    sectionSpeedTimeX = 98
                    sectionSpeedTimeY = 12 + 16 + 2
                    sectionSpeedTimeWidth = 70
                    sectionSpeedTimeHeight = 24
                }
            }
            else {
                sectionSpeedWrap.y = 1
                sectionSpeedWidth = 178
                sectionSpeedHeight = 90

                sectionSpeedAverageTextX = 98
                sectionSpeedAverageTextY = 12
                sectionSpeedAverageSpeedX = 98
                sectionSpeedAverageSpeedY = 12 + 16 + 2
                sectionSpeedAverageSpeedWidth = 70
                sectionSpeedAverageSpeedHeight = 24

                sectionSpeedTimeTextX = 98
                sectionSpeedTimeTextY = 12
                sectionSpeedTimeX = 98
                sectionSpeedTimeY = 12 + 16 + 2
                sectionSpeedTimeWidth = 70
                sectionSpeedTimeHeight = 24
            }
            sectionSpeedAverageFont = Qt.font({
                family: GlobalConstants.kFontFamily,
                pixelSize: Resource.pixelToDp( 16 )
            });

            if ( signType === RgConstants.SdiCircledNumber ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 0
                signNumberY = 37
            }
            else if ( signType === RgConstants.SdiSymbol ) {
                signNumberWrap.visible = false
                signNumberRepeater.visible = false
            }
            else if ( signType === RgConstants.SdiNumberWithSymbol ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 0
                signNumberY = 32
            }
            else if ( signType === RgConstants.SdiSectionSpeed ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 1
                signNumberY = 30
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = false
            }
            else if ( signType === RgConstants.SdiChangeableSectionSpeed ) {
                signNumberWrap.visible = true
                signNumberRepeater.visible = true
                signNumberSpacing = 1
                signNumberY = 28
                sectionSpeedToggleTimer.running = true
            }
            else if ( signType === RgConstants.SdiChangeableSectionSpeedButSpeedLimitNA ) {
                signNumberWrap.visible = false
                signNumberRepeater.visible = false
                signSectionSpeedToggle = false
                sectionSpeedToggleTimer.running = true
            }
        }
    }

    onSignVisibleChanged: {
        if ( appearance != WidgetConstants.SdiCurrentSpeed ) {
            signWrap.visible = signVisible
        }
    }

    onRemainDistVisibleChanged: {
        if ( appearance != WidgetConstants.SdiSignOrCurrentSpeed ) {
            remainDistWrap.visible = remainDistVisible
        }
    }

    onRemainDistNumberImageSourcesChanged: {
    }

    onCurrentSpeedNumberImageSourcesChanged: {
    }

    Item {
        id: sectionSpeedWrap
        objectName: "sectionSpeedWrap"
        width: sectionSpeedWidth
        height: sectionSpeedHeight
        visible: sectionSpeedVisible

        Image {
            id: sectionSpeedBg
            source: {
                sectionSpeedBg.y = 0
                if ( widgetSize == WidgetConstants.SizeXXXL ) {
                    return Resource.imageUrl( 'sdi', 'tmap_speed_average_bg.png' )
                }
                else if ( widgetSize == WidgetConstants.SizeLarge ) {
                    if ( !isRg ) {
                        return Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_average_bg.png' )
                    }
                    else {
                        sectionSpeedBg.y = -10
                        return Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_average_l_bg.png' )
                    }
                }
                else {
                    return Resource.imageUrl( 'widgetsdi', 'wid_tmap_speed_average_bg.png' )
                }
            }
        }

        // average
        Item {
            id: sectionSpeedAverageWrap
            visible: (averageAndTimeSectionSpeedToggle || !sectionSpeedAverageAndTimeToggleTimer.running) && sectionSpeedAverageVisible

            Text {
                id: sectionSpeedAverageText
                objectName: "sectionSpeedAverageText"
                x: sectionSpeedAverageTextX
                y: sectionSpeedAverageTextY
                width: sectionSpeedTimeTextWidth
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
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater {
                        id: sectionSpeedAverageRepeater
                        objectName: 'sectionSpeedAverageRepeater'
                        model: sectionSpeedAverageNumberImageSources

                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source: modelData

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
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater {
                        id: sectionSpeedTimeRepeater
                        objectName: 'sectionSpeedTimeRepeater'
                        model: sectionSpeedTimeImageSources
                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source: modelData
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
        id: signWrap
        objectName: "signWrap"
        width: signWidth
        height: signHeight
        visible: false
        z: 100
        onVisibleChanged: {
        }
        Image {
            id: signImage
            objectName: "signImage"
            source: signImageSource
        }
        Row {
            id: signNumberWrap
            objectName: "signNumberWrap"
            y: signNumberY
            width: childrenRect.width
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: signNumberSpacing
            Repeater {
                id: signNumberRepeater
                objectName: "signNumberRepeater"
                model: signNumberImageSources
                delegate: Item {
                    width: childrenRect.width
                    height: childrenRect.height
                    property string source: modelData
                    Image {
                        id: signNumberImage
                        source: parent.source
                    }
                }
            }
        }
    }
	
    Item {
        id: remainDistWrapWrap
        objectName: "remainDistWrapWrap"
        visible: false
        width: remainDistWidth
        height: remainDistHeight
        anchors.bottom: signWrap.bottom
        z: 1
        Item {
            id: remainDistWrap
            objectName: "remainDistWrap"
            anchors.fill: parent
            visible: false

            Image {
                id: remainDistNumberBgImage
                source: remainDistNumberBgImageSource
            }
            Row {
                width: childrenRect.width
                height: childrenRect.height
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    id: remainDistNumberRepeater
                    objectName: 'remainDistNumberRepeater'
                    model: remainDistNumberImageSources
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source:  modelData
                        Image {
                            id: remainDistNumberImage
                            source: parent.source
                        }
                    }
                }
            }
        }
    }


    Item {
        id: currentSpeedNumberWrap
        width: parent.width
        anchors.top: remainDistWrapWrap.bottom
        anchors.topMargin: currentSpeedNumberTopMargin
        visible: false

        Row {
            width: childrenRect.width
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            Repeater {
                id: currentSpeedNumberRepeater
                objectName: 'currentSpeedNumberRepeater'
                model: currentSpeedNumberImageSources
                delegate: Item {
                    width: childrenRect.width
                    height: childrenRect.height
                    property string source: modelData
                    Image {
                        id: currentSpeedNumberImage
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
            toggleSpeedSpectionSign()
        }
    }

    onSectionSpeedToggleAverageAndTimeChanged: {
        sectionSpeedAverageAndTimeToggleTimer.running = sectionSpeedToggleAverageAndTime
    }

    property bool averageAndTimeSectionSpeedToggle: false

    function toggleSpeedSectionAverageAndTime() {
        averageAndTimeSectionSpeedToggle = !averageAndTimeSectionSpeedToggle
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
