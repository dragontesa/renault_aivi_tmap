import QtQuick 2.1
import CrossImageViewEvent 1.0
import "../../component"
import "../../view"

// 480x540

View {
    id: root
    property int imageWidth: 360
    property int imageHeight: 360
    property int progressY: 20
    property int progressWidth: 62
    property int progressHeight: 320
    property int progressRightMargin: 23
    property int progressTextMargin: 20
    property int barWidth: 16
    property int carvatarMargin: 20
    width: imageWidth
    height: imageHeight
    property int showDistance: 0
    property int distance: 0
    visible: false
    property bool visibleRequest

    onMediated: {
        sendEvent( CrossImageViewEvent.Mediated, {mapView:mapView} )
    }

    onVisibleChanged: {
        sendEvent( CrossImageViewEvent.VisibleChanged, {visible: visible} )
    }

    function reset() {
        visible = false
        visibleRequest = false
        image.source = ""
        showDistance = 0
        distance = 0
    }

    Image {
        id: image
        objectName: "image"
        anchors.right: parent.right
        width: imageWidth
        height: imageHeight
        asynchronous: true
        cache: false

        function statusToString() {
            var ret = ''
            if (status === Image.Null) ret = 'Image.Null'
            if (status === Image.Loading) ret = 'Image.Loading'
            if (status === Image.Error) ret = 'Image.Error'
            if (status === Image.Ready) ret = 'Image.Ready'
            return ret
        }

        onSourceChanged: {
        }

        onStatusChanged: {
            //console.debug( 'source: ' + source + ' status: ' + statusToString())

            if (status != Image.Ready) {
                progressWrap.visible = false
                root.visible = false

                if (status != Image.Loading) {
                    source = ''
                }
            } else {
                progressWrap.visible = true
                if ( root.visibleRequest ) {
                    root.visible = true
                }
            }
        }
    }

    Item {
        id: progressWrap        
        y: progressY
        anchors.right: parent.right
        anchors.rightMargin: progressRightMargin
        anchors.verticalCenter: parent.verticalCenter
        width: progressWidth
        height: progressHeight
        Image {
            id: progressBgImage
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: Resource.imageUrl( 'junction', 'junction_progress_bg.png' )
        }
        Image {
            id: progress0
            y: progressBgImage.y
            anchors.right: progressBgImage.left
            source: Resource.imageUrl( 'junction', 'junction_progress_0.png' )
        }
        Image {
            id: progress100
            y: progressBgImage.y + progressBgImage.height / 3 - progressTextMargin
            anchors.right: progressBgImage.left
            source: Resource.imageUrl( 'junction', 'junction_progress_100.png' )
        }
        Image {
            id: progress200
            y: progressBgImage.y + progressBgImage.height / 3 * 2 - progressTextMargin
            anchors.right: progressBgImage.left
            source: Resource.imageUrl( 'junction', 'junction_progress_200.png' )
        }
        Image {
            id: progress300
            y: progressBgImage.y + progressBgImage.height / 3 * 3 - progressTextMargin
            anchors.right: progressBgImage.left
            source: Resource.imageUrl( 'junction', 'junction_progress_300.png' )
        }
        Item {
            id: barWrap
            x: progressBgImage.x
            y: progressBgImage.y
            width: barWidth
            height: showDistance ? (distance>300?300:distance) / 300 * progressBgImage.height : 0
            Image {
                id: barTop
                anchors.top: parent.top
                source: Resource.imageUrl( 'junction', 'junction_progress_top.png' )
            }
            Image {
                id: barMiddle
                anchors.top: barTop.bottom
                anchors.bottom: barBottom.top
                source: Resource.imageUrl( 'junction', 'junction_progress_middle.png' )
                fillMode: Image.TileVertically
            }
            Image {
                id: barBottom
                anchors.bottom: parent.bottom
                source: Resource.imageUrl( 'junction', 'junction_progress_bottom.png' )
            }
            Image {
                id: carvatar
                y: barBottom.y - carvatarMargin
                anchors.horizontalCenter: parent.horizontalCenter
                source: Resource.imageUrl( 'junction', 'junction_progress_cavata.png' )
            }
        }
    }
}
