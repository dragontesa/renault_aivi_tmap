import QtQuick 2.0
import CommonConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import '../../view/util/'


Item {
    id: root

    property var items                      : ([])
    property var images                     : ([])
    property var normalColors               : ([])
    property var selectColors               : ([])
    property var txtObj                     : ([])
    property font textFont
    property color textColor                : CommonConstants.Transparent
    property color backgroundBorderColor    : CommonConstants.Transparent
    property color backgroundColor          : CommonConstants.Transparent
    property color controllerBorderColor    : CommonConstants.Transparent
    property color controllerColor          : CommonConstants.Transparent
    property int backgroundBorderWidth      : 0
    property int controllerBorderWidth      : 0
    property int controllerMargin           : 0
    property int itemIndex                  : 0
    property int pressIndex                 : itemIndex
    property bool isPressed                 : false
    property bool isMoveSelector            : false
    property alias list                     : listView

    property int highlightBorderWidth       : 0
    property color highlightBorderColor     : CommonConstants.Transparent
    property color highlightColor           : CommonConstants.Transparent

    property string selectSource            : CommonConstants.Empty
    property string bgSource                : CommonConstants.Empty
    property size selectSize                : Qt.size(151,41)
    property int leftMargin                 : 0
    property int rightMargin                : 0
    property int topMargin                  : 0
    property int bottomMargin               : 0
    property int itemWidth                  : (width - (leftMargin+rightMargin)) / items.length
    property int textOffsetY                : 3

    //[ID]
    property alias root                     : root
    property alias box                      : box
    property alias img                      : img
    property var cswObj                     : null

    opacity: (!enabled) ? 0.4 : 1.0

    onImagesChanged: {
        if(images.length <= 0)
            return
        img.source = images[0]
    }

    onItemsChanged: {
        for(var i = 0; i<items.length; i++) {
            var item = items[i]
            listModel.set(i, {text: item})
        }
    }

    onSelectColorsChanged: {
        setIndex(itemIndex)
    }

    onItemIndexChanged: {
        setIndex(itemIndex)
    }

    function startselectAnim(index, duration) {
        if (CommonUtils.isEqual(index , itemIndex)) {
            setIndex(index)
            return
        }

        selectAnim.index = index
        selectAnim.from = selector.x
        //selectAnim.to = itemWidth * index
        selectAnim.to = leftMargin + (itemWidth * index)
        selectAnim.duration = duration
        selectAnim.start()
    }

    function setIndex(index) {
        if (index < 0)
            return
        if (itemIndex !== index) {
            itemIndex = index
            return
        }
        pressIndex = index
        selector.x = leftMargin + (index * itemWidth)
        if (images.length>index)
            img.source = images[index]
        for ( var i=0; i < txtObj.length; i++ )
            txtObj[i].color = (CommonUtils.isEqual(i , index)) ? selectColors[i] : normalColors[i]
    }

    ListModel { id: listModel }

    // Background
    Rectangle {
        id: box
        anchors.fill: parent
        color: backgroundColor
        border.color: backgroundBorderColor
        border.width: backgroundBorderWidth
        clip: true
        Image {
            id: img
            width: parent.width
            height: parent.height
            sourceSize.width: width
            sourceSize.height: height
            source: bgSource
            Item {
                id: selector
                width: itemWidth
                height: parent.height
                onXChanged: {
                    if (!isMoveSelector)
                        return x
                    for (var i =0; i<items.length; i++) {
                        if ((txtObj[i].parent.x + txtObj[i].x) <= selector.x   &&
                             selector.x <= (txtObj[i].parent.x + txtObj[i].x +  txtObj[i].contentWidth ) )  {
                          txtObj[i].color = selectColors[i]
                        } else if ((txtObj[i].parent.x + txtObj[i].x) <= selector.x + txtObj[i].contentWidth &&
                                    selector.x + txtObj[i].contentWidth <= (txtObj[i].parent.x + txtObj[i].x + txtObj[i].contentWidth ) )  {
                          txtObj[i].color = selectColors[i]
                        } else {
                          txtObj[i].color = normalColors[i]
                        }
                    }
                }
                Image {
                    source: selectSource
                    width: selectSize.width
                    height: selectSize.height
                    sourceSize.width: width
                    sourceSize.height: height
                    anchors.centerIn: parent
                    visible: selectSource != CommonConstants.Empty
                    opacity: ((itemIndex == pressIndex)  && isPressed) ? 0.6 : 1.0
                }
            }
            GridView {
              id: listView
              Loader {sourceComponent: pressSelector}
              width: parent.width - (root.leftMargin + root.rightMargin)
              height: parent.height - (root.topMargin + root.bottomMargin)
              anchors.centerIn: parent
              cellWidth: itemWidth
              cellHeight: height
              clip: true
              currentIndex: -1
              model: listModel
              highlight: highlightBar
              highlightFollowsCurrentItem: false
              interactive: false
              delegate: columnDelegate
            }
        }
    }

    Component {
        id: columnDelegate
        Item {
            width: listView.cellWidth
            height: listView.cellHeight
            Text2 {
                property var componentOnCompleted: txtObj[model.index] = this
                width: parent.width; height: parent.height
                color: (normalColors.length > index) ? ( CommonUtils.isEqual(index , itemIndex) ? selectColors[index] : normalColors[index]) : textColor
                font: textFont
                offsetY: textOffsetY
                text: model.text
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var index = model.index
                        if (CommonUtils.isEmpty(selectSource)) {
                            if (CommonUtils.isEqual(items.length , 2)) {
                                if (CommonUtils.isEqual(itemIndex , model.index)) {
                                    index = (++index % items.length)
                                    setIndex(index)
                                    return
                                }
                            }
                        }
                        startselectAnim(index, 200)
                    }

                    function pressedFunction(obj) {
                        if (cswObj) {
                            cswObj.forceLostFocus()
                        }
                        pressIndex = model.index
                        isPressed = true
                        if (pressIndex != itemIndex) {
                            idDrag.target = undefined
                        } else {
                            idDrag.target = selector
                        }
                        txtObj[pressIndex].color = selectColors[pressIndex]
                        if (!CommonUtils.isEqual(idDrag.target , undefined)) {
                            isMoveSelector = true
                        }
                    }

                    function releaseFunction() {
                        isPressed = false                        
                        if (CommonUtils.isEqual(idDrag.target , undefined)) {
                            //토글메뉴 위로 드래그시 텍스트 사라지는 현상
                            txtObj[pressIndex].color = normalColors[pressIndex]
                            return
                        }
                        isMoveSelector = false
                        idDrag.target = undefined

                        var index = 0
                        for (var i =0; i<items.length; i++) {
                            if ( ((i*itemWidth) + (itemWidth/2)) < selector.x )  {
                                index = i+1
                            }
                        }
                        if (pressIndex != index) {
                            setIndex(index)
                        } else {
                            setIndex(pressIndex)
                        }
                    }

                    onPressedChanged: {
                        if (pressed) {
                            pressedFunction(this)
                        }
                        else {
                            releaseFunction()
                        }
                    }


                    drag {
                        id: idDrag
                        target: selector
                        axis: Drag.XAxis
                        minimumX: leftMargin
                        maximumX: leftMargin + (itemWidth * (items.length - 1))
                    }
                    onPositionChanged:  {
                        if (drag.active) {
                        }
                    }
                }
            }
        }
    }

    Component {
          id: highlightBar
          Rectangle {
              property var currentItem : listView.currentItem
              onCurrentItemChanged: {
                  if (!currentItem) {
                      x=y=width=height=0
                      return
                  }
                  x = Qt.binding(function(){ return currentItem.x + controllerMargin})
                  y =  Qt.binding(function(){ return currentItem.y  + controllerMargin})
                  width = Qt.binding(function(){ return listView.cellWidth - (controllerMargin*2)})
                  height = Qt.binding(function(){ return listView.cellHeight - (controllerMargin*2)})
                  radius = Qt.binding(function(){ return height / 2})
              }
              anchors.margins: controllerMargin
              border.color : highlightBorderColor
              border.width : highlightBorderWidth
              color: highlightColor
          }
    }

    Component {
          id: pressSelector
          Image {
            property var currentItem : txtObj[pressIndex] ? txtObj[pressIndex].parent : null
            onCurrentItemChanged: {
                if (!currentItem) {
                    x=y=width=height=0
                    return
                }
                parent = currentItem
                width =  selectSize.width
                height = selectSize.height
                sourceSize.width = width
                sourceSize.height = height
                anchors.centerIn = parent
                z = parent.z + 1
                txtObj[pressIndex].z = z+1
            }
            source: selectSource
            visible: ((selectSource != CommonConstants.Empty  && itemIndex != pressIndex) && isPressed)
            opacity: 0.6
          }
    }    

    Timer {
        id: timer
        interval: 10
        running: false
        repeat: false
        property int index: -1
        onTriggered: {
            setIndex(index)
        }
    }

    NumberAnimation {
        id: selectAnim
        target: selector
        properties: "x"        
        property int index: -1
        onStarted: {
            isMoveSelector = true
        }
        onStopped: {
          isMoveSelector = false
          timer.index = index;
          timer.running = true;
        }
    }
}
