import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingAttentionSectionViewEvent 1.0
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

    //csw
    property bool focused           : false

    property color highlightColor   : CommonConstants.Transparent

    //Timer
    property var timer              : null


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
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("주의구간 음성 안내")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "Name" , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"  , i++)
        CommonUtils.insertEnum(listAttrEnum, "Image", i++)
        listArr= [
                [qsTr("과속방지턱"),       SettingConstants.SpeedBump               , "sign_s_05.png"],
                [qsTr("사고다발"),         SettingConstants.AccidentFrequent        , "sign_s_02.png"],
                [qsTr("어린이보호구역"),   SettingConstants.ChildProtectedArea      , "sign_s_06.png"],
                [qsTr("안개주의"),         SettingConstants.FogCaution              , "sign_s_03.png"],
                [qsTr("야생동물주의"),     SettingConstants.AttentionToWildAnimals  , "sign_s_16.png"],
                [qsTr("철길건널목"),       SettingConstants.RailroadXing            , "sign_s_07.png"],
                [qsTr("급경사구간"),       SettingConstants.SteepSlopeSection       , "sign_s_15.png"],
                [qsTr("급커브"),           SettingConstants.SharpCurve              , "sign_s_01.png"],
                [qsTr("일방통행"),         SettingConstants.OneWay                  , "sign_s_17.png"],
                [qsTr("신호위반 다발"),    SettingConstants.FrequentSignalViolation , "sign_s_11.png"],
                [qsTr("교통혼잡지역"),     SettingConstants.TrafficJamArea          , "sign_s_18.png"],
                [qsTr("차로 변경 구간"),   SettingConstants.ChangeSectionOfCar      , "sign_s_19.png"],
                [qsTr("충전소"),           SettingConstants.ChargingStation         , "sign_p_03.png"],
                [qsTr("터널"),             SettingConstants.Tunnel                  , "sign_p_02.png"],
                [qsTr("휴게소"),           SettingConstants.RestArea                , "sign_p_01.png"],
                [qsTr("톨게이트"),         SettingConstants.FeeStation              , "sign_p_04.png"],
                [qsTr("시야불량"),         SettingConstants.BadSight                , "sign_s_08.png"],
                [qsTr("졸음쉼터"),         SettingConstants.SleepShelter            , "sign_p_05.png"],
            ]
    }

    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.GuidSubAttentionSection
        sendEvent(SettingAttentionSectionViewEvent.RequestSetting, data )
        resetCreateModel()
    }

    function saveSetting() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.GuidSubAttentionSection
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingAttentionSectionViewEvent.SaveSetting, data )
    }

     function createModel(){
        var componentName = "TextWithCheck"
        for (var i = 0; i<listArr.length; i++) {
            settingList.rowComponent[componentName] = textWithCheck
            listModel.set(i,
            {
                component: componentName,
                idx : i,
                image: listArr[i][listAttrEnum.Image],
                text: listArr[i][listAttrEnum.Name],
                value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(CommonConstants.False)
            })
        }
    }

     function resetCreateModel() {
         createModel()
         settingRoot.settingList.list.model = null
         settingRoot.settingList.list.model = listModel
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
             sendEvent(SettingAttentionSectionViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
             resetCreateModel()
         }
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingAttentionSectionView)
              sendEvent(SettingAttentionSectionViewEvent.CloseView, data)
              break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
          default:
              break
        }
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return

        var key, value
        for (var i = 0; i<listArr.length; i++){
            key = listArr[i][listAttrEnum.Key]
            if (!settings.hasOwnProperty(key))
                continue
            value = settings[key][CommonConstants.Value]
            listModel.setProperty(i, CommonConstants.Value, value)
        }
    }

    onMediated: {
        sendEvent(SettingAttentionSectionViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        init()
        sendEvent(SettingAttentionSectionViewEvent.ResetCsw)
        sendEvent(SettingAttentionSectionViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingAttentionSectionViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingAttentionSectionViewEvent.Activated, [CommonConstants.NoOption])
        if (isActivated)
            return
        isActivated = true
        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingAttentionSectionViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
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

    Component {
        id: textWithCheck
        Column {
            width: settingList.width
            height: GlobalConstants.kListTextItemHeight
            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: checkButton.visible


            function execButton() {
                settingList.list.currentIndex = itemModel.idx
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
                        if (checkButton.enabled) {
                            checkButton.selected = checkButton.selected ? CommonConstants.False: CommonConstants.True
                        }
                    } catch(e) {
                    }
                })
            }

            Item{
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
                Row {
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Item {
                        width:  63
                        height: parent.height
                        Image{
                            width: 63
                            height: 63
                            source: Resource.imageUrl("setting", itemModel.image)
                            sourceSize.width : width
                            sourceSize.height: height
                            anchors.centerIn: parent
                        }
                    }
                    Item {width: 20;height:parent.height}
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
                    TCheckButton {
                        id: checkButton
                        width: settingRoot.layout.checkboxWith
                        height: settingRoot.layout.checkboxHeight
                        selected:  CommonUtils.number(itemModel.value)
                        img.width:  width
                        img.height: height
                        imgCheck:  Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        input.font.pixelSize: Resource.pixelToDp(21)
                        anchors.verticalCenter: parent.verticalCenter
                        onSelectedChanged: {
                            itemModel.value = CommonUtils.string(selected)
                            var key = listArr[itemModel.idx][listAttrEnum.Key]
                            newSettings[key][CommonConstants.Value]= CommonUtils.string(selected)
                            saveSetting()
                        }

                        onClicked: execButton()
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
                if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
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
            if (!CommonUtils.isNull(item.isButtonFocused)   && item.isButtonFocused) {
                item.execButton()
            } else {
                item.clickItem()
            }
        }
    }

}
