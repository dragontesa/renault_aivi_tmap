import QtQuick 2.0
import AddressSearchViewEvent 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../singleton"

Rectangle {
    id: root
    objectName: "lotInput"
    property var owner
	property var layout
    property variant lotListModel
    property alias sanCheckButton: lotSanCheckButton
    property alias aptCheckButton: lotAptCheckButton
    property alias lotInputField: lotInputField
    readonly property int maxInputLength: 20
    property string lang
    property bool cswFocused: false
    property int cswFocusedIndex: -1;
    property var cswItems: [lotInputField, lotSanCheckButton, lotAptCheckButton]

    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        id: cswFocus
        width: 16
        height: 16
        radius: 8
        color: 'red'
        visible: false
        z: 99
        function updateState(focused, obj){
            if (focused) {
                var pt = obj.mapToItem(parent, 0, 0)
                x = pt.x
                y = pt.y
                if (QmlAppHelper.isDebug()) {
                    visible = true
                }
            }
            else {
                visible = false
            }
            console.log('[CSW][AddressSearchView][lotInput]cswFocus: ' + obj.objectName)
        }
    }

    onVisibleChanged: {
        if (visible) {
            csw.changeFocusedObj(root)
        }
    }

    onCswFocusedChanged: {
        console.log('[CSW][AddressSearchView][lotInput]cswFocused: ' + cswFocused)
        if (cswFocused) {
            if (cswFocusedIndex == -1) {
                cswFocusedIndex = 0
            }
        }
        else {
            cswSanFocus.visible = false;
            cswAptFocus.visible = false;
        }

        //cswFocus.updateState(cswFocused, cswItems[cswFocusedIndex])
    }

    onCswFocusedIndexChanged: {
        console.log('[CSW][AddressSearchView][lotInput]cswFocusedIndex: ' + cswFocusedIndex)
        //cswFocus.updateState(cswFocused, cswItems[cswFocusedIndex])
        cswSanFocus.visible = false;
        cswAptFocus.visible = false;
        if (cswFocusedIndex == 1) {
            cswSanFocus.visible = true;
        }
        else if (cswFocusedIndex == 2) {
            cswAptFocus.visible = true;
        }
    }

    function onCswCcw() {
        if ( cswFocusedIndex > 0 ) {
            cswFocusedIndex--
        }
    }

    function onCswCw() {
        if ( cswFocusedIndex + 1 < cswItems.length ) {
            cswFocusedIndex++
        }
    }

    function onCswLeft() {

    }

    function onCswRight() {
    }

    function onCswSelect( selectedIndex ) {
        console.log('[CSW][AddressSearchView][lotInput]onCswSelect')
        cswItems[cswFocusedIndex].onCswSelect()
    }
	
	function showKeyboard() {
        // chunjae.lee 2018.04.12  텍스트 유지
		lotInputField.forceActiveFocus()
        sendEvent(AddressSearchViewEvent.RegistKeyboardInput, {"keyboardInput": lotInputField})
        root.owner.showKeyboard(lotKeyboard)
    }
	
    function available(able) {
        lotLabel.enabled = lotSanLabel.enabled = lotSanCheckButton.enabled = lotInputFieldWrap.enabled = able
    }

    function init(able) {
         available(able)
         if (able) {
            lotAptCheckButton.selected = lotSanCheckButton.selected = false
            lotInputField.text = ""
             sendEvent(AddressSearchViewEvent.ClearKeyBuffer)
         }
    }

    onLangChanged: {
        //console.debug(lang)
        if (lang == 'en') {
            lotLabel.horizontalAlignment = Text.AlignHCenter
        }
        else if (lang == 'kr') {
            lotLabel.horizontalAlignment = Text.AlignLeft
        }
    }
	
	Rectangle {
		id: lotInputWrap
		height: layout.inputWrapHeight
        anchors.left: parent.left
        anchors.leftMargin: layout.inputLeftRightMargin
        anchors.right: parent.right
        anchors.rightMargin: layout.inputLeftRightMargin

		anchors.top: parent.top
		anchors.topMargin: layout.inputTopMargin
		color: layout.inputBgColor

		property alias sanCheckButton: lotSanCheckButton
		property alias aptCheckButton: lotAptCheckButton

		Text {
			id: lotLabel
			width: layout.lotInputLotLabelWidth
			anchors.left: parent.left
			anchors.leftMargin: layout.lotInputLotLabelLeftMargin
			anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
			font.bold: false
            font.pixelSize: Resource.pixelToDp(35)
            text: qsTr('번지') + Tr.d
			color: layout.inputLabelColor

            // chunjae.lee 2018.04.12
            opacity: (!enabled) ? 0.4 : 1.0
        }

		Rectangle {
			id: lotInputDivideLine1
			anchors.left: lotLabel.right
            anchors.leftMargin: layout.inputDivideLineLeftMargin
			width: layout.inputDivideLineWidth
			height: parent.height
			color: layout.inputDivideLineColor
		}

        Item {
			id: lotInputFieldWrap
			width: layout.lotInputInputWidth
			anchors.left: lotInputDivideLine1.right
			anchors.leftMargin: layout.lotInputInputLeftMargin
			anchors.top: parent.top
			anchors.bottom: parent.bottom
            clip: true
			
			TextInput {
				id: lotInputField
                objectName: "lotInputField"
				color: layout.inputTextColor
				focus: false
				clip: true
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter

                // chunjae.lee 2018.04.12
                opacity: (!enabled) ? 0.4 : 1.0
                visible: enabled

                onTextChanged: {
                    if ( lotAptCheckButton.selected ) {
                        text = ''
                    }

                    text = text.substring(0, maxInputLength)
                    var temp = text.replace( /[^0-9-]/g, '')
                    text = temp
                    var arr = []
                    for ( var i = 0; i < text.length; i++ ) {
                        var ch = text.charAt( i )
                        arr.push( Resource.imageUrl( "input_num_" + ch + ".png" ) )
                    }
                    lotInputFieldDisplay.lotInputFieldNumberImageSources = arr
                }

                function onCswSelect() {
                    if (lotInputField.text.length > 0) {
                        //owner.showSearchResultView(lotInputField.text)
                    }
                }
			}
            Rectangle {
                id: lotInputFieldDisplay
                width: childrenRect.width
                height: childrenRect.height
                anchors.verticalCenter: parent.verticalCenter
                color: layout.inputBgColor
                visible: true

                // chunjae.lee 2018.04.12
                opacity: (!enabled) ? 0.4 : 1.0

                property var lotInputFieldNumberImageSources: []

                Row {
                    width: childrenRect.width
                    height: childrenRect.height
                    Repeater {
                        model: lotInputFieldDisplay.lotInputFieldNumberImageSources
                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source: modelData
                            Image {
                                source: parent.source
                            }
                        }
                    }
                }
            }
		}

        Rectangle {
            id: cswSanFocus
            color: 'white'
            anchors.right: lotInputAptDivideLine.left
            width: 145
            height: parent.height
            visible: false
        }

        Rectangle {
            id: cswAptFocus
            color: 'white'
            anchors.left: lotInputAptDivideLine.right
            width: 191
            height: parent.height
            visible: false
        }

		Text {
			id: lotSanLabel
			width: layout.lotInputSanLabelWidth
			anchors.left: lotInputFieldWrap.right
			anchors.leftMargin: layout.lotInputSanLabelLeftMargin
			anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignRight
			font.bold: false
            font.pixelSize: Resource.pixelToDp(30)
			text: qsTr('산')
            color: cswSanFocus.visible ? layout.inputLabelCswTextColor : layout.inputLabelTextColor

            // chunjae.lee 2018.04.12
            opacity: (!enabled) ? 0.4 : 1.0
		}

		TCheckButton {
			id: lotSanCheckButton
            objectName: "lotSanCheckButton"
			anchors.left: lotSanLabel.right
			anchors.leftMargin: layout.lotInputSanCheckLeftMargin
			width: img.width
			height: img.height
			img.width: layout.inputCheckButtonImgWidth
			img.height: layout.inputCheckButtonImgHeight
			imgCheck:  Resource.imageUrl("checkbox_on.png")
			imgUncheck: Resource.imageUrl("checkbox_off.png")
			anchors.verticalCenter: parent.verticalCenter
			selected: 0
			onClicked: {
			}
            onSelectedChanged: {
                console.log( 'selected: ' + selected )
            }

            function onCswSelect() {
                selected = 1 - selected
            }
		}

		Rectangle {
			id: lotInputAptDivideLine
			anchors.left: lotSanCheckButton.right
			anchors.leftMargin: layout.lotInputAptDivideLeftMargin
			width: layout.inputDivideLineWidth
			height: parent.height
			color: layout.inputDivideLineColor
		}

		Text {
			id: lotAptLabel
			width: layout.lotInputAptLabelWidth
			anchors.left: lotInputAptDivideLine.right
			anchors.leftMargin: layout.lotInputAptLabelLeftMargin
			anchors.verticalCenter: parent.verticalCenter
			font.bold: false
            font.pixelSize: Resource.pixelToDp(30)
            horizontalAlignment: Text.AlignRight
			text: qsTr('아파트')
            color: cswAptFocus.visible ? layout.inputLabelCswTextColor : layout.inputLabelTextColor
		}

		TCheckButton {
			id: lotAptCheckButton
            objectName: "lotAptCheckButton"
			anchors.left: lotAptLabel.right
			anchors.leftMargin: layout.lotInputAptCheckLeftMargin
			width: img.width
			height: img.height
			img.width: layout.inputCheckButtonImgWidth
			img.height: layout.inputCheckButtonImgHeight
			imgCheck:  Resource.imageUrl("checkbox_on.png")
			imgUncheck: Resource.imageUrl("checkbox_off.png")
			anchors.verticalCenter: parent.verticalCenter
			onClicked: {
                // chunjae.lee 2018.04.11 - v1.27
				if ( selected ) {
                    available(false)
				}
				else {
                    available(true)
				}
                sendEvent(AddressSearchViewEvent.ClearKeyBuffer)
			}

            function onCswSelect() {
                selected = 1 - selected
            }
		}

        Rectangle {
            id: bldInputDivideLine2
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: layout.inputBottomDivideLineColor
        }
	}

	// list
	ListView {
		id: lotListView
		width: parent.width
		anchors.top: lotInputWrap.bottom
		anchors.bottom: lotKeyboard.top
		model: root.lotListModel ? root.lotListModel.length : 0
		property int rowHeight: GlobalConstants.kListTextHeight
		interactive: true
		clip: true
		currentIndex: -1
        //highlight: listViewHighlightBar
		highlightFollowsCurrentItem: true
		highlightMoveDuration: 1
		highlightResizeDuration: 1
		flickDeceleration: 0
		pressDelay: 0
		snapMode: ListView.SnapOneItem
		delegate: lotListViewDelegate
        cacheBuffer: roadListView.rowHeight * 3
		Component {
			id: lotListViewDelegate
			Item {
				property var model: lotListModel ? lotListModel[index] : null
				anchors.left: parent.left
				anchors.leftMargin: GlobalConstants.kListBackgroundLeftMargin
				anchors.right: parent.right
				anchors.rightMargin: GlobalConstants.kListBackgroundRightMargin
				height: roadListView.rowHeight
				Text {
					id: lotListLabel
					anchors.verticalCenter: parent.verticalCenter
					font.bold: false
			                font.pixelSize: Resource.pixelToDp(GlobalConstants.kListTextFontSize)
					text: model ? model.name : ''
					color: GlobalConstants.kListTextColor
				}
				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: root.layout.lotListItemDividierHeight
					color: GlobalConstants.kListLineColor
				}
			}
		}
        Item {
            anchors.fill: parent
            visible: lotListViewScrollBar
            Image {
                source: Resource.imageUrl( 'zone', 'zone2_img_mask_2.png' )
            }
            Image {
                anchors.bottom: parent.bottom
                source: Resource.imageUrl( 'zone', 'zone2_img_mask.png' )
            }
        }
        TScrollBar {
            id: lotListViewScrollBar
            flickable: parent
            width: 2
            color: MultiSenseCoordinator.kAmbientColor
            grooveColor: GlobalConstants.kScrollBarBackColor
            rightMargin: GlobalConstants.kScrollBarRightMargin
        }

        // chunjae.lee s 2018.04.12  - v1.27 168p
        Text {
            text: qsTr("선택한 동/읍/면 내의 모든 아파트를 검색합니다.")
            anchors.centerIn:  parent
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
            color: GlobalConstants.kListTextColor
            visible: lotAptCheckButton.selected
        }
        // chunjae.lee e 2018.04.12 - v1.27 168p
	}

	Item {
		id: lotKeyboard
		anchors.bottom: parent.bottom
		width: parent.width
		height: layout.inputKeyboardHeight
		visible: false
		Component.onCompleted: {
		}
		onVisibleChanged: {
		}
	}	
}
