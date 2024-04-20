import QtQuick 2.0
import "../../component"
import Csw 1.0

Item {
    id: root

    property var csws: []
    property bool okClickedByCsw: false

    width: childrenRect.width
    height: childrenRect.height

    signal upClicked()
    signal downClicked()
    signal okClicked()

    Component {
        id: compButton

        Rectangle {
            id: rect
            // width,height : loader's size

            signal clicked()

            state: "normal"
            states: [
                State {
                    name: "normal"
                    PropertyChanges {
                        target: rect
                        color: "#3a3a40"
                    }
                },
                State {
                    name: "pressed"
                    PropertyChanges {
                        target: rect
                        color: "#2d2d31"
                    }
                },
                State {
                    name: "focused"
                    PropertyChanges {
                        target: rect
                        color: "#ffffff"
                    }
                }
            ]
            Image {
                anchors.centerIn: parent
                source: rect.state == "focused" ? imageSourceFocused : imageSource
                scale: layout.arrowIconScale
            }

            MouseArea {
                anchors.fill: parent
                onPressed: rect.state = "pressed"
                onReleased: {
                    rect.state = "normal"
                }
                onClicked: {
                    rect.clicked()
                    csw.forceLostFocus()
                }
            }

            Csw {
                id: csw
                section: Csw.Section_RouteSettingControl
                sequence: parent.index
                enable: root.visible
                isFixSection: true
                onFocused: {
                    rect.state = "focused"
                }
                onLostFocusd: {
                    rect.state = "normal"
                    if (bySection)
                        root.visible = false
                }
                onSelected: {
                    if (sequence == 2) {
                        // csw로 확인을 눌렀을 때
                        // ListView에 선택된 아이템을 focus처리하기 위해.
                        okClickedByCsw = true
                    }
                    rect.clicked()
                }
                Component.onCompleted: root.csws.push(this)
            }
        }
    }

    Column {
        spacing: layout.controllerVerticalSpacing

        Loader {
            id: loaderUp
            sourceComponent: compButton
            width: layout.moveButtonSize
            height: layout.moveButtonSize
            property var imageSource: Resource.imageUrl('route/floating_btn_up.png')
            property var imageSourceFocused: Resource.imageUrl('route/floating_btn_up_f.png')
            property int index: 0
            Connections {
                target: loaderUp.item
                onClicked: root.upClicked()
            }
        }
        Loader {
            id: loaderDown
            sourceComponent: compButton
            width: layout.moveButtonSize
            height: layout.moveButtonSize
            property var imageSource: Resource.imageUrl('route/floating_btn_down.png')
            property var imageSourceFocused: Resource.imageUrl('route/floating_btn_down_f.png')
            property int index: 1
            Connections {
                target: loaderDown.item
                onClicked: root.downClicked()
            }
        }
        Loader {
            id: loaderOk
            sourceComponent: compButton
            width: layout.okButtonWidth
            height: layout.okButtonHeight
            property var imageSource: lang == 'ko' ? Resource.imageUrl('route/floating_btn_confirm.png')
                                                   : Resource.imageUrl('route/floating_btn_confirm_eng.png')
            property var imageSourceFocused: lang == 'ko' ? Resource.imageUrl('route/floating_btn_confirm_f.png')
                                                          : Resource.imageUrl('route/floating_btn_confirm_f_eng.png')
            property int index: 2
            Connections {
                target: loaderOk.item
                onClicked: root.okClicked()
            }
        }
    }
}
