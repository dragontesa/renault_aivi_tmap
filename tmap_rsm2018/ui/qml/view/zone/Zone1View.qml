import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view"
import ApplicationEvent 1.0
import Zone1ViewEvent 1.0
import SceneGraphRendering 1.0
import Csw 1.0

View {
    id: root

    objectName: "Zone1View"
    //property real ratio: 800.0 / GlobalConstants.kScreenWidth
    property int screenWidth: GlobalConstants.kScreenWidth
    property string title
    property string padding

    property bool online: false
    property alias onlineStatusVisible: onlineStatus.visible
    property bool enableReroute: false
    property bool setTvas: false
    property bool rpRequest: false
    property bool periodicRerouteArea: false
    property string mapMode // N : Normal
                            // M : MapControl
                            // Q : MapInquery
                            // V : Navi
                            // v : MenuNavi
                            // S : Simul
                            // C : NormalMapControl

    property bool noMap: false

    readonly property int itemHeight: 10
    property string via1
    property string via2
    property string via3
    property string via4
    property string via5
    property date gpsTime
    property string tvaslist
    property string activeView

    property int searchResultCount
    property double mapViewLevel

    property color naviStatusColor
    property string naviStatus
    onNaviStatusChanged: {
        switch (naviStatus) {
        case 'Unknown':
            naviStatusColor = 'lightgray'
            break
        case 'Busy':
        case 'RP...':
        case 'Map update':
            naviStatusColor = 'white'
            break
        case 'RG':
        case 'Ando':
            naviStatusColor = 'yellow'
            break
        case 'NG':
            naviStatusColor = 'red'
            break
        default:
            console.log('NG...' + naviStatus)
            break
        }
    }

    onVia1Changed: {
        via1Loader.item.pos = via1
    }
    onVia2Changed: {
        via2Loader.item.pos = via2
    }
    onVia3Changed: {
        via3Loader.item.pos = via3
    }
    onVia4Changed: {
        via4Loader.item.pos = via4
    }
    onVia5Changed: {
        via5Loader.item.pos = via5
    }

    function updateViaList() {
        via1Loader.item.viaTextColor = "gray"
        via1Loader.item.update()
        via2Loader.item.viaTextColor = "gray"
        via2Loader.item.update()
        via3Loader.item.viaTextColor = "gray"
        via3Loader.item.update()
        via4Loader.item.viaTextColor = "gray"
        via4Loader.item.update()
        via5Loader.item.viaTextColor = "gray"
        via5Loader.update()
    }

    width: screenWidth
    //height: GlobalConstants.kScreenZone1_Height * ratio * 0.5
    height: GlobalConstants.kScreenZone1Height

    //chunjae.lee 2018.03.15 - 입력박스 (18)글자 제한
    function normalized(text) {
//        if (text.length >= 18)
//            compTextInput.maximumLength = compTextInput.text.length
//        else
//            compTextInput.maximumLength = 32767
    }

    //chunjae.lee 2017.11.10  - str인자 추가
    function setTextInput(str) {
        compTextInput.visible = true
        if(str != undefined)
           compTextInput.text = str
        compTitle.visible = false
    }
    function setTitle(str) {
        compTextInput.visible = false
        compTitle.visible = true
        compTitle.title = str
    }

    function __inputFieldForceActiveFocus() {
            forceActiveFocus()
            inputField.forceActiveFocus()
    }

    function updateModal(visible) {
        dimmingWindow.visible = visible
    }

    function setTvasFilename(list) {
        console.log('list : ' + list)
        tvaslist = ''
        for (var i=0; i<list.length; i++) {
            var filename = list[i]
            if (0 <= filename.indexOf('1.db')) {
                tvaslist += '1'
            } else if (0 <= filename.indexOf('2.db')) {
                tvaslist += '2'
            } else if (0 <= filename.indexOf('3.db')) {
                tvaslist += '3'
            } else if (filename == 'tvas_route.db') {
                tvaslist += filename
            }
            tvaslist += ','
        }
    }

    onMediated: {
        sendEvent(Zone1ViewEvent.Mediated, {keyboardInput: inputField})
        sendEvent(Zone1ViewEvent.RegistCsw, [csw])
        sendEvent(Zone1ViewEvent.RegistCommonKeyboardInput, {keyboardInput: inputField})
    }

    Rectangle {
        anchors.fill: parent
        color: "black"

        Image {
            id: imageBackground

            anchors.centerIn: parent
            //height: 69
            height: parent.height
            fillMode: Image.PreserveAspectFit
            source: Resource.imageUrl("zone/zone1_dummy.png")

            Item {
                id: compTextInput

                property alias text: inputField.text
                property alias maximumLength : inputField.maximumLength
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: childrenRect.width
                height: childrenRect.height

                Image {
                    y: layout.compTextInputBgY
                    width: imageBackground.width
                    height: layout.compTextInputBgHeight

                    fillMode: Image.PreserveAspectFit
                    source: Resource.imageUrl("zone/search_input_bg.png")

                    TextInput {
                        id: inputField

                        Component.onCompleted: {
                            //console.log("compTextInput created...")
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }

                        onTextChanged: sendEvent( Zone1ViewEvent.RequestNomalize, {text: text})

                        onVisibleChanged: {
                            if (visible)
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }

                        focus: false                        
                        anchors { fill: parent; leftMargin: layout.compTextInputFieldLeftMargin; rightMargin: layout.compTextInputFieldRightMargin }
                        verticalAlignment: Qt.AlignVCenter
                        color: 'black'
                        clip: true
                        font.pixelSize: Resource.pixelToDp(35);
                        font.family: 'Noto Sans Mono CJK KR'

                        onActiveFocusChanged: {
                            console.log( 'TextInput.onActiveFocusChanged #1 activeFocus: ' + activeFocus )

                            if ( activeFocus && visible ) {
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                                sendEvent( Zone1ViewEvent.TextInputFocusActivated )
                            }
                        }

                        onFocusChanged: {
                            //console.log( 'onFocusChanged focus: ' + focus )
                        }

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            verticalAlignment: Text.AlignVCenter
                            text: '명칭/주소/전화번호'
                            color: '#a1a2b3'
                            visible: inputField.text.length === 0
                        }

                        // 17.11.10 hskim force to send Zone1Constants::FocusZone1TextInput notification
                        // so that search tab view is created even when text input has active focus and it is clicked
                        FocusScope {
                            id: inputFieldFocusScope
                            focus: true
                            anchors.fill: parent
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    inputFieldFocusScope.forceActiveFocus()
                                    inputField.forceActiveFocus()
                                }
                            }
                        }
                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
                            text: searchResultCount < 100 ? searchResultCount : '99'
                            color: 'black'
                            visible: inputField.text.length > 0
                        }
                    }//inputField


                    Rectangle {
                        id: inputFieldBorder

                        anchors.fill: inputField
                        color: 'transparent'
                        border { color: 'red'; width: 2 }
                        visible: false
                    }
                }//Image
            }//compTextInput
            Item {
                id: compTitle

                property alias title: label.text

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: layout.compTitleLabelBottomMargin
                width: childrenRect.width
                height: childrenRect.height

                Text {
                    id: label
                    width: layout.compTitleLabelWidth; height: layout.compTitleLabelHeight
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(layout.compTitleLabelFontPixelSize) }//pixelSize
                    color: "#ffffff"
                }
            }//compTilte

            Csw {
                id: csw
                objectName: 'editbox'
                section: Csw.Section_IntegratedSearch
                sequence: 0
                enable: inputField.visible
                onFocused: inputFieldBorder.visible = true
                onLostFocusd: inputFieldBorder.visible = false
                deletable: false
                onSelected: {
                    sendEvent(Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                    sendEvent(Zone1ViewEvent.TextInputFocusActivated)
                    csw.forceLostFocus()
                }
            }
        } Rectangle {
            id: onlineStatus
            visible: false
            width: 16; height: 16; radius: width/2
            color: online ? "green" : "red"
            anchors { left: parent.left; top: parent.top; leftMargin: 4; topMargin: 4 }
            MouseArea {anchors.fill: parent; onClicked: viaList.visible = true }
        }
        Rectangle {
            id: enableRerouteStatus
            visible: onlineStatus.visible
            width: 16; height: 16; radius: width/2
            color: enableReroute ? "white" : "#444444"
            anchors { left: parent.left; top: onlineStatus.bottom; leftMargin: 4; topMargin: 4 }
            Text { anchors.centerIn: parent; color: 'red'; text: 'X'; visible: !periodicRerouteArea }
        }
        Rectangle {
            id: rpRequestStatus
            width: 16; height: 16; radius: width/2
            color: rpRequest ? 'yellow' : '#444444'
            anchors { top : enableRerouteStatus.top; left: enableRerouteStatus.right; leftMargin: 4 }
            Text { anchors.centerIn: parent; color: '#000000'; text: 'R' }
        }

        Rectangle {
            id: mapModeStatus
            visible: onlineStatus.visible
            width: 16; height: 16
            color: "black"; border.color: "white"
            Text {
                anchors.fill: parent
                text: root.mapMode
                color: "white"
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            }
            anchors { left: parent.left; top: enableRerouteStatus.bottom; leftMargin: 4; topMargin: 4 }
        }
        Rectangle {
            id: guidView
            width: 16; height: 16
            color: "lightsteelblue"
            visible: onlineStatus.visible
            Text {
                anchors.fill: parent; text: "G"; color: "white"
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: sendEvent(Zone1ViewEvent.ToggleGuidView)
            }
            anchors { left: parent.left; top: mapModeStatus.bottom; leftMargin: 4; topMargin: 4 }
        }
        Text {
            width: 32; height: 16
            anchors { top: guidView.top; left: guidView.right }
            color: naviStatusColor
            text: naviStatus
        }
        Rectangle {
            id: moveModeView
            width: 32; height: 16
            color: "orange"
            visible: onlineStatus.visible
            Text {
                anchors.fill: parent; color: "yellow"
                text: {
                    var s = ''
                    if (typeof(mapView) !== 'undefined') {
                        switch (mapView.moveMode) {
                        case Renderer.Normal: s = 'N'
                            break
                        case Renderer.Simulator: s = 'S'
                            break
                        case Renderer.Navigation: s = 'V'
                            break
                        }
                        switch (mapView.viewMode) {
                        case Renderer.NorthBound: s += 'N'
                            break
                        case Renderer.HeadUp: s += 'H'
                            break
                        case Renderer.BirdView: s += '3'
                            break
                        }
                    }
                    return s
                }

                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: sendEvent(Zone1ViewEvent.ToggleGuidView)
            }
            anchors { left: parent.left; top: guidView.bottom; leftMargin: 4; topMargin: 4 }
        }        
        Rectangle {
            id: tvas
            width: 16; height: 16; color: 'black'; border.color: 'white'
            visible: onlineStatus.visible
            Text { anchors.fill: parent; color: setTvas ? 'red' : 'darkgray'; text: 'T'
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            }
            anchors { left: parent.left; top: moveModeView.bottom; leftMargin: 4; topMargin: 4 }
        }
        Rectangle {
            id: pullDownKeyboard
            width: 16; height: 16; color: 'gray'; border.color: 'darkgray'
            visible: onlineStatus.visible

            Rectangle { width: 10; height: 1; color: 'black'; y: 12
                anchors.horizontalCenter: parent.horizontalCenter
            }

            anchors { left: tvas.right; top: tvas.top }
            MouseArea { anchors.fill: parent;
                onClicked: sendEvent(Zone1ViewEvent.PullDownKeyboard)
            }
        }
        Rectangle {
            id: pullUpKeyboard
            width: 16; height: 16; color: 'gray'; border.color: 'darkgray'
            visible: onlineStatus.visible

            Rectangle { width: 10; height: 1; color: 'black'; y: 4
                 anchors.horizontalCenter: parent.horizontalCenter
            }
            anchors { left: pullDownKeyboard.right; top: pullDownKeyboard.top }
            MouseArea { anchors.fill: parent;
                onClicked: sendEvent(Zone1ViewEvent.PullUpKeyboard)
            }
        }
        Text {
            id: paddingText
            width: 256; height: 16; anchors.left: pullUpKeyboard.right; anchors.top: pullUpKeyboard.top
            text: padding
            color: 'white'
        }
        Text {
            anchors.bottom: paddingText.top
            anchors.left: paddingText.left
            width: 256; height: 16
            text: tvaslist
            color: 'yellow'
        }

        Component {
            id: compVia
            Rectangle {
                property string beforePos
                property string pos
                property alias viaTextColor: viaText.color
                anchors.fill: parent
                color: "black"
                onPosChanged: {
                    console.log("pos = " + pos + ", length = " + pos.length)
                    console.log("beforePos = " + beforePos)

                    if (pos.length === 0) {
                        if (beforePos.length > 0) {
                            viaText.text = beforePos
                            viaText.color = "red"
                        }
                    } else {
                        if (beforePos.length === 0) {
                            viaText.text = pos
                            viaText.color = "yellow"
                        } else {
                            if (pos.localeCompare(beforePos)) {
                                viaText.color = "lightgray"
                            } else {
                                viaText.text = pos
                                viaText.color = "white"
                            }
                        }
                    }
                    beforePos = pos
                }

                Text {
                    id: viaText
                    anchors.fill: parent
                    text: (pos.length === 0 && beforePos.length === 0) ? "" : pos
                    font.pixelSize: 10
                }
            }
        }

        Item {
            id: viaList
            width: 128; height: itemHeight*5
            anchors { left: onlineStatus.right; leftMargin: 4; top: onlineStatus.top }
            visible: false
            Column {
                anchors.fill: parent
                Loader {
                    id: via1Loader
                    width: 128; height: itemHeight; sourceComponent: compVia
                    onLoaded: { item.pos = via1 }
                }
                Loader {
                    id: via2Loader
                    width: 128; height: itemHeight; sourceComponent: compVia
                    onLoaded: { item.pos = via2 }
                }
                Loader {
                    id: via3Loader
                    width: 128; height: itemHeight; sourceComponent: compVia
                    onLoaded: { item.pos = via3 }
                }
                Loader {
                    id: via4Loader
                    width: 128; height: itemHeight; sourceComponent: compVia
                    onLoaded: { item.pos = via4 }
                }
                Loader {
                    id: via5Loader
                    width: 128; height: itemHeight; sourceComponent: compVia
                    onLoaded: { item.pos = via5 }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: dimmingWindow.visible

            Rectangle {
                id: dimmingWindow
                anchors.fill: parent
                visible: false
                color: "black"
                opacity: 0.7
            }
        }

        Rectangle {
            visible: onlineStatus.visible
            anchors { top: parent.top; right: parent.right }
            width: 128; height: 32; color: 'gray'
            Text {
                anchors.fill: parent
                text: gpsTime.toTimeString()
            }
        }
    }
    Text {
        width: parent.width; height: 16
        anchors.bottom: parent.bottom
        color: 'green'
        text: 'viewLevel: ' + mapViewLevel + ', activeView: ' + activeView
    }    
}
