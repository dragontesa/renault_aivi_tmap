import QtQuick 2.0
import TestConstants 1.0

Item {
    id: root
    z: 99
    property var owner: 33

    signal keyClicked(variant data)

	Row {
		spacing: 10		
		Repeater {
            model: [
                {value:1},{value:2},{value:3},{value:4},{value:5},{value:6},{value:7},{value:8},{value:9},{value:10},
                {value:-1},{value:-2},{value:-3},{value:-4},{value:-5},{value:-6},{value:-7},{value:-8},{value:-9},{value:-10},
            ]
			Rectangle {
				width: 30
				height: 30
                color: 'red'
                Text {
                    anchors.centerIn: parent
                    text: modelData.value
                }

				MouseArea {
					anchors.fill: parent
					onClicked: {
                        console.log("TestConstants.KeypadClicked owner: " + root.owner + " event: " + TestConstants.KeypadClicked)
                        root.owner.sendEvent(TestConstants.KeypadClicked, modelData.value)
					}
				}
			}
		}
	}	
}
