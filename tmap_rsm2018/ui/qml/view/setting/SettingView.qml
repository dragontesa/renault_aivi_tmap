import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0

import "../../component"
import "../../view"
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {    
    //Enum
    property var softKeyEnum : ({})
    property var toggleEnum  : ({})

    property var softKeyMenu : ([])
    property var toggleArr   : ([])

    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum,"Back"                 , i++, [CommonConstants.Name], [ZoneConstants.Back] )
        CommonUtils.insertEnum(softKeyEnum,"Option"               , i++, [CommonConstants.Name], [ZoneConstants.Option])
        CommonUtils.insertEnum(softKeyEnum,"Navi"                 , i++, [CommonConstants.Name], [ZoneConstants.Navi])
        CommonUtils.insertEnum(softKeyEnum,"Menu"                 , i++, [CommonConstants.Name], [ZoneConstants.Menu])
        CommonUtils.insertEnum(softKeyEnum,"Confirm"              , i++, [CommonConstants.Name], [qsTr("확인")])

        CommonUtils.fetchArr(softKeyEnum, CommonConstants.Name, softKeyMenu)
    }


    function allocateToggleEnum(){
        if (CommonUtils.getObjectLen(toggleEnum) <= 0) {
            toggleEnum = CommonUtils.createEnum(toggleEnum)
        }
        var i = 0
        CommonUtils.insertEnum(toggleEnum, "Off"                  , i++, [CommonConstants.Name], [SettingConstants.Off])
        CommonUtils.insertEnum(toggleEnum, "On"                   , i++, [CommonConstants.Name], [SettingConstants.On])

        CommonUtils.fetchArr(toggleEnum, CommonConstants.Name, toggleArr)
    }

}
