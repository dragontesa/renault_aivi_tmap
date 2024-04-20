import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import ChangeDestinationViewEvent 1.0
import "../../component"
import "../../view"
import '../../view/util'
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: root

    anchors.fill: parent

    property var parentObj: null
    property var view
    property string cancelCallback
    property string okCallback: 'close'

    property string name
    property string address
    property string title: '목적지 변경'
    property string text: '목적지를 현재 선택하신 위치로\n변경 하시겠습니까?'
    property int destX: 0
    property int destY: 0
    property int poiID: 0
    property int rpFlag: 0
    property int navSeq: 0

    property bool changeDestinationProcessed: false
    property alias captureEnable: dialog.captureEnable

    signal rejected()
    signal accepted()

    onMediated: {
        sendEvent(ChangeDestinationViewEvent.Mediated)
        sendEvent(ChangeDestinationViewEvent.UpdateModal)
        sendEvent(ChangeDestinationViewEvent.RegistCSW, [dialog.csw])
    }
    onActivated: {
        sendEvent(ChangeDestinationViewEvent.Activated)
    }    

    function showAddress() {
        show(address)
    }

    function show(address) {
        root.address = address
        dialog.showDialog()
    }
    function changeDestination(data) {
        sendEvent(ChangeDestinationViewEvent.Accepted, data)
        close()
    }
    function changeDestinationRejected() {
        sendEvent(ChangeDestinationViewEvent.Canceled)
        close()
    }
    function captureScreen() {
        sendEvent(ChangeDestinationViewEvent.CaptureScreen)
    }
    function goBack() {
        changeDestinationRejected()
    }
    function pause() {
        dialog.pauseDialog()
    }
    function resumeDialog() {
        dialog.resume()
    }

    CommonDialogView {
        id: dialog

        anchors.fill: parent
        title: root.title
        isChangeDestination: true

        onCaptureScreen: {
            root.captureScreen()
        }

        Item {
            width: parent.width; height: 150
            anchors {
                left: dialog.contentsArea.left; leftMargin: 30
                right: dialog.contentsArea.right; rightMargin: 30
                top: dialog.contentsArea.top; topMargin: 90 }

            Column {
                id: column
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: 118
                Text2 {
                    width: parent.width;  height: contentHeight
                    offsetY: 4
                    text: root.text
                    color: '#ffffff'
                    maximumLineCount: 3
                    lineHeight: 26+6; lineHeightMode: Text.FixedHeight
                    wrapMode: Text.WordWrap
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27) }
                    horizontalAlignment: Text.AlignHCenter
                }
                Item {
                    width: parent.width; height: 22
                }
                Text2 {
                    width: parent.width;  height: 30
                    offsetY: 4
                    text: root.name.length > 0 ? root.name : root.address
                    color: '#ffffff'
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27) }
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }
        }

        menus: ['취소', '확인']
        showTimer: true
        embeded: true
        visible: true
        onAccepted: {
            if (!changeDestinationProcessed) {
                changeDestinationProcessed = true
                var data = {
                    mapView : mapView,
                    x : destX,
                    y : destY,
                    address : address,
                    name : name,
                    poiID : poiID,
                    navSeq : navSeq,
                    rpFlag : rpFlag
                }

                changeDestination(data)
            }
        }
        onRejected: {
            changeDestinationRejected()
        }
    }
}
