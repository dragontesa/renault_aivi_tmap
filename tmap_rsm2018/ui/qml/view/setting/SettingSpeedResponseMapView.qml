import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingSpeedResponseMapViewEvent 1.0
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

    background.visible: true
    background.color: GlobalConstants.kBasicBackgoundColor


    //언어 관련
    property var __lang             : null
    property bool needInit          : true

    property var caller             : null
    property var settings           : null
    property var newSettings        : settings
    property bool isActivated       : false

    property var descObj            : ([])
    property var radioObj           : ([])

    //인덱스
    property int descIdx            : 0
    property int radioIdx           : 0
    property int selectedRadioIdx   : 0


    property bool focused           : false
    property color highlightColor   : CommonConstants.Transparent

    //Timer
    property var timer              : null

    //ID
    property var settingList        : null
    property var map                : null

    //Enum
    property var textEnum           : ({})
    property var distanceViewEnum   : ({})
    property var componentEnum      : ({})
    property var listAttrEnum       : ({})

    property var textArr            : ([])
    property var distanceViewArr    : ([])
    property var componentCNameArr  : ([])
    property var componentCIDArr    : ([])
    property var listArr            : ([])

    function allocateVariant() {
        radioIdx = 0
        descIdx = 0
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",       i++, [CommonConstants.Name], [qsTr("속도 반응 지도")])
        CommonUtils.insertEnum(textEnum, "SampleMap",        i++, [CommonConstants.Name], [qsTr("지도 예시 이미지 영역")])
        CommonUtils.insertEnum(textEnum, "FourtyKm",         i++, [CommonConstants.Name], [qsTr("40km/h 이하")])
        CommonUtils.insertEnum(textEnum, "FourtyToEightyKm", i++, [CommonConstants.Name], [qsTr("40~80km/h")])
        CommonUtils.insertEnum(textEnum, "EightyKm",         i++, [CommonConstants.Name], [qsTr("80km/h이상")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateDistanceViewEnum(){
        if (CommonUtils.getObjectLen(distanceViewEnum) <= 0) {
            distanceViewEnum = CommonUtils.createEnum(distanceViewEnum)
        }
        var i = 0
        CommonUtils.insertEnum(distanceViewEnum, "Far",    i++, [CommonConstants.Name], [qsTr("지도 더 멀리 보기")])
        CommonUtils.insertEnum(distanceViewEnum, "Basic",  i++, [CommonConstants.Name], [qsTr("지도 기본 보기(추천)")])
        CommonUtils.insertEnum(distanceViewEnum, "Detail", i++, [CommonConstants.Name], [qsTr("지도 더 자세히 보기")])

        CommonUtils.fetchArr(distanceViewEnum, CommonConstants.Name, distanceViewArr)
    }

    function allocateComponentEnum(){
        if (CommonUtils.getObjectLen(componentEnum) <= 0) {
            componentEnum = CommonUtils.createEnum(componentEnum)
        }
        var i = 0
        CommonUtils.insertEnum(componentEnum, "Option", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithOption2", textWithOption2])
        CommonUtils.insertEnum(componentEnum, "Radio",  i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithRadio", textWithRadio])

        CommonUtils.fetchArr(componentEnum, CommonConstants.Name, componentCNameArr)
        CommonUtils.fetchArr(componentEnum, CommonConstants.Kind, componentCIDArr)
    }

   function allocateListAttrEnum(){
        if (CommonUtils.isEmpty(toggleArr)) { allocateToggleEnum() }
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

       listArr = [
               [componentCNameArr[componentEnum.Option],
                componentCIDArr[componentEnum.Option],
                qsTr("속도 반응 지도 사용"),
                SettingConstants.MapSubSpeedResponse,
                SettingConstants.SpeedResponseMapUse,
                toggleArr[toggleEnum.Off],
                toggleArr[toggleEnum.On]],
               [componentCNameArr[componentEnum.Radio ],
                componentCIDArr[componentEnum.Radio ],
                distanceViewArr[distanceViewEnum.Far],
                SettingConstants.MapSubSpeedResponse,
                SettingConstants.Distance],
               [componentCNameArr[componentEnum.Radio ],
                componentCIDArr[componentEnum.Radio ],
                distanceViewArr[distanceViewEnum.Basic],
                SettingConstants.MapSubSpeedResponse,
                SettingConstants.Distance],
               [componentCNameArr[componentEnum.Radio ],
                componentCIDArr[componentEnum.Radio ],
                distanceViewArr[distanceViewEnum.Detail],
                SettingConstants.MapSubSpeedResponse,
                SettingConstants.Distance]
           ]
    }

    function requestSetting() {
        settings = null
        selectedRadioIdx = 0
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        sendEvent(SettingSpeedResponseMapViewEvent.RequestSetting, data )

        resetCreateModel()
    }

    function saveSetting() {
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingSpeedResponseMapViewEvent.SaveSetting, data )
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listArr[i][listAttrEnum.Name],
                    option1: listArr[i][listAttrEnum.Arg0],
                    option2: listArr[i][listAttrEnum.Arg1],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })
            } else  if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Radio])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listArr[i][listAttrEnum.Name],
                    enabled: CommonUtils.string(CommonConstants.False),
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)

                })
            }
        }
    }

    function setOptionProperty(value) {
        listModel.setProperty(componentEnum.Option, CommonConstants.Value,  CommonUtils.string(value))
    }

    function setRadioProperty(value) {
        for (var i = componentEnum.Radio; i<listArr.length; i++) {
            listModel.setProperty(i, CommonConstants.Value,  CommonUtils.string(CommonConstants.False))
        }
        listModel.setProperty(componentEnum.Radio + CommonUtils.number(value), CommonConstants.Value,  CommonUtils.string(CommonConstants.True))
        selectedRadioIdx = CommonUtils.number(value)
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
        allocateToggleEnum()

        allocateTextEnum()
        allocateDistanceViewEnum()
        allocateComponentEnum()
        allocateListAttrEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingSpeedResponseMapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingSpeedResponseMapViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        init()
        sendEvent(SettingSpeedResponseMapViewEvent.ResetCsw)
        sendEvent(SettingSpeedResponseMapViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingSpeedResponseMapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingSpeedResponseMapViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingSpeedResponseMapViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        for (var i=0; i< componentCNameArr.length; i++) {
            var key = listArr[i] [listAttrEnum.Key]
            if (!settings.hasOwnProperty(key))
                continue
            var value = settings[key][CommonConstants.Value]
            if (CommonUtils.isEqual(i , componentEnum.Option)) {
                setOptionProperty(value)
                enableRadioBtn(value)
            } else if ( CommonUtils.isEqual(i , componentEnum.Radio)) {
                setRadioProperty(value)
            }
        }        
    }


    function enableRadioBtn(value) {
        map.enabled = CommonUtils.number(value)
        for (var i = componentEnum.Radio; i<listArr.length; i++) {
            listModel.setProperty(i, "enabled", CommonUtils.string(value))
            if (CommonUtils.isNull(radioObj[i]) || CommonUtils.isNull(descObj[i]))
                continue
            radioObj[i].enabled = CommonUtils.number(value)
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
                data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingSpeedResponseMapView)
                sendEvent(SettingSpeedResponseMapViewEvent.CloseView, data)
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
                id: listBg
                width: parent.width
                height:  (GlobalConstants.kListTextItemHeight * 4)
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
                }
            }
            Row {
                    Component.onCompleted: settingRoot.map = this
                    width: parent.width
                    height: parent.height - listBg.height
                    opacity: enabled ? 1.0 : 0.3
                    Image {
                        width: (parent.width-2)/3
                        height:parent.height
                        sourceSize.width: width
                        sourceSize.height: height
                        source : {
                            var imgName
                            var value = selectedRadioIdx
                            switch(CommonUtils.number(value)) {
                                case 0:  imgName = "scale_10_5.png"; break;
                                case 1:  imgName = "scale_11.png"; break;
                                case 2:  imgName = "scale_11_5.png"; break;
                             }
                             return Resource.imageUrl("setting", imgName)
                        }
                        Rectangle {
                            width: 170
                            height: 48
                            color: GlobalConstants.kNormalColor
                            opacity: 0.85
                            Text {
                                width: parent.width
                                height: parent.height
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27)}
                                color: GlobalConstants.kListTextColor
                                text: textArr[textEnum.FourtyKm]
                            }
                        }
                    }
                    Rectangle {
                     width: 1
                     height: parent.height
                     color: CommonConstants.Black
                    }
                    Image {
                        width: (parent.width-2)/3
                        height:parent.height
                        sourceSize.width: width
                        sourceSize.height: height
                        source : {
                            var imgName
                            var value = selectedRadioIdx
                            switch(CommonUtils.number(value)) {
                             case 0:  imgName = "scale_10.png"; break;
                             case 1:  imgName = "scale_10_5.png"; break;
                             case 2:  imgName = "scale_11.png"; break;
                            }
                            return Resource.imageUrl("setting", imgName)
                        }
                        Rectangle {
                            width: 170
                            height: 48
                            color: GlobalConstants.kNormalColor
                            opacity: 0.85
                            Text {
                                width: parent.width
                                height: parent.height
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27)}
                                color: GlobalConstants.kListTextColor
                                text: textArr[textEnum.FourtyToEightyKm]
                            }
                        }
                    }
                    Rectangle {
                     width: 1
                     height: parent.height
                     color: CommonConstants.Black
                    }
                    Image {
                        width: (parent.width-2)/3
                        height: parent.height
                        sourceSize.width: width
                        sourceSize.height: height
                        source : {
                            var imgName
                            var value = selectedRadioIdx
                            switch(CommonUtils.number(value)) {
                             case 0: imgName = "scale_9_5.png"; break;
                             case 1: imgName = "scale_10.png"; break;
                             case 2: imgName = "scale_10_5.png"; break;
                            }
                            return Resource.imageUrl("setting", imgName)
                        }
                        Rectangle {
                            width: 170
                            height: 48
                            color: GlobalConstants.kNormalColor
                            opacity: 0.85
                            Text {
                                width: parent.width
                                height: parent.height
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27)}
                                color: GlobalConstants.kListTextColor
                                text: textArr[textEnum.EightyKm]
                            }
                        }
                    }
                }
        }
    }


    ListModel { id: listModel   }
   
    ExclusiveGroup { id: group }

   Component {
       id: textWithOption2
       Column {
           width: settingList.width
           height: GlobalConstants.kListTextItemHeight
           property var itemModel

           property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
           property bool isButtonFocused: false
           property alias buttonVisible: options.visible
           property alias button: options


           function execButton() {
               var key = listArr[itemModel.idx][listAttrEnum.Key]
               var value = options.itemIndex
               setOptionProperty(value)
               newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
               enableRadioBtn(value)
               saveSetting()
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
                       font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize)}
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

                       cswObj: csw
                       box.radius: height/2
                       backgroundColor: GlobalConstants.kBasicBackgoundColor


                       highlightBorderWidth: 2
                       highlightBorderColor: CommonConstants.Transparent
                       highlightColor : CommonConstants.Transparent


                       itemIndex: CommonUtils.number(itemModel.value)
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
              height: CommonUtils.isEqual(itemModel.idx , 0) ? 2 : 1
              color: GlobalConstants.kListLineColor
           }
       }
   }

   Component {
        id: textWithRadio
        Column {
            width: settingList.width
            height: GlobalConstants.kListTextItemHeight
            opacity: radioBtn.enabled ? 1.0 : 0.3
            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: radioBtn.visible


            function execButton() {
                options.execButton()
            }

            function clickItem(){                
                if (!radioBtn.enabled) {
                    return
                }
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
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Text {
                        width: parent.width
                        height: parent.height
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize)}
                        color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                        text: itemModel.text
                        Component.onCompleted: {
                            descObj[++descIdx] = this
                            CommonUtils.applyRemainWidth(this,parent)
                        }                        
                    }
                    ImageRadioButton {
                        id: radioBtn
                        width:  settingRoot.layout.radioboxWidth
                        height:  settingRoot.layout.radioboxHeight
                        anchors.verticalCenter: parent.verticalCenter
                        exclusiveGroup: group
                        checked: CommonUtils.number(itemModel.value)
                        enabled: CommonUtils.number(itemModel.enabled)
                        img.sourceSize.height: height
                        img.sourceSize.width: width
                        imgPressed: Resource.imageUrl("radio_on.png")
                        imgNormal: Resource.imageUrl("radio_off.png")
                        Component.onCompleted: {
                            radioObj[++radioIdx] = this
                        }
                        onButtonClicked: {
                            var idx = itemModel.idx
                            settingList.list.currentIndex = idx

                            var startIdx = componentEnum.Radio
                            var key = listArr[idx][listAttrEnum.Key]
                            var value = idx - startIdx

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
               if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
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
           if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
               item.execButton()
           } else {
               item.clickItem()
           }
       }
   }
}
