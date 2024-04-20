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

MultiColumnList {
    id: listView
    objectName: "siGunGuList"
    property var owner
    property var csw
    property var layout

	x: layout.listLeftMargin
	y: layout.listTopMargin
	width: layout.listWidth
	height: layout.listHeight	
	modelAccessor: listAccessor
	spacing: 0
	columns: 4
	visibleRows: 5
	property bool cswFocused: false
	property int cswFocusedIndex: -1;
	// 18.3.5 hskim to keep gun/gu when toggle is pressed
	property var selectedGunGuModel

	onVisibleChanged: {
        console.log('[CSW][AddressSearchView][siGunGuList]visible: ' + visible)
		if (visible) {
            csw.changeFocusedObj(listView)
		}
	}

	onCswFocusedChanged: {
        console.log('[CSW][AddressSearchView][siGunGuList]cswFocused: ' + cswFocused)
	}

	onCswFocusedIndexChanged: {
        console.log('[CSW][AddressSearchView][siGunGuList]cswFocusedIndex: ' + cswFocusedIndex)
	}

	onSelectedGunGuModelChanged: {
        console.log('[CSW][AddressSearchView][siGunGuList]selectedGunGuModel: ' + selectedGunGuModel)
	}

	Component.onCompleted: {
		cswFocusedIndex = 0
	}

	onModelChanged: {
        listView.cswFocusedIndex = 0
	}

	function onCswCcw() {
        if ( listView.cswFocusedIndex > 0 ) {
            listView.cswFocusedIndex--
		}
	}

	function onCswCw() {
        if ( listView.cswFocusedIndex + 1 < modelAccessor.count() ) {
            listView.cswFocusedIndex++
		}
	}

	function onCswLeft() {
	}

	function onCswRight() {
	}

    function onCswSelect( selectedIndex ) {
        selectedModel = listAccessor.get( selectedIndex )

        if ( bar.currentIndex == 0 ) {
            sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'mcd', from:'list', code:selectedModel.code, name:selectedModel.name, params:selectedModel} )
        }
        else if ( bar.currentIndex == 1 ) {
            selectedGunGuModel = selectedModel
            if ( searchType == roadSearch ) {
                sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'road', from:'list', code:selectedModel.code, name:selectedModel.name, params:selectedModel} )
                listView.visible = false
                roadListView.visible = true
            }
            else if ( searchType == lotSearch ) {
                sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'dong', from:'list', code:selectedModel.code, name:selectedModel.name, params:selectedModel} )
                listView.visible = true
                roadListView.visible = false
            }
        }
        else if ( bar.currentIndex == 2 ) {
            owner.scdName = selectedModel.name

            roadListShortcut.visible = false
            var value = {code:selectedModel.code,name:selectedModel.name}
            bar.setValue(2, value)
            // chunjae.lee s 2018.04.12 - init함수 호출 추가
            bldInputWrap.init(bldInputWrap.visible = false)
            lotInputWrap.init(lotInputWrap.visible = true)
            // chunjae.lee e 2018.04.12 - init함수 호출 추가
        }
        saveCurrentState();
	}

	itemDelegate: Item {
		id: listItemDelegate
		property int rowIndex: parent.rowIndex
		property int columnIndex: parent.columnIndex

		width: layout.listItemWidth
		height: layout.listItemHeight

		Item {
			id: listItem
			anchors.fill: parent
			visible: itemModel ? true : false
			state: 'normal'
			property int rowIndex: parent.rowIndex
			property int columnIndex: parent.columnIndex
			property int itemIndex: -1
            property bool cswFocused: listView.cswFocused
            property int cswFocusedIndex: listView.cswFocusedIndex

			onStateChanged: {
				if (itemIndex == 0) {
                    //console.debug('state: ' + state)
				}
			}

            function updateState() {
                if ( listView.cswFocused &&
                    itemIndex == listView.cswFocusedIndex ) {
                    state = 'focused'
                }
                else {
                    state = 'normal'
                }
            }

			Component.onCompleted: {
				itemIndex = Qt.binding(function(){
                    return rowIndex * listView.columns + columnIndex
				})
			}

			onItemIndexChanged: {
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
					PropertyChanges { target: listItemImage; source: Resource.imageUrl('address','address_btn_n.png') }
					PropertyChanges { target: listItemText; color: 'white' }
					PropertyChanges { target: colorOverlay; visible: true }
				},
				State {
					name: "pressed"
					PropertyChanges { target: listItemImage; source: Resource.imageUrl('address','address_btn_p.png') }
					PropertyChanges { target: listItemText; color: 'white' }
					PropertyChanges { target: colorOverlay; visible: true }
				},
				State {
					name: "focused"
					PropertyChanges { target: listItemImage; source: Resource.imageUrl('address','address_btn_f.png') }
					PropertyChanges { target: listItemText; color: 'black' }
					PropertyChanges { target: colorOverlay; visible: false }
				}
			]

			Image {
				id: listItemImage
                property int isFill: layout.listItemImageFill
				onIsFillChanged: {
					if ( isFill ) {
						anchors.fill = parent
					}
				}
			}                        

			ColorOverlay {
				id: colorOverlay
				anchors.fill: listItemImage
				source: listItemImage
				cached: true
				color: MultiSenseCoordinator.kAmbientColor
			}

			Item {
				anchors.fill: listItemImage
				Text {
					id: listItemText
					width: 128;
					horizontalAlignment: Text.AlignHCenter;
					verticalAlignment: Text.AlignVCenter;
					wrapMode: Text.WordWrap
					anchors.centerIn: parent
					text: (visible ? (itemModel ? itemModel.name : '') : '')
					font.pixelSize: Resource.pixelToDp(30)

				}
			}

			TMouseArea {
				id: listItemMouseArea
				anchors.fill: listItem
				onPressedChanged: {
					if (pressed) {
						listItem.state = "pressed"
						listItem.scale = 0.85
					}
					else {
						listItem.state = "normal"
						listItem.scale = 1
					}
				}
				onClicked: {
                    console.log( "rowIndex: " + listItem.rowIndex + " columns: " + listView.columns + " colIndex: " + listItem.columnIndex )
                    listView.onCswSelect( listItem.rowIndex * listView.columns + listItem.columnIndex )
				}
			}
		}
	}
}

            
