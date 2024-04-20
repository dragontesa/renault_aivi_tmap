import QtQuick 2.1

FocusScope {
    id: root
    property string className: "__TFocusItem__"
    property string focusName: ""
    property alias mouseArea: mouseArea
    property bool hasFocus: true

    onActiveFocusChanged: {
        //QmlAppHelper.debug( focusName + ".activeFocus: " + activeFocus )
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: hasFocus
        onPressed: {

            ////////////////////////////////////////////////////////////////////////////////
            // focus debugging
            ////////////////////////////////////////////////////////////////////////////////

//            var tParent = parent
//            while( tParent ) {
//                if ( tParent.className &&
//                    (tParent.className === "__TFocusItem__" ||
//                    tParent.className === "__View__" ||
//                    tParent.className === "__PopupView__" )
//                    ) {
//                    break
//                }

//                tParent = tParent.parent
//            }

//            QmlAppHelper.debug( "objectName:" + objectName )
//            QmlAppHelper.debug( "parent.objectName:" + parent.objectName )
//            QmlAppHelper.debug( "parentView.objectName:" + tParent.objectName + " visible:" + tParent.visible )

            if ( root.hasFocus ) {
                root.forceActiveFocus()
            }
        }
    }
}
