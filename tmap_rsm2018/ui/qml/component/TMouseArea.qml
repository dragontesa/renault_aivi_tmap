import QtQuick 2.0

MouseArea {
    id: root
    property bool hasFocus: true

    onPressed: {
        //QmlAppHelper.debug( parent.objectName )

        if ( !hasFocus ) {
            return;
        }

        mouse.accepted = true;
        var tParent = parent
        while( tParent ) {
            if ( tParent.className &&
                (tParent.className === "__TFocusItem__" ||
                tParent.className === "__View__" ||
                tParent.className === "__PopupView__" )
                ) {
                tParent.forceActiveFocus()
                break
            }

            tParent = tParent.parent
        }

        ////////////////////////////////////////////////////////////////////////////////
        // focus debugging
        ////////////////////////////////////////////////////////////////////////////////

//        QmlAppHelper.debug( "objectName:" + objectName )
//        QmlAppHelper.debug( "parent.objectName:" + parent.objectName )
//        QmlAppHelper.debug( "parentView.objectName:" + tParent.objectName + " visible:" + tParent.visible )
    }    
}
