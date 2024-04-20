import QtQuick 2.1
import MapViewMode 1.0
import SummaryTbtViewEvent 1.0
import Csw 1.0
import QtGraphicalEffects 1.0
import MouseEventListener 1.0

import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants

View {
    id: root
    hasFocus: false

    property int listTopMargin: 11
    property int arrowWidth: 38
    property int progressWidth: 22
    property int progressRightMargin: 20
    property int panelBgWidth: 303
    property int panelWidth: panelBgWidth + progressWidth + progressRightMargin
    property int panelHeight: 86
    property int maxPanelNumbers: 4
    property int listWidth: arrowWidth + panelWidth
    property int turnIconLeftMargin: 16
    property int turnIconTopMargin: 12
    property int distanceLeftMargin: 9
    property int distanceTopMargin: 12
    property int distanceWidth: 188
    property int distanceHeight: 25
    property int nameTopMargin: 7
    property int nameY: turnIconTopMargin + distanceHeight + nameTopMargin
    property font nameTextFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(25)
    });
    property color nameTextColor: '#ffffff'

    width: listWidth
    height: panelHeight * maxPanelNumbers
    visible: false
    property bool isBuildTarget: false
    property var csws: [rightArrowCsw]
    property int cswFocusTopMargin: 8
    property int cswFocusRightMargin: 44

    property string onlineOrOffline: "offline"
    property string dayOrNight: "day"

    onMediated: {
        sendEvent( SummaryTbtViewEvent.Mediated )
        sendEvent(SummaryTbtViewEvent.RegistCsw, csws)
    }

    onVisibleChanged: {
        sendEvent( SummaryTbtViewEvent.VisibleChanged, {visible: visible} )

        if (visible) {
            //sendEvent(SummaryTbtViewEvent.SelectCsw, csws)
            listView.positionViewAtEnd()
        }
    }

    ListModel {
        id: listModel
        ListElement {
            img: ''
            name: ''
            distanceLength: 0
            distance: [
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                }
            ]
            progress: 'tbt_progress_null'
            noName: false
        }
    }

    function reset() {
        visible = false
        listModel.clear()
        listModel.append( {
            'img': '',
            'name': '',
            'distanceLength': 0,
            'distance': [{img:''},{img:''},{img:''},{img:''},{img:''}],
            'progress': '',
            'noName': false
        })
    }

    function setListModel( models ) {
        if ( models.length !== listModel.count ) {
            listModel.clear()
            var listModelAppend = listModel.append
            for ( var i = 0; i < models.length; i++ ) {
                listModelAppend( {
                    'img': '',
                    'name': '',
                    'distanceLength': 0,
                    'distance': [{img:''},{img:''},{img:''},{img:''},{img:''}],
                    'progress': '',
                    'noName': false
                })
            }
        }

        var listModelSetProperty = listModel.setProperty
        var listModelGet = listModel.get

        for ( var i = 0; i < models.length; i++ ) {
            var model = models[i]
            var index = listModel.count - i - 1;
            listModelSetProperty( index, 'img', model.img )
            listModelSetProperty( index, 'name', model.name )

            listModelSetProperty( index, 'distanceLength', model.distanceLength )

            if ( listModelGet( index ).distance.count !== model.distanceLength ) {
                listModelGet( index ).distance.clear()
                for ( var j = 0; j < model.distanceLength; j++ ) {
                    listModelGet( index ).distance.append( {'img': model.distance[j]} )
                }
            }
            else {
                for ( j = 0; j < model.distanceLength; j++ ) {
                    listModelGet( index ).distance.setProperty( j, 'img', model.distance[j] )
                }
            }
            listModelSetProperty( index, 'progress', model.progress )
            listModelSetProperty( index, 'noName', model.noName )
        }

        listView.positionViewAtEnd()
    }

    //5초 후에는 스크롤 위치 자동 원복
    Timer {
        id: scrollTimer
        interval: 0
        running: false
        repeat: false
        onTriggered: {
            listView.positionViewAtEnd()
        }
    }
    function scrollTouched() {
        scrollTimer.stop()
        scrollTimer.interval = 5000
        scrollTimer.start()
    }

    function setListModelDistance( index, distanceImages ) {
        var index2 = listModel.count - index - 1;
        var listModelGet = listModel.get

        listModel.setProperty( index2, 'distanceLength', distanceImages.length )

        if ( listModelGet( index2 ).distance.count !== distanceImages.length ) {
            listModelGet( index2 ).distance.clear()
            for ( var i = 0; i < distanceImages.length; i++ ) {
                listModelGet( index2 ).distance.append( {'img': distanceImages[i]} )
            }
        }
        else {
            for ( i = 0; i < distanceImages.length; i++ ) {
                listModelGet( index2 ).distance.setProperty( i, 'img', distanceImages[i] )
            }
        }
    }

    Component {
        id: panelComp
        TFocusItem {
            id: panel
            width: panelWidth
            height: panelHeight
            z: index

            MouseArea {
                anchors.fill: parent
                enabled: index == 0
                onClicked: {
                    sendEvent( SummaryTbtViewEvent.ClosingTouchAreaClicked )
                    rightArrowCsw.forceLostFocus()
                    mouse.accepted = false
                }
            }

            Image {
                id: bgImg
                source: index < listModel.count - 1 ? Resource.imageUrl( 'tbt', 'tbt_3rd_bg1.png' ) : Resource.imageUrl( 'tbt', 'tbt_3rd_bg2.png' )
            }

            Image {
                id: turnIcon
                anchors.left: parent.left
                anchors.leftMargin: turnIconLeftMargin
                anchors.top: parent.top
                anchors.topMargin: turnIconTopMargin
                source: img
            }

            Item {
                id: distanceWrap
                anchors.left: turnIcon.right
                anchors.leftMargin: distanceLeftMargin
                anchors.top: noName ? undefined : parent.top
                anchors.topMargin: noName ? undefined : distanceTopMargin
                anchors.verticalCenter: noName ? parent.verticalCenter : undefined
                width: distanceWidth
                height: distanceHeight
                Row {
                    anchors.left: parent.left
                    Repeater {
                        id: distanceNumberRepeater
                        model: distance
                        delegate: Image {
                            id: distanceNumberImage
                            visible: index < distanceLength ? true : false
                            source: img
                        }
                    }
                }
            }

            MarqueeText {
                id: marqueeText
                anchors.left: turnIcon.right
                anchors.leftMargin: distanceLeftMargin
                y: nameY
                width: 188
                font: nameTextFont
                textColor: nameTextColor
                text: name
                visible: !noName

                layer.enabled: scrolling
                layer.samplerName: "textSource"
                layer.effect: ShaderEffect {
                    property var colorSource: gradientRect;
                    fragmentShader: "
                                uniform lowp sampler2D colorSource;
                                uniform lowp sampler2D textSource;
                                uniform lowp float qt_Opacity;
                                varying highp vec2 qt_TexCoord0;
                                void main() {
                                    gl_FragColor =
                                        texture2D(colorSource, qt_TexCoord0).a
                                        * texture2D(textSource, qt_TexCoord0)
                                        * qt_Opacity;
                                }
                            "
                }

                LinearGradient {
                    id: gradientRect
                    anchors.fill: marqueeText
                    gradient: Gradient {
                        GradientStop { position: 0; color: Qt.rgba(0,0,0,0) }
                        GradientStop { position: 16/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                        GradientStop { position: (gradientRect.width-16)/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                        GradientStop { position: 1; color: Qt.rgba(0,0,0,0) }
                    }
                    start: Qt.point(0, 0)
                    end: Qt.point(gradientRect.width, 0)
                    visible: false
                    layer.enabled: true
                    layer.smooth: true
                }

                onScrollingChanged: {
                    //marqueeHighlight.visible = scrolling
                }
            }

            Item {
                id: progressWrap
                width: progressWidth
                height: parent.height
                anchors.left: bgImg.right

                Image {
                    id: barImg
                    anchors.top: dotImg.top
                    anchors.topMargin: 11
                    property bool isEnd: index == listView.count - 1
                    fillMode: Image.TileVertically
                    source: {
                        var ret = ''

                        if ( isEnd ) {
                            ret = model.progress ? Resource.imageUrl( 'tbt', onlineOrOffline, dayOrNight, model.progress + '_end.png' ) : ''
                        }
                        else {
                            //[ljaeho] tbt스크롤시 dotImg를 덮는문제 발생하여 높이 지정함.
                            height = panelHeight-7
                            ret = model.progress ? Resource.imageUrl( 'tbt', onlineOrOffline, dayOrNight, model.progress + '.png' ) : ''
                        }

                        return ret
                    }
                }

                Image {
                    id: dotImg
                    //anchors.verticalCenter: parent.verticalCenter
                    y: 28
                    source: Resource.imageUrl( 'tbt', 'tbt_progress_dot.png' )
                    z: 1
                }
            }
        }
    }

    Image {
        id: cswFocusMark
        source: Resource.imageUrl( 'tbt', 'map_tbt_2nd_csw_f.png' )
        visible: false
        z: 99
        anchors.top: parent.top
        anchors.topMargin: cswFocusTopMargin
        anchors.right: parent.right
        anchors.rightMargin: cswFocusRightMargin
    }

    MouseEventListener {
        id: mouseEventListener
        anchors.fill: parent        

        property bool hasPressedAtTheTouchArea: false
        property bool hadMovement: false

        function isPointAtTouchAreaClosingList(pos) {
            if ( pos.x >= listView.x && pos.x < listView.x + panelWidth ) {
                if ( pos.y >= listView.y && pos.y < listView.y + panelHeight ) {
                    return true
                }
            }

            return false
        }

        onPressed: {
            hadMovement = false
            hasPressedAtTheTouchArea = isPointAtTouchAreaClosingList(pos);
        }

        onReleased: {
            if ( !hadMovement && hasPressedAtTheTouchArea && isPointAtTouchAreaClosingList(pos) ) {
                console.info('[SummaryTbtView][MouseEventListener]Touched! pos:' + pos + ' listView.x: ' + listView.x)
                sendEvent( SummaryTbtViewEvent.ClosingTouchAreaClicked )
                rightArrowCsw.forceLostFocus()
            }
            hasPressedAtTheTouchArea = false
        }

        Item {
            id: listWrap
            objectName: 'list'
            anchors.top: parent.top
            anchors.topMargin: listTopMargin
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: listWidth
            Image {
                id: rightArrowImg
                source: Resource.imageUrl( 'tbt', 'tbt_arw_r.png' )
                TMouseArea {
                    anchors.fill: parent
                    onClicked: {
                        sendEvent( SummaryTbtViewEvent.ArrowClicked )
                    }
                }
                Csw {
                    id: rightArrowCsw
                    objectName: 'rightArrowCsw'
                    section: Csw.Section_Map
                    sequence: Csw.Sequence_SummaryTbtArrow
                    deletable: false
                    enable: root.visible
                    onFocused: {
                        console.log('[CSW][SummaryTbtView]onFocused')
                        cswFocusMark.visible = true
                    }
                    onLostFocusd: {
                        console.log('[CSW][SummaryTbtView]onLostFocusd')
                        cswFocusMark.visible = false
                    }
                    onSelected: {
                        console.log('[CSW][SummaryTbtView]onSelected')
                        sendEvent( SummaryTbtViewEvent.ArrowClicked, {byCsw:true} )
                    }
                }
            }

            ListView {
                id: listView
                objectName: 'listView'
                width: listWidth
                height: panelHeight * (listModel.count > maxPanelNumbers ? maxPanelNumbers : listModel.count)
                anchors.left: rightArrowImg.right
                model: listModel
                interactive: true
                cacheBuffer: panelHeight * 3
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                delegate: panelComp

                onMovementStarted: {
                    mouseEventListener.hadMovement = true
                }

                onMovementEnded: {
                    mouseEventListener.hadMovement = true
                    scrollTouched()
                }
            }
        }
    }
}
