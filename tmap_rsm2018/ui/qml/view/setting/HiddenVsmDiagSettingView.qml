import QtQuick 2.0
import HiddenVsmDiagSettingViewEvent 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import LayoutConstants 1.0
import HiddenSettingConstants 1.0
import CommonConstants 1.0

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
    property ListView listView: null

    readonly property var kMenus: {
        0: {
            menu: "Level Setting UI",
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_VsmLevelSettingUi,
                    key: HiddenSettingConstants.VsmLevelSettingUi,
                    values:[ "OFF", "ON"]
                },
            ]
        },
        1: {
            menu: "Debug Setting UI",
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_VsmDebugSettingUi,
                    key: HiddenSettingConstants.VsmDebugSettingUi,
                    values:[ "OFF", "ON"]
                },
            ]
        },
        2: {
            menu: qsTr("VSM 서버 (restart)"),
            items: [
                {
                    type: "radio",
                    id:HiddenSettingConstants.K_VsmServer,
                    key: HiddenSettingConstants.VsmServer,
                    values: [ qsTr("준상용"), qsTr("상용"), qsTr("개발") ]
                }
            ]
        },
        3: {
            menu: qsTr("속도반응형 Map"),
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_VsmAutoLevelMap,
                    key: HiddenSettingConstants.VsmAutoLevelMap,
                    values: [ qsTr("기본"), qsTr("고급") ]
                }
            ]
        },
        4: {
            menu: "App Config ID (restart)",
            items: [
                {
                    type:"text",
                    id:HiddenSettingConstants.K_VsmConfigId,
                    key: HiddenSettingConstants.VsmConfigId,
                    values: [ "" ]
                }
            ]
        },
        5: {
            menu: "App Config VER (restart)",
            items: [
                {
                    type:"text",
                    id:HiddenSettingConstants.K_VsmConfigVer,
                    key: HiddenSettingConstants.VsmConfigVer,
                    values: [ "" ]
                }
            ]
        },
        6: {
            menu: "Use FPS (restart)",
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_UseFps,
                    key: HiddenSettingConstants.VsmUseFps,
                    values: [ "OFF", "ON" ]
                }
            ]
        },
        7: {
            menu: "Disable 3D Objects (Skybox, Building pattern .. ",
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_Disable3dObjects,
                    key: HiddenSettingConstants.VsmDisable3dObjects,
                    values: [ "OFF", "ON" ]
                },
            ]
        },
        8: {
            menu: "GL Anti Aliasing (0 ~ 4) (restart)",
            items: [
                {
                    type:"pickerButton",
                    id:HiddenSettingConstants.K_GLAntiAliasing,
                    key: HiddenSettingConstants.VsmGLAntiAliasing,
                    values: [ "LEVEL" ]
                },
            ]
        },
        9: {
            menu: "Flick Speed Test (restart)",
            items: [
                {
                    type:"radio",
                    id:HiddenSettingConstants.K_FlickSpeedTest,
                    key: HiddenSettingConstants.VsmFlickSpeedTest,
                    values: [ "OFF", "ON" ]
                },
            ]
        },
    }

    function __processKeyboard(key, text) {
        switch(key) {
            case CustomKeyboard.CodeCancel:
                sendEvent(HiddenVsmDiagSettingViewEvent.HideKeyboard)
                keypad.curEditor.focus = false
                break
            default:
                keypad.curEditor.text = text
                break
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        sendEvent(HiddenVsmDiagSettingViewEvent.HideKeyboard)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        sendEvent(HiddenVsmDiagSettingViewEvent.ShowKeyboard, {
                owner: objectName,
                x: keypad.x,
                y: keypad.y,
                z: keypad.z+1,
                width: keypad.width,
                height: keypad.height,
                searchBtnAlwaysEnable: true,
                dontUseEditBox: true
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

    function responseSettings( settings ){
        var menuNum = CommonUtils.getObjectLen(kMenus)
        for (var i=0;i<menuNum;i++) {
            console.log("menu "+i+", key cnt:"+kMenus[i].items.length)
            var keyvalue = {}
            for (var j=0;j<kMenus[i].items.length;j++) {
                var id   = kMenus[i].items[j].id
                var key  = kMenus[i].items[j].key
                var val  =  CommonUtils.isUndefined(typeof settings[key]) ? undefined : settings[key][CommonConstants.Value]
                keyvalue["id"] = id
                keyvalue[key] = val
                if (CommonUtils.isEmpty(val)) {
                    switch(kMenus[i].items[j].type) {
                       case "radio":
                       case "check":
                       case "pickerButton": keyvalue[key] = 0;          break;
                       case "text":         keyvalue[key] = "";         break;
                       default:             keyvalue[key] = undefined;  break;
                    }
                }
            }

            listModel.set(i,
            {
                menu: kMenus[i].menu,
                items: kMenus[i].items,
                values: keyvalue
            })
        }
        listView.model = null
        listView.model = listModel
    }

    function setKeySetting( menuIndex, key, value) {
        var keyvalue = {}
        keyvalue = listModel.get(menuIndex).values
        keyvalue[key] = value
        listModel.setProperty(menuIndex, "values", keyvalue)
    }

    function applyKeySetting( id, key, value) {
        var data = {}
        data[CommonConstants.Index]    = id
        data[CommonConstants.Section] = HiddenSettingConstants.HiddenVsmSetting
        data[CommonConstants.Key]     = key
        data[CommonConstants.Value]   = value
        sendEvent(HiddenVsmDiagSettingViewEvent.ApplyKeySetting, data)
    }

    function setDriveAutoMapConfig(config)
    {
        driveMapLevelConfigGroup.setCustomConfig(config)
    }


    onMediated: {
        // createModel("",13)
        /** demo sample data
        var testDataset = {}
        testDataset[HiddenSettingConstants.TnappServerIp] = { section:"", key: HiddenSettingConstants.TnappServerIp, value:"" }
        testDataset[HiddenSettingConstants.TnappServerPort] = { section:"", key:HiddenSettingConstants.TnappServerPort, value: 0 }
        testDataset[HiddenSettingConstants.NddsServer] =  { sectioN:"", key:HiddenSettingConstants.NddsServer, value:1 } // default semi comercial
        testDataset[HiddenSettingConstants.NetworkLog] = { section:"", key:HiddenSettingConstants.NetworkLog, value:0} // default OFF
        testDataset[HiddenSettingConstants.SafeHelperServerIp] = { section:"", key:HiddenSettingConstants.SafeHelperServerIp, value:""}
        testDataset[HiddenSettingConstants.SafeHelperServerPort] = { section:"", key:HiddenSettingConstants.SafeHelperServerPort, value:0 }
        testDataset[HiddenSettingConstants.TmapLog] = { section:"", key:HiddenSettingConstants.TmapLog, value:1 } // default ON
        testDataset[HiddenSettingConstants.GpsMode] = { section:"", key:HiddenSettingConstants.GpsMode, value:0 } // default system
        testDataset[HiddenSettingConstants.GpsLog] = { section:"", key:HiddenSettingConstants.GpsLog, value:1 } // default ON
        testDataset[HiddenSettingConstants.FixedRerouteTime] = { section:"", key:HiddenSettingConstants.FixedRerouteTime, value:0 } // default server
        testDataset[HiddenSettingConstants.MapMatching] = { section: "", key: HiddenSettingConstants.MapMatching, value: 1} // default ON
        testDataset[HiddenSettingConstants.TvasVersion] = { section:"", key:HiddenSettingConstants.TvasVersion, value:2 }
        responseSettings(testDataset)
        ***/

        sendEvent(HiddenVsmDiagSettingViewEvent.RequestVsmHiddenSetting, {section:HiddenSettingConstants.HiddenVsmSetting})
    }

    onClosed: {
        var newSettings = {}
        var id = 0
        for (var i=0;i<listModel.count;i++) {
            var values = {}
            values = listModel.get(i).values
            for (var v in values) {
                if (CommonUtils.isEqual(v, "id"))
                    continue
                newSettings[v] = values[v]
            }
        }

        var data = {}
        data[CommonConstants.Section] = HiddenSettingConstants.HiddenVsmSetting
        data[CommonConstants.Settings] = newSettings
        sendEvent(HiddenVsmDiagSettingViewEvent.SaveHiddenSetting, data)
    }


    ListModel { id: listModel }

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
                readonly property int keyItemWidth: width / kMenus[index].items.length - 180  // substract currentValue label' width

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
                                readonly property string keyName: kMenus[menuIndex].items[index].key
                                readonly property int valueLength: kMenus[menuIndex].items[index].values.length

                                Component.onCompleted: {
                                    if (CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index].values[valueSet[keyName]%valueLength]))
                                        console.log("^^ menu:"+menuIndex+"\n, key name : " + keyName+"\n, index="+keyIndex+"\n, valuelen="+valueLength)
                                    if (!CommonUtils.isUndefined(typeof valueSet[keyName]) &&
                                        !CommonUtils.isUndefined(typeof kMenus[menuIndex].items[index].values[valueSet[keyName]%valueLength])) {
                                        currentValue.text = kMenus[menuIndex].items[index].values[valueSet[keyName]%valueLength]
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
                                            model: valueLength //kMenus[menuIndex].items[keyIndex].values.length
                                            Item {
                                                width: valueItems.width / valueLength
                                                height: valueItems.height
                                                Row {
                                                    width: parent.width
                                                    height: parent.height
                                                    spacing: 1
                                                    Text {
                                                        width: content.width
                                                        height: 22
                                                        font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                                                        color: GlobalConstants.kListSubTextColor
                                                        verticalAlignment: Text.AlignVCenter
                                                        visible: !CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type,  "button")
                                                        text: kMenus[menuIndex].items[keyIndex].values[index]
                                                    }

                                                    // Radio value items
                                                    Item {
                                                        width: 32
                                                        height: 32
                                                        visible: false
                                                        property bool checked: ( CommonUtils.isEqual(valueSet[keyName], index))
                                                        Image {
                                                            id: radioImg
                                                            anchors.fill: parent
                                                            source: (parent.checked)?Resource.imageUrl("radio_on.png"):Resource.imageUrl("radio_off.png")
                                                        }
                                                        MouseArea {
                                                            anchors.fill: parent
                                                            onClicked: {
                                                                setKeySetting(menuIndex, keyName, index)
                                                                if (!CommonUtils.isEqual(menuIndex, 3))
                                                                    applyKeySetting( menuItem.valueSet["id"], keyName, index) // apply directly that setting if only on selection keys
                                                                else if (CommonUtils.isEqual(index, 0))
                                                                    applyKeySetting( menuItem.valueSet["id"], keyName, {"VsmAutoLevelMap":0} ) // to pass more params just for map-drive-auto-level config
                                                                else {
                                                                    driveMapLevelConfigGroup.load()
                                                                    applyKeySetting( menuItem.valueSet["id"], keyName, {"VsmAutoLevelMap":1} ) // to pass more params just for map-drive-auto-level config
                                                                }

                                                                sendEvent(HiddenVsmDiagSettingViewEvent.HideKeyboard)
                                                                if (!CommonUtils.isNull(keypad.curEditor))
                                                                    keypad.curEditor.focus = false
                                                                keypad.curEditor = null
                                                         }
                                                     }
                                                     Component.onCompleted: {
                                                        if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type,  "radio"))
                                                            visible = true
                                                    }
                                                }

                                                    // Input value items
                                                    Item {
                                                        width: parent.width
                                                        height: valueItems.height - 4
                                                        visible: false
                                                        Loader {
                                                            id: inputboxLoader
                                                            onLoaded: {
                                                                item.width = parent.width - currentValue.width
                                                                item.height = parent.height
                                                                item.inputText = CommonUtils.isUndefined(typeof menuItem.valueSet[keyName]) ? "" : menuItem.valueSet[keyName]
                                                                item.hMenuIndex = menuIndex
                                                                item.hKeyName   = keyName
                                                                item.hKeyId     = menuItem.valueSet["id"]
                                                            }
                                                        }
                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "text")) {
                                                                inputboxLoader.sourceComponent = compTextInput
                                                                visible = true
                                                            }
                                                        }
                                                    }

                                                    // Button value item
                                                    Item {
                                                        width: parent.width
                                                        height:valueItems.height - 4
                                                        visible: false
                                                        Rectangle {
                                                            color: "#212121"
                                                            anchors.fill:parent
                                                            Text {
                                                                anchors.centerIn: parent
                                                                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize*0.6 }
                                                                color: "#dfdfdf"
                                                                text: kMenus[menuIndex].items[keyIndex].values[index]
                                                            }
                                                            MouseArea {
                                                                anchors.fill: parent
                                                                onClicked: {
                                                                }
                                                            }
                                                        }
                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "button"))
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
                                                                item.label = kMenus[menuIndex].items[keyIndex].values[index]
                                                                var v =  parseInt(menuItem.valueSet[keyName])
                                                                item.value = v

                                                                if (!CommonUtils.isUndefined(typeof kMenus[menuIndex].items[keyIndex].labels) &&
                                                                    kMenus[menuIndex].items[keyIndex].labels.length > index) {
                                                                    item.unitLabel = kMenus[menuIndex].items[keyIndex].labels[index]
                                                                }
                                                            }
                                                        }

                                                        Connections {
                                                            target: pickerLoader.item
                                                            onValueChanged: {
                                                                setKeySetting(menuIndex, keyName, pickerLoader.item.value)
                                                                applyKeySetting(menuItem.valueSet["id"], keyName, pickerLoader.item.value) // apply directly that setting if only on selection keys
                                                            }
                                                        }
                                                        Component.onCompleted: {
                                                            if (CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "pickerButton")) {
                                                             pickerLoader.sourceComponent = Qt.createComponent("../../component/TPickerButton.qml")
                                                             visible = true
                                                         }
                                                     }
                                                 }

                                                 // Check button value item
                                                 Item {
                                                    width: 63
                                                    height: 63
                                                    visible: false
                                                    TCheckButton {
                                                        id: checkButton
                                                        width: parent.width
                                                        height: parent.height
                                                        selected: parseInt(menuItem.valueSet[keyName]) > 0
                                                        img.width:  width
                                                        img.height: height
                                                        imgCheck:  Resource.imageUrl("checkbox_on.png")
                                                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                                                        anchors.verticalCenter: parent.verticalCenter
                                                        onClicked: {
                                                            setKeySetting(menuIndex, keyName, selected)
                                                            applyKeySetting( menuItem.valueSet["id"], keyName, index)
                                                        }
                                                    }
                                                    Component.onCompleted: {
                                                          visible = CommonUtils.isEqual(kMenus[menuIndex].items[keyIndex].type, "check")
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
        id: compTextInput
        Rectangle {
            color: "#1f1f1f"
            width: 50
            height: 20
            property alias inputText: inputEdit.text
            property int hMenuIndex
            property int hKeyId
            property string hKeyName
            property bool useKeySetting: true
            TextInput {
                id: inputEdit
                anchors.fill:parent
                color: "#bbbbbb"
                verticalAlignment: Text.AlignVCenter
                clip:true
                font { family: GlobalConstants. kFontFamily; pixelSize: 20;}
                text: ""
                onActiveFocusChanged: {
                    if (!activeFocus) {
                        if (useKeySetting) {
                           setKeySetting(hMenuIndex, hKeyName, text)
                        }
                        applyKeySetting(hKeyId, hKeyName, text)
                        if ( CommonUtils.isNull(keypad.curEditor)) { //donot hide keypad if it is visible for other input editor
                            sendEvent(HiddenVsmDiagSettingViewEvent.HideKeyboard)
                        }
                    }
                    else {
                       sendEvent(HiddenVsmDiagSettingViewEvent.RegistKeyboardInput, {"keyboardInput": inputEdit})
                       sendEvent(HiddenVsmDiagSettingViewEvent.ShowKeyboard, {
                               owner: objectName,
                               x: keypad.x,
                               y: keypad.y,
                               z: keypad.z+1,
                               width: keypad.width,
                               height: keypad.height,
                               searchBtnAlwaysEnable: true,
                               dontUseEditBox: true
                           })
                       keypad.curEditor = inputEdit
                        sendEvent(HiddenVsmDiagSettingViewEvent.SetText, keypad.curEditor.text)
                   }
                } // active focus changed
             } // text input
         }
     }


      // sub value speed map level config group
     HiddenVsmMapAutoLevelConfig {
         id: driveMapLevelConfigGroup
         width: 490
         height: 250
         anchors.left: parent.left
         anchors.leftMargin: 80
         visible: false

         onLoaded: {
             viewRoot.height =  (GlobalConstants.kScreenContentHeight - GlobalConstants.kScreenZone3Height) * 0.4
             viewRoot.listView.visible = false
         }

          onApplyClicked: {
            applyKeySetting(HiddenSettingConstants.K_VsmAutoLevelMap, HiddenSettingConstants.VsmAutoLevelMap, data)
          }

          onQuitClicked: {
              viewRoot.height =  GlobalConstants.kScreenContentHeight - GlobalConstants.kScreenZone3Height
              viewRoot.listView.visible = true
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
                cacheBuffer: layout.rowHeight * 2
                Component.onCompleted: viewRoot.listView = this

                TScrollBar {
                    flickable: parent
                    width: 2
                    color: MultiSenseCoordinator.kAmbientColor
                    grooveColor: GlobalConstants.kScrollBarBackColor
                    rightMargin: GlobalConstants.kScrollBarRightMargin
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
