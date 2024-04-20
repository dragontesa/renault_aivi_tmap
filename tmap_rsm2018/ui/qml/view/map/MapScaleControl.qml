import QtQuick 2.0
import Csw 1.0
import "../../component"

Item {
    id: root

    property int viewLevel //0~12
    property bool returnButtonVisible: true
    property int sectionId
    property int startSequence: 0
    property bool undeletable: false

    width: childrenRect.width
    height: childrenRect.height

    signal scaleUpClicked()
    signal scaleDownClicked()
    signal returnClicked()
    signal resetTimer()

    property var csws: [plusCsw, minusCsw, returnCsw]

    function initButtonState() {
        plusButton.item.state = 'normal'
        minusButton.item.state = 'normal'
    }

    Component {
        id: compScaleButton
        Item {
            id: scaleButtonRoot

            width: image.width
            height: image.height

            property var sources: parent.sources
            property string beforeState
            property string icon

            signal clicked()

            Image {
                id: image
                Image {
                    visible: scaleButtonRoot.icon.length > 0
                    source: Resource.imageUrl(scaleButtonRoot.icon)
                }
            }
            MouseArea {
                anchors.fill: image
                onPressed: {
                    beforeState = scaleButtonRoot.state
                    scaleButtonRoot.state = 'pressed'
                }
                onReleased: {
                    scaleButtonRoot.state = 'normal'
                    scaleButtonRoot.clicked()
                }
            }

            state: 'normal'
            states: [
                State {
                    name: 'normal'
                    PropertyChanges {
                        target: image
                        source: Resource.imageUrl(scaleButtonRoot.sources[0])
                    }
                },
                State {
                    name: 'pressed'
                    PropertyChanges {
                        target: image
                        source: Resource.imageUrl(scaleButtonRoot.sources[1])
                    }
                },
                State {
                    name: 'focused'
                    PropertyChanges {
                        target: image
                        source: Resource.imageUrl(scaleButtonRoot.sources[2])
                    }
                }
            ]
        }
    }

    Column {
        scale: layout.scale// 0.5

        Loader {
            id: plusButton
            property var sources: [
                "map_btn_plus_n.png",
                "map_btn_plus_p.png",
                "map_btn_plus_f.png",
            ]
            sourceComponent: compScaleButton
            Csw {
                id: plusCsw
                objectName: 'plusCsw'
                section: sectionId
                sequence: startSequence
                deletable: !undeletable
                enable: plusButton.visible
                onFocused: {
                    plusButton.item.state = 'focused'
                    root.resetTimer()
                }
                onLostFocusd: {
                    plusButton.item.state = 'normal'
                }
                onSelected: {
                    root.scaleUpClicked()
                }
            }
        }
        Item {
            width: 69; height: 60
            Image {
                id: scaleImage
                source: {
                    return viewLevel < 10
                           ? Resource.imageUrl("map_scale_0" + viewLevel + ".png")
                           : Resource.imageUrl("map_scale_" + viewLevel + ".png")
                }
            }
            Rectangle {
                anchors.fill: parent
                color: '#000000'; opacity: 0.74
                visible: scaleImage.status != Image.Ready
            }
        }

        Loader {
            id: minusButton
            property var sources: [
                "map_btn_minus_n.png",
                "map_btn_minus_p.png",
                "map_btn_minus_f.png",
            ]
            sourceComponent: compScaleButton
            Csw {
                id: minusCsw
                objectName: 'minusCsw'
                section: sectionId
                sequence: startSequence + 1
                deletable: !undeletable
                enable: minusButton.visible
                onFocused: {
                    minusButton.item.state = 'focused'
                    root.resetTimer()
                }
                onLostFocusd: {
                    minusButton.item.state = 'normal'
                }
                onSelected: {
                    root.scaleDownClicked()
                }
            }
        }
        Item { width: 10; height: returnButtonVisible ? 10 : 0 } // padding
        Loader {
            id: returnButton
            property var sources: [
                "map_btn_bg_n.png",
                "map_btn_bg_p.png",
                "map_btn_bg_f.png",
            ]
            sourceComponent: returnButtonVisible ? compScaleButton : null
            onLoaded: item.icon = "map_btn_yah.png"
            Csw {
                id: returnCsw
                objectName: 'returnCsw'
                section: sectionId
                sequence: startSequence + 2
                deletable: !undeletable
                enable: returnButtonVisible
                onFocused: {
                    returnButton.item.state = 'focused'
                    returnButton.item.icon = 'map_btn_yah_f.png'
                    root.resetTimer()
                }
                onLostFocusd: {
                    returnButton.item.state = 'normal'
                    returnButton.item.icon = 'map_btn_yah.png'
                }
                onSelected: {
                    root.returnClicked()
                }
            }
        }

        Connections {
            target: plusButton.item
            onClicked: {
                root.scaleUpClicked()
                plusCsw.forceLostFocus()
            }
        }
        Connections{
            target: minusButton.item
            onClicked: {
                root.scaleDownClicked()
                minusCsw.forceLostFocus()
            }
        }
        Connections {
            target: returnButton.item
            onClicked: {
                root.returnClicked()
                returnCsw.forceLostFocus()
            }
        }
    }
}
