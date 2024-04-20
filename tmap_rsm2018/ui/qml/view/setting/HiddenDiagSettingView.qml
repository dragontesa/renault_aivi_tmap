import QtQuick 2.0
import HiddenDiagSettingViewEvent 1.0
import CommonConstants 1.0
import SettingConstants 1.0
import ZoneConstants 1.0
import LayoutConstants 1.0
import HiddenSettingConstants 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../component/internal/Keyboard.js" as CustomKeyboard

PopupView {
    id: viewRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    property ListView listView              : null
    property real listViewContentY          : 0.0

    //삭제할 (파일,디렉토리)로그
    readonly property string screenshotPath : SettingConstants.ScreenshotPath
    readonly property string applogPath     : SettingConstants.ApplogPath
    readonly property string gpsLogPath     : SettingConstants.GPSlogPath
    readonly property string databasePath   : SettingConstants.DatabasePath
    readonly property string sessionDbName  : databasePath + "/session.db"

    readonly property var kMenus            : {
        0: {
            menu: qsTr("TNAPP 서버 설정"),
            items: [
                {
                    type:"text",
                    id:HiddenSettingConstants.K_TnappServerIp,
                    key: HiddenSettingConstants.TnappServerIp,
                    values:[qsTr("IP: ")]
                },
                {
                    type:"text",
                    id:HiddenSettingConstants.K_TnappServerPort,
                    key: HiddenSettingConstants.TnappServerPort,
                    values:[qsTr("포트: ")]
                },
            ],
        },
        1: {
            menu: qsTr("NDDS 서버 설정"),
            items: [
                {
                    type: "radio", id:HiddenSettingConstants.K_NddsServer,
                    key: HiddenSettingConstants.NddsServer,
                    values: [ qsTr("상용"), qsTr("준용"), qsTr("개발")]
                },
            ],
        },
        2: {
            menu: qsTr("avNext 서버 설정"),
            items: [
                {
                    type: "radio", id:HiddenSettingConstants.K_AvNextServer,
                    key:HiddenSettingConstants.AvNextServer,
                    values: [ qsTr("PRD"), qsTr("STG"), qsTr("OFF")  ]}
            ],
        },
        3: {
            menu: qsTr("Network Log 출력"),
            items: [
                {
                    type:"radio", id:HiddenSettingConstants.K_NetworkLog,
                    key: HiddenSettingConstants.NetworkLog,
                    values: [ qsTr("OFF"), qsTr("ON") ]
                }
            ]
        },
        4: {
            menu: qsTr("T map Log 출력"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_TmapLog,
                    key:HiddenSettingConstants.TmapLog,
                    values: [qsTr("OFF"), qsTr("ON")]
                }
            ]
        },
        5: {
            menu: qsTr("안전운전도우미 서버설정"),
            items: [
                {
                    type:"text", id:HiddenSettingConstants.K_SafeHelperServerIp,
                    key: HiddenSettingConstants.SafeHelperServerIp,
                    values: [qsTr("IP:")]
                },
                {
                    type:"text",
                    id:HiddenSettingConstants.K_SafeHelperServerPort,
                    key: HiddenSettingConstants.SafeHelperServerPort,
                    values: [qsTr("포트:")]
                }
            ]
        },
        6: {
            menu: qsTr("GPS 모드"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_GpsMode,
                    key: HiddenSettingConstants.GpsMode,
                    values: [ qsTr("System GPS"), qsTr("FILE GPS")]
                }
            ]
        },
        7: {
            menu: qsTr("GPS Log 저장"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_GpsLog,
                    key: HiddenSettingConstants.GpsLog,
                    values: [ qsTr("OFF"), qsTr("ON")]
                }
            ]
        },
        8: {
            menu: qsTr("정주기재탐색 시간"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_FixedRerouteTime,
                    key:HiddenSettingConstants.FixedRerouteTime,
                    values: [ qsTr("서버"), qsTr("10분"), qsTr("5분"), qsTr("1분"), qsTr("0.5분")]
                }
            ]
        },
        9: {
            menu: qsTr("맵 매칭 정보 (GPS궤적)"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_MapMatching,
                    key:HiddenSettingConstants.MapMatching,
                    values: [ qsTr("OFF"), qsTr("ON")]
                }
            ]
        },
        10: {
             menu: qsTr("화면캡쳐"),
             items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_CaptureScreenshot,
                    key:HiddenSettingConstants.UseScreenCapture,
                    values: [ qsTr("OFF"), qsTr("ON") ]
                }
            ]
        },
        11: {
            menu: qsTr("TVAS 버전"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_TvasVersion,
                    key:HiddenSettingConstants.TvasVersion,
                    values: [ "5.0", "5.1", "5.2", "5.3", "5.4"]
                }
            ]
        },
        12: {
            menu: qsTr("VMS 테스트"),
            items: [
                {
                    type:"text",
                    id:HiddenSettingConstants.K_VsmTest,
                    key:HiddenSettingConstants.VmsTest,
                    values: [ qsTr("VMS ID:") ]
                }
            ]
        },
        13: {
            menu: qsTr("Moneky Test"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_MonkeyTest,
                    key:HiddenSettingConstants.EnableMonkeyTest,
                    values: [ qsTr("OFF"), qsTr("ON") ]
                }
            ]
        },
        14: {
            menu: qsTr("Theme"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_MultisenseTheme,
                    key:HiddenSettingConstants.Theme,
                    values: [ "Echo", "Comfort", "Regular", "Sport"],
                    colors: [0x75e04b, 0x0091ff, 0xadb7c9, 0xe82e15]
                }
            ]
        }, // 75e04b, 0091ff, adb7c9, e82e15
        15: {
            menu: qsTr("APP 강제종료"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_ForceCrash,
                    key:HiddenSettingConstants.ForceCrash,
                    values: [ qsTr("종료하기") ],
                    command: "sendEvent(HiddenDiagSettingViewEvent.ForceCrashApp,{})"
                }
            ]
        },
        16: {
            menu: qsTr("언어변경"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_ChangeLanguage,
                    key:HiddenSettingConstants.LanguageTest,
                    values: [ qsTr("한국어"), qsTr("영어") ]
                }
            ]
        },
        17: {
            menu: qsTr("Map In Cluster Centeral 윈도우 표시"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_DisplayCenteralMapInClusterInWindow,
                    key:HiddenSettingConstants.CenteralMapInClusterTest,
                    values: [ qsTr("표시") ]
                }
            ]
        },
        18: {
            menu: qsTr("Map In Cluster Side 윈도우 표시"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_DisplaySideMapInClusterInWindow,
                    key:HiddenSettingConstants.SideMapInClusterTest,
                    values: [ qsTr("표시") ]
                }
            ]
        },
        19: {
            menu: qsTr("Map In Cluster HUD 윈도우 표시"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_DisplayHudMapInClusterInWindow,
                    key:HiddenSettingConstants.HudMapInClusterTest,
                    values: [ qsTr("표시") ]
                }
            ]
        },
        20: {
            menu: qsTr("로그파일"),
            items: [
                {
                    type:"text",
                    id:HiddenSettingConstants.K_FileName,
                    key: 0,
                    values:[CommonConstants.Empty]
                },
                {
                    type:"button",
                    id:HiddenSettingConstants.K_DeleteFile,
                    key: 1,
                    values: [ qsTr("삭제") ]
                }
            ]
        },
        21: {
            menu: qsTr("Gadget shortcut icon"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_DisplayGadgetShortcutIcon,
                    key:HiddenSettingConstants.GadgetShortcutIconTest,
                    values: [ qsTr("표시") ]
                }
            ]
        },
        22: {
            menu: qsTr("SDI 로그 출력"),
            items: [
                {
                    type:"button",
                    id:HiddenSettingConstants.K_SdiLog,
                    key:HiddenSettingConstants.SdiLog,
                    values: [ qsTr("표시") ]
                }
            ]
        },
     }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            sendEvent(HiddenDiagSettingViewEvent.HideKeyboard)
            break
        default:
            break
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        sendEvent(HiddenDiagSettingViewEvent.HideKeyboard)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        sendEvent(HiddenDiagSettingViewEvent.ShowKeyboard, {
              owner: objectName,
              x: keypad.x,
              y: keypad.y,
              z: keypad.z+1,
              width: keypad.width,
              height: keypad.height,
              searchBtnAlwaysEnable: true
          })
    }

    property var layout: QtObject {
        property int rowHeight: 90
        property int menuLbHeight: 50
        property int menuKeyLbHeight: 39
        property int rowSplitHeight: 1
        function updateLayout(resolution) {
            switch (resolution) {
                case LayoutConstants.L820x1024:
                rowHeight = 115
                menuLbHeight = 63 - 6
                menuKeyLbHeight = 51 - 6
                break
                case LayoutConstants.L1920x720:
                break
                default:
                rowHeight = 90
                menuLbHeight = 50 - 6
                menuKeyLbHeight = 39 - 6
            }
        }
    }

    function responseSettings( settings ){
        var menuNum = CommonUtils.getObjectLen(kMenus)
        for (var i=0;i<menuNum;i++) {
            var keyvalue = {}
            for (var j=0;j<kMenus[i].items.length;j++) {
                var id   = kMenus[i].items[j].id
                var key  = kMenus[i].items[j].key
                keyvalue["id"+j] = id
                if (!CommonUtils.isUndefined(typeof settings[key]) &&
                    !CommonUtils.isNull(settings[key])) {
                    var value = settings[key][CommonConstants.Value];
                    keyvalue[key] = value
                }
                else {
                    if (CommonUtils.isEqual(id , HiddenSettingConstants.K_MultisenseTheme))
                        keyvalue[key] = -1
                    else
                        keyvalue[key] = CommonConstants.Empty
                }
            }

            var column = 1
            if (!CommonUtils.isUndefined(typeof kMenus[i].column))
                column = kMenus[i].column

            listModel.set(i,
            {
                menu: kMenus[i].menu,
                items: kMenus[i].items,
                values: keyvalue,
                column: column
            })

        }
        listView.model = null
        listView.model = listModel
    }

    //파일/디렉토리 이름및 크기 수신
    function responseFile(name,size) {
        var findIdx = CommonUtils.lastIndexOf(name, "/")
        var fileName = CommonUtils.substring(name, findIdx+1, name.length)
        if (CommonUtils.isEqual(fileName, "session.db"))
            return
        var fileInfoRegion = kMenus[getIndexSettingById(HiddenSettingConstants.K_FileName)].items[0].values[0]
        var index = getIndexSettingById(HiddenSettingConstants.K_FileName)
        kMenus[index].items[0].values[0] +=  fileName +"("+ size + ")<br>"
        setKeySettingByIndex(index, 0, kMenus[index].items[0].values[0])
        //[임시] 텍스트가 바뀌지 않고 있어서....
        main.sourceComponent = undefined
        main.sourceComponent = compMenuView
        listView.contentY = listViewContentY
    }

    //메뉴의 id를 사용하여 메뉴의 인덱스를 얻는다.
    function getIndexSettingById(menuId) {
        var menuIndex = -1
        var menuNum = CommonUtils.getObjectLen(kMenus)
        for (var i=0;i<menuNum;i++) {
            var keyvalue = {}
            for (var j=0;j<kMenus[i].items.length;j++) {
                if (CommonUtils.isEqual(menuId , kMenus[i].items[j].id)) {
                    menuIndex = i
                    break
                }
            }
            if (menuIndex >= 0)
                break
        }
        return  menuIndex
    }

    function setKeySettingById( id, key, value) {
        var menuIndex = getIndexSettingById(id)
        setKeySettingByIndex(menuIndex, key, value)
    }

    function setKeySettingByIndex(menuIndex, key, value) {
        var keyvalue = {}
        keyvalue = listModel.get(menuIndex).values
        keyvalue[key] = value
        listModel.setProperty(menuIndex, "values", keyvalue)
    }

    function applyKeySetting( id, key, value) {
        var data = {}
        data[CommonConstants.Index]    = id
        data[CommonConstants.Section] = HiddenSettingConstants.HiddenSetting
        if (CommonUtils.isEqual(id, HiddenSettingConstants.K_MapMatching)) {
           data[CommonConstants.Section] = HiddenSettingConstants.HiddenVsmSetting
        }

        data[CommonConstants.Key]     = key
        data[CommonConstants.Value]   = value
        sendEvent(HiddenDiagSettingViewEvent.ApplyKeySetting, data)
    }

    onMediated: {
        sendEvent(HiddenDiagSettingViewEvent.RequestHiddenSetting, {section:HiddenSettingConstants.HiddenSetting})
        sendEvent(HiddenDiagSettingViewEvent.GetFileSize, [gpsLogPath, screenshotPath, applogPath])
    }

    ListModel { id: listModel }

    Loader {
        id: main
        sourceComponent: compMenuView
    }

    Component {
        id: menuDele
        Rectangle {
            id: menuItem
            width: viewRoot.width
            height: layout.menuLbHeight + layout.menuKeyLbHeight * modelColumn + layout.rowSplitHeight
            border.width: 0
            border.color: "green"
            color: "transparent"
            property var valueSet: model.values
            property int menuIndex: index
            property int modelColumn: model.column
            Column {
                width: parent.width
                height: parent.height
                Text {
                    id: menuLabel
                    x: 10
                    width: parent.width - x
                    height: layout.menuLbHeight
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize }
                    color: GlobalConstants.kListSubTextColor
                    verticalAlignment: Text.AlignVCenter
                    text: model.menu
                }
                Column {
                    width: parent.width
                    Repeater {
                        model: modelColumn
                        Item {
                            id: keyItemBody
                            x: 10
                            width: parent.width - x
                            height: layout.menuKeyLbHeight
                            property int columnIndex: index
                            Row {
                                    height:keyItemBody.height
                                    Item {
                                        width: {
                                            var m = columnIndex ? modelColumn : 1
                                            return (180 * m)
                                        }
                                        height:keyItemBody.height
                                        Text {
                                            id: currentValue
                                            width:parent.width
                                            height:parent.height
                                            font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                                            color: GlobalConstants.kListSubTextColor
                                        }
                                    }
                                    Repeater {
                                        id: keyItemList
                                        model: kMenus[menuIndex].items.length / modelColumn
                                        Item {
                                            id: keyItem
                                            width: {
                                                var m = columnIndex ? modelColumn : 1
                                                return (keyItemBody.width / (kMenus[menuIndex].items.length/modelColumn)) - (180 * m)
                                            }
                                            height: keyItemBody.height
                                            visible: {
                                                currentValue.height = keyItemBody.height
                                                currentValue.visible = true
                                                keyItemBody.height = layout.menuKeyLbHeight
                                                menuItem.height = layout.menuLbHeight + (layout.menuKeyLbHeight * modelColumn) + layout.rowSplitHeight
                                                return true
                                            }

                                            readonly property int keyIndex: index + columnIndex
                                            readonly property var  keyName: {
                                                if (!CommonUtils.isUndefined(typeof kMenus[menuIndex])  &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items)  &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex]) &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex].key)) {
                                                    return kMenus[menuIndex].items[keyIndex].key
                                                }
                                                return undefined
                                            }
                                            readonly property int valueLength: {
                                                if (!CommonUtils.isUndefined(typeof kMenus[menuIndex])  &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items)  &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex]) &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex].values) &&
                                                    !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex].values.length)) {
                                                    return kMenus[menuIndex].items[keyIndex].values.length
                                                }
                                                return 0
                                            }

                                            Component.onCompleted: {
                                                if (CommonUtils.isNull(keyName)) {
                                                    return
                                                }
                                                if ( CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_TnappServerIp)) ||
                                                     CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_SafeHelperServerIp)) ) {
                                                    var v = menuItem.valueSet[keyName]
                                                    if ( CommonUtils.isEmpty(v) ||
                                                         CommonUtils.isNull(v) ||
                                                         CommonUtils.isEqual(v , 0)) {
                                                        currentValue.text = qsTr("상용")
                                                    } else {
                                                        currentValue.text = qsTr("개발")
                                                    }
                                                }
                                                else {
                                                    if (CommonUtils.isNull(kMenus[menuIndex].items[keyIndex].values[valueSet[keyName]%valueLength]))
                                                        console.log("^^ menu:"+menuIndex+"\n, key name : " + keyName+"\n, index="+keyIndex+"\n, valuelen="+valueLength)
                                                    if (!CommonUtils.isNull(valueSet[keyName]) &&
                                                         CommonUtils.isAlpaNum(valueSet[keyName])) {
                                                        var txt = kMenus[menuIndex].items[keyIndex].values[CommonUtils.number(valueSet[keyName])%valueLength]
                                                        if (!CommonUtils.isNull(txt))
                                                            currentValue.text = txt
                                                    }
                                                }
                                            }

                                            // value list
                                            Item {
                                                id: valueItems
                                                width: parent.width
                                                height: parent.height
                                                Row {
                                                    spacing: 1
                                                    Repeater {
                                                        model: valueLength
                                                        Item {
                                                            width: valueItems.width / valueLength
                                                            height: valueItems.height
                                                            property int valueIndex: index
                                                            Row {
                                                                spacing: 1
                                                                Text {
                                                                    id: valueLb
                                                                    width: contentWidth
                                                                    height: valueItems.height
                                                                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                                                                    color: GlobalConstants.kListSubTextColor
                                                                    verticalAlignment: Text.AlignVCenter
                                                                    visible: !CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type , "button")
                                                                    text: kMenus[menuIndex].items[keyIndex].values[valueIndex]
                                                                    clip: false
                                                                    onContentHeightChanged:{
                                                                      if (height <= 0)
                                                                        return
                                                                      if (contentHeight <= 0)
                                                                        return
                                                                      if (contentHeight < height) {
                                                                        y = 0
                                                                        return
                                                                      }
                                                                      height = contentHeight
                                                                      y = -(layout.menuKeyLbHeight)
                                                                    }
                                                                }

                                                                //파일 이름과 삭제 버튼의 갭을 유지하기 위함
                                                                Item {width: (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_FileName)) ||
                                                                                   CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DeleteFile))
                                                                                   ) ? 80 : 0; height:parent.height}

                                                                // Radio value items
                                                                Item {
                                                                    width: 32
                                                                    height: 32
                                                                    visible: false
                                                                    property bool checked: CommonUtils.isEqual(valueSet[keyName], valueIndex)
                                                                    Image {
                                                                        id: radioImg
                                                                        anchors.fill: parent
                                                                        source: (parent.checked)?Resource.imageUrl("radio_on.png"):Resource.imageUrl("radio_off.png")
                                                                    }
                                                                    MouseArea {
                                                                        anchors.fill: parent
                                                                        onClicked: {
                                                                            setKeySettingByIndex(menuIndex, keyName, valueIndex)
                                                                            applyKeySetting( menuItem.valueSet["id"+keyIndex], keyName, valueIndex) // apply directly that setting if only on selection keys
                                                                            sendEvent(HiddenDiagSettingViewEvent.HideKeyboard)
                                                                            if (!CommonUtils.isEqual(keypad.curEditor , null) )
                                                                                 keypad.curEditor.focus = false
                                                                            keypad.curEditor = null
                                                                            if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_NddsServer))) {
                                                                                sendEvent(HiddenDiagSettingViewEvent.DeleteFile,  [sessionDbName] )
                                                                            }
                                                                        }
                                                                    }
                                                                    Component.onCompleted: {
                                                                        if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type , "radio")) {
                                                                         visible = true
                                                                         if (!CommonUtils.isNull(kMenus[menuIndex].items[keyIndex].colors) &&
                                                                            kMenus[menuIndex].items[keyIndex].colors.length > valueIndex) {
                                                                            var hex = kMenus[menuIndex].items[keyIndex].colors[valueIndex]
                                                                             var r = hex >> 16 & 255
                                                                             var g = hex >> 8  & 255
                                                                             var b = hex & 255
                                                                            valueLb.color = Qt.rgba(r/255,g/255,b/255,1)
                                                                         }
                                                                        }
                                                                    }
                                                                }
                                                                // Input value items
                                                                Item {
                                                                    width: 100
                                                                    height: valueItems.height - 4
                                                                    visible: false
                                                                    Rectangle {
                                                                        color: "#1f1f1f"
                                                                        anchors.fill: parent
                                                                        TextInput {
                                                                            id: inputEdit
                                                                            anchors.fill:parent
                                                                            color: "#bbbbbb"
                                                                            verticalAlignment: Text.AlignVCenter
                                                                            clip:true
                                                                            text: (!CommonUtils.isNull(menuItem.valueSet[keyName])) ? menuItem.valueSet[keyName] : ""
                                                                            onActiveFocusChanged: {
                                                                                if (!activeFocus) {
                                                                                    setKeySettingByIndex(menuIndex, keyName, text)
                                                                                    if ( CommonUtils.isEqual(keypad.curEditor , null) ) { //donot hide keypad if it is visible for other input editor
                                                                                        sendEvent(HiddenDiagSettingViewEvent.HideKeyboard)
                                                                                    }
                                                                                }
                                                                                else {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.RegistKeyboardInput, {keyboardInput: inputEdit})
                                                                                     sendEvent(HiddenDiagSettingViewEvent.ShowKeyboard, {
                                                                                       owner: objectName,
                                                                                       x: keypad.x,
                                                                                       y: keypad.y,
                                                                                       z: keypad.z+1,
                                                                                       width: keypad.width,
                                                                                       height: keypad.height,
                                                                                       searchBtnAlwaysEnable: true
                                                                                       })
                                                                                    keypad.curEditor = inputEdit
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                    Component.onCompleted: {
                                                                        if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type , "text") &&
                                                                            !CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_FileName))) {
                                                                            visible = true
                                                                        }
                                                                    }
                                                                }

                                                                // Button value item
                                                                Item {
                                                                    width: 120
                                                                    height:valueItems.height - 4
                                                                    visible: false
                                                                    Rectangle {
                                                                        color: "#212121"
                                                                        anchors.fill:parent
                                                                        Text {
                                                                            anchors.centerIn: parent
                                                                            font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                                                                            color: "#dfdfdf"
                                                                            text: kMenus[menuIndex].items[keyIndex].values[valueIndex]
                                                                        }
                                                                        MouseArea {
                                                                            anchors.fill: parent
                                                                            onClicked: {
                                                                                if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_ForceCrash))) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.ForceCrashApp,{})
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DisplayCenteralMapInClusterInWindow)) ) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.DisplayCenteralMapInClusterInWindow,{})
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DisplaySideMapInClusterInWindow)) ) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.DisplaySideMapInClusterInWindow,{})
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DisplayHudMapInClusterInWindow)) ) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.DisplayHudMapInClusterInWindow,{})
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DeleteFile)) ) {
                                                                                    kMenus[getIndexSettingById(HiddenSettingConstants.K_FileName)].items[0].values[0] = ""
                                                                                    sendEvent(HiddenDiagSettingViewEvent.DeleteFile,  [gpsLogPath, screenshotPath, applogPath] )
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_DisplayGadgetShortcutIcon)) ) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.DisplayGadgetShortcutIcon,{})
                                                                                }
                                                                                else if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_SdiLog)) ) {
                                                                                    sendEvent(HiddenDiagSettingViewEvent.OutputSdiLog,{})
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                    Component.onCompleted: {
                                                                        if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type , "button"))
                                                                            visible = true
                                                                    }
                                                                }

                                                                // Picker Button Value Items
                                                                Item {
                                                                    width: 50
                                                                    height: valueItems.height - 4
                                                                    visible: false
                                                                    Loader {
                                                                        id: pickerLoader
                                                                        onLoaded: {
                                                                            item.height = layout.menuKeyLbHeight - 6
                                                                            item.label = kMenus[menuIndex].items[keyIndex].values[valueIndex]
                                                                            var v =  parseInt(menuItem.valueSet[keyName])
                                                                            item.value = v

                                                                            if (kMenus[menuIndex].items[keyIndex].labels.length > valueIndex)
                                                                               item.unitLabel = kMenus[menuIndex].items[keyIndex].labels[valueIndex]
                                                                        }
                                                                    }

                                                                    Connections {
                                                                        target: pickerLoader.item
                                                                        onValueChangedByUi: {
                                                                            setKeySettingByIndex(menuIndex, keyName, pickerLoader.item.value)
                                                                        }
                                                                    }
                                                                    Component.onCompleted: {
                                                                        if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type , "pickerButton")) {
                                                                           pickerLoader.sourceComponent = Qt.createComponent("../../component/TPickerButton.qml")
                                                                           visible = true
                                                                        }
                                                                    }
                                                                }//Item
                                                            } // value row
                                                        } // value item
                                                    }//Repeater
                                                }//Row
                                            } // value items
                                        } // key item
                                    }//Repeater
                                }//Row
                            }//Item
                    }//Repeater
                }//Column
                Rectangle {
                   id: splitBar
                   x: GlobalConstants.kListLineLeftMargin
                   width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
                   height: layout.rowSplitHeight
                   color: GlobalConstants.kListLineColor
               }
            }//Column
        }//Item
       }

     Component {
        id: compMenuView
        Rectangle {
            width: viewRoot.width
            height: viewRoot.height
            color: GlobalConstants.kListNormalColor
            ListView {
                anchors.fill: parent
                model: listModel
                delegate: menuDele
                cacheBuffer: layout.rowHeight * 2
                Component.onCompleted: viewRoot.listView = this

                TScrollBar {
                    flickable: parent
                    width: 2
                    color: MultiSenseCoordinator.kAmbientColor
                    grooveColor: GlobalConstants.kScrollBarBackColor
                    rightMargin: GlobalConstants.kScrollBarRightMargin
                }

                onContentYChanged: {
                    listViewContentY = contentY
                }
            }
        }
    }

     Item {
         id: keypad
         property TextInput curEditor: null
         anchors.bottom: parent.bottom
         width: parent.width
         height: 419
         visible: false
     }
}


