import QtQuick 2.0
import AddressSearchViewEvent 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import Csw 1.0

Rectangle {
    id: root
    objectName: "bldInput"
    property var owner
	property var layout
    property variant bldListModel
    property alias bldInputField : bldInputField
    readonly property int maxInputLength: 20
    property int cswFocusedIndex: -1;
	
	function showKeyboard() {
        // chunjae.lee 2018.04.12  텍스트 유지
        bldInputField.forceActiveFocus()
        sendEvent(AddressSearchViewEvent.RegistKeyboardInput, {"keyboardInput": bldInputField})
        root.owner.showKeyboard(bldKeyboard)
	}	

    function init(able) {
        if (able) {
            bldInputField.text = ""
            sendEvent(AddressSearchViewEvent.ClearKeyBuffer)
        }
    }

    property bool cswFocused: false
    property color highlightColor: GlobalConstants.kListFocusColor

    onVisibleChanged: {
        if (visible) {
            csw.changeFocusedObj(root)
        }
    }

    onCswFocusedChanged: {
        console.log('[CSW][AddressSearchView][bldInput]cswFocused: ' + cswFocused)
        cswFocus.updateState(cswFocused, bldInputField)
    }

    function onCswCcw() {
    }

    function onCswCw() {
    }

    function onCswLeft() {
    }

    function onCswRight() {
    }

    function onCswSelect( selectedIndex ) {
        console.log('[CSW][AddressSearchView][bldInput]onCswSelect')
        if (bldInputField.text.length > 0) {
            //owner.showSearchResultView(bldInputField.text)
        }
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
        }
    }
	
	Rectangle {
		id: bldInputWrap		
		height: layout.inputWrapHeight
		anchors.left: parent.left
		anchors.leftMargin: layout.inputLeftRightMargin
        anchors.right: parent.right
        anchors.rightMargin: layout.inputLeftRightMargin
		anchors.top: parent.top
		anchors.topMargin: layout.inputTopMargin
        color: layout.inputBgColor

		Text {
			id: bldLabel
			width: layout.bldInputLotLabelWidth
			anchors.left: parent.left
			anchors.leftMargin: layout.bldInputLotLabelLeftMargin
			anchors.verticalCenter: parent.verticalCenter
			font.bold: false
            font.pixelSize: Resource.pixelToDp(layout.inputLabelFontSize)
			text: qsTr('건물번호') + Tr.d
			color: layout.inputLabelColor
            horizontalAlignment: Text.AlignLeft
		}

		Rectangle {
			id: bldInputDivideLine1
			anchors.left: bldLabel.right
            anchors.leftMargin: layout.inputDivideLineLeftMargin
			width: layout.inputDivideLineWidth
			height: parent.height
			color: layout.inputDivideLineColor
		}

		Item {
			id: bldInputFieldWrap
			width: layout.bldInputInputWidth
			anchors.left: bldInputDivideLine1.right
			anchors.leftMargin: layout.bldInputInputLeftMargin
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			TextInput {
				id: bldInputField
				color: layout.inputTextColor
				focus: false
				clip: true
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
                opacity: (!enabled) ? 0.4 : 1.0
                onTextChanged: {
                    //[TMCRSM-211 leejaeho] support hyphen
                    text = text.substring(0, maxInputLength)

                    QmlAppHelper.debug( text )

                    var temp = text.replace( /[^0-9-]/g, '')
                    text = temp
                    var arr = []
                    for ( var i = 0; i < text.length; i++ ) {
                        var ch = text.charAt( i )
                        arr.push( Resource.imageUrl( "input_num_" + ch + ".png" ) )
                    }
                    bldInputFieldDisplay.bldInputFieldNumberImageSources = arr
                }
			}
            Rectangle {
                id: bldInputFieldDisplay                
                width: childrenRect.width
                height: childrenRect.height
                anchors.verticalCenter: parent.verticalCenter
                color: layout.inputBgColor
                visible: true
                clip: true

                property var bldInputFieldNumberImageSources: []

                Row {
                    width: childrenRect.width
                    height: childrenRect.height
                    Repeater {
                        model: bldInputFieldDisplay.bldInputFieldNumberImageSources
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
            id: bldInputDivideLine2
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: layout.inputBottomDivideLineColor
        }
	}
	
    // 18.3.4 hskim not used
	// list
    Component {
        id: bldListViewHighlightBar
        Rectangle {
            anchors.left: parent ? parent.left : undefined
            anchors.leftMargin: GlobalConstants.kListBackgroundLeftMargin
            anchors.right: parent ? parent.right : undefined
            anchors.rightMargin: GlobalConstants.kListBackgroundRightMargin
            height: GlobalConstants.kListTextHeight
            color: MultiSenseCoordinator.kAmbientColor
        }
    }
	
	ListView {
		id: bldListView
		width: parent.width
		anchors.top: bldInputWrap.bottom
		anchors.bottom: bldKeyboard.top
		model: root.bldListModel ? root.bldListModel.length : 0
		property int rowHeight: GlobalConstants.kListTextHeight
		interactive: true
        clip: true
        currentIndex: -1
		delegate: bldListViewDelegate
        highlight: bldListViewHighlightBar
        highlightFollowsCurrentItem: false
		highlightMoveDuration: 1
		highlightResizeDuration: 1
		flickDeceleration: 0
		pressDelay: 0
		snapMode: ListView.SnapOneItem
        cacheBuffer: bldListView.rowHeight * 3

		Component {
			id: bldListViewDelegate
			Item {
				property var model: bldListModel ? bldListModel[index] : null
				anchors.left: parent.left
				anchors.leftMargin: GlobalConstants.kListBackgroundLeftMargin
				anchors.right: parent.right
				anchors.rightMargin: GlobalConstants.kListBackgroundRightMargin
				height: bldListView.rowHeight
				Text {
					id: bldListLabel
					anchors.verticalCenter: parent.verticalCenter
					font.bold: false
                    font.pixelSize: GlobalConstants.kListTextFontSize * root.layout.bldListLabelFontPointSizeRatio
					text: model ? model.name : ''
					color: GlobalConstants.kListTextColor
				}
				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: root.layout.bldListItemDividierHeight
					color: GlobalConstants.kListLineColor
				}
			}
		}
        Item {
            anchors.fill: parent
            visible: bldListViewScrollBar
            Image {
                source: Resource.imageUrl( 'zone', 'zone2_img_mask_2.png' )
            }
            Image {
                anchors.bottom: parent.bottom
                source: Resource.imageUrl( 'zone', 'zone2_img_mask.png' )
            }
        }
        TScrollBar {
            id: bldListViewScrollBar
            flickable: parent
            width: 2
            color: MultiSenseCoordinator.kAmbientColor
            grooveColor: GlobalConstants.kScrollBarBackColor
            rightMargin: GlobalConstants.kScrollBarRightMargin
        }
	}

	Item {
		id: bldKeyboard
		anchors.bottom: parent.bottom
		width: parent.width
		height: layout.inputKeyboardHeight
		visible: true
		Component.onCompleted: {
		}
		onVisibleChanged: {
		}
	}	
}
