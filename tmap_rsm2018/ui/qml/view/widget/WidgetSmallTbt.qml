import QtQuick 2.1
import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root
    property string firstImageSource
    property var firstDistNumberImageSources
    property bool reset

    // test
//    property var distNumberSources: [
//        Resource.imageUrl( "widgettbt", "wid_tbt_2nd_num_9.png" ),
//        Resource.imageUrl( "widgettbt", "wid_tbt_2nd_num_9.png" ),
//        Resource.imageUrl( "widgettbt", "wid_tbt_2nd_num_9.png" ),
//        Resource.imageUrl( "widgettbt", "wid_tbt_2nd_num_km.png" )
//    ]
//    property string imageSource: Resource.imageUrl( "widgettbt", "wid_m_tbt_2nd_02.png" )

	width: 194	
	
	Image {
		id: tbtImage
		width: 86
		height: 86
		anchors.top: parent.top        
		anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        source: firstImageSource
	}
	Row {
		width: childrenRect.width
		height: childrenRect.height
        anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: tbtImage.bottom
		anchors.topMargin: 6
		Repeater {
			id: distNumberRepeater
            model: firstDistNumberImageSources
			delegate: Item {
				width: childrenRect.width
				height: childrenRect.height
                property string source: modelData
				Image {
					id: distNumberImage
					source: parent.source
				}
			}
		}
	}	
}

