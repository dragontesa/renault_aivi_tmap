import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property alias currentIndex: listView.currentIndex

    property int itemWidth
    property string bgSource
    property string slideSource
    property var menus
    property font font: Qt.font({ family: GlobalConstants.kFontFamily, pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize) })
    property int lrMargin
    property int tbMargin

    property int itemIndex: -1
    property int unit
    property int pressedIndex
    property int pressedX
    property int slideX
    property bool slideMode: false
    readonly property color selectedColor: '#222226'
    readonly property color normalColor: '#ffffff'

    signal clicked(int button)

    onBgSourceChanged: bg.source = bgSource
    onSlideSourceChanged: {
        slide.source = slideSource
        slide2.source = slideSource
    }
    onItemIndexChanged: {
        unselectAll()
        select(itemIndex)
        updateListView()

        startSlideAnimation(itemIndex, 0)
        listView.currentIndex = itemIndex
    }
    onMenusChanged: {
        for (var i = 0; i < menus.length; i++) {
            listModel.set(i, {'menu' : menus[i], 'selected' : false })
        }
    }

    function getItemIndex(x, width, count) {
        var unit = width / count
        var index = Math.floor(x/unit).toFixed(0)

        return index
    }

    function getUnit(width, count) {
        var unit = width / count

        return Math.floor(unit).toFixed(0)
    }

    function getSlideX(x, width, count) {
        var unit = width / count
        var index = getItemIndex(x, width, count)
        var cx = index * unit + unit / 2
        var dx = x - cx

        return Math.floor(index * unit + dx).toFixed(0)
    }

    function setSlideX(x, width, count) {
        var rX = width + lrMargin * 2
        var unit = width / count
        if (x + unit > rX) {
            x = width - unit
        } else if (x < lrMargin) {
            x = lrMargin
        }

        slide.x = x
    }

    function unselectAll() {
        for (var i=0; i<menus.length; i++) {
            listModel.get(i).selected = false
        }
    }
    function select(index) {
        listModel.get(index).selected = true
    }
    function unselect(index) {
        listModel.get(index).selected = false
    }

    NumberAnimation {
        id: slideAnimation
        target: slide
        property: "x"
        duration: 0
        easing.type: Easing.InOutQuad
    }

    function startSlideAnimation(index, duration) {
        var srcX = slide.x
        var destX = lrMargin + itemWidth * index
        slideAnimation.from = srcX
        slideAnimation.to = destX
        slideAnimation.duration = duration
        slideAnimation.start()
    }

    function updateListView() {
        var index = listView.currentIndex
        listView.model = []
        listView.forceLayout()
        listView.model = listModel
        listView.currentIndex = index
    }

    function cancelSelectMenu() {
        unselect(pressedIndex)

        pressedIndex = -1
        updateListView()
        mouseArea.hoverEnabled = false
        slide2.visible = false
    }

    opacity: enabled ? 1.0 : 0.3

    Image { id: bg }
    Image { id: slide; anchors.verticalCenter: parent.verticalCenter }
    Image { id: slide2; anchors.verticalCenter: parent.verticalCenter; visible: false }

    Component {
        id: menuTextComp
        Text {
            width: itemWidth; height: root.height;
            text: menu
            color: selected ? selectedColor : normalColor
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font: root.font
        }
    }

    ListView {
        id: listView
        anchors { fill: parent; leftMargin: lrMargin; rightMargin: lrMargin; topMargin: tbMargin; bottomMargin: tbMargin }
        orientation: ListView.Horizontal
        interactive: false
        model: listModel
        delegate: menuTextComp
    }
    ListModel {
        id: listModel
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: false
        preventStealing: true

        anchors.fill: listView
        onPressed: {
            pressedX = mouse.x
            pressedIndex = getItemIndex(pressedX, listView.width, menus.length)

            if (pressedIndex == listView.currentIndex) {
                slideMode = true
                slideX = slide.x
                unselectAll()
                updateListView()
            } else {
                var index = listView.currentIndex
                var unit = getUnit(listView.width, menus.length)
                slide2.x = lrMargin + unit * pressedIndex
                slide2.visible = true
                select(pressedIndex)
                updateListView()
            }
            mouseArea.hoverEnabled = true
        }
        onReleased: {
            mouseArea.hoverEnabled = false

            if (slideMode) {
                var x = slide.x
                var unit = listView.width / menus.length
                x += unit / 2
                var index = getItemIndex(x, listView.width, menus.length)
                var indexChanged = index != listView.currentIndex

                select(index)
                updateListView()
                startSlideAnimation(index, 200)

                if (indexChanged) {
                    listView.currentIndex = index

                    root.clicked(index)
                }
            } else {
                if (0 <= pressedIndex) {
                    slide2.visible = false
                    unselectAll()
                    select(pressedIndex)
                    updateListView()
                    startSlideAnimation(pressedIndex, 200)

                    listView.currentIndex = pressedIndex
                    root.clicked(pressedIndex)
                }
            }
            slideMode = false
        }
        onMouseXChanged: {
            if (slideMode) {
                var dx = mouse.x - pressedX
                var x = slideX + dx
                setSlideX(x, listView.width, menus.length)
            } else {
                if (pressedIndex != getItemIndex(mouse.x, listView.width, menus.length)) {
                    cancelSelectMenu()
                }
            }
        }
        onExited: {
            if (mouseArea.hoverEnabled) {
                if (!slideMode) {
                    cancelSelectMenu()
                }
            }
        }
    }
}
