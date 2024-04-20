import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingCameraWarningSoundViewEvent 1.0
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
    property var __lang                 : null
    property bool needInit              : true


    property var caller                 : null
    property var settings               : null
    property var newSettings            : settings
    property bool safeDrivingSoundAtMute: true
    property bool isActivated           : false


    property bool focused               : false
    property color highlightColor       : CommonConstants.Transparent

    //Timer
    property var timer                  : null

    //ID
    property var settingList            : null

    //Enum
    property var textEnum               : ({})
    property var listEnum               : ({})
    property var listAttrEnum           : ({})

    property var textArr                : ([])
    property var listNameArr            : ([])
    property var listSectionArr         : ([])
    property var listKeyArr             : ([])
    property var listImage              : ([])
    property var listArr                : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",  i++, [CommonConstants.Name], [qsTr("경고 카메라 음성 안내")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateListEnum(){
        if (CommonUtils.getObjectLen(listEnum) <= 0) {
            listEnum = CommonUtils.createEnum(listEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listEnum, "CrackdownOnSpeedViolation"    , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("과속단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.CrackdownOnSpeedViolation,
                      "sign_c_01.png"
                    ])
        CommonUtils.insertEnum(listEnum, "SectionControl"                , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("구간단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.SectionControl,
                      "sign_c_10.png"
                    ])
        CommonUtils.insertEnum(listEnum, "SignalControl"                , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("신호/과속단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.SignalControl,
                      "sign_c_03.png"
                    ])
        CommonUtils.insertEnum(listEnum, "DetachableCrackdown"          , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("이동식단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.DetachableCrackdown,
                      "sign_c_16.png"
                    ])
        CommonUtils.insertEnum(listEnum, "CrackdownWithBusExclusiveCar" , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("버스전용차로단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.CrackdownWithBusExclusiveCar,
                      "sign_c_06.png"
                    ])
        CommonUtils.insertEnum(listEnum, "ParkingControl"               , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("주차단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.ParkingControl,
                      "sign_c_07.png"
                    ])
        CommonUtils.insertEnum(listEnum, "InterruptRegulation"          , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("끼어들기단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.InterruptRegulation,
                      "sign_c_12.png"
                    ])
        CommonUtils.insertEnum(listEnum, "MonitoringOfRoadShoulder"     , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("갓길감시단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.MonitoringOfRoadShoulder,
                      "sign_c_08.png"
                    ])
        CommonUtils.insertEnum(listEnum, "CollectingTrafficInformation" , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("교통정보수집"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.CollectingTrafficInformation,
                      "sign_c_04.png"
                    ])
        CommonUtils.insertEnum(listEnum, "ControlOfVariableVehicles"    , i++,
                    [ CommonConstants.Name,
                      CommonConstants.Section,
                      CommonConstants.Key,
                      CommonConstants.Url
                    ],
                    [ qsTr("가변차로단속"),
                      SettingConstants.GuidSubCamera,
                      SettingConstants.ControlOfVariableVehicles,
                      "sign_c_14.png"
                    ])


        CommonUtils.fetchArr(listEnum, CommonConstants.Name, listNameArr)
        CommonUtils.fetchArr(listEnum, CommonConstants.Section, listSectionArr)
        CommonUtils.fetchArr(listEnum, CommonConstants.Key, listKeyArr)
        CommonUtils.fetchArr(listEnum, CommonConstants.Url, listImage)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"    , i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"      , i++)
        CommonUtils.insertEnum(listAttrEnum, "Image"    , i++)
        CommonUtils.insertEnum(listAttrEnum, "Name"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Section"  , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"      , i++)
        listArr = [
                ["TextWithCheck", textWithCheck, listImage[0], listNameArr[0], listSectionArr[0], listKeyArr[0]],
                ["TextWithCheck", textWithCheck, listImage[1], listNameArr[1], listSectionArr[1], listKeyArr[1]],
                ["TextWithCheck", textWithCheck, listImage[2], listNameArr[2], listSectionArr[2], listKeyArr[2]],
                ["TextWithCheck", textWithCheck, listImage[3], listNameArr[3], listSectionArr[3], listKeyArr[3]],
                ["TextWithCheck", textWithCheck, listImage[4], listNameArr[4], listSectionArr[4], listKeyArr[4]],
                ["TextWithCheck", textWithCheck, listImage[5], listNameArr[5], listSectionArr[5], listKeyArr[5]],
                ["TextWithCheck", textWithCheck, listImage[6], listNameArr[6], listSectionArr[6], listKeyArr[6]],
                ["TextWithCheck", textWithCheck, listImage[7], listNameArr[7], listSectionArr[7], listKeyArr[7]],
                ["TextWithCheck", textWithCheck, listImage[8], listNameArr[8], listSectionArr[8], listKeyArr[8]],
                ["TextWithCheck", textWithCheck, listImage[9], listNameArr[9], listSectionArr[9], listKeyArr[9]],
            ]
    }   

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateListEnum()
        allocateListAttrEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingCameraWarningSoundViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingCameraWarningSoundViewEvent.RequestLang)
        init()
        createModel()        
    }

    onActivated: {
        init()
        sendEvent(SettingCameraWarningSoundViewEvent.ResetCsw)
        sendEvent(SettingCameraWarningSoundViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingCameraWarningSoundViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingCameraWarningSoundViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingCameraWarningSoundViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        sendEvent(SettingCameraWarningSoundViewEvent.RequestSetting, data )
        resetCreateModel()
    }

    function saveSetting() {
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingCameraWarningSoundViewEvent.SaveSetting, data )
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return

        var key,value
        for (var i = 0; i< listArr.length; i++) {
           key = listArr[i][listAttrEnum.Key]
            if (!settings.hasOwnProperty(key))
                continue
           value = settings[key][CommonConstants.Value]
           listModel.setProperty(i, CommonConstants.Value, value)
        }
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            listModel.set(i,
            {
                component: componentName ,
                idx : i,
                image: listArr[i][listAttrEnum.Image],
                text: listArr[i][listAttrEnum.Name],
                value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
            })
        }
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingCameraWarningSoundView)
              sendEvent(SettingCameraWarningSoundViewEvent.CloseView, data)
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
                            sourceSize.width: width
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
                        selected: CommonUtils.number(itemModel.value)
                        img.width: width
                        img.height: height
                        imgCheck:  Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        input.font.pixelSize: Resource.pixelToDp(21)
                        anchors.verticalCenter: parent.verticalCenter
                        onSelectedChanged: {
                            itemModel.value = CommonUtils.string(selected)
                            var key = listArr[itemModel.idx][listAttrEnum.Key]
                            newSettings[key][CommonConstants.Value] = CommonUtils.string(selected)
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
            if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
                item.execButton()
            } else {
                item.clickItem()
            }
        }
    }
}
