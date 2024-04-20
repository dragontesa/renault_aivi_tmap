import QtQuick 2.0
import BookmarkPageEditViewEvent 1.0
import CommonConstants 1.0
import SearchConstants 1.0
import "../../singleton"
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard

PopupView {
    id: root

    property string dialogText
    property string aliasText
    property string type
    property string name
    property int id: -1
    property bool isShowKeyboard: false

    width: GlobalConstants.kScreenWidth
    height: GlobalConstants.kScreenContentHeight

    function showEditFavoriteAliasDialog() {
        root.dialogText = name
        root.aliasText = name
        dialogLoader.sourceComponent = null
        dialogLoader.sourceComponent = editFavoriteAliasDialog
        dialogLoader.anchors.bottom = keyboard.top
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        keyboardDown = true
        hideKeyboard()
    }

    function __pullUpKeyboard() {
        keyboardDown = false
        showKeyboard()
    }

    function showKeyboard() {
        var data = ({})
        data[CommonConstants.X] = keyboard.x
        data[CommonConstants.Y] = keyboard.y
        data[CommonConstants.Z] = keyboard.z + 1
        data[CommonConstants.Width] = keyboard.width
        data[CommonConstants.Height] = keyboard.height
        data[CommonConstants.Text] = name
        data[CommonConstants.Pressed] = CustomKeyboard.hangleKeybd
        data[CommonConstants.DontUseEditBox] = true

        sendEvent(BookmarkPageEditViewEvent.ShowKeyboard, data)
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.DontUseEditBox] = true
        data[CommonConstants.Text] = CommonConstants.Empty
        sendEvent(BookmarkPageEditViewEvent.HideKeyboard, data)
    }

    function handleKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            cancelEdit()
            break

        case CustomKeyboard.CodeSymbol:
        case CustomKeyboard.CodeKorean:
        case CustomKeyboard.CodeEnglish:
        case CustomKeyboard.CodeShift:
        case CustomKeyboard.CodeClear:
        case CustomKeyboard.CodeSetting:
        case CustomKeyboard.CodeSearch:
        case CustomKeyboard.CodeConfirm:
            break

        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            dialogText = text
            break
        }
    }
    function goBack() {
        if (type == 'editAlias') {
            cancelEdit()
        }
    }

    onMediated: {
        updateModal(true)

        if (type == 'deleteAll') {
            sendEvent(BookmarkPageEditViewEvent.ShowDeleteAllPopup)
        } else if (type == 'editAlias') {
            showEditFavoriteAliasDialog()
            if (!isShowKeyboard) {
                // hideKeyboard() 호출 후 키보드 뷰가 닫히면서 일시적으로 즐겨찾기 편집뷰가
                // activated됨.
                // 다시 키보드가 표시되는 문제가 있어서 한번만 showKeyboard()가 호출 되도록 처리
                // 17.11.17 ymhong
                isShowKeyboard = true
                showKeyboard()
            }
        }
    }

    onActivated: {
    }
    onDeactivated: {
        updateModal(false)
    }

    function pause() {
    }

    function resume() {
        if (type == 'editAlias') {
            showKeyboard()
        }
    }

    function updateModal(isModal) {
        var data = ({})
        data[CommonConstants.Modal] = isModal
        sendEvent( BookmarkPageEditViewEvent.ModalChanged, data )
    }

    function cancelEdit() {
        hideKeyboard()
        close()
        root.close()
    }

    Loader {
        id: dialogLoader

        width: root.width
        anchors.top: root.top

        onLoaded: {
            switch (root.dialogType) {
            case 0:
                item.content.sourceComponent = deleteAllContents
                break
            case 1:
                item.content = editFavoriteAliasComponent
                break
            }
            if (item !== null) {
                item.registCsw()
            }
        }

        Component {
            id: editFavoriteAliasDialog

            Item {
                anchors.fill: parent
                function registCsw() {
                    sendEvent(BookmarkPageEditViewEvent.RegistCsw, [dialog.csw])
                }

                CommonDialog {
                    id: dialog
                    width: 640; height: 350; anchors.centerIn: parent
                    content: Item {
                        anchors.fill: parent
                        Text {
                            id: message
                            y: 26; width: 500; height: 26
                            font { pixelSize: Resource.pixelToDp(27) }
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr('변경할 장소 명칭을 입력해주세요.')
                            color: '#ffffff'
                        }
                        Rectangle {
                            id: inputBox
                            width: 500; height: 56;
                            color: '#ffffff'
                            anchors { top: message.bottom; topMargin: 18; horizontalCenter: parent.horizontalCenter }
                            TextInput {
                                id: textInput
                                anchors {
                                    left: parent.left; leftMargin: 20;
                                    right: parent.right; rightMargin: 20;
                                }
                                y: 8
                                height: parent.height-10
                                verticalAlignment: Text.AlignVCenter
                                font { pixelSize: Resource.pixelToDp(38) }
                                color: '#000000'
                                text: root.dialogText
                                onTextChanged: {
                                    root.aliasText = text
                                    cursorVisible = true
                                }
                                onCursorPositionChanged: {
                                    cursorPosition = text.length
                                }
                                cursorDelegate: Rectangle {
                                    width: 1; height: 41
                                    color: MultiSenseCoordinator.kAmbientColor
                                    Timer {
                                        repeat: true
                                        running: true
                                        interval: 500
                                        onTriggered: {
                                            parent.visible = ! parent.visible
                                        }
                                    }
                                }

                                clip: true
                            }                            
                        }
                    }
                    title: qsTr('명칭변경')
                    menus: [qsTr('취소'), qsTr('저장')]

                    onAccepted: {

                    }
                    onMenuClicked: {
                        if (menu === qsTr('저장')) {
                            var data = ({})
                            data[CommonConstants.Name] = root.aliasText
                            data[SearchConstants.Id] = root.id
                            sendEvent(BookmarkPageEditViewEvent.EditFavoriteAlias, data)

                            if (root.aliasText.length > 0) {
                                hideKeyboard()
                                root.close()
                            } else {
                                // 버튼을 누르는 순간 다이얼로그가 닫히기 때문에 다시 표시
                                dialogLoader.sourceComponent = null
                                dialogLoader.sourceComponent = editFavoriteAliasDialog
                            }
                        } else if (menu === qsTr('취소')) {
                            cancelEdit()
                        }
                    }

                    onRejected: {
                        cancelEdit()
                    }
                }
            }
        }
    }

    Item {
        id: keyboard
        width: parent.width
        height: 419
        anchors.bottom: parent.bottom
    }
}
