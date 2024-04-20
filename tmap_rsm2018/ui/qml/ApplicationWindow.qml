import QtQuick 2.1
import QtQuick.Window 2.1
import ApplicationEvent 1.0
import ApplicationConstants 1.0
import CommonConstants 1.0
import TestConstants 1.0
import "GlobalConstants.js" as GlobalConstants
import QtTest 1.0
import SceneGraphRendering 1.0
import "./view/map"
import RouteRegistViewEvent 1.0
import QtGraphicalEffects 1.0
import WidgetConstants 1.0

Window {
    id: root
    title: qsTr("T map")
    width: GlobalConstants.kScreenWidth
    height: GlobalConstants.kScreenHeight
    visible: true    
    property int appState: ApplicationConstants.StateNone
    property bool interaction: true
    property var contentWrap
    property bool enableScreenCapture: false
    property bool enableBlur: false
    property bool isNaviVisible: true
    property bool applyBlur: enableBlur && isNaviVisible
    property var testPoint
    property var testTouchPoint: new Array;
    property bool verticalFlip: false

    signal onEvent( int event, var data )
    signal mediated

    function __sendEvent( event, data ) {
        onEvent( event, data )
    }

    function viewMediated() {
        mediated()
    }

    function showTestClickPoint( x, y, action ) {
        if ( !testPoint ) {
            testPoint = Qt.createQmlObject(
                qsTr('import QtQuick 2.1
                Rectangle {
                    x: %1
                    y: %2
                    width: %3
                    height: %4
                    color: \'%5\'
                    opacity: 0.75
                    radius: width/2
                }').arg(x).arg(y).arg(30).arg(30).arg('red'),
                effectWrap,
                ''
            );
            //point.destroy( 1000 )
        }     
    }

    function showTestTouchPoint( index, x, y, action ) {
        if ( !(testTouchPoint[index] != null) ) {
            testTouchPoint[index] = Qt.createQmlObject(
                qsTr('import QtQuick 2.1
                Rectangle {
                    x: %1
                    y: %2
                    width: %3
                    height: %4
                    color: \'%5\'
                    opacity: 0.75
                    radius: width/2
                }').arg(x).arg(y).arg(30).arg(30).arg('red'),
                effectWrap,
                ''
            );
        }

        testTouchPoint[index].x = x
        testTouchPoint[index].y = y
        if (index == 0) {
            testTouchPoint[index].color = 'red'
        }
        else {
            testTouchPoint[index].color = 'blue'
        }

        if (action == TestConstants.TouchActionPress) {
        }
        else if (action == TestConstants.TouchActionMove) {
        }
        else if (action == TestConstants.TouchActionRelease) {
        }
    }

    Component.onCompleted: {
        root.color = QmlAppHelper.isTarget() ? 'transparent' : 'black'
        QmlAppHelper.startup( root )
        __sendEvent( ApplicationEvent.ComponentCompleted )
        //interaction = false
    }

    onAppStateChanged: {
        console.log("app state: "+appState)
    }

    property var contentRoot
    property var contentRootWithoutSoftkey
    property var zoneWrap
    property var toastWrap
    property var alertWrap
    property var progressWrap

    function changeDisplayMode(isGadget, gadgetType, gadgetSize) {
        console.log('[Gadget]changeDisplayMode isGadget:' + isGadget)

        hardKeyWrap.visible = !isGadget

        if (isGadget) {
            var newY = GlobalConstants.kScreenHeight - 390
            rootWrap.transform[0].y = newY;

            contentRoot.x = 9999
            contentRootWithoutSoftkey.x = 9999
            zoneWrap.x = 9999
            toastWrap.x = 9999
            alertWrap.x = 9999
            progressWrap.x = 9999
        }
        else {
            rootWrap.transform[0].y = 0;

            contentRoot.x = 0
            contentRootWithoutSoftkey.x = 0
            zoneWrap.x = 0
            toastWrap.x = 0
            alertWrap.x = 0
            progressWrap.x = 0
        }
    }

    Rectangle {
        id: blankWrap
        objectName: 'blankWrap'
        width: parent.width
        height: parent.height
        color: 'black'
        z: 999
        visible: false
    }

    Item {
        id: rootWrap
        width: root.width
        height: root.height

        transform: Translate { y: 0 }

        Loader {
            id: contentLoader
            anchors.fill: parent
            sourceComponent: withMapDataComp
            onLoaded: {
                item.visible = true                
            }
            layer.enabled: applyBlur
        }

        Item {
            anchors.fill: parent
            visible: applyBlur
            layer.enabled: applyBlur
            layer.effect: GaussianBlur {
                source: contentLoader
                radius: 10
                samples: 16
            }
        }

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

                Item {
                    id: contentWrap
                    objectName: CommonConstants.ContentWrap
                    y: GlobalConstants.kScreenZone1Height
                    width: parent.width
                    height: GlobalConstants.kScreenContentHeight

                    function changeToApp() {
                        y = GlobalConstants.kScreenZone1Height
                    }

                    function changeToWidget(size) {
                        y = 0
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
                        width: parent.width
                        height: parent.height
                        z: 2
                        Component.onCompleted: {
                            root.alertWrap = alertWrap
                        }
                    }

                    Item {
                        id: contentRoot
                        objectName: CommonConstants.ContentRoot
//                        anchors.fill: parent
//                        anchors.bottom: parent.bottom
//                        anchors.bottomMargin: GlobalConstants.kScreenZone3Height
                        width: parent.width
                        height: parent.height - GlobalConstants.kScreenZone3Height
                        z: 3
                        Component.onCompleted: {
                            root.contentRoot = contentRoot
                        }
                    }

                    Item {
                        id: contentRootWithoutSoftkey
                        objectName: CommonConstants.ContentRootWithoutSoftkey
                        width: parent.width
                        height: parent.height
                        z: 4
                        Component.onCompleted: {
                            root.contentRootWithoutSoftkey = contentRootWithoutSoftkey
                        }
                    }

                    Item {
                        id: contentRootForCommonDialog
                        objectName: CommonConstants.ContentRootForCommonDialog
                        width: parent.width
                        height: parent.height
                        z: 5
                    }
                }

                Item {
                    id: zoneWrap
                    objectName: CommonConstants.ZoneWrap
                    width: parent.width
                    height: parent.height
                    z: 2
                    Component.onCompleted: {
                        root.zoneWrap = zoneWrap
                    }
                }

                Item {
                    id: progressWrap
                    objectName: CommonConstants.ProgressWrap
                    width: parent.width
                    height: parent.height
                    z: 3
                    Component.onCompleted: {
                        root.progressWrap = progressWrap
                    }
                }

                Item {
                    id: widgetWrap
                    objectName: CommonConstants.WidgetWrap
                    anchors.fill: parent
                    z: 5
                    //color: 'red'
                }

                Item {
                    id: toastWrap
                    objectName: CommonConstants.ToastWrap
//                    anchors.top: parent.top
//                    anchors.topMargin: GlobalConstants.kScreenZone1Height
//                    anchors.bottom: parent.bottom
//                    width: parent.width
                    y: parent.y + GlobalConstants.kScreenZone1Height
                    width: parent.width
                    height: parent.height - y - GlobalConstants.kScreenZone1Height

                    z: 60

                    Component.onCompleted: {
                        root.toastWrap = toastWrap
                    }
                }

                Item {
                    id: testWrap
                    objectName: CommonConstants.TestWrap
                    anchors.fill: parent
                    z: 70

//                    Item {
//                        id: gestureWrap
//                        anchors.fill: parent
//                        MouseArea {
//                            id: gestureArea
//                            enabled: enableScreenCapture
//                            anchors.fill: parent
//                            propagateComposedEvents: true
//                            property real fromX: -1
//                            property bool isGestureMode: false
//                            onPressed: {
//                                if (mouseY < GlobalConstants.kScreenZone3Y+GlobalConstants.kScreenZone3Height/2)
//                                mouse.accepted = false
//                                else {
//                                    isGestureMode = true
//                                    fromX = mouseX
//                                }
//                            }
//                            onReleased: {
//                                isGestureMode = false
//                            }

//                            onMouseYChanged: {
//                                if ( isGestureMode && mouseY < height/2) {
//                                    var slope = Math.abs((mouseX-fromX) / mouseY);
//                                    // debug message
//                                    if ( slope < 0.5) {
//                                        console.log("gesture detected!!!!!!!!!!!! slope :"+slope);
//                                        __sendEvent( ApplicationEvent.CaptureScreenshot)
//                                        isGestureMode = false
//                                    }
//                                }
//                            }
//                        }
//                    }

                        // Map Auto Level Config GUI only for engineer
                        MapAutoLevelConfigLauncher {
                            id: mapAutoLevelGui
                        }
                        // Map Config Debug GUI only for engineer
                        MapConfigDebugUiLauncher {
                            id: mapDebugUi
                            topParent: testWrap
                        }
                }

                Item {
                    id: faderWrap
                    width: root.width
                    height: root.height
                    visible: false
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                        }
                    }
                    z: 80
                }
            }
        }

        Item {
            id: effectWrap
            width: root.width; height: root.height
            visible: true
            z: 99
        }
        Item {
            id: popupWrap
            objectName: 'popupWrap'
            width: root.width; height: root.height
            z: 100
        }
        Item {
            id: hardKeyWrap
            objectName: 'hardKeyWrap'
            visible: true
            width: root.width; height: root.height
            z: 101
        }
    }
}
