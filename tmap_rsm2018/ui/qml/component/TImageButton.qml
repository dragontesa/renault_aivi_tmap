import QtQuick 2.0

Item {
    id: root

    width: childrenRect.width
    height: childrenRect.height

    property alias bg: image
    property alias icon: iconLoader

    property var bgSources: function(state) {
        return ""
    }

    property var iconSources: undefined

    onIconSourcesChanged: {
        if (iconSources !== undefined)
            iconLoader.sourceComponent = iconComponent
    }

    property string beforeState

    signal clicked()

    Image {
        id: image
        source: Resource.imageUrl(bgSources(root.state))
    }
    Component {
        id: iconComponent
        Image {
            source: Resource.imageUrl(iconSources(root.state))
        }
    }

    Loader {
        id: iconLoader
        anchors.centerIn: image
    }

    MouseArea {
        anchors.fill: image
        onPressed: {
            beforeState = root.state
            root.state = "pressed"
        }
        onReleased: {
            root.state = beforeState
            root.clicked()
        }
        //onDoubleClicked: root.state = "focused"
    }

    state: "normal"
}
