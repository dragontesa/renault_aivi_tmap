import QtQuick 2.1
import QtQuick.Window 2.1
import SelfTestViewEvent 1.0
import CommonConstants 1.0
import SearchConstants 1.0
import ViewRegistry 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/setting"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../component/internal/Keyboard.js" as CustomKeyboard

SettingView {
    id: settingRoot

    background.visible                          : false
    fader.fadingOpacity                         : 0
    mouseArea.enabled                           : false

    property var timer                          : null
    property int recordEnumCount                : 0
    property int maxScenarioCount               : 10
    property int keyboardHeight                 : 419
    property int zone1EditRegionX               : (130+20)
    property int zone1EditRegionY               : middleLoader? (middleLoader.height+1) : 0
    property int zone1EditRegionW               : zone1EditRegion? (zone1EditRegion.width - 40) : 0
    property int zone1EditRegionH               : zone1EditRegion? (zone1EditRegion.height + 1) : 0
    property string normal                      : CommonConstants.Gray
    property string press                       : MultiSenseCoordinator.kAmbientColor

    property bool clickedDownBtn                : true

    //ID
    property var leftGroup                      : null
    property var leftLoader                     : null
    property var leftRandomButton               : null
    property var leftScenarioButton             : null

    property var rightGroup                     : null
    property var rightLoader                    : null
    property var rightRunButton                 : null
    property var rightStopButton                : null
    property var rightExitButton                : null

    property var listView                       : null
    property var middleGroup                    : null
    property var middleLoader                   : null
    property var middleClearSaveButton          : null
    property var middleRecordEditButton         : null
    property var middleInputFieldRepeatPerSec   : null
    property var middleInputFieldDurationMin    : null
    property var middleInputFieldTitle          : null

    property var zone1EditRegion                : null
    property var topLoader                      : null
    property var bottomTouchRange               : null


    //Enum
    property var textEnum                       : ({})
    property var recordEnum                     : ({})

    property var textArr                        : ([])

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Random",          i++, [CommonConstants.Name], ["Random"])
        CommonUtils.insertEnum(textEnum, "Scenario",        i++, [CommonConstants.Name], ["Scenario"])
        CommonUtils.insertEnum(textEnum, "Etc",             i++, [CommonConstants.Name], ["기타"])
        CommonUtils.insertEnum(textEnum, "Run",             i++, [CommonConstants.Name], ["Run"])
        CommonUtils.insertEnum(textEnum, "Stop",            i++, [CommonConstants.Name], ["Stop"])
        CommonUtils.insertEnum(textEnum, "Exit",            i++, [CommonConstants.Name], ["Exit"])
        CommonUtils.insertEnum(textEnum, "Record",          i++, [CommonConstants.Name], ["Record"])
        CommonUtils.insertEnum(textEnum, "Clear",           i++, [CommonConstants.Name], ["Clear"])
        CommonUtils.insertEnum(textEnum, "Save",            i++, [CommonConstants.Name], ["Save"])
        CommonUtils.insertEnum(textEnum, "Edit",            i++, [CommonConstants.Name], ["Edit"])
        CommonUtils.insertEnum(textEnum, "None",            i++, [CommonConstants.Name], ["없음"])
        CommonUtils.insertEnum(textEnum, "InputText",       i++, [CommonConstants.Name], ["시나리오 명을 넣어주세요."])
        CommonUtils.insertEnum(textEnum, "RepeatsPerSecond",i++, [CommonConstants.Name], ["RepeatsPerSecond:"])
        CommonUtils.insertEnum(textEnum, "DurationMinutes" ,i++, [CommonConstants.Name], ["Duration Minutes:"])
        CommonUtils.insertEnum(textEnum, "Comma",           i++, [CommonConstants.Name], [","])
        CommonUtils.insertEnum(textEnum, "Down",            i++, [CommonConstants.Name], ["V"])
        CommonUtils.insertEnum(textEnum, "Up",              i++, [CommonConstants.Name], ["^"])


        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateRecordEnum() {
        if (CommonUtils.getObjectLen(recordEnum) <= 0) {
            recordEnum = CommonUtils.createEnum(recordEnum)
        }
        var i = 0
        CommonUtils.insertEnum(recordEnum, "I", i++)
        CommonUtils.insertEnum(recordEnum, "X", i++)
        CommonUtils.insertEnum(recordEnum, "Y", i++)

        recordEnumCount = CommonUtils.getObjectLen(recordEnum)
    }

   function randomClick() {
        var min = zone1EditRegionY
        var max = zone1EditRegionY + zone1EditRegionH + bottomTouchRange.height
        var y = Math.floor(Math.random()*(max-min+1)) + min
        if(zone1EditRegionY <= y && y <= zone1EditRegionY+zone1EditRegionH) {
            min = zone1EditRegionX
            max = zone1EditRegionX+zone1EditRegionW
        } else {
            min = bottomTouchRange.x
            max = bottomTouchRange.width
        }
        var x = Math.floor(Math.random()*(max-min+1)) + min
        var data = ({})
        data[CommonConstants.X] = x
        data[CommonConstants.Y] = y
        sendEvent(SelfTestViewEvent.MouseClick, data )
    }

    function visibleKeyboard(visible, text, type) {
        var data = ({})
        if (visible) {
            keyboard.height = keyboardHeight
            data[CommonConstants.Owner  ] = CommonConstants.objectName(ViewRegistry.SelfTestView)
            data[CommonConstants.X      ] = keyboard.x
            data[CommonConstants.Y      ] = keyboard.y
            data[CommonConstants.Z      ] = keyboard.z + 1
            data[CommonConstants.Width  ] = keyboard.width
            data[CommonConstants.Height ] = keyboard.height
            data[CommonConstants.Pressed] = type
            sendEvent(SelfTestViewEvent.ShowKeyboard, data)
        } else {
            keyboard.height = 0
            if (!CommonUtils.isNull(text))
                data[CommonConstants.Text] = text
            sendEvent(SelfTestViewEvent.HideKeyboard, data)
        }
    }

    function __processKeyboard(key, text) {
        switch(key) {
            case CustomKeyboard.CodeCancel :
            case CustomKeyboard.CodeConfirm:
                visibleKeyboard(false, CommonConstants.Empty)
                break
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        visibleKeyboard(false, CommonConstants.Empty)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        visibleKeyboard(true, CommonConstants.Empty, CustomKeyboard.symbolKeybd)
    }


    function init(){
        allocateTextEnum()
        allocateRecordEnum()

        var data = ({})
        for(var i = 0; i < settingRoot.maxScenarioCount; i++) {
            data[CommonConstants.Text ] = textArr[textEnum.None]
            data[CommonConstants.Value] = textArr[textEnum.None]
            listModel.set(i, data)
            data = ({})
            data[CommonConstants.Key  ] = CommonUtils.string(i)
            sendEvent(SelfTestViewEvent.GetScenario, data)
        }
    }

    onMediated: {
        init()
        loader.sourceComponent = componentMain
    }

    onActivated: sendEvent(SelfTestViewEvent.Activated)

    onDeactivated: sendEvent(SelfTestViewEvent.Deactivated)

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    //main
    Rectangle {
        width: parent.width
        height: parent.height
        color: CommonConstants.Transparent
        Loader { id: loader
             width: parent.width
             height:parent.height - keyboard.height
             onLoaded: {
                 settingRoot.visible = true
                 settingRoot.leftRandomButton.checked = CommonConstants.True
                 settingRoot.leftRandomButton.buttonClicked()
                 settingRoot.rightStopButton.checked = CommonConstants.True
                 settingRoot.rightStopButton.buttonClicked()
             }
        }
        Item {id: keyboard
            width: parent.width
            height: 0
        }
    }

    Component { id: componentMain
        Column {
            width: settingRoot.width
            height: settingRoot.height
            Item { width: parent.width; height: clickedDownBtn ? GlobalConstants.kScreenZone1Height : 0}
            Rectangle {
                width: parent.width
                height: GlobalConstants.kScreenZone1Height
                color: CommonConstants.Transparent
                Loader { Component.onCompleted: settingRoot.topLoader = this
                     width: parent.width
                     height: parent.height
                     sourceComponent: componentTop
                }
            }
            Rectangle { Component.onCompleted: settingRoot.bottomTouchRange = this
                width: parent.width
                height: {
                    var dynamicHeight = clickedDownBtn ? GlobalConstants.kScreenZone1Height : 0
                    return parent.height  - (GlobalConstants.kScreenZone1Height + GlobalConstants.kScreenHardKeyHeight + dynamicHeight  )
                }
                color: CommonConstants.Transparent
            }
        }
    }

    Component { id: componentTop
        Row {
            width: parent.width
            height: parent.height
            Item {width: 30; height: parent.height}
            Item {
                width: 100
                height: parent.height
                Loader { Component.onCompleted: settingRoot.leftLoader = this
                    width: parent.width
                    height: parent.height
                    sourceComponent: componentLeft
                }
            }
            Column {
                width: parent.width - (30+100+128)
                height: parent.height
                Rectangle {
                    width: parent.width
                    height: parent.height/2
                    color: GlobalConstants.kBasicBackgoundColor
                    Loader { Component.onCompleted: settingRoot.middleLoader = this
                         width: parent.width
                         height:parent.height
                         onLoaded: {
                             if (CommonUtils.isEqual(sourceComponent,  componentRecord))
                                 listView.currentIndex = 0
                         }
                    }
                }
                Item { Component.onCompleted: settingRoot.zone1EditRegion = this
                    width: parent.width
                    height: parent.height/2
                }
            }
            Column {
                width: 128
                height: parent.height
                Item { width: parent.width;height: 30}
                Rectangle {
                    width: parent.width
                    height: parent.height - 30
                    color: GlobalConstants.kBasicBackgoundColor
                    Loader { Component.onCompleted: settingRoot.rightLoader = this
                         width: parent.width
                         height:parent.height
                         sourceComponent: componentRight
                    }
                }
            }
        }
    }


    Component { id: componentLeft
        Column {
            width: parent.width
            height: parent.height
            ExclusiveGroup { Component.onCompleted: settingRoot.leftGroup = this}
            TPushButton { Component.onCompleted: settingRoot.leftRandomButton = this
                width: parent.width
                height: parent.height/3
                radius: 5
                enabled: CommonUtils.isEqual(settingRoot.rightGroup.selected , settingRoot.rightStopButton) &&
                         (CommonUtils.isNull(middleInputFieldTitle) || !middleInputFieldTitle.visible)
                exclusiveGroup: leftGroup
                normalBackgroundColor: normal
                pressedBackgroundColor: press
                label: textArr[textEnum.Random]
                onButtonClicked: {
                    settingRoot.rightRunButton.visible = true
                    settingRoot.rightStopButton.visible = true
                    settingRoot.rightExitButton.visible = true
                    middleLoader.sourceComponent = componentRandome
                }
            }
            TPushButton { Component.onCompleted: settingRoot.leftScenarioButton = this
                width: parent.width
                height: parent.height/3
                radius: 5
                enabled: CommonUtils.isEqual(settingRoot.rightGroup.selected , settingRoot.rightStopButton) &&
                         (CommonUtils.isNull(middleInputFieldTitle) || !middleInputFieldTitle.visible)
                exclusiveGroup: leftGroup
                normalBackgroundColor: normal
                pressedBackgroundColor: press
                label: textArr[textEnum.Scenario]
                onButtonClicked: {
                    settingRoot.rightRunButton.visible = true
                    settingRoot.rightStopButton.visible = true
                    settingRoot.rightExitButton.visible = true
                    middleLoader.sourceComponent = componentRecord
                }
            }
        }
    }


    function rightStop() {
        settingRoot.rightGroup.selected = settingRoot.rightStopButton
        rightStopButton.buttonClicked()
    }

    Component { id: componentRight
        Column {
            width: parent.width
            height: parent.height
            clip: true
            ExclusiveGroup { Component.onCompleted: settingRoot.rightGroup = this }
            TPushButton { Component.onCompleted: settingRoot.rightRunButton = this
                width: 80
                height: parent.height/3
                radius: 5
                enabled: !CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) ||
                         (CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) &&
                         (CommonUtils.isNull(middleInputFieldTitle) || !middleInputFieldTitle.visible) &&
                         !CommonUtils.isEqual(settingRoot.rightGroup.selected ,middleRecordEditButton) &&
                         !CommonUtils.isEmpty(getTextCurrentIndex()))

                exclusiveGroup: rightGroup
                normalBackgroundColor: normal
                pressedBackgroundColor: press
                label: textArr[textEnum.Run]
                onButtonClicked:  {
                    forceActiveFocus()
                    settingRoot.clickedDownBtn = false
                    visibleKeyboard(false, CommonConstants.Empty)
                    switch(settingRoot.leftGroup.selected) {
                        case settingRoot.leftRandomButton:
                            var c = 0
                            var r = CommonUtils.number(middleInputFieldDurationMin.text) * 60 * CommonUtils.number(middleInputFieldRepeatPerSec.text)
                            var t = parseInt(1000 / CommonUtils.number(middleInputFieldRepeatPerSec.text))
                            timer = CommonUtils.destroy(timer)
                            timer = CommonUtils.delay(settingRoot, t, function() {
                                try {
                                    randomClick()
                                    if (c++ < r) {
                                        timer.start()
                                    } else {
                                        rightStop()
                                    }
                                } catch(e) {
                                }
                            }, false)
                            break
                        case settingRoot.leftScenarioButton:
                             var data = getValueCurrentIndex()
                             var playRecordArr = deserialize(data)
                             var i = playRecordArr.length
                             if (i > 0) {
                                 timer = CommonUtils.destroy(timer)
                                 timer = CommonUtils.delay(settingRoot, playRecordArr[0][0] , function() {
                                     try {
                                         if ( i >= playRecordArr.length) {
                                             sendEvent(SelfTestViewEvent.CloseAllView)
                                             i = 0
                                             timer.interval = playRecordArr[i][recordEnum.I]
                                             timer.start()
                                             return
                                         }
                                         var data = ({})
                                         data[CommonConstants.X] = playRecordArr[i][recordEnum.X]
                                         data[CommonConstants.Y] = playRecordArr[i][recordEnum.Y]
                                         sendEvent(SelfTestViewEvent.MouseClick, data )

                                         timer.interval = (++i < playRecordArr.length) ? playRecordArr[i][0] : 1000
                                         timer.start()
                                     } catch(e) {
                                     }
                                 }, false)
                             }
                             break
                    }//switch
                }
            }
            Row {
                height: parent.height/3
                    TPushButton { Component.onCompleted: settingRoot.rightStopButton = this
                        width: 80
                        height: parent.height
                        radius: 5
                        enabled: !CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) ||
                                 CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) &&
                                 (CommonUtils.isNull(middleInputFieldTitle) || !middleInputFieldTitle.visible)
                        exclusiveGroup: rightGroup
                        normalBackgroundColor: normal
                        pressedBackgroundColor: press
                        label: textArr[textEnum.Stop]
                        onButtonClicked:  {
                            settingRoot.clickedDownBtn = true
                            switch(settingRoot.leftGroup.selected) {
                                case settingRoot.leftRandomButton:
                                    break
                                case settingRoot.leftScenarioButton:
                                    middleInputFieldTitle.text = textArr[textEnum.InputText]
                                    middleInputFieldTitle.parent.visible = settingRoot.recordArr.length

                                    var isScenario = (settingRoot.recordArr.length ||
                                                      !CommonUtils.isEmpty(getTextCurrentIndex())) ? true : false
                                    middleClearSaveButton.label  = settingRoot.recordArr.length
                                                                 ? textArr[textEnum.Save]
                                                                 : textArr[textEnum.Clear]
                                    middleRecordEditButton.label = isScenario
                                                                 ? textArr[textEnum.Edit]
                                                                 : textArr[textEnum.Record]
                                    break
                            }
                            mousearea.propagateComposedEvents = false
                            mousearea.visible = false
                            timer = CommonUtils.destroy(timer)
                        }
                }
                Item {width: 10;height: parent.height}
                TPushButton { Component.onCompleted: settingRoot.rightStopButton = this
                    width: 30
                    height: parent.height
                    radius: 5
                    normalBackgroundColor: normal
                    pressedBackgroundColor: press
                    label: settingRoot.clickedDownBtn ? textArr[textEnum.Up] :textArr[textEnum.Down]
                    onButtonClicked:  {
                       settingRoot.clickedDownBtn = !settingRoot.clickedDownBtn
                    }
                }
            }
            TPushButton { Component.onCompleted: settingRoot.rightExitButton = this
                width: 80
                height: parent.height/3
                radius: 5
                enabled: !CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) ||
                         CommonUtils.isEqual(settingRoot.leftGroup.selected,settingRoot.leftScenarioButton) &&
                         (CommonUtils.isNull(middleInputFieldTitle) || !middleInputFieldTitle.visible)
                exclusiveGroup: rightGroup
                normalBackgroundColor: normal
                pressedBackgroundColor: press
                label: textArr[textEnum.Exit]
                onButtonClicked:  {
                    if (timer) {
                        rightStop()
                    }
                    visibleKeyboard(false, CommonConstants.Empty)
                    var data = ({})
                    data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SelfTestView)
                    sendEvent(SelfTestViewEvent.CloseView, data)
                }
            }
        }
    }

    //Random
    Component { id: componentRandome
        Column {
            width: middleLoader.width
            height: middleLoader.height
            spacing: 3
            clip: true
            property var focusObjectId: null
            Row {
                width: parent.width
                height: parent.height/2
                spacing: 3
                Item { width: 30; height: parent.height}
                Text {
                    width: 160
                    horizontalAlignment: Text.AlignLeft
                    font.bold: false
                    font.pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize )
                    color: GlobalConstants.kListTextColor
                    text: textArr[textEnum.RepeatsPerSecond]
                }
                Rectangle {
                    width: 120
                    height: parent.height
                    color: GlobalConstants.kListTextColor
                    TextInput { Component.onCompleted: settingRoot.middleInputFieldRepeatPerSec = this
                        width: parent.width
                        height: parent.height
                        verticalAlignment: Qt.AlignVCenter
                        horizontalAlignment: Qt.AlignHCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize )}
                        color: GlobalConstants.kBasicBackgoundColor
                        text: "10"
                        maximumLength: 80
                        focus: true
                        clip: true
                        onActiveFocusChanged: {
                            if ( activeFocus && visible ) {
                                focusObjectId = this
                                sendEvent(SelfTestViewEvent.HideKeyboard)
                                sendEvent(SelfTestViewEvent.RegistKeyboardInput, {keyboardInput: this})
                                visibleKeyboard(false, CommonConstants.Empty)
                                visibleKeyboard(true, CommonConstants.Empty, CustomKeyboard.symbolKeybd)
                            }
                        }
                    }
                }
                Repeater {
                    model: 5
                    TPushButton {
                        width: 30
                        height: 30
                        label: CommonUtils.string(index)
                        onButtonClicked: {
                            if (CommonUtils.isNull(focusObjectId))
                                return
                            focusObjectId.text += label
                        }
                    }
                }
            }
            Row {
                width: parent.width
                height: parent.height/2
                spacing: 3
                Item { width: 30; height: parent.height}
                Text {
                    width: 160
                    horizontalAlignment: Text.AlignLeft
                    font.bold: false
                    font.pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize )
                    color: GlobalConstants.kListTextColor
                    text: textArr[textEnum.DurationMinutes]
                }
                Rectangle {
                    width: 120
                    height: parent.height
                    color: GlobalConstants.kListTextColor
                    TextInput { Component.onCompleted: settingRoot.middleInputFieldDurationMin = this
                        width: parent.width
                        height: parent.height
                        verticalAlignment: Qt.AlignVCenter
                        horizontalAlignment: Qt.AlignHCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize )}
                        color: GlobalConstants.kBasicBackgoundColor
                        text: "180"
                        focus: false
                        clip: true
                        onActiveFocusChanged: {
                            if ( activeFocus && visible ) {
                                focusObjectId = this
                                sendEvent(SelfTestViewEvent.HideKeyboard)
                                sendEvent(SelfTestViewEvent.RegistKeyboardInput, {keyboardInput: this})
                                visibleKeyboard(false, CommonConstants.Empty)
                                visibleKeyboard(true, CommonConstants.Empty, CustomKeyboard.symbolKeybd)
                            }
                        }
                    }
                }
                Repeater {
                    model: 5
                    TPushButton {
                        width: 30
                        height: 30
                        label: CommonUtils.string(5+index)
                        onButtonClicked: {
                            if (CommonUtils.isNull(focusObjectId))
                                return
                            focusObjectId.text += label
                        }
                    }
                }
                TPushButton {
                    width: 30
                    height: 30
                    label: "<-"
                    onButtonClicked: {
                        if (CommonUtils.isNull(focusObjectId))
                            return
                        var  start = focusObjectId.cursorPosition
                        if (start > 0){
                            focusObjectId.select(start - 1, start)
                            focusObjectId.cut()
                        }
                    }
                }
            }//Row
        }//Column
    }


    //Scenario
    function __getScenario(key, value) {
        var v = textArr[textEnum.None]
        if (!CommonUtils.isNull(value)) {
             v = value
        }
        v = CommonUtils.trim(v)
        if (CommonUtils.isEmpty(v)) {
            v = textArr[textEnum.None]
            listModel.setProperty(CommonUtils.number(key), CommonConstants.Text, v)
        } else {
            var index = CommonUtils.lastIndexOf(v, textArr[textEnum.Comma])
            if (index >= 0) {
             var text = CommonUtils.substring(v, index+1)
             text = CommonUtils.trim(text)
             if (!CommonUtils.isEmpty(text)) {
                 listModel.setProperty(CommonUtils.number(key), CommonConstants.Text, text)
             } else {
                 listModel.setProperty(CommonUtils.number(key), CommonConstants.Text, textArr[textEnum.None])
             }
            }
        }
        listModel.setProperty(CommonUtils.number(key), CommonConstants.Value, v)
        if(!CommonUtils.isNull(listView) && !CommonUtils.isNull(listView.currentIndex)) {
            var currIndex = listView.currentIndex
            listView.currentIndex = -1
            listView.model = listModel
            listView.currentIndex = currIndex
        }
    }

    property var  csv : ([])
    property bool wait: false
    function __wait(start, detail) {
        wait = start
        switch(settingRoot.leftGroup.selected) {
             case settingRoot.leftRandomButton:
                 return
             case settingRoot.leftScenarioButton:
                 break
        }

        var index = detail[CommonConstants.Type   ]
        if (CommonUtils.isNull(detail[CommonConstants.Keyword]) ||
            CommonUtils.isEmpty(detail[CommonConstants.Keyword])) {
            detail[CommonConstants.Keyword] =  textArr[textEnum.None]
        }
        detail[SearchConstants.Time] = CommonUtils.getTime()
        if(wait) {
            if (!CommonUtils.isNull(timer)) {
                timer.stop()
            }
            csv[index] = detail
        } else {
            if (!CommonUtils.isNull(csv[index])) {
                var data = csv[index]
                data[SearchConstants.Time] = detail[SearchConstants.Time] - data[SearchConstants.Time]
                var log = CommonConstants.Empty
                    log += "[MonkeyTest]"
                    log += ","
                    log += data[CommonConstants.Name   ]
                    log += ","
                    log += data[CommonConstants.Type   ]
                    log += ","
                    log += data[CommonConstants.Keyword]
                    log += ","
                    log += data[SearchConstants.Time   ]
                QmlAppHelper.log(log)
                csv[index] = null
            }
            if (!CommonUtils.isNull(timer)) {
                timer.start()
            }
        }
    }

    function getTextCurrentIndex() {
        var text = CommonConstants.Empty
        if(CommonUtils.isNull(listView) || CommonUtils.isNull(listView.currentIndex))
            return text
        if (listView.currentIndex < 0)
            return text
        var item  = listModel.get(listView.currentIndex)
        if (CommonUtils.isNull(item))
            return CommonConstants.Empty
        text = item.text
        if (CommonUtils.isNull(text))
            return CommonConstants.Empty
        text = CommonUtils.replace(text, textArr[textEnum.None], CommonConstants.Empty)
        text = CommonUtils.trim(text)
        return text
    }

    function getValueCurrentIndex() {
        var text = CommonConstants.Empty
        if(CommonUtils.isNull(listView) || CommonUtils.isNull(listView.currentIndex))
            return text
        if (listView.currentIndex < 0)
            return text
        var item  = listModel.get(listView.currentIndex)
        if (CommonUtils.isNull(item))
            return CommonConstants.Empty
        text = item.value
        if (CommonUtils.isNull(text))
            return CommonConstants.Empty
        text = CommonUtils.replace(text, textArr[textEnum.None], CommonConstants.Empty)
        text = CommonUtils.trim(text)
        return text
    }

    function serialize(doubleArrData) {
        var data = CommonConstants.Empty
        for(var i= 0; i<doubleArrData.length; i++) {
            for (var j= 0; j<settingRoot.recordEnumCount; j++ ) {
                data += doubleArrData[i][j]
                data += textArr[textEnum.Comma]
            }
        }
        return data
    }

    function deserialize(serialData) {
        var i=0,j=0
        var data = []
        var arrData = CommonUtils.split(serialData, textArr[textEnum.Comma])
        var len = parseInt(arrData.length/settingRoot.recordEnumCount)
        for (i=0; i< len; i++) {
            for (j=0; j<settingRoot.recordEnumCount; j++) {
               if (j<=0) data[i] = []
               data[i][j] = arrData[(i*settingRoot.recordEnumCount)+j]
            }
        }
        return data
    }


    ListModel { id: listNoneModel  }
    ListModel { id: listModel      }
    Component { id: componentRecord
        Row {width:middleLoader.width;height:middleLoader.height
            property var focusObjectId: null
            Item { width: 30; height: parent.height}
            Rectangle {
                width: parent.width - 130
                height: parent.height
                color: GlobalConstants.kBasicBackgoundColor
                Row {
                    width: parent.width
                    height: parent.height
                    spacing: 1
                    ListView {
                        Component.onCompleted: {settingRoot.listView = this}
                        width: parent.width/1.5
                        height: parent.height
                        enabled: !CommonUtils.isEqual(settingRoot.rightGroup.selected, settingRoot.rightRunButton)
                        clip: true
                        currentIndex: -1
                        model: listModel
                        delegate: Item { width: (parent.width)-20; height: 50;
                                    MouseArea {
                                        width: parent.width
                                        height: parent.height
                                        onClicked: listView.currentIndex = index
                                    }
                                    Column {width: parent.width; height: parent.height
                                        Row {width: parent.width; height: parent.height - 1
                                            Item {width:5;height:parent.height}
                                            Item {width:20;height:parent.height
                                                Rectangle {width: parent.width;height:20;radius:20; color:"white";anchors.centerIn: parent
                                                    Text {anchors.centerIn: parent; text:CommonUtils.string(index); color:"black"}
                                                }
                                            }
                                            Item {width:5;height:parent.height}
                                            Item {width:parent.width - 30;height:parent.height
                                                Text {anchors.centerIn: parent; text:model.text; color:"blue"}
                                            }
                                        }
                                        Rectangle{width: parent.width;height:1;color:GlobalConstants.kListLineColor}
                                    }
                        }
                        highlight: Rectangle {
                            property var currentItem : listView.currentItem
                            onCurrentItemChanged: {
                                if (!currentItem) {
                                    x=y=width=height=0
                                    return
                                }
                                x = listView.currentItem.x
                                y = listView.currentItem.y
                                width = listView.currentItem.width
                                height = listView.currentItem.height
                            }
                            color: MultiSenseCoordinator.kAmbientColor
                        }
                        highlightFollowsCurrentItem: false
                        property string currentValue :  CommonConstants.Empty
                        Rectangle {
                            x:30; y:5
                            width: !CommonUtils.isNull(listView.currentItem) ?  listView.currentItem.width - x : 0
                            height: !CommonUtils.isNull(listView.currentItem) ? listView.currentItem.height    : 0
                            visible: false
                            color: "white"
                            TextInput { Component.onCompleted: settingRoot.middleInputFieldTitle = this
                                anchors.fill: parent
                                verticalAlignment: Qt.AlignVCenter
                                horizontalAlignment: Qt.AlignHCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(20)}
                                color: GlobalConstants.kBasicBackgoundColor
                                text: textArr[textEnum.InputText]
                                maximumLength: 80
                                focus: true
                                clip: true
                                onActiveFocusChanged: {
                                    if ( activeFocus && visible ) {
                                        focusObjectId = this
                                        if (CommonUtils.isEqual(text,textArr[textEnum.InputText]))
                                            text = CommonConstants.Empty
                                        sendEvent(SelfTestViewEvent.HideKeyboard)
                                        sendEvent(SelfTestViewEvent.RegistKeyboardInput, {keyboardInput: this})
                                        visibleKeyboard(false, CommonConstants.Empty)
                                        visibleKeyboard(true, CommonConstants.Empty, CustomKeyboard.hangleKeybd)
                                    }
                                }
                            }
                        }

                        TScrollBar {
                            flickable: parent
                            width: 4
                            color: MultiSenseCoordinator.kAmbientColor
                            grooveColor: GlobalConstants.kScrollBarBackColor
                            rightMargin: GlobalConstants.kScrollBarRightMargin
                        }

                        onCurrentIndexChanged: {
                            if (CommonUtils.isEmpty(getTextCurrentIndex())) {
                                middleClearSaveButton.label = textArr[textEnum.Save]
                                middleRecordEditButton.label = textArr[textEnum.Record]
                            }else {
                                middleClearSaveButton.label = textArr[textEnum.Clear]
                                middleRecordEditButton.label = textArr[textEnum.Edit]
                            }
                            rightStop()
                        }
                    }
                    Column {
                        width: parent.width - ((parent.width/1.5) + 40)
                        height: parent.height
                        Row {
                            height: parent.height/2
                            spacing: 2
                            Repeater {
                                model: 3
                                TPushButton {
                                    width: 30
                                    height: 30
                                    property var textArr:["A","B","C"]
                                    label: textArr[index]
                                    onButtonClicked: {
                                        if (CommonUtils.isNull(focusObjectId))
                                            return
                                        focusObjectId.text += label
                                    }
                                }
                            }
                        }
                        Row {
                            height: parent.height/2
                            spacing: 2
                            Repeater {
                                model: 3
                                TPushButton {
                                    width: 30
                                    height: 30
                                    label: CommonUtils.string(index)
                                    onButtonClicked: {
                                        if (CommonUtils.isNull(focusObjectId))
                                            return
                                        focusObjectId.text += label
                                    }
                                }
                            }
                        }
                    }
                    TPushButton {
                        width: 30
                        height: 65
                        label: "<-"
                        onButtonClicked: {
                            if (CommonUtils.isNull(focusObjectId))
                                return
                            var  start = focusObjectId.cursorPosition
                            if (start > 0){
                                focusObjectId.select(start - 1, start)
                                focusObjectId.cut()
                            }
                        }
                    }
                }
            }
            Column { width: 100; height: parent.height
                Item { width: parent.width; height: 2 }
                TPushButton { Component.onCompleted: settingRoot.middleClearSaveButton = this
                    width: 80
                    height: (parent.height-6)/2
                    radius: 5
                    normalBackgroundColor: normal
                    pressedBackgroundColor: press
                    label: textArr[textEnum.Clear]
                    enabled:  !CommonUtils.isEqual(settingRoot.rightGroup.selected, settingRoot.rightRunButton) &&
                              (!CommonUtils.isEmpty(getTextCurrentIndex()) || middleInputFieldTitle.visible)
                    onButtonClicked:  {
                        var inputText = middleInputFieldTitle.text
                        inputText = CommonUtils.replaceAll(inputText, ",", CommonConstants.Empty)
                        inputText = CommonUtils.trim(inputText)
                        if (middleInputFieldTitle.visible &&
                           (CommonUtils.isEmpty(inputText) ||
                            CommonUtils.isEqual(inputText,textArr[textEnum.InputText]))) {
                            return
                        }
                        label = textArr[textEnum.Clear]
                        middleInputFieldTitle.parent.visible = false
                        visibleKeyboard(false, CommonConstants.Empty)

                        var scenario = serialize(settingRoot.recordArr)
                        if (!CommonUtils.isEmpty(scenario)) {
                            scenario += inputText
                        }
                        settingRoot.recordArr = []

                        var data = ({})
                        data[CommonConstants.Key  ] = CommonUtils.string(listView.currentIndex)
                        data[CommonConstants.Value] = scenario
                        sendEvent(SelfTestViewEvent.SaveScenario, data)
                        sendEvent(SelfTestViewEvent.GetScenario, data)

                        var text = getTextCurrentIndex()
                        if (CommonUtils.isEmpty(text)) {
                            middleRecordEditButton.label = textArr[textEnum.Record]
                        }else {
                            middleRecordEditButton.label = textArr[textEnum.Edit]
                        }
                    }
                }
                Item { width: parent.width; height: 2 }
                TPushButton { Component.onCompleted: settingRoot.middleRecordEditButton = this
                    width: 80
                    height: (parent.height-6)/2
                    radius: 5
                    exclusiveGroup: rightGroup
                    normalBackgroundColor: normal
                    pressedBackgroundColor: press
                    label: textArr[textEnum.Record]
                    enabled:  !CommonUtils.isEqual(settingRoot.rightGroup.selected, settingRoot.rightRunButton) &&
                              listView.currentIndex >= 0 && !middleInputFieldTitle.visible
                    onButtonClicked:  {
                        settingRoot.clickedDownBtn = false
                        settingRoot.recordArr = []
                        var value = getValueCurrentIndex()
                        if (value.length > 0) {
                            settingRoot.recordArr = deserialize(value)
                            middleInputFieldTitle.text = getTextCurrentIndex()
                            middleInputFieldTitle.parent.visible = settingRoot.recordArr.length
                            middleClearSaveButton.label = settingRoot.recordArr.length
                                                        ? textArr[textEnum.Save]
                                                        : textArr[textEnum.Clear]
                            rightStop()
                            return
                        }
                        sendEvent(SelfTestViewEvent.CloseAllView)

                        beginTime = CommonUtils.getTime()
                        mousearea.propagateComposedEvents = true
                        mousearea.visible = true
                    }
                }
                Item { width: parent.width; height: 2 }
            }
        }
    }


    //기타
    Component { id: componentNone
        Item {width:middleLoader.width;height:middleLoader.height}
    }


    //터치 레코딩
    property var  recordArr: ([])
    property real beginTime: 0
    property real interval: 0
     MouseArea {
        id: mousearea
        anchors.fill: parent
        propagateComposedEvents: false
        onPressed: receiveEvent(mouse, "pressed")
        function receiveEvent(event, eventType) {
            event.accepted = false
            var pos = Qt.point(event.x, event.y)
            if(event.y <= bottomTouchRange.y) {
                if(event.y <= zone1EditRegionY)
                    return
                if(zone1EditRegionX >= event.x || event.x >= zone1EditRegionX+zone1EditRegionW)
                    return
            }
            if (beginTime <= 0)
                beginTime = CommonUtils.getTime()
            interval = (CommonUtils.getTime() - beginTime)
            beginTime = CommonUtils.getTime()

            var i = settingRoot.recordArr.length
            settingRoot.recordArr[i] = ([])
            settingRoot.recordArr[i][recordEnum.I] = interval
            settingRoot.recordArr[i][recordEnum.X] = pos.x
            settingRoot.recordArr[i][recordEnum.Y] = pos.y
        }
     }
}
