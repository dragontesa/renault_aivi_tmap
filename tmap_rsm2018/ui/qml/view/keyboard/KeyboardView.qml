import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import KeyboardViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


View {
    id: root
    visible: false
    anchors.bottom: parent ? parent.bottom : undefined

    property var key: null
    property var text: null
    property var owner: null
    property var inputField: null
    property var pressed:([-1,-1])
    property var buttonText: [-1,-1,CommonConstants.Empty]

    property color highlightColor: CommonConstants.Transparent    
    property color highlightBorderColor: CommonConstants.Red
    property int highlightBorderWidth: 2
    property int controllerMargin: 0

    property bool isSymbol: false
    property bool isHangle: false
    property bool isEnglish: false
    property bool dontVisible: false

    property bool searchBtnAlwaysEnable: keyboard.searchBtnAlwaysEnable
    readonly property string defaultSearchBtnText: qsTr("검색")
    readonly property int searchKeybdRow: keyboard.searchKeybd[0]
    readonly property int searchKeybdCol: keyboard.searchKeybd[1]

    property var keyboard: dummyKeyboard

    Item {
        id: dummyKeyboard
        property bool searchBtnAlwaysEnable: false
        property var searchKeybd: [3,5]
        property var symbolKeybd: [3,1]
        property var hangleKeybd: [3,2]
        property var englishKeybd: [3,3]
        property var pressed:([-1,-1])
        property int currentIndex: -1
        property bool focused: false
        property color highlightColor
        property var currentItem
        function setTextBtn(row, col, text) {}
        function isSymbol() { return false }
        function isHangle() { return false }
        function isEnglish(){ return false }
    }

    function loadSwKeyboard() {
        swKeyboardLoader.source = '../../component/TKeyboard.qml'
    }

    function initKeyboarRadioBtn() {
        if (isSymbol) {
            keyboard.pressed = keyboard.symbolKeybd
        } else if (isHangle) {
            keyboard.pressed = keyboard.hangleKeybd
        } else if (isEnglish) {
            keyboard.pressed = keyboard.englishKeybd
        }        
        keyboard.setTextBtn(searchKeybdRow, searchKeybdCol, defaultSearchBtnText)
    }

    onButtonTextChanged: {
        var row = buttonText[0]
        var col = buttonText[1]
        var text = buttonText[2]
        if (row < 0 || col < 0) {
            return
        }
        keyboard.setTextBtn(row, col, text)
    }

    function isValidProcessKeyboard() {
        if(CommonUtils.isNull(key) || CommonUtils.isNull(inputField) || !inputField.text.length)
            return false
        return true
    }

    function textInput(text) {
        if (CommonUtils.isNull(inputField))
            return
        if (CommonUtils.isEqual(text , inputField.text))
            return
         inputField.text = text
        if (!isValidProcessKeyboard())
            return
        processKeyboard(owner, key, inputField.text)
    }

    function processKeyboard(owner,key,text){
        root.text = text
        root.key = null

        sendEvent(KeyboardViewEvent.ProcessKeyboard,
                  { owner: owner,
                    key: key,
                    text: text
                  })
    }

    function visibleChange(visible) {
        console.log("###keyboradView:owner="+owner+",visible="+visible)
        keyboard.currentIndex = -1
        if (visible) {
           sendEvent(KeyboardViewEvent.RemoveCsw, csw)
           sendEvent(KeyboardViewEvent.RegistCsw, [ csw ])
       } else {
            searchBtnAlwaysEnable = false
            sendEvent(KeyboardViewEvent.RemoveCsw, csw)                                                
        }
    }

    onTextChanged: {
        if (CommonUtils.isNull(text))
            return
        textInput(text)
    }

    onKeyChanged: {
        if (!isValidProcessKeyboard())
            return
        processKeyboard(owner, key, inputField.text)
    }    

    onPressedChanged: {
        var row = pressed[0]
        var col = pressed[1]
        if (row < 0 || col < 0) {
            return
        }
        isSymbol = keyboard.isSymbol()
        isHangle = keyboard.isHangle()
        isEnglish = keyboard.isEnglish()
        keyboard.pressed = [row,col]
    }

    onOwnerChanged: {
        visibleChange(visible)        
    }

    onVisibleChanged: {        
        visibleChange(visible)
        if(!visible) {
            initKeyboarRadioBtn()
        }
        if (dontVisible) {
            root.height = 0
        }
    }

    onMediated: {
        sendEvent(KeyboardViewEvent.Mediated)
    }

    onActivated: {
        visibleChange(visible)
        sendEvent(KeyboardViewEvent.Activated)
    }

    Loader {
        id: swKeyboardLoader
        anchors.fill: parent
        z: parent.z

        property var currentItem
        onLoaded: {
            root.keyboard = item
            item.focusObjectId = inputField
            keyboardBinder0.target = item
            keyboardBinder1.target = item
            keyboardBinder2.target = swKeyboardLoader
            item.textInput.connect(onTextInput)
            item.width = 820
            item.height = 419
        }
        Rectangle {
            x: ((parent.currentItem) ? parent.currentItem.x + parent.currentItem.parent.x : 0) + controllerMargin
            y: ((parent.currentItem) ? parent.currentItem.y + parent.currentItem.parent.y : 0) + controllerMargin
            z: ((parent.currentItem) ? parent.currentItem.z + 1 : 0)
            width: ((parent.currentItem) ? parent.currentItem.width : 0) - (controllerMargin*2)
            height:((parent.currentItem) ? parent.currentItem.height : 0) - (controllerMargin*2)
            border.color: highlightBorderColor
            border.width: highlightBorderWidth
            radius: height / 2
            color: highlightColor
            visible: (parent.currentItem && parent.currentItem.visible) ? true : false
        }
    }
    Binding {
        id: keyboardBinder0
        property: 'focusObjectId'; value: inputField
    }
    Binding {
        id: keyboardBinder1
        property: 'visible'; value: !dontVisible
    }
    Binding {
        id: keyboardBinder2
        property: 'currentItem'
        value: swKeyboardLoader.item ? swKeyboardLoader.item.currentItem : null
    }

    function onTextInput(key) {
        var text = (inputField &&inputField.text) ? inputField.text : CommonConstants.Empty
        processKeyboard(owner, key, text)
    }

    Csw {
            id: csw

            section: Csw.Section_Ime
            sequence: 0
            enable: root.visible

            onFocused: {
                keyboard.focused = true
                keyboard.highlightColor = GlobalConstants.kListFocusColor
                if (keyboard.currentIndex < 0)
                    keyboard.currentIndex = 0
            }
            onLostFocusd: {
                keyboard.highlightColor = CommonConstants.Transparent
                keyboard.focused = false
                keyboard.currentIndex = -1
            }
            onLeftPressed: {
                var item = keyboard.currentItem

            }
            onRightPressed: {
                var item = keyboard.currentItem
            }
            onSpinCwPressed: {
                if (keyboard.currentIndex < keyboard.count - 1) {
                    ++keyboard.currentIndex
                    while(keyboard.currentItem && !keyboard.currentItem.visible) {
                        if (keyboard.currentIndex < keyboard.count - 1)
                            ++keyboard.currentIndex
                        else
                            keyboard.currentIndex = 0
                    }
                } else {
                    keyboard.currentIndex = 0
                }
                var item = keyboard.currentItem
                keyboard.highlightColor = GlobalConstants.kListFocusColor
            }
            onSpinCcwPressed: {
                if (0 < keyboard.currentIndex) {
                    --keyboard.currentIndex
                    while(keyboard.currentItem && !keyboard.currentItem.visible) {
                      if (0 < keyboard.currentIndex)
                        --keyboard.currentIndex
                      else
                         keyboard.currentIndex = 0
                    }
                } else {
                    keyboard.currentIndex = 0
                }
                var item = keyboard.currentItem
                keyboard.highlightColor = GlobalConstants.kListFocusColor
            }
            onSelected: {                
                var item = keyboard.currentItem                
                item.selectItemTwice()
            }
        }

}
