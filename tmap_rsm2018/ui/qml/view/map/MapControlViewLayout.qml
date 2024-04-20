import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
	// 세로 기본
    property int mapScaleX: 10
    property int mapScaleOffsetY: -287
    property real scale: 1.0

    Component.onCompleted: {
    }
	
	function updateLayout( resolution ) {
        if ( resolution === LayoutConstants.L820x1024 ) {                        
        } else if (resolution === LayoutConstants.L800x480) {
            scale = 0.5
            mapScaleX = 0
            mapScaleOffsetY = -216
        }
	}
}

