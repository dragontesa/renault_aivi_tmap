import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import MapConfigControl 1.0

import "../../singleton"
import "../../component"
import "../../GlobalConstants.js" as GlobalConstants


Item {
    id: rootItem
    width: 340
    height: GlobalConstants.kScreenContentHeight
    readonly property variant kMenus: {
        0: {
            menu: "Density DPI",
            items: [
                { type: "slider", values: [0,1000], current:0, step: 10 }
            ]
        },
        1: {
            menu: "TileSidkCache",
            items: [
                 { type: "radio", values: [ "STREAMING ONLY", "HYBRID", "EMBEDED" ], current:0 }
            ]
        },
        2: {
            menu: "Building Filter Mode",
            items: [
              { type: "radio", values: [ "NONE", "L1", "L2" ], current:0  }
            ]
        },
        3: {
            menu: "Fovy",
            items: [
                { type: "slider", values: [ 5.0, 90.0 ], current:6.0, step: 1.0 }
            ]
        },
        4: {
            menu: "Copycat Building",
            items: [
               { type: "radio", values: [ "NO", "YES"], current:0 }
            ]
        },
        5: {
            menu: "Show POI",
            items: [
                { type: "radio", values: [ "OFF", "ON"], current:0 }
            ]
        },
        6: {
            menu: "POI Scale for Navi",
            items: [
                { type: "slider", values: [0.0,30.0], current:1.0, step: 0.1  }
            ]
        },
        7: {
            menu: "POI Scale Force Apply",
            items: [
               { type: "radio", values: [ "NO", "YES"], current:0 }
            ]
        },
        8: {
            menu: "POI Occlusion",
            items: [
               { type: "radio", values: [ "NO", "YES"], current:0 }
            ]
        },
        9: {
            menu: "Render Subway Lines",
            items: [
               { type: "radio", values: [ "NO", "YES"], current:0 }
            ]
        },
        10: {
            menu: "RouteTi",
            items: [
               { type: "radio", values: [ "NO", "YES"], current:0 }
            ]
        },
        11: {
            menu: "Select Theme",
            splitter: "Route Style Theme",
            items: [
                { type: "radio",  values: ["DAYLIGHT", "NIGHT"], current:0}
            ]
        },
        12: {
            menu: "Route Line Style",
            items: [
                { type: "picker",   values: [ 0.0, 100.0 ], current:5.0, step: 1.0 , /*orientation: 1, */label:"thick", width:120 },
            ]
        },
        13: {
            menu: "Route Line Turnpoint Style",
            items: [
                { type: "picker",       values: [ 0, 100 ], current:5.0, step: 1.0,/* orientation: 1,*/ label:"width", width:90},
                { type: "picker",       values: [ 0.0, 100.0 ], current:30.0, step: 1.0, /*orientation: 1, */label:"gap", width:90 },
                { type: "picker",       values: [ 0.0, 1.0 ], current:0.4, step: 0.1, /*orientation: 1, */label:"ratio", width:90 },
            ]
        },
        14: {
            menu: "Route Candidates Default Style",
            items: [
                { type: "picker",   values: [ 0.0, 100.0 ], current:5.0, step: 1.0 , /*orientation: 1, */label:"thick", width:120 },
            ]
        },
        15: {
            menu: "Route Candidates Selected Style",
            items: [
                { type: "picker",   values: [ 0.0, 100.0 ], current:5.0, step: 1.0 , /*orientation: 1, */label:"thick", width:120 },
            ]
        },
        16: {
            menu: "Route Alternate Style",
            items: [
                { type: "picker",   values: [ 0.0, 100.0 ], current:5.0, step: 1.0 , /*orientation: 1, */label:"thick", width:120 },
            ]
        },
        17: {
            menu: "Track Point Style ",
            items: [
                { type: "picker",       values: [ 0, 20 ], current:1.0, step: 1.0,/* orientation: 1,*/ label:"line width  ", width:120},
            ]
        },
        18: {
            menu: "3D Landmark",
            splitter: "Rendering Performance",
            items: [
               { type: "radio", values: [ "NO", "YES" ], current:0 }
            ]
        },
        19: {
            menu: "Building Outline 3D",
            items: [
               { type: "radio", values: [ "NO", "YES" ], current:1 }
            ]
        },
        20: {
            menu: "Building Outline 2D",
            items: [
               { type: "radio", values: [ "NO", "YES" ], current:1 }
            ]
        },
        21: {
            menu: "Show only 2D",
            items: [
               { type: "radio", values: [ "NO", "YES" ], current:1 }
            ]
        }
    }


    property variant mapView: null
    signal changeMenuData(int menuId, variant data, int keyIndex)

    function setConfigSettings() {
        controlItem.setConfigSettings();
    }
    function saveConfigSettings() {
        controlItem.saveConfigSettings();
    }

    function decRGBA2Color(d32) {
        var r = d32 >> 24 & 255
        var g = d32 >> 16 & 255
        var b = d32 >> 8  & 255
        var a = d32  & 255
        var color = Qt.rgba(r/255,g/255,b/255,a/255)
        console.log(" color : "+color.toString(16));
        return Qt.rgba(r/255,g/255,b/255,a/255)
    }

    function hexARGB2dec(hex){
        var c;
        if(/^#([A-Fa-f0-9]{4}){1,3}$/.test(hex)){
            c=hex.substring(1).split('');
            if(c.length === 3){ // shortcut hex format as 3 characters
                c=[c[0], c[0], c[1], c[1], c[2], c[2]];
            }
            c='0x'+c.join('');
            return parseInt(c,16)
        }
        return 0
    }


    Component.onCompleted: {
        for (var i=0;i<22;i++) {
            // if a menu has multiple items
            var spline  = false
            var splabel = ""
            if (kMenus[i].splitter != undefined) {
               spline = true
               splabel = kMenus[i].splitter
            }
               
            var subValues = {}
            for (var j=0;j<kMenus[i].items.length;j++)
                subValues[j] = kMenus[i].items[j].current

            listModel.append (
                        {
                            id: i,
                            menu: kMenus[i].menu,
//                            value: kMenus[i].items[0].current,
                            value: subValues,
                            splitter: {"enabled":spline, "label":splabel}
                        }
                        )
        }
    }

    onMapViewChanged: {
        controlItem.connectUi(this, mapView);
    }

    Rectangle {
        anchors.fill: parent
        color: "#236470"
        opacity: 0.80

    }

    ListModel {
        id: listModel;
    }


    ListView {
        id: listView
        width: parent.width * 0.94
        height: parent.height * 0.94
        anchors.centerIn: parent
        model: listModel
        delegate: menuDele
        cacheBuffer: height * 2

        TScrollBar {
            flickable: parent
            anchors.right: parent.right
            anchors.rightMargin: -7
            width: 2
            color: MultiSenseCoordinator.kAmbientColor
            grooveColor: GlobalConstants.kScrollBarBackColor
            rightMargin: GlobalConstants.kScrollBarRightMargin
        }
    }

    // quick control
    MapConfigControl {
        id: controlItem
        onSetMenuData: {
            var keyvalue = {};
            keyvalue = listModel.get(menuId).value
            keyvalue[keyIndex] = data
            listModel.setProperty(menuId, "value", keyvalue)
        }
        onMenuDataFinished: {
           listView.model = null;
           listView.model = listModel;
        }
        onUpdateView: {
            var cx = listView.contentX;
            var cy = listView.contentY;
            listView.model = null;
            listView.model = listModel;
            listView.contentX = cx;
            listView.contentY = cy;
        }
    }

    // Menu Splitter
    Component {
        id: menuSplitter
        Item {
            width: rootItem.width
            height: 31
            property string label
            Text {
                id: splitterLabel
                width: parent.width
                height: parent.height - 1
                font { family: GlobalConstants.kFontFamily; pixelSize: 22; bold:true}
                color: "white"
            }
            Rectangle {
                width: parent.width * 0.9
                height: 1
                color: "white"
                anchors.top: splitterLabel.bottom
                anchors.topMargin: 0
            }
            Component.objectName: {
                if (label.length > 0) {
                    splitterLabel.text = label
                    splitterLabel.visible = true
                }
            }
        }
   }


    // Menu Delegate
    Component {
        id: menuDele
        Item {
            id: menuItem
            property variant valueSet: model.value
            property int menuIndex: index
            property bool hasSplitter: model.splitter["enabled"]
            property string splitterString: model.splitter["label"]
            width: listView.width
            height: 76 + (hasSplitter ===  true)*(31 + 20) // splitter height + margin top

            Loader {
                id: splitterLoader
                sourceComponent: {
                    if (hasSplitter)
                        return menuSplitter
                    else
                        return null
                }
                onLoaded: {
                    if (item !== null) {
                        item.width = menuItem.width
                        item.label = splitterString
                        item.height = 31
                        height = item.height
                        y = 20
                    }
                    else {
                        visible = false
                        height = 0
                    }
                }
            }

            Text {
                id: menuLabel
                width: parent.width - 10
                height: 20
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: splitterLoader.bottom
                anchors.topMargin: 2
                font { family: GlobalConstants.kFontFamily; pixelSize: 20 }
                color: "white"
                verticalAlignment: Text.AlignVCenter
                text: model.menu
            }

            Item {
                id: keyItemBody
                width: parent.width - 10
                height: parent.height - menuLabel.height - splitterLoader.height - splitterLoader.y
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top : menuLabel.bottom
                anchors.topMargin: 0
                readonly property int keyItemWidth: width / kMenus[index].items.length


            Row {
                spacing: 4
            Repeater { // uncomment if it has multiple items per a menu
                id: keyItemList
                model: kMenus[menuIndex].items.length
            // Key Item
                Item {
                    id: keyItem
                    width: keyItemBody.keyItemWidth
                    height: keyItemBody.height
                    readonly property int keyIndex: index // should take a index in repeater if it has multiple items
                    readonly property int valueLength: kMenus[menuIndex].items[keyIndex].values.length
                    readonly property variant values:  kMenus[menuIndex].items[keyIndex].values

                    // Value Item
                    Loader {
                        id: valueItem
                        sourceComponent: {
                            var valueType = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].type
                            if (valueType === "radio")
                                return radioGroupComp
                            else if ( valueType === "slider" ) {
                                return sliderComp
                            }
                            else if ( valueType === "picker" ) {
                                return pickerbtnComp
                            }
                            else if ( valueType === "colorPalette") {
                                return paletteComp
                            }
                            else if ( valueType === "checkbox") {
                                return checkboxComp
                            }

                            else
                                return null
                        }

                        onLoaded: {
                            if (item !== null) {
                                if (item.controlType === "radio") {
                                    item.optionNum = keyItem.valueLength
                                    item.optionLabels = keyItem.values

                                } else if (item.controlType === "slider") {
                                    item.minValue = keyItem.values[0]
                                    item.maxValue = keyItem.values[1]
                                    item.stepSize = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].step
                                    var ori = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].orientation
                                    if (ori !== undefined && ori == 1) {
                                        item.orientation = Qt.Vertical
                                        item.width = keyItem.width
                                        item.height = keyItem.height
                                    }
                                    var lb = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].label
                                    if (lb !== undefined)
                                        item.extraLabel = lb
                                }
                                else if ( item.controlType === "picker" ) {
                                    item.step = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].step
                                    var lb = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].label
                                    if (lb !== undefined)
                                        item.label = lb
                                }
                                else if (item.controlType === "colorPalette") {
                                    item.label = keyItem.values[0]
                                }
                                else if ( item.controlType === "checkbox") {
                                    item.text  = keyItem.values[0]
                                }

                                if ( kMenus[menuItem.menuIndex].items[keyItem.keyIndex].width !== undefined ) {
                                    keyItem.width = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].width
//                                    item.width = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].width
                                }

                                if ( kMenus[menuItem.menuIndex].items[keyItem.keyIndex].x !== undefined ) {
                                    keyItem.x = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].x
                                }

                                if ( kMenus[menuItem.menuIndex].items[keyItem.keyIndex].y !== undefined ) {
                                    keyItem.y = kMenus[menuItem.menuIndex].items[keyItem.keyIndex].y
                                }

                                item.currentValue = menuItem.valueSet[keyItem.keyIndex]
//                                item.currentValue = model.values[keyItem.keyIndex] // for multiple items
                            }
                        }

                        Connections {
                            target: valueItem.item
                            onValueChangedByUi: {
                                  rootItem.changeMenuData(menuIndex,valueItem.item.currentValue, keyIndex)
                                  var keyvalue = {};
                                  keyvalue = listModel.get(menuIndex).value
                                  keyvalue[keyIndex] = valueItem.item.currentValue
                                  listModel.setProperty(menuIndex, "value", keyvalue)
                            }
                        }
                    }
                } // key items
              } // key item repeater
             }
            } // key item body
        }
    }

    // GUI controlset component like Progressbar, Check Button, Radio Button ..
    Component {
        id: sliderComp
        Item {
            id: sliderItem
            width: rootItem.width*0.78
            height: 20
            readonly property string controlType: "slider"
            property real maxValue: 0.0
            property real minValue: 0.0
            property alias currentValue: sliderControl.value
            property alias stepSize: sliderControl.stepSize
            property alias orientation: sliderControl.orientation
            property alias extraLabel: extraLb.text
            signal valueChangedByUi(real value)

            onCurrentValueChanged: {
               if (sliderControl.pressed)
                   valueChangedByUi(currentValue)
            }

            Slider {
                id: sliderControl
                anchors.fill:parent
                maximumValue: parent.maxValue
                minimumValue: parent.minValue
                value: 0.0
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: sliderItem.width
                        implicitHeight: 8
                        color: "#55eada"
                        radius: 8
                    }
                   handle: Rectangle {
                        anchors.centerIn: parent
                        color: sliderControl.pressed ? "white" : "#55eada"
                        border.color: "#55eada"
                        border.width: 2
                        implicitWidth: 22
                        implicitHeight: 22
                        radius: 12
                    }
                }

            }

            TPickerButton {
                id:subPickerBtn
                width: 96
                height: 50
                anchors.left: sliderControl.right
                anchors.leftMargin: 4
                value: sliderControl.value
                step: sliderControl.stepSize

                bgColor: "#55eada"
                fgColor: "white"
                onValueChangedByUi : {
                    if (value > maxValue ) value = maxValue;
                    else if (value < minValue) value = minValue;

                    sliderControl.value = value
                    sliderItem.valueChangedByUi(value)
                }
            }

            Text {
                id:extraLb
                anchors.bottom: sliderControl.top
                anchors.topMargin: 2
                font { family: GlobalConstants.kFontFamily; pixelSize: 12 }
                color: "white"
                text: ""
                visible: text !== undefined && text.length > 0
            }

            Text {
                id: tooltip
                anchors.top: sliderControl.bottom
                anchors.topMargin: 1
                font { family: GlobalConstants.kFontFamily; pixelSize: 20 }
                color: "cyan"
                verticalAlignment: Text.AlignVCenter
                text: currentValue.toFixed(1)
                visible: sliderControl.hovered
            }

        }
    }


    // Radio Button Group
    Component {
        id: radioGroupComp
        Item {
            id: radioGroup
            readonly property string controlType: "radio"
            property int optionNum: 0
            property variant optionLabels: []
            property int currentValue: 0
            property bool isUiHandling: false
            ExclusiveGroup { id: exGroup }

            signal valueChangedByUi(int value)

            onCurrentValueChanged: {
               if (isUiHandling)
                   valueChangedByUi(currentValue)

               isUiHandling = false
            }

            RowLayout {
                spacing: 20
                Repeater {
                    model: radioGroup.optionNum //kMenus[menuIndex].items[keyIndex].values.length
                    RadioButton {
                        exclusiveGroup: exGroup
                        style: RadioButtonStyle {
                            label: Text {
                                font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                                color: "white"
                                text: radioGroup.optionLabels[index]
                            }

                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                radius: 9
                                border.color: control.activeFocus ? "#55eada" : "white"
                                border.width: 1
                                color: "white"
                                Rectangle {
                                    anchors.fill: parent
                                    visible: control.checked
                                    color: "#55eada"
                                    radius: 9
                                    anchors.margins: 4
                                }
                            }
                        }

                        onClicked: {
                            radioGroup.isUiHandling = true
                            radioGroup.currentValue = index
                        }

                        Component.objectName: {
                            checked = index === radioGroup.currentValue
                        }
                    }
                }
            }
        }
    }

    // check button
    Component {
        id: checkboxComp
        CheckBox {
            id:checkButton
            readonly property string controlType: "checkbox"
            property alias currentValue: checkButton.checked
            property bool isUiHandling: false
            style: CheckBoxStyle {
                label: Text {
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                    color: "white"
                    text: checkButton.text
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 9
                    border.color: checkButton.activeFocus ? "#55eada" : "white"
                    border.width: 1
                    color: "white"
                    Rectangle {
                        visible: checkButton.checked
                        color: "#55eada"
                        radius: 9
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            signal valueChangedByUi(bool isChecked)
            onClicked: {
                valueChangedByUi(checked)
            }

        }
    }

    // picker button
    Component {
        id: pickerbtnComp
        TPickerButton {
            id:pickerbtn
            width: 120
            height: 55
            readonly property string controlType: "picker"
            property alias currentValue: pickerbtn.value
            bgColor: "#55eada"
            fgColor: "white"
        }
    }

    // color palette
    Component {
        id: paletteComp
        Item {
            id: paletteItem
            width: 80
            height: 25
            readonly property string controlType: "colorPalette"
            property real currentValue: 0.0
            property bool isUiHandling: false
            property alias label: buttonLabel.text
            signal valueChangedByUi(real value)
            onCurrentValueChanged: {
                console.log("color changed:"+ currentValue);
                if (isUiHandling)
                    valueChangedByUi(currentValue)
                isUiHandling = false

                button.color = decRGBA2Color(currentValue)
                colorDlg.color = button.color
            }

            Row {
                spacing: 1
                Rectangle {
                    id: button
                    width: 22
                    height: 22
                    radius: 11
                    color: { return decRGBA2Color(paletteItem.currentValue) }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorDlg.visible = true
                        }
                    }
                }
                Text {
                    id: buttonLabel
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                    color: "white"
                }
            }
            ColorDialog {
                id: colorDlg
                visible: false
                showAlphaChannel: true
                onAccepted: {
                    paletteItem.isUiHandling = true
                    var rgb = color.toString().substring(1)
                    var alp = parseInt(color.a*255).toString(16)
                    paletteItem.currentValue = parseInt(rgb+alp,16)
                    visible = false
                }
                onRejected: {
                    visible = false
                }
            }

        }

    }

}
