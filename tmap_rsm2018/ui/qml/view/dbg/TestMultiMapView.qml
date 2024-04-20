import QtQuick 2.1
import SceneGraphRendering 1.0
import "../../component"
import "../../view"
import TestMultiMapViewEvent 1.0

PopupView {
    id: viewRoot
    title: "Test"
    width: parent ? parent.width: undefined
    height: parent ? parent.height : undefined
    modal: true

    readonly property int testItemRows: 5
    readonly property int testItemCols: 3
    readonly property int cellHeight: 30

    property var testItems: [
        {name:'Navigation', event:TestMultiMapViewEvent.Navigation, data:{}},
        {name:'DrawRouteAll(API)', event:TestMultiMapViewEvent.DrawRouteAllClickedUsingApi, data:{}},
        {name:'DrawRouteAll(Quick)', event:TestMultiMapViewEvent.DrawRouteAllClickedUsingMapViewQuick, data:{}}
    ]

    onMediated: {
        sendEvent(TestMultiMapViewEvent.Mediated)
    }

    Component.onDestruction: {
        QmlAppHelper.debug( "#2" )
    }

    Column {
        id: testItemColumn
        anchors.top: parent.top
        anchors.topMargin: 10
        spacing: 5
        width: parent.width
        height: childrenRect.height

        GridView {
            id: gridView
            width: parent.width
            height: viewRoot.cellHeight * testItemRows
            model: testItems
            cellWidth: width / testItemCols
            cellHeight: height / testItemRows
            delegate: Item {
                width: gridView.cellWidth
                height: gridView.cellHeight
                TPushButton {
                    width: parent.width - 3
                    height: viewRoot.cellHeight - 3
                    anchors.centerIn: parent
                    label: modelData.name + "(" + index + ")"
                    onButtonClicked: {
                        console.log( "index: " + index )
                        viewRoot.sendEvent( modelData.event, modelData.data )
                    }
                }
            }
        }

        TPushButton {
            id: closeButton
            label: "Close"
            width: 120
            height: 30
            anchors.horizontalCenter: parent.horizontalCenter
            onButtonClicked: {
                close()
            }
        }
    }

    function drawRouteAll1() {
        QmlAppHelper.debug("#1")
        var w = renderer1.width
        var h = renderer1.height
        renderer1.drawRouteAll(0, w, h, 1)
    }

    function drawRouteAll2() {
        QmlAppHelper.debug("#2")
        var w = renderer2.width
        var h = renderer2.height
        renderer2.drawRouteAll(0, w, h, 1)
    }


    Item {
        id: mapViewWrap
        width: parent.width
        anchors.top: testItemColumn.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        Row {
            anchors.fill: parent
            Renderer {
                id: renderer1
                objectName: "renderer1"
                width: parent.width / 2 - 1
                height: parent.height
                moveMode: Renderer.Navigation
                Component.onCompleted: {
                    QmlAppHelper.debug( "Renderer #1" )
                }

                Component.onDestruction: {
                    QmlAppHelper.debug( "Renderer #2" )
                    viewRoot.sendEvent(TestMultiMapViewEvent.MapViewOnDestruction,renderer1)
                }
            }
            Rectangle {
                width: 2
                height: parent.height
                color: 'red'
            }
            Renderer {
                id: renderer2
                objectName: "renderer2"
                width: parent.width / 2 - 1
                height: parent.height
                moveMode: Renderer.Navigation
                Component.onDestruction: {
                    QmlAppHelper.debug( "Renderer #2" )
                    viewRoot.sendEvent(TestMultiMapViewEvent.MapViewOnDestruction,renderer2)
                }
            }
        }
    }
}

