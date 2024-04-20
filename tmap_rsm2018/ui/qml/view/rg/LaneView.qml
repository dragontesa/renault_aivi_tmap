import QtQuick 2.1
import LaneViewEvent 1.0
import "../../component"
import "../../view"

View {
    id: root
    width: lanes * laneWidth + (lanes > 0 ? laneDistanceWidth : 0)
    height: laneHeight
    visible: true

    property int laneWidth: 48
    property int laneHeight: 48
    property int laneDistanceWidth: 67

    property int laneIndex : 0
    property int lanes : 0
    property int distNumberLen : 0
    property var laneImages: []
    property var laneCoverImages: []
    property var distNumberImages: []

    onMediated: {
        root.visible = true
        sendEvent( LaneViewEvent.Mediated )
    }    

    onLaneImagesChanged: {
        lanes = laneImages.length
        var laneRepeaterItemAt = laneRepeater.itemAt
        for ( var i = 0; i < laneImages.length; i++ ) {
            laneRepeaterItemAt( i ).source = laneImages[i]
        }
    }

    onLaneCoverImagesChanged: {
        var laneRepeaterItemAt = laneRepeater.itemAt
        for ( var i = 0; i < laneCoverImages.length; i++ ) {
            laneRepeaterItemAt( i ).coverSource = laneCoverImages[i]
        }
    }

    onDistNumberImagesChanged: {
        distNumberLen = distNumberImages.length
        var distNumberRepeaterItemAt = distNumberRepeater.itemAt
        for ( var i = 0; i < distNumberImages.length; i++ ) {
            distNumberRepeaterItemAt( i ).source = distNumberImages[i]
        }
    }

    function reset() {
        lanes = 0
        distNumberLen = 0
        laneImages = []
        laneCoverImages = []
        distNumberImages = []
    }

    Item {
        id: laneWrap
        objectName: "laneWrap"
        visible: true

        function reset() {
            lanes = 0
            distNumberLen = 0
            laneImages = []
            laneCoverImages = []
            distNumberImages = []
        }

        onVisibleChanged: {
            //console.log( "#1 objectName: " + objectName + " visible: " + visible )
        }

        Row {
            anchors.fill: parent
            Repeater {
                id: laneRepeater
                model: lanes
                delegate: Item {
                    width: laneWidth
                    height: laneHeight
                    property string source
                    property string coverSource
                    Rectangle {
                        anchors.fill: parent
                        property string source
                        color: "#2e3239"
                        opacity: 0.94
                    }
                    Image {
                        anchors.right: parent.right
                        visible: lanes > 0
                        source: Resource.imageUrl( 'lane', 'laneguid_line.png' )
                    }
                    Image {
                        source: parent.source
                    }
                    Image {
                        source: parent.coverSource
                        z: 99
                    }
                }
            }

            Item {
                id: distWrap
                width: distBgImg.width
                height: distBgImg.height
                Image {
                    id: distBgImg
                    visible: lanes > 0
                    source: Resource.imageUrl( 'lane', 'laneguid_bg.png' )
                }
                Row {
                    anchors.horizontalCenter: distBgImg.horizontalCenter
                    anchors.verticalCenter: distBgImg.verticalCenter
                    Repeater {
                        id: distNumberRepeater
                        model: distNumberLen
                        delegate: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property string source
                            Image {
                                source: parent.source
                            }
                        }
                    }
                }
            }
        }
    }
}

