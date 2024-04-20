import QtQuick 2.1
import CommonConstants 1.0

import "../../component"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

/*
다양한 Setting 형태를 재사용 가능하게 할 수 있는 베이스를 제공한다.
list의 delegate는 ListElement의 component 필드에 의해 동적으로 변한다.
delegate는 캐쉬되어 표시되므로 현재 row의 component 필드에 의해 지정된 동적으로 변경되도록 구현하였다.
*/

Item {
//Rectangle {
    id: root
    //color: 'red'
    property var rowComponent: ({})
    property int rowHeight: 32
    property alias list: list
    property alias imgMask : imgMask
    property var scrollbar : null

    Component.onCompleted: {
    }

    Component {
        id: listDelegate
        Item {
            id: rowItem
            width: parent.width
            height: children.length ? children[0].height : rowHeight
            property int modelIndex: -1
            property string modelComponent
            Component.onCompleted: {
                rowItem.modelIndex = Qt.binding( function() { return model.index });
            }

            onModelIndexChanged: {
                if (CommonUtils.isUndefined(typeof component))
                    return
                if (!CommonUtils.isEqual(component , rowItem.modelComponent) &&
                      CommonUtils.getObjectLen(root.rowComponent) > 0          ) {
                    rowItem.modelComponent = component
                    var comp = root.rowComponent[component]
                    var obj = comp.createObject( rowItem )
                    obj.itemModel = Qt.binding( function() { return list.model.get( modelIndex ) } )
                }
            }
        }
    }

    ListView {
        id: list
        anchors.fill: parent
        delegate: listDelegate
		clip: true
        //cacheBuffer: rowHeight * 3
        //currentIndex: 1
        //snapMode: ListView.SnapOneItem
        //highlightFollowsCurrentItem : true
        //highlightMoveDuration: 1
        //highlightResizeDuration: 1
        //flickDeceleration: 0
        //pressDelay: 0

        // masking
        //onMovementStarted:  imgMask.visible = true
        //onMovementEnded:  imgMask.visible = false
        Image {
              id: imgMask
              width: parent.width
              height: 30
              y: parent.y + (parent.height - height)
              visible:  {
                  if (!scrollbar)
                      return false
                  return scrollbar.heightRatio < 1.0 && !list.dragging
              }
              sourceSize.width: width
              sourceSize.height: height
              source: Resource.imageUrl("zone/zone2_img_mask.png")
        }        
    }
}


