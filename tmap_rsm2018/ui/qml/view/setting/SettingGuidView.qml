import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingGuidViewEvent 1.0
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
    property var settingsMain           : null
    property var settingsCamera         : null
    property bool isActivated           : false


    property bool focused               : false
    property color selectColor          : MultiSenseCoordinator.kAmbientColor
    property color highlightColor       : CommonConstants.Transparent

    //Timer
    property var timer                  : null

    //ID
    property var settingList            : null
    property var roadViolationNotice    : null

    //Enum
    property var textEnum               : ({})
    property var roadViolationEnum      : ({})
    property var componentEnum          : ({})
    property var reqEnum                : ({})
    property var listAttrEnum           : ({})

    property var textArr                : ([])
    property var roadViolationArr       : ([])
    property var componentCNameArr      : ([])
    property var componentCIDArr        : ([])
    property var listNameArr            : ([])
    property var listSectionArr         : ([])
    property var listKeyArr             : ([])
    property var listUrlArr             : ([])
    property var listArr                : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("안내 설정")])
        CommonUtils.insertEnum(textEnum, "Notice"    , i++, [CommonConstants.Name], [qsTr("과속 경고음은 1km로 설정하여도<br>일반도로에서는 600m에서 제공됩니다.")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function  allocateRoadViolationEnum(){
        if (CommonUtils.getObjectLen(roadViolationEnum) <= 0) {
            roadViolationEnum = CommonUtils.createEnum(roadViolationEnum)
        }
        CommonUtils.insertEnum(roadViolationEnum, "Km1",  SettingConstants.RoadViolation1Km,  [CommonConstants.Name], [qsTr("1km")])
        CommonUtils.insertEnum(roadViolationEnum, "M600", SettingConstants.RoadViolation600m, [CommonConstants.Name], [qsTr("600m")])
        CommonUtils.insertEnum(roadViolationEnum, "M300", SettingConstants.RoadViolation300m, [CommonConstants.Name], [qsTr("300m")])

        CommonUtils.fetchArr(roadViolationEnum, CommonConstants.Name, roadViolationArr)
    }

    function allocateComponentEnum(){
        if (CommonUtils.getObjectLen(componentEnum) <= 0) {
            componentEnum = CommonUtils.createEnum(componentEnum)
        }
        var i = 0
        CommonUtils.insertEnum(componentEnum, "SubMenu", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithSubMenu", textWithSubMenu])
        CommonUtils.insertEnum(componentEnum, "Option2", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithOption2", textWithOption2])
        CommonUtils.insertEnum(componentEnum, "Option3", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithOption3", textWithOption3])

        CommonUtils.fetchArr(componentEnum, CommonConstants.Name, componentCNameArr)
        CommonUtils.fetchArr(componentEnum, CommonConstants.Kind, componentCIDArr)
    }

    function allocateReqEnum(){
        if (CommonUtils.getObjectLen(reqEnum) <= 0) {
            reqEnum = CommonUtils.createEnum(reqEnum)
        }
        var i = 0
        CommonUtils.insertEnum(reqEnum, "GuidSubCamera"             , i++,
                    [CommonConstants.Name,
                     CommonConstants.Section,
                     CommonConstants.Key,
                     CommonConstants.Url],
                    [qsTr("경고 카메라 음성 안내"),
                     SettingConstants.GuidSubCamera,
                     cameraWarnSoundKeyArr,
                     CommonConstants.url(ViewRegistry.SettingCameraWarningSoundView)])
        CommonUtils.insertEnum(reqEnum, "GuidSubAttentionSection"   , i++,
                    [CommonConstants.Name,
                     CommonConstants.Section,
                     CommonConstants.Key,
                     CommonConstants.Url],
                    [qsTr("주의구간 음성 안내"),
                     SettingConstants.GuidSubAttentionSection,
                     attentionSectionKeyArr,
                     CommonConstants.url(ViewRegistry.SettingAttentionSectionView)])
        CommonUtils.insertEnum(reqEnum, "GuidSubVoiceGuidTime"      , i++,
                    [CommonConstants.Name,
                     CommonConstants.Section,
                     CommonConstants.Key,
                     CommonConstants.Url],
                    [qsTr("음성 길안내 시점"),
                     SettingConstants.GuidSubVoiceGuidTime,
                     voiceGuidTimeKeyArr,
                     CommonConstants.url(ViewRegistry.SettingVoiceGuidTimeView)])
        CommonUtils.insertEnum(reqEnum, "GuidMain"                  , i++,
                    [CommonConstants.Section],
                    [SettingConstants.GuidMain])

        CommonUtils.fetchArr(reqEnum, CommonConstants.Name, listNameArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Section, listSectionArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Key, listKeyArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Url, listUrlArr)
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
        CommonUtils.insertEnum(listAttrEnum, "Arg0"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg1"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg2"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg3"     , i++)

        listArr = [
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr    [reqEnum.GuidSubCamera],
             listSectionArr [reqEnum.GuidSubCamera],
             listKeyArr     [reqEnum.GuidSubCamera]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr    [reqEnum.GuidSubAttentionSection],
             listSectionArr [reqEnum.GuidSubAttentionSection],
             listKeyArr     [reqEnum.GuidSubAttentionSection]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr    [reqEnum.GuidSubVoiceGuidTime],
             listSectionArr [reqEnum.GuidSubVoiceGuidTime],
             listKeyArr     [reqEnum.GuidSubVoiceGuidTime]
            ],
            [componentCNameArr[componentEnum.Option3],
             componentCIDArr[componentEnum.Option3],
             qsTr("과속 경고음 시작 시점"),
             listSectionArr [reqEnum.GuidMain],
             SettingConstants.RoadViolation,
             roadViolationArr[roadViolationEnum.Km1],
             roadViolationArr[roadViolationEnum.M600],
             roadViolationArr[roadViolationEnum.M300],
             CommonUtils.string(0)
            ],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("내비 음소거 시 안전운전 경고음 안내"),
             listSectionArr [reqEnum.GuidMain],
             SettingConstants.SafeDrivingSoundAtMute,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On],
             CommonUtils.string(0)
            ]
        ]
    }

    property var cameraWarnSoundKeyArr: [
        SettingConstants.CrackdownOnSpeedViolation,
        SettingConstants.SectionControl,
        SettingConstants.SignalControl,
        SettingConstants.DetachableCrackdown,
        SettingConstants.CrackdownWithBusExclusiveCar,
        SettingConstants.ParkingControl,
        SettingConstants.InterruptRegulation,
        SettingConstants.MonitoringOfRoadShoulder,
        SettingConstants.CollectingTrafficInformation,
        SettingConstants.ControlOfVariableVehicles
    ]

    property var attentionSectionKeyArr: [
        SettingConstants.SpeedBump,
        SettingConstants.ChildProtectedArea,
        SettingConstants.FogCaution,
        SettingConstants.AttentionToWildAnimals,
        SettingConstants.RailroadXing,
        SettingConstants.SteepSlopeSection,
        SettingConstants.SharpCurve,
        SettingConstants.OneWay,
        SettingConstants.FrequentSignalViolation,
        SettingConstants.SpeedTravelFrequently,
        SettingConstants.TrafficJamArea,
        SettingConstants.ChangeSectionOfCar,
        SettingConstants.ChargingStation,
        SettingConstants.Tunnel,
        SettingConstants.RestArea,
        SettingConstants.FeeStation,
        SettingConstants.BadSight,
    ]

    property var voiceGuidTimeKeyArr: [
        SettingConstants.Km2,
        SettingConstants.Km1,
        SettingConstants.M600,
        SettingConstants.M300,
    ]


    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = listArr[reqEnum.GuidMain][listAttrEnum.Section]
        sendEvent(SettingGuidViewEvent.RequestSetting, data )
        resetCreateModel()
    }

    function init() {
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateToggleEnum()

        allocateTextEnum()
        allocateRoadViolationEnum()
        allocateComponentEnum()
        allocateReqEnum()
        allocateListAttrEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingGuidViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            requestSetting()
            resetCreateModel()            
        }
    }

    onMediated: {
        sendEvent(SettingGuidViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        init()

        enabled = true
        sendEvent(SettingGuidViewEvent.ResetCsw)
        sendEvent(SettingGuidViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingGuidViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingGuidViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        enabled = false
        sendEvent(SettingGuidViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    onSettingsChanged: {
        var i,key,value
        var data = ({})
        if (CommonUtils.isNull(settings))
            return
        settingsMain = settings
        for (i=reqEnum.GuidMain; i<listArr.length; i++) {
            key = listArr[i][listAttrEnum.Key]
            if (!settings.hasOwnProperty(key))
                continue
            value = settings[key][CommonConstants.Value]
            setOptionProperty(i, value)
        }
        key = SettingConstants.RoadViolation
        value = settings[key][CommonConstants.Value]
        showHideRoadViolationNotice(value)
    }

    function showHideRoadViolationNotice(value) {
        roadViolationNotice.visible = CommonUtils.isEqual(CommonUtils.number(value), roadViolationEnum.Km1)
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.SubMenu])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listArr[i][listAttrEnum.Name]
                })
            } else if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option2])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listArr[i][listAttrEnum.Name],
                    option1 : listArr[i][listAttrEnum.Arg0],
                    option2 : listArr[i][listAttrEnum.Arg1],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })

            } else if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option3])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listArr[i][listAttrEnum.Name],
                    option1 : listArr[i][listAttrEnum.Arg0],
                    option2 : listArr[i][listAttrEnum.Arg1],
                    option3 : listArr[i][listAttrEnum.Arg2],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })
            }
        }
    }

    function setOptionProperty(index , value) {
        listModel.setProperty(index, CommonConstants.Value,  CommonUtils.string(value))
    }

    function resetCreateModel() {
        createModel()
        settingRoot.settingList.list.model = null
        settingRoot.settingList.list.model = listModel
    }


    function createView(index) {
        settingList.list.currentIndex = index
        if (index < listUrlArr.length) {
            isActivated = false

            var data = ({})
            data[CommonConstants.Url] = listUrlArr[index]
            data[CommonConstants.Caller] = caller
            if (CommonUtils.isEqual(data[CommonConstants.Url] , CommonConstants.url(ViewRegistry.SettingCameraWarningSoundView))) {
                data[SettingConstants.SafeDrivingSoundAtMute] = CommonUtils.number(settingsMain[SettingConstants.SafeDrivingSoundAtMute][CommonConstants.Value])
            }
            sendEvent(SettingGuidViewEvent.CreateView, data )
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingGuidView)
              sendEvent(SettingGuidViewEvent.CloseView, data)
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
        Rectangle{
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            color: GlobalConstants.kBasicBackgoundColor
            Column {
                width: parent.width
                height: parent.height
                SettingList {
                    Component.onCompleted: settingRoot.settingList = this
                    width: parent.width
                    height: GlobalConstants.kListTwoTextItemHeight * 5
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
                Item { width: parent.width; height: 40}
                Row {
                    Component.onCompleted: {settingRoot.roadViolationNotice=this}
                    width: parent.width
                    height: 62
                    visible: false

                    Item {width:60;height: parent.height}
                    Text {
                        width: parent.width - (60*2)
                        height: parent.height

                        wrapMode: Text.WrapAnywhere

                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                        color: GlobalConstants.kListSubTextColor
                        text: textArr[textEnum.Notice]
                        lineHeightMode: Text.FixedHeight
                        lineHeight: settingRoot.layout.textLineHeight
                    }
                    Item {width:60;height:parent.height}
                }
             }
        }
    }

    ListModel { id: listModel   }

    Component {
        id: textWithSubMenu
        Column {
            width: settingList.width
            height: GlobalConstants.kListTwoTextItemHeight

            property var itemModel: null
            property var subOb: null
            property bool isSelected: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && CommonUtils.isEqual(highlightColor , settingRoot.selectColor)
            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: rightColumn.visible

            //Normal
            property color mainTxtColor      : GlobalConstants.kListTextColor
            property color subTxtColor       : GlobalConstants.kListSubTextColor

            onItemModelChanged: {
                if (CommonUtils.isUndefined(typeof itemModel) ||
                    CommonUtils.isNull(itemModel)             ||
                    CommonUtils.isUndefined(typeof itemModel.idx)) {
                    return
                }
            }



            function execButton() {
                createView(itemModel.idx)
            }

            function clickItem() {
                var idx = itemModel.idx
                settingList.list.currentIndex = idx
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
                        createView(idx)
                    } catch(e) {
                    }
                })
            }

            function normalState() {
                mainTxtColor      = GlobalConstants.kListTextColor
                subTxtColor       = GlobalConstants.kListSubTextColor
                rightColumn.state = CommonConstants.Normal
            }

            function focusState() {
                mainTxtColor      = GlobalConstants.kBasicBackgoundColor
                subTxtColor       = GlobalConstants.kListSubTextColor
                rightColumn.state = CommonConstants.Pressed
            }

            function selectState() {
                mainTxtColor      = GlobalConstants.kListTextColor
                subTxtColor       = GlobalConstants.kListTextColor
                rightColumn.state = CommonConstants.Normal
            }

            onIsSelectedChanged: {
                if (isFocused) {
                    focusState()
                } else if (isSelected) {
                    selectState()
                } else {
                    normalState()
                    isButtonFocused = false
                }
            }

            onIsFocusedChanged: {
                if (isFocused) {
                    focusState()
                } else if (isSelected) {
                    selectState()
                } else {
                    normalState()
                    isButtonFocused = false
                }
            }

            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                    normalState()
                } else {
                    if (isFocused) {
                        focusState()
                    } else if (isSelected) {
                        selectState()
                    }
                }
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
                Row {
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Loader {
                       width: parent.width
                       height:parent.height
                       Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                       sourceComponent: {
                           var ret = oneTextItem
                           return ret
                       }
                    }
                    TImageButton {
                          id: rightColumn
                          width: 63
                          height: 63
                          bg.sourceSize.width: width
                          bg.sourceSize.height: height
                          anchors.verticalCenter : parent.verticalCenter
                          bgSources: function (state) {
                              switch (state) {
                              case CommonConstants.Normal:  return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              case CommonConstants.Pressed: return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              }
                          }
                          onClicked: execButton()

                   }
                   Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
               }

               Component {
                   id: oneTextItem
                   Text {
                         width: parent.width
                         height: parent.height
                         horizontalAlignment: Qt.AlignLeft
                         verticalAlignment: Qt.AlignVCenter
                         font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                         color: mainTxtColor
                         text: itemModel.text
                    }
               }

                Component {
                    id: twoTextItem
                    Item {width:parent.width;height:parent.height
                        Column {
                               width: parent.width
                               height: CommonUtils.applyHeight(this)
                               anchors.verticalCenter: parent.verticalCenter
                               Item {width: parent.width; height: 32
                                   Text {
                                         width: parent.width
                                         height: parent.height
                                         horizontalAlignment: Qt.AlignLeft
                                         verticalAlignment: Qt.AlignVCenter
                                         font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                         color: mainTxtColor
                                         text: itemModel.text
                                    }
                               }
                               Item {width: parent.width; height: 12
                                   Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                               }
                               Item {width: parent.width; height: 28
                                   Text {
                                        width: parent.width
                                        height: parent.height
                                        horizontalAlignment: Qt.AlignLeft
                                        verticalAlignment: Qt.AlignVCenter
                                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize)}
                                        color: subTxtColor
                                        text: itemModel.value
                                        Component.onCompleted: {
                                            subOb = this
                                        }
                                   }
                               }
                        }
                    }
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

    Component {
        id: textWithOption2
        Column {
            width: settingList.width
            height: GlobalConstants.kListTwoTextItemHeight

            property var itemModel


            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: options.visible
            property alias button: options


            function execButton() {
                var idx = itemModel.idx
                var newSettings = settingsMain
                var key = listArr[idx][listAttrEnum.Key]
                var value = options.itemIndex                
                if (CommonUtils.isEqual(CommonUtils.number(itemModel.value),  value))
                    return
                setOptionProperty(idx, value)
                itemModel.value = CommonUtils.string(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                var data = ({})
                data[CommonConstants.Section] = listArr[idx][listAttrEnum.Section]
                data[CommonConstants.Settings] = newSettings
                sendEvent(SettingGuidViewEvent.SaveSetting, data )
            }

            function clickItem(){
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

                        var itemIndex = options.itemIndex
                        options.itemIndex = (++itemIndex % 2)
                    } catch(e) {
                    }
                })
            }

            function isBtnOnOff() {
                if (CommonUtils.isEqual(itemModel.option1 , toggleArr[toggleEnum.Off]) &&
                    CommonUtils.isEqual(itemModel.option2 , toggleArr[toggleEnum.On])) {
                    return true
                }
                return false
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
                    OptionButton {
                        id: options
                        width:            isBtnOnOff() ? settingRoot.layout.option2OnOffWidth : settingRoot.layout.option2Width
                        height:           isBtnOnOff() ? settingRoot.layout.option2OnOffHeight : settingRoot.layout.option2Height
                        onWidthChanged:   CommonUtils.applyRemainWidth(parent.children[1],parent)
                        items:            [itemModel.option1,itemModel.option2]
                        images:           isBtnOnOff() ? [Resource.imageUrl("setting", "toggle_off.png"), Resource.imageUrl("setting", "toggle_on.png")] : []//[Resource.imageUrl("setting", "toggle_left.png"), Resource.imageUrl("setting", "toggle_right.png")]
                        bgSource:         isBtnOnOff() ? CommonConstants.Empty : Resource.imageUrl('toggle_btn_bg_2.png')
                        selectSource:     isBtnOnOff() ? CommonConstants.Empty : Resource.imageUrl('toggle_btn_slide_1.png')
                        normalColors:     isBtnOnOff() ? [CommonConstants.Transparent, CommonConstants.Transparent] : [GlobalConstants.kListTextColor, GlobalConstants.kListTextColor]
                        selectColors:     isBtnOnOff() ? [CommonConstants.Transparent, CommonConstants.Transparent] : [GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor]


                        highlightBorderWidth: 2
                        highlightBorderColor: CommonConstants.Transparent
                        highlightColor : CommonConstants.Transparent

                        cswObj: csw
                        box.radius: height/2
                        backgroundColor: GlobalConstants.kBasicBackgoundColor


                        itemIndex : CommonUtils.number(itemModel.value)
                        textFont { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize)}
                        anchors.verticalCenter: parent.verticalCenter

                        onItemIndexChanged: execButton()
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

    Component {
        id: textWithOption3
        Column {
            width: settingList.width
            height: GlobalConstants.kListTwoTextItemHeight

            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: options.visible
            property alias button: options


            function execButton() {
                var idx = itemModel.idx
                var newSettings = settingsMain
                var key = listArr[idx][listAttrEnum.Key]
                var value = options.itemIndex                
                if (CommonUtils.isEqual(CommonUtils.number(itemModel.value),  value))
                    return
                setOptionProperty(idx , value)
                itemModel.value = CommonUtils.string(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                var data = ({})
                data[CommonConstants.Section] = listArr[idx][listAttrEnum.Section]
                data[CommonConstants.Settings] = newSettings
                sendEvent(SettingGuidViewEvent.SaveSetting, data )
				if (CommonUtils.isEqual(key, SettingConstants.RoadViolation)) {
                	showHideRoadViolationNotice(value)
                }
            }

            function clickItem(){
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

                        var itemIndex = options.itemIndex
                        options.itemIndex = (++itemIndex % 3)
                    } catch(e) {
                    }
                })

            }

            function isBtnOnOff() {
                if (CommonUtils.isEqual(itemModel.option1 , toggleArr[toggleEnum.Off]) &&
                    CommonUtils.isEqual(itemModel.option2 , toggleArr[toggleEnum.On])) {
                    return true
                }
                return false
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
                Row {
                    width: parent.width
                    height: parent.height - splitBar.height
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
                    OptionButton {
                        id: options
                        width:            settingRoot.layout.option3Width
                        height:           settingRoot.layout.option3Height                        
                        items:            [itemModel.option1,itemModel.option2,itemModel.option3]
                        bgSource:         Resource.imageUrl('toggle_btn_bg_2.png')
                        selectSource:     Resource.imageUrl('toggle_btn_slide_2.png')
                        normalColors:     [GlobalConstants.kListTextColor, GlobalConstants.kListTextColor, GlobalConstants.kListTextColor]
                        selectColors:     [GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor]


                        selectSize:        Qt.size(104,41)
                        leftMargin:        5
                        rightMargin:       5

                        cswObj: csw
                        box.radius: height/2
                        backgroundColor: GlobalConstants.kBasicBackgoundColor

                        highlightBorderWidth: 2
                        highlightBorderColor: CommonConstants.Transparent
                        highlightColor : CommonConstants.Transparent

                        itemIndex : CommonUtils.number(itemModel.value)
                        textColor: 'white'
                        textOffsetY: 1

                         textFont { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize)}
                        anchors.verticalCenter: parent.verticalCenter

                        onItemIndexChanged: execButton()
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
            var item = settingList.list.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused)) {
                if (!CommonUtils.isNull(item.button)) {
                    if (!CommonUtils.isNull(item.button.items)) {
                          item.button.list.currentIndex = -1
                    }
                }
            }

        }
        onLeftPressed: selected()
        onRightPressed: selected()
        onSpinCwPressed: {
            if (settingList.list.currentIndex < settingList.list.count - 1) {                
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                    if (!CommonUtils.isNull(item.button)) {
                        if (!CommonUtils.isNull(item.button.items)) {
                            item.button.list.currentIndex = -1
                        }
                    }
                    item.isButtonFocused = false
                }
                ++settingList.list.currentIndex
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if (0 < settingList.list.currentIndex) {                
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                    if (!CommonUtils.isNull(item.button)) {
                        if (!CommonUtils.isNull(item.button.items)) {
                            item.button.list.currentIndex = -1
                        }
                    }
                    item.isButtonFocused = false
                }
                --settingList.list.currentIndex
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
