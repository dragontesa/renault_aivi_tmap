import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingGuidVoiceViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


SettingView {
    id: settingRoot


    //언어 관련
    property var __lang             : null
    property bool needInit          : true

    property var caller             : null
    property var settings           : null
    property var newSettings        : settings
    property bool isActivated       : false


    property bool focused           : false
    property color highlightColor   : CommonConstants.Transparent


    //ID
    property var settingList        : null

    //Enum
    property var textEnum           : ({})
    property var listAttrEnum       : ({})

    property var textArr            : ([])
    property var listArr            : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("안내 음성 설정")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"    , i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"      , i++)
        CommonUtils.insertEnum(listAttrEnum, "Name"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Section"  , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"      , i++)

        listArr = [
            ["TextWithRadio", textWithRadio, qsTr("여성"), SettingConstants.GuidSubVoice, SettingConstants.Male, 0],
            ["TextWithRadio", textWithRadio, qsTr("남성"), SettingConstants.GuidSubVoice, SettingConstants.Male, 0]
        ]
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()

        allocateListAttrEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingGuidVoiceViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingGuidVoiceViewEvent.RequestLang)
        init()
        createModel()        
    }

    onActivated: {
        init()
        sendEvent(SettingGuidVoiceViewEvent.ResetCsw)
        sendEvent(SettingGuidVoiceViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingGuidVoiceViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingGuidVoiceViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingGuidVoiceViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        sendEvent(SettingGuidVoiceViewEvent.RequestSetting, data )        
        resetCreateModel()
    }

    function saveSetting() {
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingGuidVoiceViewEvent.SaveSetting, data )
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        var key = listArr[0][listAttrEnum.Key]
        if (!settings.hasOwnProperty(key))
            return
        var idx = settings[key][CommonConstants.Value]        
        setRadioProperty(idx)
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            listModel.set(i,
            {
                component: componentName ,
                idx : i,
                text: listArr[i][listAttrEnum.Name],
                value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
            })
        }
    }

    function setRadioProperty(value) {
        for (var i = 0; i<listArr.length; i++) {
            listModel.setProperty(i, CommonConstants.Value,  CommonUtils.string(CommonConstants.False))
        }
        listModel.setProperty(CommonUtils.number(value), CommonConstants.Value,  CommonUtils.string(CommonConstants.True))
    }

    function resetCreateModel() {
        createModel()
        settingRoot.settingList.list.model = null
        settingRoot.settingList.list.model = listModel
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        settingRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingGuidVoiceView)
            sendEvent(SettingGuidVoiceViewEvent.CloseView, data)
            break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
          default:
              break
        }
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height:parent.height
    }

    Component {
        id: main
        Column {
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            Rectangle{
                width: parent.width
                height: parent.height
                color: GlobalConstants.kBasicBackgoundColor
                SettingList {
                    Component.onCompleted: settingRoot.settingList = this
                    width: parent.width
                    height: parent.height
                    list.clip: true
                    list.model: listModel
                    list.currentIndex: -1
                    list.interactive: false

                    property var pressedItem: null
                    PressBar {
                        currentItem: settingList.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    list.highlight: HighlightBar {
                        currentItem: settingList.list.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    list.highlightFollowsCurrentItem: false

                    TScrollBar {
                        Component.onCompleted: parent.scrollbar = this
                        flickable: parent.list
                        width: 2
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                        onScroll:    settingList.imgMask.visible = true
                        onMouseRelease:  settingList.imgMask.visible = false
                    }
                }
            }
         }
    }

    ListModel { id: listModel   }

    ExclusiveGroup { id: group }

    Component {
        id: textWithRadio
        Column {
            width: settingList.width
            height: GlobalConstants.kListTextItemHeight
            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: radioBtn.visible

            function execButton() {
            }

            onIsFocusedChanged: {
                if (isFocused) {
                } else {
                    isButtonFocused = false
                }
            }
            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                } else {
                }
            }

            function clickItem() {
                settingList.list.currentIndex = itemModel.idx
                if (!settingRoot.focused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                }
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(settingRoot, sleep, function() {
                    try {
                        if (settingRoot.focused) {
                            highlightColor = GlobalConstants.kListFocusColor
                        } else {
                            highlightColor = CommonConstants.Transparent
                            //settingList.list.currentIndex = -1
                        }
                        if (radioBtn.enabled) {
                            radioBtn.checked = 1
                            radioBtn.buttonClicked()
                        }
                    } catch(e) {
                    }
                })

            }

            Item {
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height

                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: settingList.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, settingList.pressedItem)) {
                                return
                            }
                            settingList.pressedItem = currentItem
                        }
                    }

                    onClicked: clickItem()
                }
                Row{
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Text {
                        width: parent.width
                        height: parent.height
                        Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                        color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                        text: itemModel.text
                    }
                    ImageRadioButton {
                        id: radioBtn
                        width: settingRoot.layout.radioboxWidth
                        height: settingRoot.layout.radioboxHeight
                        anchors.verticalCenter: parent.verticalCenter
                        exclusiveGroup: group
                        checked: CommonUtils.number(itemModel.value)
                        img.sourceSize.height: height
                        img.sourceSize.width: width
                        imgPressed: Resource.imageUrl("radio_on.png")
                        imgNormal: Resource.imageUrl("radio_off.png")
                        onButtonClicked: {
                            var idx = itemModel.idx
                            settingList.list.currentIndex = idx

                            var key = listArr[idx][listAttrEnum.Key]
                            var value = idx
                            setRadioProperty(value)
                            newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                            saveSetting()
                        }
                    }
                    Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
                }
            }
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: 1
               color: GlobalConstants.kListLineColor
            }
          }
    }

   Csw {
       id: csw

       section: Csw.Section_Setting
       sequence: 0
       enable: settingRoot.visible

       onFocused: {
           settingRoot.focused = true
           highlightColor = GlobalConstants.kListFocusColor
           settingList.list.currentIndex = -1
           settingList.list.currentIndex = 0
       }
       onLostFocusd: {
           highlightColor = CommonConstants.Transparent
           settingRoot.focused = false
       }
       onLeftPressed: selected()
       onRightPressed: selected()
       onSpinCwPressed: {
           if (settingList.list.currentIndex < settingList.list.count - 1) {
               ++settingList.list.currentIndex
               var item = settingList.list.currentItem.children[0]
               if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                   item.isButtonFocused = false
               }
               highlightColor = GlobalConstants.kListFocusColor
           }
       }
       onSpinCcwPressed: {
           if (0 < settingList.list.currentIndex) {
               --settingList.list.currentIndex
               var item = settingList.list.currentItem.children[0]
               if (!CommonUtils.isNull(item.isButtonFocused))
                   item.isButtonFocused = false
               highlightColor = GlobalConstants.kListFocusColor
           }
       }
       onSelected: {           
           var item = settingList.list.currentItem.children[0]
           if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
               item.execButton()
           } else {
               item.clickItem()
           }           
       }
   }

}
