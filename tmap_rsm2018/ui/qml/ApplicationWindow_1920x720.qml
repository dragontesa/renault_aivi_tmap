import QtQuick 2.1
import QtQuick.Window 2.1
import ApplicationEvent 1.0
import ApplicationConstants 1.0
import CommonConstants 1.0
import "GlobalConstants.js" as GlobalConstants
import WidgetConstants 1.0

Window {
    id: root
    title: qsTr("T map")
    width: GlobalConstants.kFullScreenWidth
    height: GlobalConstants.kFullScreenHeight
    visible: true
    color: 'black' // 'transparent'
    property int appState: ApplicationConstants.StateNone
    property bool interaction: true
    property var contentWrap

    signal onEvent( int event, var data )

    signal mediated

    function __sendEvent( event, data ) {
        onEvent( event, data )
    }

    function viewMediated() {
        mediated()
    }

    Component.onCompleted: {
        QmlAppHelper.startup( root )
        __sendEvent( ApplicationEvent.ComponentCompleted )
        //interaction = false
    }

    onAppStateChanged: {
        console.log("app state: "+appState)
    }

    Item {
        id: rootWrap
        width: root.width
        height: root.height

        Loader {
            id: contentLoader
            anchors.fill: parent
            sourceComponent: withMapDataComp
//            sourceComponent: {
//                if ( root.appState == ApplicationConstants.StateNoMapData ) {
//                    console.log( "ApplicationConstants.StateNoMapData #1" )
//                    return noMapDataComp
//                }
//                else if ( root.appState == ApplicationConstants.StateWithMapData ) {
//                    //console.log( "ApplicationConstants.StateWithMapData #2" )
//                    return withMapDataComp
//                }
//            }
            onLoaded: {
                item.visible = true                
            }            
        }

//        Component {
//            id: noMapDataComp
//            MapNotfoundGuideView  {
//                id: noMapDataWrap
//                width: parent.width
//                height: GlobalConstants.kScreenContentHeight
//                anchors.fill: parent
//                visible: true
//            }
//        }

        Component {
            id: withMapDataComp
            Item {
                id: withMapDataWrap
                anchors.fill: parent

                property bool interaction: root.interaction

                onInteractionChanged: {
                    if ( interaction ) {
                        faderWrap.visible = false
                    }
                    else {
                        faderWrap.visible = true
                    }
                }
				
				Rectangle {
					id: dummyLeft
					width: 173
                    height: parent.height
                    color: 'blue'

                    Rectangle {
                        x: button2.x
                        y: 24
                        width: 80
                        height: 80
                        color: 'white'
                    }

                    Rectangle {
                        id: button1
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: button2.y - 119
                        width: 80
                        height: 80
                        color: 'white'
                    }

                    Rectangle {
                        id: button2
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 80
                        height: 80
                        color: 'white'
                    }

                    Rectangle {
                        id: button3
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: button2.y + 119
                        width: 80
                        height: 80
                        color: 'white'
                    }

                    Rectangle {
                        id: button4
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 24
                        width: 80
                        height: 80
                        color: 'white'
                    }
				}
				
				Rectangle {
					id: dummyRight
                    x: dummyLeft.width + GlobalConstants.kScreenWidth
					width: 173
                    height: parent.height
                    color: 'blue'

                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 24
                        width: 80
                        height: 80
                        color: 'white'
                    }
				}
				
                Rectangle {
                    id: contentWrapWrap
                    x: dummyLeft.width
                    width: GlobalConstants.kScreenWidth
                    height: parent.height
                    color: 'white'

                    onWidthChanged: {
                        console.log( "contentWrapWrap #1 width: " + width )
                    }

                    Item {
                        id: contentWrap
                        objectName: CommonConstants.ContentWrap
                        width: parent.width
                        y: GlobalConstants.kScreenZone1Height
                        height: GlobalConstants.kScreenHeight

                        onWidthChanged: {
                            console.log( "contentWrap #1 width: " + width )
                        }

                        Item {
                            id: mapViewWrap
                            objectName: CommonConstants.MapViewWrap
                            anchors.fill: parent
                            z: 1
                        }

                        Item {
                            id: alertWrap
                            objectName: CommonConstants.AlertWrap
                            anchors.fill: parent
                            z: 2
                        }

                        Item {
                            id: contentRoot
                            objectName: CommonConstants.ContentRoot
                            anchors.fill: parent
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: GlobalConstants.kScreenZone3Height
                            z: 3
                        }

                        Item {
                            id: contentRootWithoutSoftkey
                            objectName: CommonConstants.ContentRootWithoutSoftkey
                            anchors.fill: parent
                            z: 4
                        }
                    }

                    Item {
                        id: zoneWrap
                        objectName: CommonConstants.ZoneWrap
                        anchors.fill: parent
                        z: 2
                    }

                    Item {
                        id: progressWrap
                        objectName: CommonConstants.ProgressWrap
                        anchors.fill: parent
                        z: 3
                    }

                    Item {
                        id: widgetWrap
                        objectName: CommonConstants.WidgetWrap
                        anchors.fill: parent
                        z: 5
                    }

                    Item {
                        id: toastWrap
                        objectName: CommonConstants.ToastWrap
                        anchors.top: parent.top
                        anchors.topMargin: GlobalConstants.kScreenZone1Height
                        anchors.bottom: parent.bottom
                        width: parent.width
                        z: 6
                    }

                    Item {
                        id: testWrap
                        objectName: CommonConstants.TestWrap
                        anchors.fill: parent
                        z: 7
                    }

                    Item {
                        id: faderWrap
                        width: root.width
                        height: root.height
                        visible: false
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log( "#1" )
                            }
                        }
                        z: 8
                    }
                }
            }
        }
    }
}
