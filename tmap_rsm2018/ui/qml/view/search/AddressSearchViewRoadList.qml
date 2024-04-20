import QtQuick 2.0
import AddressSearchViewEvent 1.0
import CommonConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import ViewRegistry 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../util/DebugUtils.js" as Debug
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../singleton"
import QtGraphicalEffects 1.0

import Csw 1.0

ListView {
    id: listView
    objectName: 'roadList'
	
    //Timer
    property var timer : null

	property var owner
    property var csw
    property var layout
    property var shortcut
    property var siGunGuListView
    visible: false
	
	model: owner.roadListModel ? owner.roadListModel.length : 0
	property int rowHeight: GlobalConstants.kListTextItemHeight
	property var pressedItem: null
    PressBar {
        currentItem: listView.pressedItem
        leftMargin: GlobalConstants.kListBackgroundLeftMargin
        rightMargin: GlobalConstants.kListBackgroundRightMargin
    }
	highlight: HighlightBar {
        currentItem: listView.currentItem
		leftMargin: GlobalConstants.kListBackgroundLeftMargin
		rightMargin: GlobalConstants.kListBackgroundRightMargin
        color: listView.highlightColor
	}
	interactive: true
	clip: true
	highlightFollowsCurrentItem: true
	highlightMoveDuration: 1
	highlightResizeDuration: 1
//                flickDeceleration: 0
//                pressDelay: 0
//                snapMode: ListView.SnapOneItem
    cacheBuffer: listView.rowHeight * 3
    delegate: listViewDelegate

	property bool cswFocused: false
	property int cswFocusedIndex: -1;
	property color highlightColor: CommonConstants.Transparent

	onVisibleChanged: {
        console.log('[CSW][AddressSearchView][roadList]visible: ' + visible)
		if (visible) {            
            csw.changeFocusedObj(listView)
		}
		else {
            csw.changeFocusedObj(siGunGuListView)
		}
	}

	onCswFocusedChanged: {
        console.log('[CSW][AddressSearchView][roadList]cswFocused: ' + cswFocused)
		if (cswFocused) {
            listView.highlightColor = GlobalConstants.kListFocusColor
			if (cswFocusedIndex == -1) {
				cswFocusedIndex = 0
			}
		}
		else {
            listView.highlightColor = CommonConstants.Transparent
		}
	}

	onCswFocusedIndexChanged: {
        console.log('[CSW][AddressSearchView][roadList]cswFocusedIndex: ' + cswFocusedIndex)
		currentIndex = cswFocusedIndex
	}

	onCurrentIndexChanged: {
        console.log('[CSW][AddressSearchView][roadList]currentIndex: ' + currentIndex)
	}

	Component.onCompleted: {
	}

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

	onModelChanged: {                    
	}

	function onCswCcw() {
        if ( listView.cswFocusedIndex > 0 ) {
            listView.cswFocusedIndex--
		}
	}

	function onCswCw() {
        if ( listView.cswFocusedIndex + 1 < owner.roadListModel.length ) {
            listView.cswFocusedIndex++
		}
	}

	function onCswLeft() {
	}

	function onCswRight() {
        csw.changeFocusedObj(shortcut)
	}

    function onCswSelect( selectedIndex ) {
        var model = roadListModel[selectedIndex]
        owner.scdName = roadListModel[selectedIndex].name

        listView.currentIndex = selectedIndex
        selectedModel = roadListModel[selectedIndex]

        listView.highlightColor = MultiSenseCoordinator.kAmbientColor
		var sleep =  GlobalConstants.kPressedColorExposureTime
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(owner, sleep, function() {
			try {
                listView.highlightColor = CommonConstants.Transparent

				// 17.10.26 hskim fix not to disappear back after road input
                //shortcut.visible = false
				var value = {code:model.code,name:model.name}
				bar.setValue(2, value)
				// chunjae.lee s 2018.04.12 - init함수 호출 추가
				bldInputWrap.init(bldInputWrap.visible = true)
				lotInputWrap.init(lotInputWrap.visible = false)
				// chunjae.lee e 2018.04.12 - init함수 호출 추가
			} catch(e) {
			}
		})
	}

	Component {
        id: listViewDelegate
		Item {
			property var model: roadListModel ? roadListModel[index] : null
            height: listView.rowHeight
			anchors.left: parent.left
			anchors.right: parent.right
            property bool cswFocused: listView.cswFocused
            property int cswFocusedIndex: listView.cswFocusedIndex
			state: 'normal'

			function updateState() {
                if ( listView.cswFocused &&
                    index == listView.cswFocusedIndex ) {
					state = 'focused'
				}
				else {
					state = 'normal'
				}
			}

			onModelChanged: {
				updateState()
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
					PropertyChanges { target: roadListLabel; color: GlobalConstants.kListTextColor }
				},
				State {
					name: "focused"
					PropertyChanges { target: roadListLabel; color: GlobalConstants.kListTextFocusColor }
				}
			]

			Text {
				id: roadListLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: GlobalConstants.kListTextLeftMargin
				anchors.right: parent.right
				anchors.rightMargin: GlobalConstants.kListTextRightMargin
				font.bold: false
                font.pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize * layout.roadListLabelFontPointSizeRatio)
				text: model ? model.name : ''
				color: GlobalConstants.kListTextColor
			}
			Rectangle {
				id: splitBar
				anchors.bottom: parent.bottom
				x: GlobalConstants.kListLineLeftMargin
				width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
				height: 1
				color: GlobalConstants.kListLineColor
			}
			MouseArea {
				anchors.fill: parent

				PressBarMouseSync {
                    cswObj: csw
                    currentItem: listView.pressedItem
					onCurrentItemChanged: {
                        if (CommonUtils.isEqual(currentItem, listView.pressedItem)) {
							return
						}
                        listView.pressedItem = currentItem
					}
				}

				onClicked: {
                    listView.onCswSelect(index)
				}
			}
		}
	}

	Item {
		anchors.fill: parent
        visible: listViewScrollBar
		Image {
			source: Resource.imageUrl( 'zone', 'zone2_img_mask_2.png' )
		}
		Image {
			anchors.bottom: parent.bottom
			source: Resource.imageUrl( 'zone', 'zone2_img_mask.png' )
		}
	}
	TScrollBar {
        id: listViewScrollBar
		flickable: parent
		width: 2
		rightMargin: GlobalConstants.kScrollBarRightMargin
		color: MultiSenseCoordinator.kAmbientColor
		grooveColor: GlobalConstants.kScrollBarBackColor
	}
}
