import QtQuick 2.1
import "../../component"
import "../../view"
import "../../view/util"
import "../../singleton"
import TestViewEvent 1.0
import QtGraphicalEffects 1.0
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: viewRoot
    title: "Test"
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    modal: false
    hasFocus: false

    function clearStroke() {
        console.debug('#1')
        canvas.points = new Array;
        canvas.requestPaint();
    }

    function enableClickBlocker(enable) {
        //clickBlocker.enabled = enable
    }

    function addPointToStroke(stroke, point) {
        //console.debug('point x: ' + point.x + " y: " + point.y)
        canvas.points.push(point);
        canvas.requestPaint();
    }

    function addMessage(message) {
        if ( noMessage ) {
            messageListModel.clear()
            noMessage = false
        }

        messageListModel.append({message:message})
        messageListView.positionViewAtEnd()
    }

    function clearMessage() {
        noMessage = true
        messageListModel.clear()
        messageListModel.append('no message')
    }

    function changeSkyState(state) {
        if (state === TestViewEvent.SkyStateDawn) {
            mapView.skyState = 3
        }
        else if (state === TestViewEvent.SkyStateDusk) {
            mapView.skyState = 1
        }
    }

    function stopRecordTouch(index) {
        var items = miscView.testItems
        items[index].state = false
        miscView.testItems = items
    }

    MouseArea {
        id: clickBlocker
        enabled: false
        width: parent.width
        height: parent.height
        z: 999
    }

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height
        property variant points: new Array;
        z: 99

        onPaint: {
            //console.debug('#1')

            var ctx = getContext('2d');
            ctx.clearRect(0,0,canvas.width, canvas.height);
            ctx.fillStyle = "red"

            for (var i = 0; i < points.length; i++) {
                var pt = points[i];
                //console.debug('pt[' + i + '] x: ' + pt.x + " y: " + pt.y)
                ctx.fillRect(pt.x - 3, pt.y - 3, 6, 6);
            }
        }
    }

    Item {
        id: topWrap
        y: GlobalConstants.kScreenZone1Height
        width: 600
        height: GlobalConstants.kScreenContentHeight
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            anchors.fill: parent
            color: 'black'
            opacity: 0.5
        }

        Rectangle {
            id: navWrap
            width: parent.width
            height: 50
            color: 'white'
            property int activeIndex: 1

            ExclusiveGroup { id: navButtonGroup }

            Row {
                //anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10
                property var buttons: [themeButton, miscButton]

                Component.onCompleted: {
                    buttons[navWrap.activeIndex].checked = 1
                }

                TPushButton {
                    id: themeButton
                    label: "Theme"
                    width: 120
                    height: 50
                    normalBackgroundColor: 'white'
                    pressedBackgroundColor: 'gray'
                    labelNormalColor: 'black'
                    labelPressedColor: 'white'
                    exclusiveGroup: navButtonGroup
                    onButtonClicked: {
                        navWrap.activeIndex = 0
                    }
                }

                TPushButton {
                    id: miscButton
                    label: "Misc"
                    width: 120
                    height: 50
                    normalBackgroundColor: 'white'
                    pressedBackgroundColor: 'gray'
                    labelNormalColor: 'black'
                    labelPressedColor: 'white'
                    exclusiveGroup: navButtonGroup
                    onButtonClicked: {
                        navWrap.activeIndex = 1
                    }
                }
            }
        }

        Item {
            id: contentWrap

            anchors.top: navWrap.bottom
            anchors.topMargin: 5
            anchors.bottom: parent.bottom
            width: parent.width

            Item {
                id: miscView
                visible: navWrap.activeIndex == 1
                anchors.fill: parent

                readonly property int testItemRows: 3
                readonly property int testItemCols: 3
                readonly property int cellHeight: 50
                readonly property int keypadButtonWidth: 80
                readonly property int keypadButtonHeight: 60

                property var testItems: [
                    {name:'FileGPS SetPos', event:TestViewEvent.FileGpsSetPos, data:{}},
                    {name:'FileGPS Rewind', event:TestViewEvent.FileGpsRewind, data:{}},
                    {name:'FileGPS FastForward', event:TestViewEvent.FileGpsFastForward, data:{}},
                    {name:'한국어', event:TestViewEvent.Korean, data:{}},
                    {name:'영어', event:TestViewEvent.English, data:{}},
                    {name:'Gadget Icon Fuel', event:TestViewEvent.GadgetIconFuel, data:{}},
                    {name:'Gadget Icon Parking', event:TestViewEvent.GadgetIconParking, data:{}},
                    {name:'Stop Gadget Icon Fuel', event:TestViewEvent.StopGadgetIconFuel, data:{}},
                    {name:'Stop Gadget Icon Parking', event:TestViewEvent.StopGadgetIconParking, data:{}},                                        
                    {name:'RecordTouch', event:TestViewEvent.RecordTouch, type: 'toggle', state: false, data:{}},
                    {name:'ClearTouch', event:TestViewEvent.ClearTouch, data:{}},
                    {name:'Sim Panning', event:TestViewEvent.SimPanning, data:{}},
                    {name:'Sim PinchIn', event:TestViewEvent.SimPinchIn, data:{}},
                    {name:'Sim PinchOut', event:TestViewEvent.SimPinchOut, data:{}},
                    {name:'MS Echo', event:TestViewEvent.ChangeMultiSenseMode, data:0},
                    {name:'MS Comfort', event:TestViewEvent.ChangeMultiSenseMode, data:1},
                    {name:'MS Regular', event:TestViewEvent.ChangeMultiSenseMode, data:2},
                    {name:'MS Sport', event:TestViewEvent.ChangeMultiSenseMode, data:3},
                    {name:'Arbitrary 0', event:TestViewEvent.Test, data:{test:0}},
                    {name:'Arbitrary 1', event:TestViewEvent.Test, data:{test:1}},
                    {name:'Arbitrary 2', event:TestViewEvent.Test, data:{test:2}},
                    {name:'Address VR', event:TestViewEvent.AddressSearchVr, data:{}},
                    {name:'Address VR 2', event:TestViewEvent.AddressSearchVr2, data:{}},
                    {name:'Gadget SaveImage On', event:TestViewEvent.GadgetSaveImage, data:1},
                    {name:'Gadget SaveImage Off', event:TestViewEvent.GadgetSaveImage, data:0},
                    {name:'GPS Antenna ON', event:TestViewEvent.GpsAntennaOn, data:0},
                    {name:'GPS Antenna OFF', event:TestViewEvent.GpsAntennaOff, data:0},
                    {name:'CrossImage vNext', event:TestViewEvent.CrossImageVnext, data:0},
                    {name:'Gadget Focus', event:TestViewEvent.GadgetFocus, data:0},              
                ]

                GridView {
                    id: miscTestItemGridView
                    width: parent.width
                    height: miscView.cellHeight * miscView.testItemRows
                    model: miscView.testItems
                    cellWidth: width / miscView.testItemCols
                    cellHeight: height / miscView.testItemRows
                    clip: true
                    delegate: Item {
                        width: miscTestItemGridView.cellWidth
                        height: miscTestItemGridView.cellHeight
                        Rectangle {
                            id: button
                            width: parent.width - 3
                            height: miscView.cellHeight - 3
                            anchors.centerIn: parent
                            color: {
                                if (modelData.type && modelData.type == 'toggle') {
                                    return modelData.state == 0 ? '#8B8B8B' : 'blue'
                                }
                                else {
                                    return state == 'normal' ? '#8B8B8B' : 'gray'
                                }
                            }
                            state: modelData.type && modelData.type == 'toggle' ? modelData.state : 'normal'
                            Text {
                                anchors.centerIn: button
                                scale: button.state === "pressed"  ? 0.85 : 1.0
                                text: modelData.name
                                color: 'white'
                            }
                            MouseArea {
                                anchors.fill: parent
                                onPressedChanged: {
                                    if (pressed) {
                                        button.state = 'pressed';
                                    } else {
                                        button.state = 'normal';
                                    }
                                }
                                onClicked: {
                                    var eventData
                                    var updateItems
                                    if (modelData.type && modelData.type == 'toggle') {
                                        var items = miscView.testItems
                                        var newState = 1 - items[index].state
                                        var itemData = modelData.data
                                        items[index].state = !items[index].state

                                        eventData = {state: newState, data: itemData, index: index}
                                        updateItems = items
                                    }
                                    else {
                                        eventData = modelData.data
                                    }

                                    if (modelData.name === 'FileGPS SetPos') {
                                        eventData = miscKeypadButtons.item.keypadTextInputText
                                    }

                                    viewRoot.sendEvent( modelData.event, eventData )

                                    if (updateItems) {
                                        miscView.testItems = updateItems
                                    }
                                }
                            }
                        }
                    }
                }

                Loader {
                    id: miscKeypadButtons
                    anchors.top: miscTestItemGridView.bottom
                    anchors.topMargin: 5
                    sourceComponent: keypadComp
                    width: parent.width
                    height: childrenRect.height
                }

                // Message
                ListModel {
                    id: messageListModel
                    ListElement {
                        message: 'no message'
                    }
                }

                property bool noMessage: true

                ListView {
                    id: messageListView
                    width: parent.width
                    anchors.top: miscKeypadButtons.bottom
                    anchors.topMargin: 5
                    //anchors.bottom: closeButton.top
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    model: messageListModel
                    delegate: Item {
                        width: parent.width
                        height: 20
                        Rectangle {
                            anchors.fill: parent
                            color: 'blue'
                            opacity: 0.75
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: message
                                color: 'white'
                            }
                        }
                    }
                }
            }

            Item {
                id: themeView
                anchors.fill: parent
                visible: navWrap.activeIndex == 0

                readonly property int testItemRows: 3
                readonly property int testItemCols: 3
                readonly property int cellHeight: 50
                readonly property int keypadButtonWidth: 80
                readonly property int keypadButtonHeight: 60

                property var themeTestItems: [
                    {name:'Cluster Centeral', event:TestViewEvent.DisplayCenteralMapInClusterWindow, data:{}},
                    {name:'Cluster Side', event:TestViewEvent.DisplaySideMapInClusterWindow, data:{}},
                    {name:'GadgetXXL', event:TestViewEvent.ShowGadgetXXL, data:{}},
                    {name:'GadgetXL', event:TestViewEvent.ShowGadgetXL, data:{}},
                    {name:'GadgetL', event:TestViewEvent.ShowGadgetL, data:{}},
                    {name:'GadgetM', event:TestViewEvent.ShowGadgetM, data:{}},
                    {name:'GadgetS', event:TestViewEvent.ShowGadgetS, data:{}},
                    {name:'QuitGadget', event:TestViewEvent.QuitGadget, data:{}},
                    {name:'Gadget Background', event:TestViewEvent.GadgetBackground, data:{}},
                ]

                // Theme
                property var themeCavatarItems: [
                    {name:'Cluster Central', event:TestViewEvent.C_C_CS_C, data:{}},
                    {name:'Cluster Side', event:TestViewEvent.C_S_CS_C, data:{}},
                    {name:'Default', event:TestViewEvent.D_CS_C, data:{}},
                    {name:'GXXL', event:TestViewEvent.GXXL_CS_C, data:{}},
                    {name:'GXL', event:TestViewEvent.GXL_CS_C, data:{}},
                    {name:'GL', event:TestViewEvent.GL_CS_C, data:{}},
                ]

                property var themeRouteLineItems: [
                    {name:'Cluster Central', event:TestViewEvent.C_C_RLT, data:{}},
                    {name:'Cluster Side', event:TestViewEvent.C_S_RLT, data:{}},
                    {name:'Default', event:TestViewEvent.D_RLT, data:{}},
                    {name:'GXXL', event:TestViewEvent.GXXL_RLT, data:{}},
                    {name:'GXL', event:TestViewEvent.GXL_RLT, data:{}},
                    {name:'GL', event:TestViewEvent.GL_RLT, data:{}},
                ]

                property var themeRouteLineTbtItems: [
                    {name:'Cluster Central', event:TestViewEvent.C_C_RLTT, data:{}},
                    {name:'Cluster Side', event:TestViewEvent.C_S_RLTT, data:{}},
                    {name:'Default', event:TestViewEvent.D_RLTT, data:{}},
                    {name:'GXXL', event:TestViewEvent.GXXL_RLTT, data:{}},
                    {name:'GXL', event:TestViewEvent.GXL_RLTT, data:{}},
                    {name:'GL', event:TestViewEvent.GL_RLTT, data:{}},
                ]

                GridView {
                    id: themeTestItemGridView
                    width: parent.width
                    height: themeView.cellHeight * themeView.testItemRows
                    model: themeView.themeTestItems
                    cellWidth: width / themeView.testItemCols
                    cellHeight: height / themeView.testItemRows
                    clip: true
                    delegate: Item {
                        width: themeTestItemGridView.cellWidth
                        height: themeTestItemGridView.cellHeight
                        TPushButton {
                            width: parent.width - 3
                            height: themeView.cellHeight - 3
                            anchors.centerIn: parent
                            label: modelData.name
                            onButtonClicked: {
                                console.log( "index: " + index )
                                viewRoot.sendEvent( modelData.event, modelData.data )
                            }
                        }
                    }
                }

                Loader {
                    id: themeKeypadButtons
                    anchors.top: themeTestItemGridView.bottom
                    anchors.topMargin: 5
                    sourceComponent: keypadComp
                    width: parent.width
                    height: childrenRect.height
                }


                ExclusiveGroup { id: themeButtonGroup }
                Item {
                    id: themeCategory
                    anchors.top: themeKeypadButtons.bottom
                    anchors.topMargin: 5
                    width: parent.width
                    height: 50

                    property color gridItemColorN: 'blue'
                    property color gridItemColorP: 'lightblue'
                    property color gridItemTextColorN: 'white'
                    property color gridItemTextColorP: 'white'

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 10

                        Component.onCompleted: {
                            cavartarButton.checked = 1
                        }

                        TPushButton {
                            id: cavartarButton
                            label: "Cavatar"
                            width: 120
                            height: 50
                            normalBackgroundColor: 'blue'
                            pressedBackgroundColor: 'lightblue'
                            labelNormalColor: 'white'
                            labelPressedColor: 'black'
                            exclusiveGroup: themeButtonGroup
                            onButtonClicked: {
                                themeCategory.gridItemColorN =  'blue'
                                themeCategory.gridItemColorP = 'lightblue'
                                themeCategory.gridItemTextColorN = 'white'
                                themeCategory.gridItemTextColorP = 'white'
                                themeItemGridView.model = themeView.themeCavatarItems
                            }
                        }
                        TPushButton {
                            id: routeLineTicknessButton
                            label: "Route Line"
                            width: 120
                            height: 50
                            normalBackgroundColor: 'green'
                            pressedBackgroundColor: 'lightgreen'
                            labelNormalColor: 'white'
                            labelPressedColor: 'black'
                            exclusiveGroup: themeButtonGroup
                            onButtonClicked: {
                                themeCategory.gridItemColorN =  'green'
                                themeCategory.gridItemColorP = 'lightgreen'
                                themeCategory.gridItemTextColorN = 'white'
                                themeCategory.gridItemTextColorP = 'white'
                                themeItemGridView.model = themeView.themeRouteLineItems
                            }
                        }
                        TPushButton {
                            id: routeLineTbtThickessButton
                            label: "Route Line TBT"
                            width: 120
                            height: 50
                            normalBackgroundColor: 'white'
                            pressedBackgroundColor: 'lightgray'
                            labelNormalColor: 'black'
                            labelPressedColor: 'white'
                            exclusiveGroup: themeButtonGroup
                            onButtonClicked: {
                                themeCategory.gridItemColorN =  'white'
                                themeCategory.gridItemColorP = 'lightgray'
                                themeCategory.gridItemTextColorN = 'black'
                                themeCategory.gridItemTextColorP = 'white'
                                themeItemGridView.model = themeView.themeRouteLineTbtItems
                            }
                        }
                    }
                }

                GridView {
                    id: themeItemGridView
                    anchors.top: themeCategory.bottom
                    anchors.topMargin: 5
                    width: parent.width
                    height: themeView.cellHeight * themeView.testItemRows
                    model: themeView.themeCavatarItems
                    cellWidth: width / themeView.testItemCols
                    cellHeight: height / themeView.testItemRows
                    clip: true
                    delegate: Item {
                        width: themeTestItemGridView.cellWidth
                        height: themeTestItemGridView.cellHeight
                        TPushButton {
                            width: parent.width - 3
                            height: themeView.cellHeight - 3
                            anchors.centerIn: parent
                            label: modelData.name
                            normalBackgroundColor: themeCategory.gridItemColorN
                            pressedBackgroundColor: themeCategory.gridItemColorP
                            labelNormalColor: themeCategory.gridItemTextColorN
                            labelPressedColor: themeCategory.gridItemTextColorP

                            onButtonClicked: {
                                console.debug( "index: " + index + " text: " + themeKeypadButtons.item.keypadTextInputText)
                                viewRoot.sendEvent( modelData.event, themeKeypadButtons.item.keypadTextInputText )
                            }
                        }
                    }
                }
            }
        }

        TPushButton {
            id: closeButton
            label: "Close"
            width: 120
            height: 60
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            onButtonClicked: {
                close()
            }
        }

        // Keypad
        Component {
            id: keypadComp
            Item {
                id: compRoot
                width: parent.width
                height: childrenRect.height
                property alias keypadTextInputText: keypadTextInput.text

                Rectangle {
                    id: keypadTextInputWrap
                    width: 120
                    height: 60
                    color: 'transparent'
                    border.width: 1
                    border.color: 'white'
                    anchors.horizontalCenter: parent.horizontalCenter
                    TextInput {
                        id: keypadTextInput
                        width: parent.width
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        color: 'white'
                        focus: true
                        clip: true
                        onTextChanged: {
                        }
                    }
                }

                Rectangle {
                    id: backspaceButton
                    anchors.left: keypadTextInputWrap.right
                    anchors.leftMargin: 5
                    anchors.top: keypadTextInputWrap.top
                    width: themeView.keypadButtonWidth
                    height: themeView.keypadButtonHeight
                    color: 'white'
                    Text {
                        anchors.centerIn: parent
                        text: '←'
                        color: 'black'
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            keypadTextInput.text = keypadTextInput.text.substring(0, keypadTextInput.text.length - 1)
                        }
                    }
                }

                Column {
                    id: keypadButtons
                    anchors.top: backspaceButton.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: themeView.keypadButtonWidth * 5 + 5 * 5
                    height: themeView.keypadButtonHeight *2 + 5 * 2
                    spacing: 5
                    Repeater {
                        model: 2
                        Row {
                            spacing: 5
                            property int rowIndex: index
                            Repeater {
                                model: 5
                                Rectangle {
                                    width: themeView.keypadButtonWidth
                                    height: themeView.keypadButtonHeight
                                    color: 'white'
                                    property int value: rowIndex * 5 + modelData
                                    Text {
                                        anchors.centerIn: parent
                                        text: value
                                        color: 'black'
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            //console.log("TestConstants.KeypadClicked owner: " + root.owner + " event: " + TestConstants.KeypadClicked)
                                            //root.owner.sendEvent(TestConstants.KeypadClicked, modelData.value)
                                            keypadTextInput.text = keypadTextInput.text + value
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: dotButton
                    anchors.left: keypadButtons.right
                    anchors.leftMargin: 0
                    anchors.top: keypadButtons.top
                    width: themeView.keypadButtonWidth
                    height: themeView.keypadButtonHeight
                    color: 'white'
                    Text {
                        anchors.centerIn: parent
                        text: '.'
                        color: 'black'
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            keypadTextInput.text = keypadTextInput.text + '.'
                        }
                    }
                }
            }
        }
    }
}

/*
    // Arbitrary
    Row {
        id: arbitrary
        anchors.top: gridView.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 30
        Repeater {
            model: 4
            TPushButton {
                width: 30
                height: 30
                label: qsTr("%1").arg(index)
                onButtonClicked: {
                    var data  = {
                        test: index
                    }
                    viewRoot.sendEvent( TestViewEvent.Test, data )
                }
            }
        }
    }

                    {name:'한국어', event:TestViewEvent.Korean, data:{}},
                    {name:'영어', event:TestViewEvent.English, data:{}},
                //        {name:'Clear Fav', event:TestViewEvent.ClearFavorites, data:{}},
                //        {name:'Add Fav In Max', event:TestViewEvent.AddFavoritesInMaximum, data:{}},
                //        {name:'DBUSEncrypt', event:TestViewEvent.DbusEncrypt, data:{}},
                //        {name:'DBUSEncryptMt', event:TestViewEvent.DbusEncryptMt, data:{}},
                //        {name:'QAESEncrypt', event:TestViewEvent.QaesEncrypt, data:{}},
                //        {name:'QAESEncryptMt', event:TestViewEvent.QaesEncryptMt, data:{}},
                //        {name:'Gadget Icon Fuel', event:TestViewEvent.GadgetIconFuel, data:{}},
                //        {name:'Gadget Icon Parking', event:TestViewEvent.GadgetIconParking, data:{}},
                //        {name:'Gadget SaveImage On', event:TestViewEvent.GadgetSaveImage, data:1},
                //        {name:'Gadget SaveImage Off', event:TestViewEvent.GadgetSaveImage, data:0},
                //        {name:'Screen Capture', event:TestViewEvent.ScreenCapture, data:{}},
                //        {name:'Address VR', event:TestViewEvent.AddressSearchVr, data:{}},
                //        {name:'Address VR 2', event:TestViewEvent.AddressSearchVr2, data:{}},
                        {name:'FileGPS SetPos', event:TestViewEvent.SetFileGpsPos, data:{}},
                //        {name:'ShowProgressIndicator', event:TestViewEvent.ShowProgressIndicator, data:{}},
                        {name:'RecordTouch', event:TestViewEvent.RecordTouch, type: 'toggle', state: false, data:{}},
                        {name:'SimPinchIn', event:TestViewEvent.SimPinchIn, data:{}},
                        {name:'SkyStateDawn', event:TestViewEvent.SkyStateDawn, data:{}},
                        {name:'SkyStateDusk', event:TestViewEvent.SkyStateDusk, data:{}},
                    {name:'SkyStateDawn', event:TestViewEvent.SkyStateDawn, data:{}},
                    {name:'SkyStateDusk', event:TestViewEvent.SkyStateDusk, data:{}},
*/

