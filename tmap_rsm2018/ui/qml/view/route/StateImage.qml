import QtQuick 2.0

Item {
    id: root

    width: image.width
    height: image.height

    property var sources: []    // normal
                                // pressed
                                // list-pressed
                                // csw-focus

    Component.onCompleted: state = "normal"

    state: ""
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: image
                source: Resource.imageUrl(sources[0])
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: image
                source: Resource.imageUrl(sources[1])
            }
        },
        State {
            name: "list-pressed"
            PropertyChanges {
                target: image
                source: Resource.imageUrl(sources[2])
            }
        },
        State {
            name: "csw-focus"
            PropertyChanges {
                target: image
                source: Resource.imageUrl(sources[3])
            }
        }
    ]
    Image {
        id: image
    }
}
