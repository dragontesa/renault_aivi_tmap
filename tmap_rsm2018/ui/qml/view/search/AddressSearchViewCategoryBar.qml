import QtQuick 2.0
import AddressSearchViewEvent 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard

Item {
    id: bar
    objectName: "bar"
    property var layout

    property int currentIndex: 0
    property var arrowImageSources: {
        'normal': ['address_tab_left_n.png','address_tab_center_n.png','address_tab_right_n.png'],
        'selected': ['address_tab_left_s.png','address_tab_center_s.png','address_tab_right_s.png'],
        'focused': ['address_tab_left_f.png','address_tab_center_f.png','address_tab_right_f.png'],
    }
    property color enabledTextColor: 'white'
    property color disabledTextColor: '#8a8a99'

    ListModel {
        id: barModel
        ListElement {
            name: qsTr('광역시/도')
        }
        ListElement {
            name: qsTr('시/군/구')
        }
        ListElement {
            name: qsTr('도로명')
        }
    }

    signal categoryClicked(int index)

    function name( index ) {
        return repeater.itemAt( index ).label.text
    }

    function setName( index, name ) {
        if ( repeater.count ) {
            repeater.itemAt( index ).label.text = name
        }
    }

    function value( index ) {
        return repeater.itemAt( index ).value
    }

    function setValue( index, value ) {
        repeater.itemAt( index ).value = value
    }

    function getState() {
        return [
            [name(0),value(0)],
            [name(1),value(1)],
            [name(2),value(2)]
        ]
    }

    function setState( state ) {
        setName(0,state[0][0])
        setValue(0,state[0][1])
        setName(1,state[1][0])
        setValue(1,state[1][1])
        setName(2,state[2][0])
        setValue(2,state[2][1])
    }

    Image {
        id: arrowImageLeft
        width: layout.categoryBarItemWidth
        height: layout.barHeight
        source: repeater.count > 0 ? repeater.itemAt(0).arrowImageSource : ""
        fillMode: layout.categoryBarArrowImageFillMode
        visible: true
    }

    Image {
        id: arrowImageCenter
        width: layout.categoryBarItemCenterWidth
        height: layout.barHeight
        anchors.horizontalCenter: parent.horizontalCenter
        source: repeater.count > 0 ? repeater.itemAt(1).arrowImageSource : ""
        fillMode: layout.categoryBarArrowImageFillMode
        visible: true
    }

    Image {
        id: arrowImageRight
        width: layout.categoryBarItemWidth
        height: layout.barHeight
        anchors.right: parent.right
        source: repeater.count > 0 ? repeater.itemAt(2).arrowImageSource : ""
        fillMode: layout.categoryBarArrowImageFillMode
        visible: true
    }

    Row {
        id: nodeRow
        Repeater {
            id: repeater
            model: barModel
            delegate: Item {
                id: node
                width: root.width / 3
                height: root.layout.barHeight
                property alias label: label
                property var value
                state: currentIndex == index ? 'selected' : 'normal'
                property string arrowImageSource: Resource.imageUrl( 'address', bar.arrowImageSources[state][index] )

                Text {
                    id: label
                    anchors.centerIn: parent
                    text: name
                    color: index == bar.currentIndex ? bar.enabledTextColor : bar.disabledTextColor
                    font.pixelSize: Resource.pixelToDp(35)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                    }
                    onReleased: {
                    }
                    onClicked: {
                        if ( index < bar.currentIndex ) {
                            bar.currentIndex = index
                            bar.categoryClicked( bar.currentIndex )
                        }
                    }
                }
            }
        }
    }
}
