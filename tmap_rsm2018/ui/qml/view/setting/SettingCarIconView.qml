import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingCarIconViewEvent 1.0
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

    property int  listTopMargin         : 10
    property int  listBottomMargin      : 10
    property int gridRow                : 2
    property int gridCol                : 3
    property int gridEmptyCount         : 0

    property var caller                 : null
    property var settings               : null
    property var newSettings            : settings
    property bool isActivated           : false

    property bool focused               : false
    property color highlightColor       : CommonConstants.Transparent

    //Timer
    property var timer                  : null

    //ID
    property var settingGrid            : null

    //Enum
    property var textEnum               : ({})
    property var carIconEnum            : ({})
    property var listAttrEnum           : ({})

    property var textArr                : ([])
    property var carIconArr             : ([])
    property var listArr                : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("자동차 아이콘")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateCarIconEnum(){
        if (CommonUtils.getObjectLen(carIconEnum) <= 0) {
            carIconEnum = CommonUtils.createEnum(carIconEnum)
        }
        CommonUtils.insertEnum(carIconEnum, "Basic0",      SettingConstants.CarIconBasic0,    [CommonConstants.Url], ["car_ic_1.png"])
        CommonUtils.insertEnum(carIconEnum, "Basic1",      SettingConstants.CarIconBasic1,    [CommonConstants.Url], ["car_ic_2.png"])
        CommonUtils.insertEnum(carIconEnum, "Basic2",      SettingConstants.CarIconBasic2,    [CommonConstants.Url], ["car_ic_3.png"])

        CommonUtils.fetchArr(carIconEnum, CommonConstants.Url, carIconArr)

        if (CommonUtils.isNull(gridRow)) {
            gridRow = getGridRow(carIconArr)
        }

        gridEmptyCount = 0
        for(var i=0; i<carIconArr.length; i++) {
            if (CommonUtils.isEmpty(carIconArr[i])) {
                ++gridEmptyCount
            }
        }
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"   , i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "Image"   , i++)
        CommonUtils.insertEnum(listAttrEnum, "Section" , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"     , i++)

        listArr = [
               ['ImageWithRadio', imageWithRadio, carIconArr[carIconEnum.Basic0]   , SettingConstants.MapSubCarIcon, SettingConstants.Icon],
               ['ImageWithRadio', imageWithRadio, carIconArr[carIconEnum.Basic1]   , SettingConstants.MapSubCarIcon, SettingConstants.Icon],
               ['ImageWithRadio', imageWithRadio, carIconArr[carIconEnum.Basic2]   , SettingConstants.MapSubCarIcon, SettingConstants.Icon],
            ]
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()

        allocateCarIconEnum()
        allocateListAttrEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingCarIconViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingCarIconViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        if (needInit) {
            init()
        }
        sendEvent(SettingCarIconViewEvent.ResetCsw)
        sendEvent(SettingCarIconViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingCarIconViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingCarIconViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        sendEvent(SettingCarIconViewEvent.RequestSetting, data )

        resetCreateModel()
    }

    function saveSetting() {
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingCarIconViewEvent.SaveSetting, data )
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        var key = listArr[0][listAttrEnum.Key]
        if (!settings.hasOwnProperty(key))
              return
        var idx = settings[key][CommonConstants.Value]
        //"고객사 아이콘" 삭제로 인하여 고객사인 경우 디폴트로 변경 - v 1.33 반영
        if (CommonUtils.isEmpty(carIconArr[idx])) {
            idx = carIconEnum.Basic0
        }
        setRadioProperty(idx)
        settingGrid.grid.currentIndex = idx
    }

    function createModel(){        
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingGrid.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            listModel.set(i,
            {
                component: componentName,
                idx : i,
                image: listArr[i][listAttrEnum.Image],
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
        settingRoot.settingGrid.grid.model = null
        settingRoot.settingGrid.grid.model = listModel
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingCarIconView)
              sendEvent(SettingCarIconViewEvent.CloseView, data)
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
                SettingGrid {
                   Component.onCompleted: settingRoot.settingGrid = this
                   width: parent.width
                   height: parent.height
                   rowHeight: height/gridRow
                   grid.cellWidth: parent.width/gridCol
                   grid.cellHeight: rowHeight
                   grid.clip: true
                   grid.model: listModel
                   grid.currentIndex: -1
                   grid.interactive: false

                   property var pressedItem: null
                   PressBar {
                       currentItem: settingGrid.pressedItem
                   }
                   grid.highlight: HighlightBar {
                       currentItem: settingGrid.grid.currentItem
                       color: highlightColor
                   }
                   grid.highlightFollowsCurrentItem: false

                   TScrollBar {
                       Component.onCompleted: parent.scrollbar = this
                       flickable: parent.grid
                       width: 2
                       color: MultiSenseCoordinator.kAmbientColor
                       grooveColor: GlobalConstants.kScrollBarBackColor
                       rightMargin: GlobalConstants.kScrollBarRightMargin
                       onScroll:    settingGrid.imgMask.visible = true
                       onMouseRelease:  settingGrid.imgMask.visible = false
                   }
                }
            }
         }
    }

    ListModel { id: listModel   }

    ExclusiveGroup { id: group }

    Component {
        id: imageWithRadio
        Item {
            width: settingGrid.grid.cellWidth
            height: settingGrid.grid.cellHeight
            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingGrid.grid.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: radioBtn.visible

            property int row: itemModel.idx / gridCol
            property int col: itemModel.idx % gridCol


            function execButton() {
                var idx = itemModel.idx
                settingGrid.grid.currentIndex = idx

                var key = listArr[idx][listAttrEnum.Key]
                var value = idx
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
                settingGrid.grid.currentIndex = itemModel.idx
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
                            //settingGrid.grid.currentIndex = -1
                        }

                        if (radioBtn.enabled) {
                            radioBtn.checked = CommonConstants.True
                            radioBtn.buttonClicked()
                        }
                    } catch(e) {
                    }
                })
            }

            MouseArea {
                width: parent.width
                height: parent.height
                enabled: !CommonUtils.isEmpty(itemModel.image)

                PressBarMouseSync {
                    cswObj: csw
                    currentItem: settingGrid.pressedItem
                    onCurrentItemChanged: {
                        if (CommonUtils.isEqual(currentItem, settingGrid.pressedItem)) {
                            return
                        }
                        settingGrid.pressedItem = currentItem
                    }
                }

                onClicked: clickItem()
            }
            Item {
                width:parent.width
                height:parent.height
                visible: !CommonUtils.isEmpty(itemModel.image)
                Column {
                    width: parent.width
                    height: parent.height
                    Item {width: parent.width; height: 31}
                    Item {
                        id: icon
                        width: parent.width
                        height: parent.height
                        Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                        Image {
                            width:  settingRoot.layout.carIconWidth
                            height:  settingRoot.layout.carIconHeight
                            sourceSize.width: width
                            sourceSize.height: height
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            source : Resource.imageUrl("setting",itemModel.image)
                            visible: !CommonUtils.isEmpty(itemModel.image)
                        }
                    }
                    Item {width: parent.width; height: 20}
                    ImageRadioButton {
                        id: radioBtn
                        width:  settingRoot.layout.radioboxWidth
                        height: settingRoot.layout.radioboxHeight
                        anchors.horizontalCenter: parent.horizontalCenter
                        exclusiveGroup: group
                        checked: CommonUtils.number(itemModel.value)
                        img.sourceSize.height: height
                        img.sourceSize.width: width
                        imgPressed: Resource.imageUrl("radio_on.png")
                        imgNormal: Resource.imageUrl("radio_off.png")
                        visible: !CommonUtils.isEmpty(itemModel.image)
                        enabled: visible
                        onButtonClicked: execButton()
                    }
                    Item {width: parent.width; height: 42}
                }
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
            settingGrid.grid.currentIndex = -1
            settingGrid.grid.currentIndex = 0
        }
        onLostFocusd: {
            highlightColor = CommonConstants.Transparent
            settingRoot.focused = false
        }
        onLeftPressed: selected()
        onRightPressed: selected()
        onSpinCwPressed: {
            if (settingGrid.grid.currentIndex < (settingGrid.grid.count - (gridEmptyCount + 1))) {
                ++settingGrid.grid.currentIndex
            } else {
                settingGrid.grid.currentIndex = 0
            }
            var item = settingGrid.grid.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                item.isButtonFocused = false
            }
            highlightColor = GlobalConstants.kListFocusColor
        }
        onSpinCcwPressed: {
            if (0 < settingGrid.grid.currentIndex) {
                --settingGrid.grid.currentIndex
            } else {
                settingGrid.grid.currentIndex = (settingGrid.grid.count - (gridEmptyCount + 1))
            }
            var item = settingGrid.grid.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused))
                item.isButtonFocused = false
            highlightColor = GlobalConstants.kListFocusColor

        }
        onSelected: {
            var item = settingGrid.grid.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused)   && item.isButtonFocused) {
                item.execButton()
            } else {
                item.clickItem()
            }
        }
    }
}
