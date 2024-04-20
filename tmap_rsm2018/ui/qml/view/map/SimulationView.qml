import QtQuick 2.0
import "../../component"
import "../../view"
import SimulationViewEvent 1.0
import SceneGraphRendering 1.0
import LayoutConstants 1.0
import Csw 1.0
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: root

    property var caller: null
    property int viewMode
    property string lang

    onViewModeChanged: {
        mapView.viewMode = viewMode
    }

    mouseArea.enabled: false

    width: parent.width
    height: parent.height

    background.visible: false
    fader.fadingOpacity: 0

    layout: SimulationViewLayout {
        onUpdateLayoutCompleted: {
            if (screenResolution === LayoutConstants.L800x480) {
                simulationControl.anchors.topMargin = -64
                simulationControl.anchors.rightMargin = 8
                simulationControl.scale = 0.5
            } else if (screenResolution === LayoutConstants.L820x1024) {
                simulationControl.anchors.topMargin = 10
                simulationControl.anchors.rightMargin = 10
                simulationControl.scale = 1.0
            }
        }
    }

    onMediated: {
        sendEvent(SimulationViewEvent.Mediated)
    }
    onActivated: {
        sendEvent(SimulationViewEvent.SetupTitle, qsTr('모의 주행'))
        sendEvent(SimulationViewEvent.Activated)
    }
    onDeactivated: {
        sendEvent(SimulationViewEvent.Deactivated)
    }

    function retranslateUi() {
        sendEvent(SimulationViewEvent.SetupTitle, qsTr('모의 주행'))
    }

    function updateProgress(value) {
        var total = value["total"]
        var remain = value["remain"]
        simulationControl.totalDistance = total
        simulationControl.remainDistance = remain
    }

    function setSimulationControlVisible( visible ) {
        simulationControl.visible = visible
    }

    // TOFIX
    function closeViewAndStopSimulator() {
        mapView.cancelDrawRouteAll()
        close()
    }

    function enableMouseArea(enable) {
        mouseArea.enabled = enable
    }

    function setMapViewLevel(level) {
        mapView.viewLevel = level
    }

    Item {
        id: view

        width: parent.width
        height: parent.height

        MouseArea {
            id: mouseArea

            // TMCRSM-178.
            // prevent moving map.
            anchors.fill: parent

            enabled: false
        }

        SimulationControl {
            id: simulationControl

            visible: false
            anchors.top: parent.top
            anchors.right: parent.right
        }

        TImageButton {
            id: volumeButton
            width: 69; height: 69

            bg.anchors.fill: volumeButton
            icon.anchors.fill: volumeButton

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            bgSources: function (state) {
                switch (state) {
                case "normal": return "map_btn_bg_n.png"
                case "pressed":return "map_btn_bg_p.png"
                case "focused":return "map_btn_bg_f.png"
                        }
                    }
            iconSources: function (state) {
                var basename = (!root.mute) ? "map_btn_volume" : "map_btn_mute"
                switch (state) {
                    case "normal":
                    case "pressed":
                        return basename + ".png"
                    case "focused":
                        return basename + "_f.png"
                }
            }
            onClicked: root.sendEvent(SimulationViewEvent.VolumeClicked)

            Csw {
                id: volumeCsw
                objectName: "volume"
                section: Csw.Section_Map; sequence: Csw.Sequence_Volume; enable: volumeButton.visible;
                onFocused: {
                    volumeButton.state = "focused"
                    sendEvent(SimulationViewEvent.ResetTimer)
                }
                onLostFocusd: volumeButton.state = "normal"
                onSelected: root.sendEvent(SimulationViewEvent.VolumeClicked)
            }
        }
    }

    Connections {
        target: simulationControl
        onSpeedChanged: {
            simulator.speed = speed
            sendEvent(SimulationViewEvent.ResetTimer)
        }
        onNextGpClicked: {
            simulator.goNextGP()
            sendEvent(SimulationViewEvent.ResetTimer)
        }
        onPrevGpClicked: {
            simulator.goPreviousGP()
            sendEvent(SimulationViewEvent.ResetTimer)
        }
        onProgressChanged: {
            var dist = simulationControl.totalDistance * progress
            simulator.goTo(dist)
            sendEvent(SimulationViewEvent.ResetTimer)
        }
        onPauseChanged: {
            simulator.pause = paused
            sendEvent(SimulationViewEvent.ResetTimer)
        }
        onResetTimer: {
            sendEvent(SimulationViewEvent.ResetTimer)
        }
    }
    Connections {
        target: simulator
        onStarted: {
            simulator.pause = false //TMCRSM-210 모의 주행 시작 시 pause 누르면 일시정지 안됨.
        }
    }
    Connections {
        target: mapView
        onViewModeChanged: {
            console.log('viewModeChanged = ' + viewMode)
        }
    }
}
