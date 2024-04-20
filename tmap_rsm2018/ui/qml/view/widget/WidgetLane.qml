import QtQuick 2.1
import WidgetConstants 1.0
import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root
    width: firstLanes * firstWidth + (firstLanes > 0 ? firstLaneDistanceWidth : 0)
    height: firstHeight
    visible: true    

    property int firstHeight: 48
    property int firstWidth: 48
    property int firstLaneDistanceWidth: 67

    property int secondHeight: 34
    property int firstLanes
    property var firstImageSources
    property var firstDistNumberImageSources
    property var secondImageSources
    property var secondDistNumberImageSources    
    property string firstLineImageSource: Resource.imageUrl( 'lane', 'laneguid_line.png' )
    property string secondLineImageSource: Resource.imageUrl( 'lane', 'laneguid2_line.png' )
    property string firstBgImageSource: Resource.imageUrl( 'lane', 'laneguid_bg.png' )
    property string secondBgImageSource: Resource.imageUrl( 'lane', 'laneguid2_bg.png' )
    property bool firstVisible: false
    property bool secondVisible: false
    property int widgetSize: WidgetConstants.SizeNone
    property bool reset: false

    function setToDefault() {
        firstHeight = 48
        firstWidth = 48
        firstLaneDistanceWidth = 67
        secondHeight = 34
    }

    onResetChanged: {
        if ( reset ) {
            firstVisible = false
            secondVisible = false
        }
    }

    onFirstVisibleChanged: {
    }

    onSecondVisibleChanged: {
    }

    onFirstImageSourcesChanged: {
        if ( firstImageSources && firstImageSources[0] ) {
            firstLanes = firstImageSources[0].length
        }
    }

    onFirstDistNumberImageSourcesChanged: {
    }

    onSecondDistNumberImageSourcesChanged: {
    }

    onWidgetSizeChanged: {
        setToDefault()

        if ( widgetSize == WidgetConstants.SizeXXXL ) {
        }
        else {
            secondHeight = 25
            secondLineImageSource = Resource.imageUrl( 'widgetlane', 'wid_laneguid2_line.png' )
            secondBgImageSource = Resource.imageUrl( 'widgetlane', 'wid_laneguid2_bg.png' )

            firstHeight = 36
            firstWidth = 36
            firstLaneDistanceWidth = 52

            firstLineImageSource = Resource.imageUrl( 'widgetlane', 'wid_laneguid_line.png' )            
            firstBgImageSource = Resource.imageUrl( 'widgetlane', 'wid_laneguid_bg.png' )
        }
    }

    Column {
        width: parent.width
        Loader {
            id: secondLoader
            sourceComponent: laneComp
            width: parent.width
            height: item ? ( item.visible ? secondHeight : 0 ) : 0                
            onLoaded: {
                item.objectName = "secondLane"
                item.laneIndex = 1
                item.laneImageSources = Qt.binding(function(){ return root.secondImageSources })
                item.distNumberImageSources = Qt.binding(function(){ return root.secondDistNumberImageSources })
                item.lineImageSource = Qt.binding(function(){ return root.secondLineImageSource })
                item.bgImageSource = Qt.binding(function(){ return root.secondBgImageSource })
                item.visible = Qt.binding(function(){ return root.secondVisible })
            }
        }
        Loader {
            id: firstLoader
            sourceComponent: laneComp
            y: secondLoader.height
            width: parent.width
            height: item ? ( item.visible ? firstHeight : 0 ) : 0
            onLoaded: {
                item.objectName = "firstLane"
                item.laneIndex = 0
                item.laneImageSources = Qt.binding(function(){ return root.firstImageSources })
                item.distNumberImageSources = Qt.binding(function(){ return root.firstDistNumberImageSources })
                item.lineImageSource = Qt.binding(function(){ return root.firstLineImageSource })
                item.bgImageSource = Qt.binding(function(){ return root.firstBgImageSource })
                item.visible = Qt.binding(function(){ return root.firstVisible })
            }
        }
    }

    Component {
        id: laneComp
        Item {
            id: laneRoot
            visible: false
            property int laneIndex
            property int lanes
            property var laneImageSources
            property var distNumberImageSources
            property string lineImageSource
            property string bgImageSource

            onLaneImageSourcesChanged: {
                lanes = 0
                if ( laneImageSources ) {
                    var imgList = laneImageSources[0]
                    if ( imgList ) {
                        var coverImgList = laneImageSources[1]
                        lanes = imgList.length
                        var laneRepeaterItemAt = laneRepeater.itemAt
                        for ( var i = 0; i < lanes; i++ ) {
                            laneRepeaterItemAt( i ).source = imgList[i]
                            laneRepeaterItemAt( i ).sourceCover = coverImgList[i]
                        }
                    }
                }
            }

            Row {
                anchors.fill: parent
                Repeater {
                    id: laneRepeater
                    model: lanes
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source
                        property string sourceCover
                        Rectangle {
                            anchors.fill: parent                            
                            color: laneIndex == 1 ? "#1c1d1e" : "#2e3239"
                            opacity: 0.94
                        }
                        Image {
                            source: parent.source
                        }
                        Image {
                            source: parent.sourceCover
                            z: 99
                        }
                    }
                }                
                Image {
                    visible: (lanes > 0) ? true : false
                    source: laneRoot.lineImageSource
                }
                Item {
                    id: distWrap
                    width: distBgImg.width
                    height: distBgImg.height
                    Image {
                        id: distBgImg
                        visible: (laneRoot.distNumberImageSources && laneRoot.distNumberImageSources.length > 0) ? true : false
                        source: laneRoot.bgImageSource
                    }
                    Row {
                        width: childrenRect.width
                        height: childrenRect.height
                        anchors.horizontalCenter: distBgImg.horizontalCenter
                        anchors.verticalCenter: distBgImg.verticalCenter
                        Repeater {
                            id: distNumberRepeater
                            model: laneRoot.distNumberImageSources
                            delegate: Item {
                                width: childrenRect.width
                                height: childrenRect.height
                                property string source: modelData
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
}

