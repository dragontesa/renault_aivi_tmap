import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingOilKindViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/setting"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


SettingView {
    id: settingRoot


    //언어 관련
    property var __lang             : null
    property bool needInit          : true

    property var caller             : null
    property var owner              : null
    property var newSettings        : null
    property bool isActivated       : false

    property int    chargeTypeValue : 0
    property int    fuelTypeValue   : 0
    property string textChargeType  : CommonConstants.Empty


    property bool focused           : false
    property color highlightColor   : CommonConstants.Transparent

    //Timer
    property var timer              : null


    //ID
    property var settingList        : null
    property var elecNotice         : null



    //Enum
    property var textEnum           : ({})
    property var oilTypeEnum        : ({})
    property var listAttrEnum       : ({})
    property var chargeTypeEnum     : ({})

    property var textArr            : ([])
    property var oilTypeArr         : ([])
    property var listArr            : ([])
    property var chargeTypeArr      : ([])

    function allocateVariant() {
    }

   function  allocateTextEnum(){
       if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
       }
       var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("유종 선택")])
        CommonUtils.insertEnum(textEnum, "Notice"    , i++, [CommonConstants.Name], [qsTr("‘환경설정 > 지도 설정 > 충전 타입’에서 차량의 충전 타입을<br>설정할 수 있습니다. (현재 설정된 충전 타입:")])
        CommonUtils.insertEnum(textEnum, "Space"     , i++, [CommonConstants.Name], [String.fromCharCode(0x20)])

       CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateOilTypeEnum(){
        if (CommonUtils.getObjectLen(oilTypeEnum) <= 0) {
            oilTypeEnum = CommonUtils.createEnum(oilTypeEnum)
        }
        CommonUtils.insertEnum(oilTypeEnum, "Gasoline",        SettingConstants.OilTypeGasoline,          [CommonConstants.Name], [qsTr("휘발유")])
        CommonUtils.insertEnum(oilTypeEnum, "PremiumGasoline", SettingConstants.OilTypePremiumGasoline,   [CommonConstants.Name], [qsTr("고급휘발유")])
        CommonUtils.insertEnum(oilTypeEnum, "Diesel",          SettingConstants.OilTypeDiesel,            [CommonConstants.Name], [qsTr("경유")])
        CommonUtils.insertEnum(oilTypeEnum, "LPG",             SettingConstants.OilTypeLPG,               [CommonConstants.Name], [qsTr("LPG")])
        CommonUtils.insertEnum(oilTypeEnum, "Elec",            SettingConstants.OilTypeElec,              [CommonConstants.Name], [qsTr("전기")])
        CommonUtils.insertEnum(oilTypeEnum, "None",            SettingConstants.OilTypeNone,              [CommonConstants.Name], [CommonConstants.Empty])

        CommonUtils.fetchArr(oilTypeEnum, CommonConstants.Name, oilTypeArr)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"    , i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"      , i++)
        CommonUtils.insertEnum(listAttrEnum, "Name"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Value"    , i++)
        CommonUtils.insertEnum(listAttrEnum, "Section"  , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"      , i++)


        //검색 || 설정
        var section = CommonUtils.isEqual(owner , CommonConstants.objectName(ViewRegistry.SettingMapView)) ? SettingConstants.MapSubOilType : SettingConstants.SearchOption
        var key = SettingConstants.OilKind

        listArr = [
            ["TextWithRadio", textWithRadio, oilTypeArr[oilTypeEnum.Gasoline]        , oilTypeEnum.Gasoline               , section, key],
            ["TextWithRadio", textWithRadio, oilTypeArr[oilTypeEnum.PremiumGasoline] , oilTypeEnum.PremiumGasoline        , section, key],
            ["TextWithRadio", textWithRadio, oilTypeArr[oilTypeEnum.Diesel]          , oilTypeEnum.Diesel                 , section, key],            
        ]
        if (CommonUtils.isEqual(owner , CommonConstants.objectName(ViewRegistry.SettingMapView))) {
        	//v 1.33 반영- 주변검색의 주유소 검색시 옵션의 유종에 LPG삭제
            listArr[listArr.length] = ["TextWithRadio", textWithRadio, oilTypeArr[oilTypeEnum.LPG]    , oilTypeEnum.LPG  , section, key]
            
            listArr[listArr.length] = ["TextWithRadio", textWithRadio, oilTypeArr[oilTypeEnum.Elec]   , oilTypeEnum.Elec , section, key]
        }
    }    

    function allocateChargeTypeEnum(){
        if (CommonUtils.getObjectLen(chargeTypeEnum) <= 0) {
            chargeTypeEnum = CommonUtils.createEnum(chargeTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(chargeTypeEnum, "All",         i++, [CommonConstants.Name], [qsTr("전체")])
        CommonUtils.insertEnum(chargeTypeEnum, "AC3",         i++, [CommonConstants.Name], [qsTr("AC3상")])
        CommonUtils.insertEnum(chargeTypeEnum, "DCdemo",      i++, [CommonConstants.Name], [qsTr("DC차데모")])
        CommonUtils.insertEnum(chargeTypeEnum, "DCcombo",     i++, [CommonConstants.Name], [qsTr("DC콤보")])
        CommonUtils.insertEnum(chargeTypeEnum, "SlowSpeed",   i++, [CommonConstants.Name], [qsTr("완속")])

        CommonUtils.fetchArr(chargeTypeEnum, CommonConstants.Name, chargeTypeArr)
    }


    function initEnum() {
        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateOilTypeEnum()
        allocateListAttrEnum()
        allocateChargeTypeEnum()
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        initEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingOilKindViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingOilKindViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        init()
        sendEvent(SettingOilKindViewEvent.ResetCsw)
        sendEvent(SettingOilKindViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingOilKindViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingOilKindViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingOilKindViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function requestSetting() {
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Slot] = CommonUtils.isEqual(owner , CommonConstants.objectName(ViewRegistry.SettingMapView)) ? "__resultSettingOilType" : "__resultSearchOption"
        sendEvent(SettingOilKindViewEvent.RequestSetting, data )

        //전기차 충전 타입
        data = ({})
        data[CommonConstants.Section] = SettingConstants.MapSubChargeType
        data[CommonConstants.Slot] = "__resultSettingEvChargeType"
        sendEvent(SettingOilKindViewEvent.RequestSetting, data )

        resetCreateModel()
    }    

    function saveSetting() {
        if (CommonUtils.isNull(newSettings))
            return
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingOilKindViewEvent.SaveSetting, data )
    }

    function resultOilTypeAndOption(settings) {
        if (CommonUtils.isNull(settings))
            return
        var key = listArr[0][listAttrEnum.Key]
        if (!settings.hasOwnProperty(key))
            return
        var value = settings[key][CommonConstants.Value]
        setRadioProperty(value)
        newSettings = settings
    }

    function __resultSearchOption(settings) {
        resultOilTypeAndOption(settings)
    }

    function __resultSettingOilType(settings) {
        resultOilTypeAndOption(settings)
    }

    function __resultSettingEvChargeType(settings) {
        if (CommonUtils.isNull(settings))
            return
        var key = SettingConstants.ChargeType
        if (!settings.hasOwnProperty(key))
            return
        var value = settings[key][CommonConstants.Value]
        chargeTypeValue = CommonUtils.number(value)
    }

    function showHideElecNotice(value) {
        elecNotice.visible = CommonUtils.isEqual(CommonUtils.number(value), oilTypeEnum.Elec)
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
        showHideElecNotice(value)
        fuelTypeValue = CommonUtils.number(value)
        for (var idx = 0; idx < listArr.length; idx++) {
            if (CommonUtils.isEqual(listArr[idx][listAttrEnum.Value] , CommonUtils.number(value))){
                listModel.setProperty(idx, CommonConstants.Value, CommonUtils.string(CommonConstants.True))
            } else {
                listModel.setProperty(idx, CommonConstants.Value, CommonUtils.string(CommonConstants.False))
            }
        }
    }

    function resetCreateModel() {
        textChargeType = chargeTypeArr[chargeTypeValue]
        showHideElecNotice(fuelTypeValue)
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingOilKindView)
              sendEvent(SettingOilKindViewEvent.CloseView, data)
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
        Rectangle {
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
           color: GlobalConstants.kBasicBackgoundColor
           Column{
               width: parent.width
               height: parent.height               
               SettingList {
                   Component.onCompleted: settingRoot.settingList = this
                   width: parent.width
                   height: GlobalConstants.kListTextItemHeight * 5
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
                 Component.onCompleted: {settingRoot.elecNotice=this}
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
                     text: textArr[textEnum.Notice] + textArr[textEnum.Space] + textChargeType + ")"
                     lineHeightMode: Text.FixedHeight
                     lineHeight: settingRoot.layout.textLineHeight
                 }
                 Item {width:60;height:parent.height}
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
                var idx =  itemModel.idx
                settingList.list.currentIndex = idx

                var key = listArr[idx][listAttrEnum.Key]
                var value = listArr[idx][listAttrEnum.Value]
                setRadioProperty(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                saveSetting()
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
                            radioBtn.checked = CommonConstants.True
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
                        color: ((isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor)
                        text: itemModel.text
                    }
                    ImageRadioButton {
                        id: radioBtn
                        width: settingRoot.layout.radioboxWidth
                        height:  settingRoot.layout.radioboxHeight
                        anchors.verticalCenter: parent.verticalCenter
                        exclusiveGroup: group
                        checked: CommonUtils.number(itemModel.value)
                        img.sourceSize.height: height
                        img.sourceSize.width: width
                        imgPressed: Resource.imageUrl("radio_on.png")
                        imgNormal: Resource.imageUrl("radio_off.png")
                        onButtonClicked: execButton()
                    }
                    Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
                }
            }
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: CommonUtils.isEqual(itemModel.idx , 0) ? 2 : 1
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
