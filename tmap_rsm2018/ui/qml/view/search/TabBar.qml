import QtQuick 2.0
//import QtQuick.Controls 1.3
//import QtQuick.Controls.Styles 1.3

Item {
    id: root

    property int tabWidth
    property int tabHeight
    property int tabBorderWidth: 0
    property int tabBorderName  : 0
    property color tabColor: "lightgray"
    property color tabActiveColor: "white"    
    property real tabActiveColorOpacity: 1
    property color tabTextColor: "white"
    property color tabTextActiveColor: "black"
    property int   tabActiveStyle : 0 // let setisfy what has color in activated : 0: Background, 1: Border
    property color tabBorderColor: "white"
    property color tabActiveBorderColor: "white"
    property real tabActiveBorderOpacity: 1

    property bool isCswControl: false
    property int cswTabIndex: 0

    property var tabActiveStyleEnum : {
        "Background" : 0,
        "Border"     : 1,
         properties : {
            0 : { name : "Background Style" },
            1 : { name : "Border Style" }
        }
    }

    property var tabBorderNameEnum: {
        "All"  : 0,
        "Left" : 1,
        "Top"  : 2,
        "Right" : 3,
        "Bottom" : 4,
        properties: {
            0: {name: "All"  },
            1: {name: "Left" },
            2: {name: "Top"  },
            3: {name: "Right"  },
            4: {name: "Bottom"  }
        }
    }

    property var headers: []
    property string iconSource

    signal clicked(int index)

    Component.onCompleted: {
        if (row.children) {
            //row.children[0].checked = true
            setActiveTab(0)
            clicked(0)
        }
    }

    function setActiveTab( index ) {
        for (var i = 0; i < row.children.length; i++) {
            if ( row.children[i].objectName == "tabButton" ) {
                row.children[i].color = Qt.binding(function(){ return root.tabColor })
                row.children[i].colorOpacity = 1
                row.children[i].textColor = Qt.binding(function(){ return root.tabTextColor })
                // fixed, TMCRSM-601 , allow inactive tabs to have their border geometry
                row.children[i].borderColor = Qt.binding(function(){ return root.tabColor })
                row.children[i].borderColorOpacity = 1
                row.children[i].borderWidth = Qt.binding(function(){ return root.tabBorderWidth })
                row.children[i].borderName  = Qt.binding(function(){ return root.tabBorderName })
                row.children[i].setButtonBorder();
            }
        }
        if (index >= row.children.length)
            return

        // chunjae.lee - s 2018.03.26
        if (index >= headers.length)
            return
        // chunjae.lee - e 2018.03.26

        if (tabActiveStyle == tabActiveStyleEnum.Background)
        {
            row.children[index].color = Qt.binding(function(){ return root.tabActiveColor });
        }
        else
        {
            row.children[index].color = Qt.binding(function(){ return root.tabActiveColor })
            row.children[index].colorOpacity = Qt.binding(function(){ return root.tabActiveColorOpacity })
            row.children[index].borderColor = Qt.binding(function(){ return root.tabActiveBorderColor })
            row.children[index].borderWidth = Qt.binding(function(){ return root.tabBorderWidth })
            row.children[index].setButtonBorder();
        }

        row.children[index].textColor = Qt.binding(function(){ return root.tabTextActiveColor })
    }

    Rectangle {
        id: bgBody
        width: parent.width
        height: parent.height
        anchors.fill : parent
        color: tabColor
        Row {
        id: row

            Repeater {
                model: headers
                delegate: TabButton {
                    id: button
                    objectName: "tabButton"

                    width: root.tabWidth
                    height: root.tabHeight
                    title: modelData
                    buttonIndex: index
                    color: root.tabColor
                    textColor: root.tabTextColor
                    isCswFocused: (isCswControl && cswTabIndex == index)

                    onClicked: {
                        console.log("tab button clicked : " + buttonIndex)
                        setActiveTab(buttonIndex)
                        root.clicked(buttonIndex)
                    }
                }
            }
        }
    } // bg body
}
