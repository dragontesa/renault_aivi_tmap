import QtQuick 2.0
import HiddenBoschSettingViewEvent 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import LayoutConstants 1.0
import HiddenSettingConstants 1.0
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: viewRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    property ListView listView      : null
    property var daynightTimeSet    : [] // yyyy-M-dd hh:mm:ss:zz
    readonly property var kMenus    : {
        0: {
             menu: "Mapmatching Path",
             items: [
                     {
                        type: "radio",
                        id:HiddenSettingConstants.K_MapMatching,
                        key:HiddenSettingConstants.MapMatching,
                        values: [ "OFF", "ON"],
                        current: 0
                     }
                 ]
            },
        1: {
            menu: "Mapmatching Feedback(MMFB)",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MapmatchingFeedback,
                        key: HiddenSettingConstants.MapmatchingFeedback,
                        values: [ "OFF", "ON" ],
                        current: 0
                    }
                ]
        },
        2: {
            menu: "Mapmatching Debug Text",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MapmatchingDebugText,
                        key: HiddenSettingConstants.MapmatchingDebugText,
                        values: [ "OFF", "ON" ],
                        current: 0
                    }
            ]
        },
        3: {
            menu: "MMFB Only for Tunnel & Underpass",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_OnlyTunnelMMFB,
                        key: HiddenSettingConstants.OnlyTunnelMMFB,
                        values: [ "OFF", "ON" ],
                        current: 0
                    }
            ]
        },
        4: {
            menu: "MMFB Log ",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MMFBLog,
                        key: HiddenSettingConstants.MMFBLog,
                        values: [ "OFF", "ON" ],
                        current: 0
                    }
            ]
        },
        5: {
            menu: "DR Only Mode",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MMFeedbackDrOnly,
                        key: HiddenSettingConstants.MmfbDrOnly,
                        values: [ "OFF", "ON" ],
                        current: 0
                    }
            ]
        },
        6: {
            menu: "Mapmatching Coordinates",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MMFeedbackGpsMethod,
                        key: HiddenSettingConstants.MmfbGpsMethod,
                        values: [ "Socket(GPS)", "DBus(GNSS)", "DBus(DR)" ],
                        current: 0
                    }
            ]
        },
        7: {
            menu: "Mapmatching Angle on Tunnel",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MMFeedbackAngleOnTunnel,
                        key: HiddenSettingConstants.MmfbAngleOnTunnel,
                        values: [ qsTr("도로선형"), "DR" ],
                        current: 0
                    }
            ]
        },
        8: {
            menu: "DR CEP",
            items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MMFeedbackDrCEP,
                        key: HiddenSettingConstants.MmfbDrCep,
                        values: [ "OFF", "ON" ],
                        current: 0
                    },
                    {
                        type:"pickerButton",
                        id:HiddenSettingConstants.K_MMFeedbackDrCEPrange,
                        key: HiddenSettingConstants.MmfbDrCepRange,
                        values: [ "" ],
                        current: 0
                    }
            ]
        },
        9: {
            menu: "CEP 맵매칭 거리(m)",
            items: [
                    {
                        type:"pickerButton",
                        id:HiddenSettingConstants.K_MMFeedbackDrCEPMMrange,
                        key: HiddenSettingConstants.MmfbDrCepMMRange,
                        values: [ "" ],
                        current: 0
                    }
            ]
        },
        10: {
             menu: "화면캡쳐",
             items: [
                     {
                        type:"radio",
                        id:HiddenSettingConstants.K_CaptureScreenshot,
                        key:HiddenSettingConstants.UseScreenCapture,
                        values: [ "OFF", "ON" ],
                        current:0
                    }
            ]
        },
        11: {
            menu: "Export LOGs to USB",
            items: [
                    {
                        type:"button",
                        id:HiddenSettingConstants.K_ExportLogsToUSB,
                        key:HiddenSettingConstants.ExportGpslogToUSB,
                        values: [ "" ],
                        current:0,
                        labels: "Export"
                    }
            ]
        },
        12: {
            menu: "GPS LOG to KML",
            items: [
                    {
                        type:"button",
                        id:HiddenSettingConstants.K_GpsLog2KML,
                        key:HiddenSettingConstants.ExportGpslogToUSB,
                        values: [ "" ],
                        current:0,
                        labels: "Convert"
                    }
            ]
        },
        13: {
            menu: "Map In Cluster Start/Stop",
            items: [
                    {
                        type:"button",
                        id:HiddenSettingConstants.K_MapInClusterStart,
                        key:HiddenSettingConstants.MClusterStart,
                        values: [ "" ],
                        current:0,
                        labels: "Start"
                    },
                    {
                        type:"button",
                        id:HiddenSettingConstants.K_MapInClusterStop,
                        key:HiddenSettingConstants.MClusterStop,
                        values: [ "" ],
                        current:0,
                        labels: "Stop"
                    }
            ]
        },
        14: {
             menu: "Map In Cluster Layout",
             items: [
                    {
                        type:"radio",
                        id:HiddenSettingConstants.K_MapInClusterLayout,
                        key: HiddenSettingConstants.MClusterLayout,
                        values: [ "Layout1", "Layout2", "Layout3" ],
                        current: 0
                    }
            ]
        },
        15: {
             menu: "Force to online",
             items: [
                    {
                        type:"button",
                        id:HiddenSettingConstants.K_ForceToOnlineState,
                        key: "ForceToOnlineState",
                        values: [ "" ],
                        current:0,
                        labels: "Force to Online"
                    },
            ]
        }
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
                    menuLbHeight = 63
                    menuKeyLbHeight = 51
                    break
                case LayoutConstants.L1920x720:
                    break
                default:
                    rowHeight = 90
                    menuLbHeight = 50
                    menuKeyLbHeight = 39
                    break
            }
        }
    }

    function initMenus()  {
        var menuNum = CommonUtils.getObjectLen(kMenus)
        for (var i=0;i<menuNum;i++) {
            var valueset = {}
            for (var j=0;j<kMenus[i].items.length;j++) {
                var id = kMenus[i].items[j].id
                var key = kMenus[i].items[j].key
                var current = kMenus[i].items[j].current
                valueset["id"] = id
                valueset[key] = current
            }
            // add into model
            listModel.set(i,
            {
                menu: kMenus[i].menu,
                items: kMenus[i].items,
                values: valueset
            })
        }
        listView.model = null
        listView.model = listModel
    }

    function responseSettings( settings )
    {
        for (var k in settings) {
            var m = settings[k]
            var section = m[CommonConstants.Section]
            var key = m[CommonConstants.Key]
            var val = m[CommonConstants.Value]
            // find menu own the key
            var i=0,j=0
            var found = false
            var menuNum = CommonUtils.getObjectLen(kMenus)
            for (i=0;i<menuNum;i++) {
                for (j=0;j<kMenus[i].items.length;j++) {
                    if (CommonUtils.isEqual(key, kMenus[i].items[j].key)) {
                        found = true
                        break
                    }
                }
                if (found)
                    break
            }

            var valueset = {}
            if (found) {
                valueset = listModel.get(i).values
                valueset["id"] = kMenus[i].items[j].id
                valueset[key]  = val
                if (CommonUtils.isEmpty(val)) {
                    switch(kMenus[i].items[j].type) {
                        case "radio":
                        case "button":
                        case "pickerButton":    valueset[key] = 0;          break;
                        case "text":            valueset[key] = "";         break;
                        default:                valueset[key] = undefined;  break;
                    }
                }
                listModel.setProperty( i,
                    "values",
                     valueset
                )
            }
        }

        listView.model = null
        listView.model = listModel
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
        data[CommonConstants.Index]   = id
        data[CommonConstants.Section] = HiddenSettingConstants.HiddenSetting
        data[CommonConstants.Key]     = key
        data[CommonConstants.Value]   = value
        sendEvent(HiddenBoschSettingViewEvent.ApplyKeySetting, data)
        if (CommonUtils.isEqual(id, HiddenSettingConstants.K_MapmatchingFeedback) && CommonUtils.isEqual(value, 0))
            setKeySettingById( HiddenSettingConstants.K_OnlyTunnelMMFB, HiddenSettingConstants.OnlyTunnelMMFB, 0)
        if (CommonUtils.isEqual(id, HiddenSettingConstants.K_OnlyTunnelMMFB) && CommonUtils.isEqual(value, 1)) {
            setKeySettingById( HiddenSettingConstants.K_MapmatchingFeedback, HiddenSettingConstants.MapmatchingFeedback, 1)
        }
        if (CommonUtils.isEqual(id, HiddenSettingConstants.K_MMFeedbackDrOnly) ) {
            setKeySettingById( HiddenSettingConstants.K_MMFeedbackGpsMethod, HiddenSettingConstants.MmfbGpsMethod, (value) ? 2 : 1)
            setKeySettingById( HiddenSettingConstants.K_MMFeedbackAngleOnTunnel, HiddenSettingConstants.MmfbAngleOnTunnel, 1*value)
        }
        if (CommonUtils.isEqual(id, HiddenSettingConstants.K_MMFeedbackGpsMethod) ) {
            setKeySettingById( HiddenSettingConstants.K_MMFeedbackDrOnly, HiddenSettingConstants.MmfbDrOnly, CommonUtils.isEqual(value, 2))
        }
    }

    onActivated: {
        sendEvent(HiddenBoschSettingViewEvent.ResetCsw)
        sendEvent(HiddenBoschSettingViewEvent.Activated, {})
    }

    onMediated: {
        initMenus()
        sendEvent(HiddenBoschSettingViewEvent.RequestHiddenSetting, {section:HiddenSettingConstants.HiddenSetting})
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        sendEvent(HiddenBoschSettingViewEvent.HideKeyboard)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        sendEvent(HiddenBoschSettingViewEvent.ShowKeyboard, {
              owner: objectName,
              x: keypad.x,
              y: keypad.y,
              z: keypad.z+1,
              width: keypad.width,
              height: keypad.height,
              searchBtnAlwaysEnable: true
          })
    }


    onDaynightTimeSetChanged: {
        if (daynightTimeSet.length > 3)   {
            daynightTime.sunrise   = daynightTimeSet[0]
            daynightTime.sunset    = daynightTimeSet[1]
            daynightTime.civstart  = daynightTimeSet[2]
            daynightTime.civend    = daynightTimeSet[3]
        }
    }

    ListModel {	id: listModel }

    Loader {
        sourceComponent: compMenuView
    }

    Component {
        id: menuDele
        Item {
            id: menuItem
            width: viewRoot.width
            height: layout.rowHeight
            property var valueSet: model.values
            property int menuIndex: index

            Text {
                id: menuLabel
                width: parent.width - 10
                height: layout.menuLbHeight-6
                anchors.left: parent.left
                anchors.leftMargin: 10
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize }
                color: GlobalConstants.kListSubTextColor
                verticalAlignment: Text.AlignVCenter
                text: model.menu
            }
            Item {
                id: keyItemBody
                width: parent.width - 10
                height: layout.menuKeyLbHeight - 6
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top : menuLabel.bottom
                anchors.topMargin: 0
                readonly property int keyItemWidth: parent.width / kMenus[index].items.length - 180  // substract currentValue label' width

                Row {
                    Text {
                        id: currentValue
                        width: 180
                        height:keyItemBody.height
                        font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                        color: GlobalConstants.kListSubTextColor
                        text: ""

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                keyItemList.visible ^= true
                                if (keyItemList.visible) {
                                        //keyItem.model = model.items
                                    }
                                }
                            }
                        }

                        Repeater {
                            id: keyItemList
                            model: kMenus[index].items.length
                            Item {
                                id: keyItem
                                width: keyItemBody.keyItemWidth
                                height: keyItemBody.height
                                readonly property int keyIndex: index
                                readonly property var  keyName: {
                                    if (!CommonUtils.isUndefined(typeof kMenus[menuIndex])  &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items)  &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index]) &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index].key)) {
                                        return kMenus[menuIndex].items[index].key
                                    }
                                    return undefined
                                }
                                readonly property int valueLength: {
                                    if (!CommonUtils.isUndefined(typeof kMenus[menuIndex])  &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items)  &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index]) &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index].values) &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index].values.length)) {
                                        return kMenus[menuIndex].items[index].values.length
                                    }
                                    return 0
                                }

                                Component.onCompleted: {
                                    if (CommonUtils.isNull(keyName)) {
                                        return
                                    }
                                    if (CommonUtils.isEqual(menuIndex ,getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEP)) ||
                                        CommonUtils.isEqual(menuIndex ,getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPrange))) {
                                        var optionLength = kMenus[menuIndex].items[0].values.length
                                        currentValue.text = kMenus[menuIndex].items[0].values[menuItem.valueSet[HiddenSettingConstants.MmfbDrCep]%optionLength]
                                    } else {
                                        if (CommonUtils.isNull(kMenus[menuIndex].items[index].values[valueSet[keyName]%valueLength]))
                                            console.log("^^ menu:"+menuIndex+"\n, key name : " + keyName+"\n, item:"+index+"\n, value:"+menuItem.valueSet[keyName])
                                        if (!CommonUtils.isNull(valueSet[keyName]))
                                           currentValue.text = kMenus[menuIndex].items[index].values[menuItem.valueSet[keyName]%valueLength]
                                    }
                                }

                                // value list
                                Item {
                                    id: valueItems
                                    width: parent.width
                                    height: parent.height
                                    Row {
                                        spacing: 30
                                        Repeater {
                                            model: valueLength
                                            Item {
                                                width: valueItems.width / valueLength
                                                height: valueItems.height
                                                Row {
                                                    spacing: 5
                                                    Text {
                                                        width: content.width
                                                        height: 22
                                                        font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                                                        color: GlobalConstants.kListSubTextColor
                                                        verticalAlignment: Text.AlignVCenter
                                                        text: kMenus[menuIndex].items[keyIndex].values[index]
                                                    }

                                                    // Radio value items
                                                    Item {
                                                        id: radioItem
                                                        width: 32
                                                        height: 32
                                                        visible: false
                                                        opacity: enabled ? 1.0 : 0.4
                                                        property bool checked: ( CommonUtils.isEqual(valueSet[keyName], index))
                                                        Image {
                                                            id: radioImg
                                                            anchors.fill: parent
                                                            source: (parent.checked)?Resource.imageUrl("radio_on.png"):Resource.imageUrl("radio_off.png")
                                                        }
                                                        MouseArea {
                                                            anchors.fill: parent
                                                            onClicked: {
                                                                setKeySettingByIndex(menuIndex, keyName, index)
                                                                applyKeySetting( kMenus[menuIndex].items[keyIndex].id, keyName, index) // apply directly that setting if only on selection keys
                                                                sendEvent(HiddenBoschSettingViewEvent.HideKeyboard)
                                                                if (!CommonUtils.isNull(keypad.curEditor))
                                                                     keypad.curEditor.focus = false
                                                                keypad.curEditor = null
                                                            }
                                                        }

                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "radio")) {
                                                                radioItem.visible = true
                                                                if (CommonUtils.isEqual(menuIndex , getIndexSettingById(HiddenSettingConstants.K_MMFeedbackGpsMethod))) {
                                                                    //Socket(GPS)
                                                                    if (CommonUtils.isEqual(index, 0)) {
                                                                        radioItem.enabled = false
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    // Input value items
                                                    Item {
                                                        id: inputItem
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
                                                                text: ""
                                                                onActiveFocusChanged: {
                                                                    if (!activeFocus) {
                                                                        setKeySettingByIndex(menuIndex, keyName, text)
                                                                        if ( CommonUtils.isNull(keypad.curEditor)) { //donot hide keypad if it is visible for other input editor
                                                                            sendEvent(HiddenBoschSettingViewEvent.HideKeyboard)
                                                                        }
                                                                    } else {
                                                                        sendEvent(HiddenBoschSettingViewEvent.RegistKeyboardInput, {"keyboardInput": inputEdit})
                                                                         sendEvent(HiddenBoschSettingViewEvent.ShowKeyboard, {
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
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type,  "text")) {
                                                                inputItem.visible = true
                                                                inputEdit.text = (CommonUtils.isUndefined(typeof menuItem.valueSet[keyName])) ? "" : menuItem.valueSet[keyName]
                                                            }
                                                        }
                                                    }

                                                    // Button value item
                                                    Item {
                                                        width: 120
                                                        height:valueItems.height - 4
                                                        visible: false
                                                        Rectangle {
                                                            id:pushbtn
                                                            color: "#212121"
                                                            anchors.fill:parent
                                                            Text {
                                                                id:btnlabel
                                                                anchors.centerIn: parent
                                                                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                                                                color: "#dfdfdf"
                                                                text: ""
                                                            }
                                                            MouseArea {
                                                                anchors.fill: parent
                                                                onClicked: {
                                                                    applyKeySetting( kMenus[menuIndex].items[keyIndex].id, keyName, 1)
                                                                }
                                                                onPressed: {
                                                                    pushbtn.color = "#4285ff"
                                                                    btnlabel.color = "white"
                                                                }
                                                                onReleased: {
                                                                    pushbtn.color = "#212121"
                                                                    btnlabel.color = "#dfdfdf"
                                                                }
                                                            }
                                                        }
                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "button")) {
                                                                visible = true
                                                                btnlabel.text = kMenus[menuIndex].items[keyIndex].labels
                                                            }
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
                                                                item.label = kMenus[menuIndex].items[keyIndex].values[index]
                                                                var v = parseFloat(menuItem.valueSet[keyName])
                                                                if (CommonUtils.isEqual(menuIndex ,
                                                                    getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPrange)) ||
                                                                    CommonUtils.isEqual(menuIndex ,
                                                                    getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPMMrange))
                                                                    ) {
                                                                    item.precision = 0
                                                                    v = CommonUtils.number(v)
                                                                }
                                                                item.value = v

                                                                if (!CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex].labels)) {
                                                                    if (kMenus[menuIndex].items[keyIndex].labels.length > index)
                                                                       item.unitLabel = kMenus[menuIndex].items[keyIndex].labels[index]
                                                                }
                                                            }
                                                        }

                                                        Connections {
                                                            target: pickerLoader.item
                                                            onValueChangedByUi: {
                                                                var pickerLoaderItemValue = pickerLoader.item.value
                                                                if (CommonUtils.isEqual(menuIndex ,
                                                                    getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPrange)) ||
                                                                    CommonUtils.isEqual(menuIndex ,
                                                                    getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPMMrange))
                                                                    ) {
                                                                    if (pickerLoader.item.value <= 1)
                                                                        pickerLoader.item.value = 1
                                                                    else if (pickerLoader.item.value >= 100 )
                                                                        pickerLoader.item.value = 100
                                                                    pickerLoaderItemValue = CommonUtils.number(pickerLoader.item.value)
                                                                }
                                                                setKeySettingByIndex(menuIndex, keyName, pickerLoaderItemValue)
                                                                applyKeySetting( kMenus[menuIndex].items[keyIndex].id, keyName, pickerLoaderItemValue) // apply directly that setting if only on selection keys
                                                            }
                                                        }
                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "pickerButton")) {
                                                               pickerLoader.sourceComponent = Qt.createComponent("../../component/TPickerButton.qml")
                                                               if (CommonUtils.isEqual(menuIndex ,
                                                                   getIndexSettingById(HiddenSettingConstants.K_MMFeedbackDrCEPrange))) {
                                                                   pickerLoader.item.step = 1
                                                                   pickerLoader.item.precision = 0
                                                                   visible = Qt.binding(function(){ return CommonUtils.number(menuItem.valueSet[HiddenSettingConstants.MmfbDrCep])})
                                                               } else {
                                                                visible = true
                                                               }
                                                            }
                                                        }
                                                    }
                                                } // value row
                                            } // value item
                                        }
                                    }
                                } // value items
                            } // key item


                            onModelChanged: {
                                if (model.count > 0)
                                itemWidth = width / model.count
                            }
                        }
                    }
                }

                Rectangle {
                   id: splitBar
                   x: GlobalConstants.kListLineLeftMargin
                   width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
                   height: layout.rowSplitHeight
                   color: GlobalConstants.kListLineColor
               }
           }
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
                Component.onCompleted: viewRoot.listView = this
            }
        }
    }

    Item {
        id: daynightTime
        width: parent.width
        height: 67
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        property string sunrise:  "0000-00-00 00:00:00:000"
        property string sunset:   "0000-00-00 00:00:00:000"
        property string civstart: "0000-00-00 00:00:00:000"
        property string civend:   "0000-00-00 00:00:00:000"

        Column {
            width: parent.width
            height: parent.height
            spacing: 1
            Item {
                width: parent.width
                height: 16
                Text {
                    id: lv3
                    width: 80
                    height: 16
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: GlobalConstants.kListSubTextColor
                    verticalAlignment: Text.AlignVCenter
                    text: "CIV START"
                }
                Text {
                    id: civStartTime
                    width: parent.width
                    height: 16
                    anchors.left:lv3.right
                    anchors.leftMargin: 0
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: "darkgreen"
                    verticalAlignment: Text.AlignVCenter
                    text: daynightTime.civstart
                }
            }


            Item {
                width: parent.width
                height: 16
                Text {
                    id: lv1
                    width: 80
                    height: 16
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: GlobalConstants.kListSubTextColor
                    verticalAlignment: Text.AlignVCenter
                    text: "SUNRISE"
                }
                Text {
                    id: sunriseTime
                    width: 250
                    height: 16
                    anchors.left:lv1.right
                    anchors.leftMargin: 0
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: "darkgreen"
                    verticalAlignment: Text.AlignVCenter
                    text: daynightTime.sunrise
                }
            }

            Item {
                width: parent.width
                height: 16
                Text {
                    id: lv2
                    width: 80
                    height: 16
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: GlobalConstants.kListSubTextColor
                    verticalAlignment: Text.AlignVCenter
                    text: "SUNSET"
                }
                Text {
                    id: sunsetTime
                    width: parent.width
                    height: 16
                    anchors.left:lv2.right
                    anchors.leftMargin: 0
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: "darkgreen"
                    verticalAlignment: Text.AlignVCenter
                    text: daynightTime.sunset
                }
            }

            Item {
                width: parent.width
                height: 16
                Text {
                    id: lv4
                    width: 80
                    height: 16
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: GlobalConstants.kListSubTextColor
                    verticalAlignment: Text.AlignVCenter
                    text: "CIV END"
                }
                Text {
                    id: civEndTime
                    width: parent.width
                    height: 16
                    anchors.left:lv4.right
                    anchors.leftMargin: 0
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                    color: "darkgreen"
                    verticalAlignment: Text.AlignVCenter
                    text: daynightTime.civend
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
