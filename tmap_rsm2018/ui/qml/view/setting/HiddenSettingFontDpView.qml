import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import HiddenSettingFontDpViewEvent 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

SettingView {
    id: settingRoot

    property var caller             : null
    property var settings           : null

    property int keyboardHeight     : 419
    property int pageIndex          : 0

    property bool focused           : false
    property color highlightColor   : CommonConstants.Transparent

    property var listView1stObj     : null

    //ID

    //Event
    property var textEnum           : ({})
    property var textArr            : ([])

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",                i++, [CommonConstants.Name], [qsTr("폰트 DP 변경")])
        CommonUtils.insertEnum(textEnum, "EnterFontDpNumber",         i++, [CommonConstants.Name], [qsTr("폰트 DP를 입력해주세요")])
        CommonUtils.insertEnum(textEnum, "ChangeFontType",            i++, [CommonConstants.Name], [qsTr("폰트DP입력")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    property var list1stArr: [
        qsTr(CommonConstants.Empty),
        qsTr("폰트 DP를 변경하는 화면입니다."),
        qsTr(CommonConstants.Empty),
        qsTr(CommonConstants.Empty),
        qsTr("폰트 DP를 변경하시려면, 폰트DP값을 입력후 버튼을 눌러주세요.."),
        qsTr(CommonConstants.Empty),
    ]    
    property int list1stArrLength:  list1stArr.length

    function init(){
        allocateSoftKeyEnum()
        allocateTextEnum()
    }

    onMediated: {
        init()
        createModel1st()
        sendEvent(HiddenSettingFontDpViewEvent.Mediated)
    }

    onActivated: {
        sendEvent(HiddenSettingFontDpViewEvent.ResetCsw)
        sendEvent(HiddenSettingFontDpViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(HiddenSettingFontDpViewEvent.Activated)
    }

    function createModel1st(){
        var len = list1stArrLength
        for (var i = 0; i<len; i++) {
            listModel1st.set(i, {text:  list1stArr[i]})
        }
        loader.sourceComponent = component1st
    }


    function changeFontDp(otp) {
        if (!CommonUtils.isNull(listView1stObj)) {
            listView1stObj.rowHeight = settingRoot.layout.rowHeight
            visibleKeyboard(false)
        }

        var data = ({})
        data[CommonConstants.Value] = otp
        sendEvent(HiddenSettingFontDpViewEvent.ChangeFontDp, data)
    }

    function visibleKeyboard(visible, text) {
        var data = ({})
        if (visible) {
            keyboard.height = keyboardHeight
            data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.HiddenSettingFontDpView)
            data[CommonConstants.X] = keyboard.x
            data[CommonConstants.Y] = keyboard.y
            data[CommonConstants.Z] = keyboard.z + 1
            data[CommonConstants.Width] = keyboard.width
            data[CommonConstants.Height] = keyboard.height
            data[CommonConstants.SearchBtnAlwaysEnable] = true
            sendEvent(HiddenSettingFontDpViewEvent.ShowKeyboard, data)
        } else {
            keyboard.height = 0
            if (!CommonUtils.isNull(text))
                data[CommonConstants.Text] = text
            sendEvent(HiddenSettingFontDpViewEvent.HideKeyboard, data)
        }
    }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
           visibleKeyboard(false, CommonConstants.Empty)
            break
        case CustomKeyboard.CodeConfirm:
            visibleKeyboard(false, CommonConstants.Empty)
            break
        default:
            break
        }
    }


    function __doSoftKeyFunction(button, menuName, menuIndex) {
      if (CommonUtils.isEqual(button, softKeyMenu[softKeyEnum.Back])) {
          var data = ({})
          data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.HiddenSettingFontDpView)
          sendEvent(HiddenSettingFontDpViewEvent.CloseView, data)
      } else if (CommonUtils.isEqual(button, softKeyMenu[softKeyEnum.Option])) {

      } else if (CommonUtils.isEqual(button, softKeyMenu[softKeyEnum.Navi])) {

      } else if (CommonUtils.isEqual(button, softKeyMenu[softKeyEnum.Menu])) {
      }
    }


    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        visibleKeyboard(false)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        visibleKeyboard(true)
    }


    //main
    Rectangle {
        width: parent.width
        height: parent.height
        color: GlobalConstants.kListNormalColor
        Loader {
             id: loader
             width: parent.width
             height:parent.height - keyboard.height
             onSourceComponentChanged: {
                 settingRoot.visible = true
             }
        }
        Item {
            id: keyboard
            width: parent.width
            height: 0
        }
    }

    ListModel { id: listModel1st   }


    Component {
        id: component1st
        Column {
            width: settingRoot.width
            height: settingRoot.height
            Rectangle{
                width: parent.width
                height: parent.height
                color: GlobalConstants.kListNormalColor
                Column {
                    width: parent.width
                    height: parent.height
                    ListView {
                        id: listView1st
                        width: parent.width
                        height: parent.height - (enterAuthNum.height + changeFontTypeRow.height)
                        property int rowHeight:   settingRoot.layout.rowHeight
                        clip: true
                        model: listModel1st
                        delegate :
                                Item {
                                    width:  listView1st.width
                                    height: listView1st.rowHeight
                                    MouseArea {
                                        width: parent.width
                                        height: parent.height
                                        onClicked: {
                                            listView1st.currentIndex = modelindex
                                        }
                                    }
                                    Text {
                                        width: parent.width - (GlobalConstants.kListLineRightMargin)
                                        height: parent.height
                                        horizontalAlignment: Qt.AlignHCenter
                                        verticalAlignment: Qt.AlignVCenter
                                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kListSubTextFontSize)}
                                        color: (CommonUtils.isEqual(model.index, 1) || CommonUtils.isEqual(model.index, 2)) ? CommonConstants.Red : GlobalConstants.kListTextColor
                                        text : model.text
                                    }
                                }
                        currentIndex: -1
                        interactive: false
                        highlight:
                                Rectangle {
                                    x: GlobalConstants.kListBackgroundLeftMargin
                                    y: listView1st.currentItem.y
                                    width: listView1st.currentItem.width - (GlobalConstants.kListBackgroundLeftMargin + GlobalConstants.kListBackgroundRightMargin)
                                    height: listView1st.currentItem.height
                                    color: MultiSenseCoordinator.kAmbientColor
                                }
                        highlightFollowsCurrentItem: false

                        // masking
                        onMovementStarted:  imgMask.visible = true
                        onMovementEnded:  imgMask.visible = false
                        Image {
                              id: imgMask
                              width: parent.width
                              height: 30
                              y: parent.y + (parent.height - height)
                              visible: false
                              sourceSize.width: width
                              sourceSize.height: height
                              source: Resource.imageUrl("zone/zone2_img_mask.png")
                        }

                        TScrollBar {
                            flickable: parent
                            width: 2
                            color: MultiSenseCoordinator.kAmbientColor
                            grooveColor: GlobalConstants.kScrollBarBackColor
                            rightMargin: GlobalConstants.kScrollBarRightMargin
                        }
                        Component.onCompleted: {
                            listView1stObj = this
                        }
                    }
                    Text {
                        id: enterAuthNum
                        width: parent.width - (GlobalConstants.kListLineRightMargin)
                        height: 30
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kListSubTextFontSize)}
                        color: GlobalConstants.kListTextColor
                        text : textArr[textEnum.EnterFontDpNumber]
                    }
                    Row {
                        id: changeFontTypeRow
                        x: (parent.width-width) /2
                        width: (changeFontTypeInputRange.width + changeFontTypeBlank.width + changeFontType.width)
                        height: 60
                        Rectangle{
                            id: changeFontTypeInputRange
                            width: 300
                            height:30
                            anchors.verticalCenter: parent.verticalCenter
                            color: 'white'
                            TextInput {
                                id: changeFontTypeInputField

                                focus: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Qt.AlignVCenter
                                horizontalAlignment: Qt.AlignHCenter
                                color: 'black'
                                clip: true
                                onActiveFocusChanged: {
                                    if ( activeFocus && visible ) {
                                        listView1st.rowHeight =   (settingRoot.layout.rowHeight - (settingRoot.layout.rowHeight/4) )
                                        sendEvent(HiddenSettingFontDpViewEvent.HideKeyboard)
                                        sendEvent(HiddenSettingFontDpViewEvent.RegistKeyboardInput, {keyboardInput: changeFontTypeInputField})
                                        visibleKeyboard(true)
                                    }
                                }
                            }
                        }
                        Item {
                            id: changeFontTypeBlank
                            width: 32
                            height: parent.height
                        }
                        TPushButton {
                            id: changeFontType
                            width: 100
                            height: 30
                            radius: 5
                            anchors.verticalCenter: parent.verticalCenter
                            label: textArr[textEnum.ChangeFontType]
                            onButtonClicked: {
                                changeFontDp(changeFontTypeInputField.text)
                            }
                        }
                    }
                }
            }
         }
    }
}
