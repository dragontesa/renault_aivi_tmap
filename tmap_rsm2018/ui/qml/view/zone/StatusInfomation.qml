import QtQuick 2.0

import "../../component"

TFocusItem {
    focusName: "StatusInfomation"
    width: parent.width
    height: 64
    property alias temperature: temperatureText.text    

    Component {
        id: compTextInput
        TextInput {
            id: inputField
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            color: 'black'
            focus: false
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            clip: true
            onActiveFocusChanged: {
                console.log( 'TextInput.onActiveFocusChanged activeFocus:' + activeFocus )
                if ( activeFocus ) {
                    if(Resource.focusOwnerObj) {
                        Resource.focusOwnerObj.showRecentSearchPage()
                        return
                    }

                    Resource.hideKeyboard()

                    Resource.inputField  = inputField
                    console.log( 'TextInput.onActiveFocusChanged !!!!!!!!!!!!!!!!!!' )
                    Resource.focusOwnerObj = createOrGetView( '../view/search/SearchTabView.qml', {}, Resource.contentRoot, 'SearchTabView')
                }
            }

            onFocusChanged: {
                console.log( 'onFocusChanged' )
            }
        }
    }
}
