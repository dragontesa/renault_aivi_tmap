import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view"
import ZoneApiTestEvent 1.0

PopupView {
    id: root

    property int appId: 3
    property var zone1Types: [
        'HOME_PAGE',
        'TITLE',
        'TITLE_PHONE',
        'SEARCH_BUTTON',
        'SEARCH_BUTTON_PHONE',
        'SEARCH_BUTTON_ABC',
        'SEARCH_BUTTON_ABC_PHONE',
        'SEARCH_BAR',
        'SEARCH_TEXT_1',
        'SEARCH_TEXT_2',
        'NAVIGATION_FEEDBACK',
        'AUDIO_FEEDBACK',
        'HOME_MENU_EDIT_1',
        'HOME_MENU_EDIT_2',
        'Zone1Type_TOGGLE',
        'VOICE_REC',
        'ONLY_CLOCK',
        'NAV_DEBUG_MENU',
        'NAV_SEARCH_BAR',
        'Zone1Type_NO_DATA',
        'ABC_SEARCHLIST_002',
    ]
    property var zone3Types: [
        'BASIC',
        'BUTTONS_1',
        'BUTTONS_2',
        'BUTTONS_2X2',
        'TABS_2',
        'TABS_3',
        'TABS_4',
        'TABS_5',
        'VIDEO_PLAYER',
        'VIDEO_PLAYER_2',
        'PHOTO_PLAYER',
        'NEXT_PREVIOUS',
        'PHONE_CALLS',
        'SLIDER',
        'PAGE_INDICATOR',
        'HOMEPAGESLIDER',
        'NAVI_NCS',
        'BUTTONS_2X2_NAVI',
        'BUTTONS_2X2_NAVI_119',
        'VOLUME',
        'VR_DESAMBIGUATE',
        'BASIC_VR',
        'BASIC_VR_C',
        'HFP_GUIDANCE',
        'ONLY_TEXT',
        'BUTTONS_3',
        'KEYBOARD_FREEMODE',
        'KEYBOARD_MATCHMODE',
        'NO_DATA',
        'TABS_3_ADAS',
        'ADORNER_BUTTON_SLIDER_ANIMATION',
        'KEYBOARD_FREEMODE_ZONE2',
    ]
    property int zone1Type: 0
    property int zone3Type: 0
    property int keyboardType: 0
    property bool updateRegion: false
    property int value: 0

    Rectangle { anchors.fill: parent; color: 'lightgray' }

    Component {
        id: buttonsComp
        Item {
            id: item
            width: parent.width; height: 32;
            signal minusClicked
            signal plusClicked
            Row {
                width: 64; height: 32

                Rectangle {
                    width: 32; height: 32; color: 'gray'; border.color: 'white'
                    Text {
                        anchors.centerIn: parent;
                        text: '-'
                        color: 'white'
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: item.minusClicked()
                    }
                }
                Rectangle {
                    width: 32; height: 32; color: 'gray'; border.color: 'white'
                    Text {
                        anchors.centerIn: parent;
                        text: '+'
                        color: 'white'
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: item.plusClicked()
                    }
                }
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: 16
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'appId - ' + appId; verticalAlignment: Text.AlignVCenter }
            Loader { id: appIdLoader; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: appIdLoader.item
                onMinusClicked: appId += 1
                onPlusClicked: appId -= 1
            }
        }
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'zone1 type - ' + zone1Types[zone1Type]; verticalAlignment: Text.AlignVCenter }
            Loader { id: typeLoader1; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: typeLoader1.item
                onMinusClicked: { zone1Type -= 1; if (zone1Type < 0) zone1Type = zone1Types.length - 1 }
                onPlusClicked: { zone1Type += 1; if (zone1Type == zone1Types.length) zone1Type = 0 }
            }
        }
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'zone3 type - ' + zone3Types[zone3Type]; verticalAlignment: Text.AlignVCenter }
            Loader { id: typeLoader3; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: typeLoader3.item
                onMinusClicked: { zone3Type -= 1; if (zone3Type < 0) zone3Type = zone3Types.length - 1 }
                onPlusClicked: { zone3Type += 1; if (zone3Type == zone3Types.length) zone3Type = 0 }
            }
        }
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'keyboard type - ' + keyboardType; verticalAlignment: Text.AlignVCenter }
            Loader { id: typeLoaderKeyboard; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: typeLoaderKeyboard.item
                onMinusClicked: { keyboardType -= 1; if (keyboardType < 0) keyboardType = 0 }
                onPlusClicked: { keyboardType += 1 }
            }
        }
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'updateRegion - ' + (updateRegion ? 'true' : 'false'); verticalAlignment: Text.AlignVCenter }
            Loader { id: updateRegionLoader; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: updateRegionLoader.item
                onMinusClicked: { updateRegion = !updateRegion }
                onPlusClicked: { updateRegion = !updateRegion }
            }
        }
        Item {
            width: parent.width; height: 32
            Text { width: 256; height: 32; text: 'value - ' + value; verticalAlignment: Text.AlignVCenter }
            Loader { id: valueLoader; x: 256; width: parent.width - 128; height: 32; sourceComponent: buttonsComp }
            Connections {
                target: valueLoader.item
                onMinusClicked: { value -= 1 }
                onPlusClicked: { value += 1 }
            }
        }
        Row {
            width: parent.width; height: 32
            Rectangle {
                width: parent.width/3; height: 32; color: 'blue';
                Text { anchors.centerIn: parent; text: 'Call zone1'; color: 'white' }
                MouseArea { anchors.fill: parent;
                    onClicked: {
                        var params = { 'appId':appId, 'type':zone1Type, 'updateRegion':updateRegion, 'value':value }
                        sendEvent(ZoneApiTestEvent.Call, params)
                        console.log('sendEvent(ZoneApiTestEvent.Call, ' + params + ')')
                    }
                }
            }
            Rectangle {
                width: parent.width/3; height: 32; color: 'red';
                Text { anchors.centerIn: parent; text: 'Call zone3'; color: 'white' }
                MouseArea { anchors.fill: parent;
                    onClicked: {
                        var params = { 'appId':appId, 'type':zone3Type+100, 'updateRegion':updateRegion, 'value':value }
                        sendEvent(ZoneApiTestEvent.Call, params)
                        console.log('sendEvent(ZoneApiTestEvent.Call, ' + params + ')')
                    }
                }
            }
            Rectangle {
                width: parent.width/3; height: 32; color: 'green';
                Text { anchors.centerIn: parent; text: 'Call keyboard'; color: 'white' }
                MouseArea { anchors.fill: parent;
                    onClicked: {
                        var params = { 'appId':appId, 'type':keyboardType, 'api':'keyboard' }
                        sendEvent(ZoneApiTestEvent.CallKeyboard, params)
                    }
                }
            }
        }
        Rectangle {
            width: parent.width; height: 32; color: 'orange'
            Text { anchors.centerIn: parent; text: 'Exit'; color: 'white' }
            MouseArea { anchors.fill: parent; onClicked: root.close() }
        }
    }
}
