import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingMapViewEvent 1.0
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

    property bool sync              : true
    property bool reFocus           : false

    //언어 관련
    property var __lang             : null
    property bool needInit          : true


    property var caller             : null
    property var settings           : null
    property var settingsMain       : null
    property bool isActivated       : false

    property int reqIdx             : 0
    property bool reqContinue       : true


    property bool focused           : false
    property color selectColor      : MultiSenseCoordinator.kAmbientColor
    property color highlightColor   : CommonConstants.Transparent

    //Timer
    property var timer              : null

    //ID
    property var settingList        : null

    //Enum
    property var textEnum           : ({})
    property var oilTypeEnum        : ({})
    property var chargeTypeEnum     : ({})
    property var componentEnum      : ({})
    property var reqEnum            : ({})
    property var nightModeEnum      : ({})
    property var addrDispMethodEnum : ({})
    property var listAttrEnum       : ({})

    property var textArr            : ([])
    property var oilTypeArr         : ([])
    property var chargeTypeArr      : ([])
    property var componentCNameArr  : ([])
    property var componentCIDArr    : ([])
    property var listNameArr        : ([])
    property var listSectionArr     : ([])
    property var listKeyArr         : ([])
    property var listUrlArr         : ([])
    property var nightModeArr       : ([])
    property var addrDispMethodArr  : ([])
    property var listArr            : ([])
    property var listObjArr         : ([])
    property var subObjArr          : ([])
    property var speedResponseArr   : ([])
    property var mapCharSizeArr     : ([])
    property var carIconArr         : ([])
    property var subVarArr          : ([])


    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title", i++, [CommonConstants.Name], [qsTr("지도설정")])

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
        CommonUtils.insertEnum(reqEnum, "MapSubSpeedResponse",  i++,
                                                                [CommonConstants.Name,
                                                                 CommonConstants.Section,
                                                                 CommonConstants.Key,
                                                                 CommonConstants.Url],
                                                                [qsTr("속도 반응 지도"),
                                                                 SettingConstants.MapSubSpeedResponse,
                                                                 SettingConstants.Distance,
                                                                 CommonConstants.url(ViewRegistry.SettingSpeedResponseMapView) ])
        CommonUtils.insertEnum(reqEnum, "MapSubMapCharacter",   i++,
                                                                [CommonConstants.Name,
                                                                 CommonConstants.Section,
                                                                 CommonConstants.Key,
                                                                 CommonConstants.Url],
                                                                [qsTr("주행 중 지도 글씨 크기"),
                                                                 SettingConstants.MapSubMapCharacter,
                                                                 SettingConstants.Character,
                                                                 CommonConstants.url(ViewRegistry.SettingMapCharSizeView) ])
        CommonUtils.insertEnum(reqEnum, "MapSubCarIcon",        i++,
                                                                [CommonConstants.Name,
                                                                 CommonConstants.Section,
                                                                 CommonConstants.Key,
                                                                 CommonConstants.Url],
                                                                [qsTr("자동차 아이콘"),
                                                                 SettingConstants.MapSubCarIcon,
                                                                 SettingConstants.Icon,
                                                                 CommonConstants.url(ViewRegistry.SettingCarIconView) ])
        CommonUtils.insertEnum(reqEnum, "MapSubOilType",        i++,
                                                                [CommonConstants.Name,
                                                                 CommonConstants.Section,
                                                                 CommonConstants.Key,
                                                                 CommonConstants.Url],
                                                                [qsTr("유종 선택"),
                                                                 SettingConstants.MapSubOilType,
                                                                 SettingConstants.OilKind,
                                                                 CommonConstants.url(ViewRegistry.SettingOilKindView) ])
        CommonUtils.insertEnum(reqEnum, "MapSubChargeType",     i++,
                                                                [CommonConstants.Name,
                                                                 CommonConstants.Section,
                                                                 CommonConstants.Key,
                                                                 CommonConstants.Url],
                                                                [qsTr("충전타입"),
                                                                 SettingConstants.MapSubChargeType,
                                                                 SettingConstants.ChargeType,
                                                                 CommonConstants.url(ViewRegistry.SettingEvChargeTypeView)])

        CommonUtils.insertEnum(reqEnum, "MapMain",              i++,
                                                                [CommonConstants.Section], [SettingConstants.MapMain])

        CommonUtils.fetchArr(reqEnum, CommonConstants.Name, listNameArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Section, listSectionArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Key, listKeyArr)
        CommonUtils.fetchArr(reqEnum, CommonConstants.Url, listUrlArr)

        var len = listNameArr.length
        for(i=0; i<len;i++){
            listObjArr[i] = null
            subObjArr[i] = null
        }
    }

    function allocateNightModeEnum(){
        if (CommonUtils.getObjectLen(nightModeEnum) <= 0) {
            nightModeEnum = CommonUtils.createEnum(nightModeEnum)
        }
        CommonUtils.insertEnum(nightModeEnum, "Auto",     SettingConstants.NightModeAuto,     [CommonConstants.Name], [qsTr("자동")])
        CommonUtils.insertEnum(nightModeEnum, "Daylight", SettingConstants.NightModeDaylight, [CommonConstants.Name], [qsTr("주간")])
        CommonUtils.insertEnum(nightModeEnum, "Night",    SettingConstants.NightModeNight,    [CommonConstants.Name], [qsTr("야간")])

        CommonUtils.fetchArr(nightModeEnum, CommonConstants.Name, nightModeArr)
    }

    function allocateaddrDispMethodEnum(){
        if (CommonUtils.getObjectLen(addrDispMethodEnum) <= 0) {
            addrDispMethodEnum = CommonUtils.createEnum(addrDispMethodEnum)
        }
        CommonUtils.insertEnum(addrDispMethodEnum, "RoadNumber", SettingConstants.AddressDisplayMethodRoadName,  [CommonConstants.Name], [qsTr("도로명")])
        CommonUtils.insertEnum(addrDispMethodEnum, "LandNumber", SettingConstants.AddressDisplayMethodLandNumber,[CommonConstants.Name], [qsTr("지번")])


        CommonUtils.fetchArr(addrDispMethodEnum, CommonConstants.Name, addrDispMethodArr)
    }


    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"    ,i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"      ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Name"     ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Section"  ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"      ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg0"     ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg1"     ,i++)
        CommonUtils.insertEnum(listAttrEnum, "Arg2"     ,i++)

        //추후 인덱스와 자동으로 매치되도록 수정 필요
        listArr = [
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr   [reqEnum.MapSubSpeedResponse],
             listSectionArr[reqEnum.MapSubSpeedResponse],
             listKeyArr    [reqEnum.MapSubSpeedResponse]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr   [reqEnum.MapSubMapCharacter],
             listSectionArr[reqEnum.MapSubMapCharacter],
             listKeyArr    [reqEnum.MapSubMapCharacter]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr   [reqEnum.MapSubCarIcon],
             listSectionArr[reqEnum.MapSubCarIcon],
             listKeyArr    [reqEnum.MapSubCarIcon]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr   [reqEnum.MapSubOilType],
             listSectionArr[reqEnum.MapSubOilType],
             listKeyArr    [reqEnum.MapSubOilType]
            ],
            [componentCNameArr[componentEnum.SubMenu],
             componentCIDArr[componentEnum.SubMenu],
             listNameArr   [reqEnum.MapSubChargeType],
             listSectionArr[reqEnum.MapSubChargeType],
             listKeyArr    [reqEnum.MapSubChargeType]
            ],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("초기화면 최근 목적지 표시"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.RecentDestinations,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
            [componentCNameArr[componentEnum.Option3],
             componentCIDArr[componentEnum.Option3],
             qsTr("야간모드"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.NightMode,
             nightModeArr[nightModeEnum.Auto],
             nightModeArr[nightModeEnum.Daylight],
             nightModeArr[nightModeEnum.Night]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("주소 표시 방법"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.AddressDisplayMethod,
             addrDispMethodArr[addrDispMethodEnum.RoadNumber],
             addrDispMethodArr[addrDispMethodEnum.LandNumber]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("주변 교통정보 표시"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.DisplayArroundTrafficInfo,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("주행선 교통정보 항상 표시"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.AlwaysDisplayDriveLineTrafficInfo,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("고속도로 주행안내"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.ExpresswayDrivingGuidance,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("GPS속도 표시"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.GpsSpeed,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
            [componentCNameArr[componentEnum.Option2],
             componentCIDArr[componentEnum.Option2],
             qsTr("스카이 박스"),
             listSectionArr[reqEnum.MapMain],
             SettingConstants.SkyeBox,
             toggleArr[toggleEnum.Off],
             toggleArr[toggleEnum.On]],
        ]
    }

    function allocateSpeedResponseArr() {
        var i = 0
        speedResponseArr[i++] = qsTr("더 멀리 보기")
        speedResponseArr[i++] = qsTr("기본 보기")
        speedResponseArr[i++] = qsTr("더 자세히 보기")
    }

    function allocateMapCharSizeArr() {
        var i = 0
        mapCharSizeArr[i++] = qsTr("작게")
        mapCharSizeArr[i++] = qsTr("보통")
        mapCharSizeArr[i++] = qsTr("크게")
    }

    function allocateCarIconArr() {
        var i = 0
        carIconArr[i++] = qsTr("기본 1")
        carIconArr[i++] = qsTr("기본 2")
        carIconArr[i++] = qsTr("기본 3")
        carIconArr[i++] = qsTr("르노 1")
        carIconArr[i++] = qsTr("르노 2")
        carIconArr[i++] = qsTr("르노 3")
    }

    function allocateSubVarArr() {
        var i = 0
        subVarArr[i++] = speedResponseArr//[주의] 동적으로 배열변수 변경되는 경우 있음
        subVarArr[i++] = mapCharSizeArr
        subVarArr[i++] = carIconArr
        subVarArr[i++] = oilTypeArr
        subVarArr[i++] = chargeTypeArr
    }

    function preSet(index) {
        settingRoot.reqIdx = index
        settingRoot.reqContinue = false
    }

    function requestSetting() {
        settings = null
        reqIdx = 0
        reqContinue = true
        var data = ({})
        data[CommonConstants.Section] = listArr[reqIdx][listAttrEnum.Section]
        sendEvent(SettingMapViewEvent.RequestSetting, data )
        resetCreateModel()
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateToggleEnum()

        allocateTextEnum()
        allocateOilTypeEnum()
        allocateChargeTypeEnum()
        allocateComponentEnum()
        allocateReqEnum()
        allocateNightModeEnum()
        allocateaddrDispMethodEnum()
        allocateListAttrEnum()

        allocateSpeedResponseArr()
        allocateMapCharSizeArr()
        allocateCarIconArr()
        allocateSubVarArr()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingMapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            requestSetting()
        }
    }

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (settingRoot.visible) {
           //viewActivated()
        }
    }


    onMediated: {
        sendEvent(SettingMapViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        if (!sync) {
            sync = true
            return
        }

        init()
        enabled = true
        sendEvent(SettingMapViewEvent.ResetCsw)
        sendEvent(SettingMapViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingMapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingMapViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        enabled = false
        sendEvent(SettingMapViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
        sendEvent(SettingMapViewEvent.ClosedView)
    }

    function showHideChargingType() {
        if (CommonUtils.isUndefined(typeof reqEnum.MapSubChargeType))
            return
        if (CommonUtils.isUndefined(typeof reqEnum.MapSubOilType))
            return
        var object =  listObjArr[reqEnum.MapSubChargeType]
        if (CommonUtils.isNull(object))
            return
        var item = listModel.get(reqEnum.MapSubOilType)
        object.visible = CommonUtils.isEqual(item.value, oilTypeArr[oilTypeEnum.Elec])
    }

    onSettingsChanged: {
        var key = null
        var value = null
        var varArr = null
        var object = null
        if (CommonUtils.isNull(settings))
            return
        if (reqIdx < reqEnum.MapMain) {
            key = listArr[reqIdx][listAttrEnum.Key]
            if (!settings.hasOwnProperty(key))
                return
            value = settings[key][CommonConstants.Value]

            //속도 반응 지도 사용 상태가 OFF이면 "OFF"로  표출하도록 수정
            if (CommonUtils.isEqual(reqIdx, reqEnum.MapSubSpeedResponse)) {
                var mainValue = settings[SettingConstants.SpeedResponseMapUse][CommonConstants.Value]
                var mapUseArr = [toggleArr[toggleEnum.Off], toggleArr[toggleEnum.On]]
                if (CommonUtils.isEqual(CommonUtils.number(mainValue),  toggleEnum.Off)) {
                    subVarArr [reqIdx] = toggleArr
                    value = mainValue
                } else {
                    subVarArr[reqIdx] = speedResponseArr
                }
            }

            varArr = subVarArr[reqIdx]
            listModel.setProperty(reqIdx, CommonConstants.Value, varArr[CommonUtils.number(value)])
            showHideChargingType()
            if (!reqContinue){
                if (CommonUtils.isNull(subObjArr[reqIdx]))
                    return
                subObjArr[reqIdx].text = varArr[CommonUtils.number(value)]
                return
            }
            var data = ({})
            data[CommonConstants.Section] = listArr[++reqIdx][listAttrEnum.Section]
            sendEvent(SettingMapViewEvent.RequestSetting, data )
         } else if (CommonUtils.isEqual(reqIdx , reqEnum.MapMain)) {
            for (var i= reqIdx; i<listArr.length; i++) {
                key = listArr[i][listAttrEnum.Key]
                if (!settings.hasOwnProperty(key))
                    continue
                value = settings[key][CommonConstants.Value]
                listModel.setProperty(i, CommonConstants.Value,  value)
            }
            settingsMain = settings
            ++reqIdx
        }
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.SubMenu])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx: i,
                    text: listArr[i][listAttrEnum.Name],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonConstants.Empty
                })
            } else if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option2])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx: i,
                    text: listArr[i][listAttrEnum.Name],
                    option1: listArr[i][listAttrEnum.Arg0],
                    option2: listArr[i][listAttrEnum.Arg1],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })
            } else if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option3])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx: i,
                    text: listArr[i][listAttrEnum.Name],
                    option1: listArr[i][listAttrEnum.Arg0],
                    option2: listArr[i][listAttrEnum.Arg1],
                    option3: listArr[i][listAttrEnum.Arg2],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })
            }
        }
    }

    function resetCreateModel() {
        createModel()
        settingRoot.settingList.list.model = null
        settingRoot.settingList.list.model = listModel
    }


    function createView(index) {
        if (index < listUrlArr.length) {
            isActivated = false

            preSet(index)

            var data = ({})
            data[CommonConstants.Url] = listUrlArr[index]
            data[CommonConstants.Caller] = caller
            data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.SettingMapView)
            sendEvent(SettingMapViewEvent.CreateView, data )
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
                data[CommonConstants.ObjectName] = caller
                data[CommonConstants.Sync] = true
                sendEvent(SettingMapViewEvent.UpdateView, data)
                data = ({})
                data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingMapView)
                sendEvent(SettingMapViewEvent.CloseView, data)
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
                    clip: true
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
        id: textWithSubMenu
        Column {
            width: settingList.width
            height: visible ? GlobalConstants.kListTwoTextItemHeight : 0

            property var itemModel: null
            property var subObj: null
            property bool isSelected: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && CommonUtils.isEqual(highlightColor , selectColor)
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
                var idx = itemModel.idx
                listObjArr[idx] = this
                subObjArr[idx] = subObj

                showHideChargingType()
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
                    Item {width:CommonUtils.applyRemainWidth(this,parent);height:parent.height
                        Column {
                              id: leftColumn
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
                                          subObj = this
                                      }
                                  }
                              }
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
            //서브텍스트가 있는 두 줄 리스트가 있을 때, 한 줄 리스트도 두 줄 리스트 높이에 맞게 조정되어야 합니다
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
                itemModel.value = CommonUtils.string(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                var data = ({})
                data[CommonConstants.Section] = listArr[idx][listAttrEnum.Section]
                data[CommonConstants.Settings] = newSettings
                sendEvent(SettingMapViewEvent.SaveSetting, data )
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

            function isBtnOnOff() {
                if (CommonUtils.isEqual(itemModel.option1 , toggleArr[toggleEnum.Off]) &&
                    CommonUtils.isEqual(itemModel.option2 , toggleArr[toggleEnum.On])) {
                    return true
                }
                return false
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
                itemModel.value = CommonUtils.string(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                var data = ({})
                data[CommonConstants.Section] = listArr[idx][listAttrEnum.Section]
                data[CommonConstants.Settings] = newSettings
                sendEvent(SettingMapViewEvent.SaveSetting, data )
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
                        width:            settingRoot.layout.option3Width
                        height:           settingRoot.layout.option3Height
                        items:            [itemModel.option1,itemModel.option2,itemModel.option3]
                        bgSource:         Resource.imageUrl('toggle_btn_bg_3.png')
                        selectSource:     Resource.imageUrl('toggle_btn_slide_1.png')
                        normalColors:     [GlobalConstants.kListTextColor, GlobalConstants.kListTextColor, GlobalConstants.kListTextColor]
                        selectColors:     [GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor]
                        leftMargin:        5
                        rightMargin:       5

                        cswObj: csw
                        box.radius: height/2
                        backgroundColor: GlobalConstants.kBasicBackgoundColor


                        highlightBorderWidth: 2
                        highlightBorderColor: CommonConstants.Transparent
                        highlightColor : CommonConstants.Transparent
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
                if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
                    if (!CommonUtils.isNull(item.button)) {
                        if (!CommonUtils.isNull(item.button.items)) {
                            item.button.list.currentIndex = -1
                        }
                    }
                    item.isButtonFocused = false
                }
                ++settingList.list.currentIndex
                while( settingList.list.currentIndex  < settingList.list.count - 1 && settingList.list.currentItem.height <= 0) {
                   ++settingList.list.currentIndex
                }
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if ( 0 < settingList.list.currentIndex) {                
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
                while( 0 < settingList.list.currentIndex && settingList.list.currentItem.height <= 0) {
                   --settingList.list.currentIndex
                }
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
