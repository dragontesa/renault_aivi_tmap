import QtQuick 2.1
import "../../component"

// rowIndex row 위
// itemModel 모델
// itemModelIndex 모델 인덱스

Item {
    id: root
    property alias list: list
    property var model
    property alias itemDelegate: list.itemDelegate
    property int visibleRows: 5
    property int columns: 1
    property alias spacing: list.spacing

    // qml ListModel
    property QtObject listModelAccessor: QtObject {
        function count() {
            return root.model.count;
        }
        function get(index) {
            return root.model.get(index);
        }
    }

    // cpp QVariantList
    property QtObject listAccessor: QtObject {
        function count() {
            return root.model.length;
        }
        function get(index) {
            return root.model[index];
        }
    }

    property QtObject modelAccessor: listModelAccessor

    onModelChanged: {
    }

    ListView {
        id: list
        anchors.fill: parent
        model: root.model ? Math.ceil(root.modelAccessor.count() / columns) : 0

        clip: true
        currentIndex: 1
        //cacheBuffer: height
        snapMode: ListView.SnapOneItem
        highlightFollowsCurrentItem : true
        highlightMoveDuration: 1
        highlightResizeDuration: 1
        //highlightResizeSpeed: 1
        flickDeceleration: 0
        pressDelay: 0
        cacheBuffer: list.height / root.visibleRows * 3

        onModelChanged: {
            //console.log( "model:" + model + " count:" + root.modelAccessor.count() + " columns:" + columns)
        }

        property Component itemDelegate

        Component.onCompleted: {
        }

        delegate: Item {
            id: rowItemContainer
            property Item rowItem
            width: list.width
            height: list.height / root.visibleRows
            Component.onCompleted: {
                rowItem = rowComponent.createObject(rowItemContainer);
                // 몇번째 줄인지
                rowItem.modelIndex = Qt.binding( function() { return model.index })
            }
        }

        Component {
            id: rowComponent
            Row {
                id: rowItem
                width: list.width
                height: list.height / root.visibleRows
                spacing: list.spacing
                property var model
                property int modelIndex
                Component.onCompleted: {                    
                }
                Repeater {
                    id: itemRepeater
                    model: root.columns
                    Loader {
                        id: itemDelegateLoader
                        width: list.width / root.columns
                        height: list.height / root.visibleRows
                        sourceComponent: list.itemDelegate

                        property int rowIndex: {
                            return rowItem.modelIndex
                        }
                        property int columnIndex: {
                            return index
                        }
                        property var itemModel: {
                            return root.modelAccessor.get( rowIndex * root.columns + index )
                        }
                        property int itemModelIndex: {
                            return rowIndex * root.columns + index
                        }
                        Component.onCompleted: {
//                            console.log( "#1 rowIndex: " + rowIndex )
//                            console.log( "#1 itemModelIndex: " + itemModelIndex )
                        }
                    }
                }
            }
        }
    }
}
