import QtQuick 2.0
import "../../component"
import Csw 1.0

Item {
    id: root

    property int totalDistance: 1
    property int remainDistance: 0
    property int speedX: 1
    property color buttonColor: "darkgray"
    property bool supportSlowSpeed: false // for engineering test.

    signal prevGpClicked()
    signal nextGpClicked()
    signal speedChanged(int speed)
    signal progressChanged(real progress)
    signal pauseChanged(bool paused)
    signal resetTimer()

    function getCommonStateImage(name, state) {
        switch (state) {
        case "normal": return name + "_n.png"
        case "pressed": return name + "_p.png"
        case "focused": return name + "_f.png"
        }
    }
    function getFocusedStateImage(name, state) {
        switch (state) {
        case "normal":
        case "pressed":
            return name + ".png"
        case "focused":
            return name + "_f.png"
        }
    }
    function setNextSpeed() {
        switch (speedX) {
        case 1: speedX = 2; break
        case 2: speedX = 4; break
        case 4: speedX = supportSlowSpeed ?  8 : 1; break
        case 8: speedX = 1; break
        }
        if (speedX != 8)
            speedChanged(speedX * 70)
        else
            speedChanged(35)
    }

    width: 69
    height: 336

    onRemainDistanceChanged: {
        simulationProgress.progress = 1.0 - (remainDistance / totalDistance)
    }

    Component {
        id: compIconButton

        Item {
            id: item

            property var getBg
            property var getIcon
            property bool cswEnable: false

            signal clicked

            width: childrenRect.width
            height: childrenRect.height

            state: {
                if (mouseArea.pressed) {
                    return 'pressed'
                } else {
                    return cswEnable ? 'focused' : 'normal'
                }
            }

            Image {
                id: bg;
                source: Resource.imageUrl(item.getBg(item.state))
            }
            Image {
                id: icon
                source: Resource.imageUrl(item.getIcon(item.state))
            }
            MouseArea {
                id: mouseArea

                anchors.fill: bg
                onPressed: {
                   root.resetTimer()
                }
                onClicked: {
                    item.clicked()
                }
            }
        }
    }

    Column {
        id: column
        anchors.fill: parent

        Loader {
            id: speedButtonLoader
            sourceComponent: compIconButton

            onLoaded: {
                item.getBg = function(state) {
                    return getCommonStateImage("route/simulate_btn_top", state)
                }
                item.getIcon = function(state) {
                    if (speedX < 8)
                        return getFocusedStateImage("route/simulate_" + root.speedX + "x", state)
                    else
                        return 'route/simulate_0.5x.png'
                }
            }
            Connections {
                target: speedButtonLoader.item
                onClicked: {
                    setNextSpeed()
                    csw.forceLostFocus()
                }
            }
            Csw {
                id: csw
                section: Csw.Section_Map; sequence: Csw.Sequence_SimulationSpeed
                enable: root.visible
                onFocused: {
                    speedButtonLoader.item.cswEnable = true
                    root.resetTimer()
                }
                onLostFocusd: {
                    speedButtonLoader.item.cswEnable = false
                }
                onSelected: {
                    //speedButtonLoader.item.clicked()
                    setNextSpeed()
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 336
            color: "#222226"
            opacity: 0.85

            Column {
                width: parent.width

                Image {
                    id: goal
                    source: Resource.imageUrl("route/simulate_ic_goal.png")
                }
                SimulationProgress {
                    id: simulationProgress
                    onProgressUpdated: root.progressChanged(progress)
                }
            }
        }

        Loader {
            id: playButtonLoader
            sourceComponent: compIconButton
            property bool paused: false

            function execButton() {
                var paused = playButtonLoader.paused ? false : true
                playButtonLoader.paused = paused
                root.pauseChanged(paused)
            }

            onLoaded: {
                item.getBg = function(state) {
                    return getCommonStateImage("route/simulate_btn_mid", state)
                }
                item.getIcon = function(state) {
                    return getFocusedStateImage("route/simulate_" + (!paused ? "pause" : "play"), state)
                }
            }
            Connections {
                target: playButtonLoader.item
                onClicked: {
                    playButtonLoader.execButton()
                    csw.forceLostFocus()
                }
            }
            Csw {
                section: Csw.Section_Map; sequence: Csw.Sequence_SimulationPause
                enable: root.visible
                onFocused: {
                    playButtonLoader.item.cswEnable = true
                    root.resetTimer()
                }
                onLostFocusd: {
                    playButtonLoader.item.cswEnable = false
                }
                onSelected: playButtonLoader.execButton()
            }
        }

        Loader {
            id: nextButtonLoader
            sourceComponent: compIconButton

            onLoaded: {
                item.getBg = function(state) {
                    return getCommonStateImage("route/simulate_btn_mid", state)
                }
                item.getIcon = function(state) {
                    if (lang == 'ko') {
                        return getFocusedStateImage("route/map_btn_next", state)
                    } else {
                        return state == 'focused' ? "route/map_btn_next_f_eng.png"
                                                  : "route/map_btn_next_eng.png"
                    }
                }
            }
            Connections {
                target: nextButtonLoader.item
                onClicked: {
                    root.nextGpClicked()
                    csw.forceLostFocus()
                }
            }
            Csw {
                section: Csw.Section_Map; sequence: Csw.Sequence_SimulationNext
                enable: root.visible
                onFocused: {
                    nextButtonLoader.item.cswEnable = true
                    root.resetTimer()
                }
                onLostFocusd: {
                    nextButtonLoader.item.cswEnable = false
                }
                onSelected: root.nextGpClicked()
            }
        }

        Loader {
            id: prevButtonLoader
            sourceComponent: compIconButton
            onLoaded: {
                item.getBg = function(state) {
                    return getCommonStateImage("route/simulate_btn_bot", state)
                }
                item.getIcon = function(state) {
                    if (lang == 'ko') {
                        return getFocusedStateImage("route/map_btn_previous", state)
                    } else {
                        return state == 'focused' ? "route/map_btn_previous_f_eng.png"
                                                  : "route/map_btn_previous_eng.png"
                    }
                }
            }
            Connections {
                target: prevButtonLoader.item
                onClicked: root.prevGpClicked()
            }
            Csw {
                section: Csw.Section_Map; sequence: Csw.Sequence_SimulationPrev
                enable: root.visible
                onFocused: {
                    prevButtonLoader.item.cswEnable = true
                    root.resetTimer()
                }
                onLostFocusd: {
                    prevButtonLoader.item.cswEnable = false
                }
                onSelected: root.prevGpClicked()
            }
        }
    }
}
