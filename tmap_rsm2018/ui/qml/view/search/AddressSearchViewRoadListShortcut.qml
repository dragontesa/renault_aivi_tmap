import QtQuick 2.0
import AddressSearchViewEvent 1.0
import CommonConstants 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../util/DebugUtils.js" as Debug
import "../../singleton"
import Csw 1.0

Item {
    id: root
    objectName: "roadListShortcut"
    property var layout    
    property var csw
    property var owner

    property var model: [
        ['ㄱ','ㅇ'],
        ['ㄴ','ㅈ'],
        ['ㄷ','ㅊ'],
        ['ㄹ','ㅋ'],
        ['ㅁ','ㅌ'],
        ['ㅂ','ㅍ'],
        ['ㅅ','ㅎ']    
    ]

    property bool cswFocused: false
    property int cswFocusedIndex: -1;
    property color highlightColor: GlobalConstants.kListFocusColor

    //Timer
    property var timer : null

    onCswFocusedChanged: {
        console.log('[CSW][AddressSearchView][shortcut]cswFocused: ' + cswFocused)
        if (cswFocused) {
            if (cswFocusedIndex == -1) {
                cswFocusedIndex = 0
            }
        }
    }

    function onCswCcw() {
        if ( root.cswFocusedIndex > 0 ) {
            root.cswFocusedIndex--
        }
    }

    function onCswCw() {
        if ( root.cswFocusedIndex + 1 < model.length * 2 ) {
            root.cswFocusedIndex++
        }
    }

    function onCswLeft() {
        csw.changeFocusedObj(owner)
    }

    function onCswRight() {
    }

    function onCswSelect( selectedIndex ) {
        highlightColor = MultiSenseCoordinator.kAmbientColor
        var sleep =  GlobalConstants.kPressedColorExposureTime
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(root, sleep, function() {
            try {
                if (cswFocused) {
                    highlightColor = GlobalConstants.kListFocusColor
                }
                else {
                    highlightColor = layout.shortcutButtonColor
                }

                var row = Math.floor(cswFocusedIndex / 2)
                var col = cswFocusedIndex % 2

                //console.debug( 'cswFocusedIndex: ' + cswFocusedIndex + ' row: ' + row + ' col: ' + col );

                shortcutClicked( root.model[row][col] )
            } catch(e) {
            }
        })
    }

    signal shortcutClicked( string shortcut )

    Component.onCompleted: {
        cswFocusedIndex = 0
    }

    ExclusiveGroup {
        id: group0
    }

    Column {
        id: col
        spacing: 4
        Repeater {
            id: shortcutRow
            model: 7
            Row {
                property int rowIndex: index
                spacing: layout.shortcutButtonSpacing
                Repeater {
                    id: shortcutColumn
                    model: 2
                    Rectangle {
                        id: button
                        width: layout.shortcutButtonWidth
                        height: layout.shortcutButtonHeight
                        state: 'normal'
                        property int itemIndex: rowIndex * 2 + index
                        property bool cswFocused: root.cswFocused
                        property int cswFocusedIndex: root.cswFocusedIndex

                        function updateState() {
                            if ( root.cswFocused &&
                                itemIndex == root.cswFocusedIndex ) {
                                state = 'focused'
                            }
                            else {
                                state = 'normal'
                            }
                        }

                        onCswFocusedChanged: {
                            updateState()
                        }

                        onCswFocusedIndexChanged: {
                            updateState()
                        }

                        states: [
                            State {
                                name: "normal"
                                PropertyChanges { target: button;  color : layout.shortcutButtonColor }
                                PropertyChanges { target: text;  color : GlobalConstants.kListTextColor }
                            },
                            State {
                                name: "focused"
                                PropertyChanges { target: button;  color : root.highlightColor }
                                PropertyChanges { target: text;  color : GlobalConstants.kListTextFocusColor }
                            }
                        ]

                        Text {
                            id: text
                            anchors.centerIn: parent
                            text: root.model[rowIndex][index]
                            font: layout.shortcutButtonFont
                            color: GlobalConstants.kListTextColor
                        }

                        TMouseArea {
                            anchors.fill: parent
                            onPressedChanged: {
                                if ( pressed ) {
                                    root.cswFocusedIndex = rowIndex * 2 + index
                                    button.color = MultiSenseCoordinator.kAmbientColor
                                }
                                else {
                                    root.cswFocusedIndex = -1
                                    button.color = layout.shortcutButtonColor
                                }
                            }
                            onClicked: {
                                root.cswFocusedIndex = rowIndex * 2 + index
                                root.onCswSelect(root.cswFocusedIndex)
                            }
                        }
                    }
                }
            }
        }
        z: 99
    }

    Item {
        id: mouseCapturer
        anchors.fill: col
        TMouseArea {
            anchors.fill: parent
        }
    }
}
