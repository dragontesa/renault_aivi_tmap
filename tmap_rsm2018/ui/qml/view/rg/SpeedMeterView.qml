import QtQuick 2.0
import "../../component"
import "../../view"
import SpeedMeterViewEvent 1.0

View {
    id: root        
    width: speedNumberRow.width
    height: speedNumberRow.height
    anchors.horizontalCenter: parent.horizontalCenter

    property int maxSpeedNumbers: 3    
    property var speedNumberImages
    visible: true

    onVisibleChanged: {
        //QmlAppHelper.debug( "visible: " + visible )
    }

    onSpeedNumberImagesChanged: {
        maxSpeedNumbers = speedNumberImages.length
        var speedNumberRepeaterItemAt = speedNumberRepeater.itemAt
        for ( var i = 0; i < speedNumberImages.length; i++ ) {
            speedNumberRepeaterItemAt( i ).source = speedNumberImages[i]
        }
    }
	
    onMediated: {
        sendEvent( SpeedMeterViewEvent.Mediated )
    }	

    Row {
        id: speedNumberRow
        width: childrenRect.width
        height: childrenRect.height
        Repeater {
            id: speedNumberRepeater
            objectName: 'speedNumberRepeater'
            model: maxSpeedNumbers
            delegate: Item {
                width: childrenRect.width
                height: childrenRect.height
                //Resource.imageUrl( 'sdi', 'tmap_speed_num_me_9.png' )
                //property string source: 'file:C:/Users/san/tmap_rsm2018/static/res/img/zone/zone1_dummy.png'
                property string source: Resource.imageUrl( 'sdi', 'tmap_speed_remain_num_0.png' )
                Image {
                    id: speedNumberImage
                    source: parent.source
                }
            }
        }
    }
}
