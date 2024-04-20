import QtQuick 2.0
import CommonConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../component"

Item {
    id: tileMenuRoot

    property var caller             : null
    property int gridRow            : 0
    property int gridCol            : 0
    property alias highlight        : listView.highlight
    property alias frame            : frame
    property alias listView         : listView
    property var clickFuntion       : null
    property var menuArr            : null
    property var menuAttrEnum       : {
                                        "Name" : 0,
                                        "Key"  : 1,
                                        "Arg"  : 2,
                                        "Obj"  : 3,
                                        "ImgN" : 4,
                                        "ImgP" : 5,
                                        "ImgF" : 6,
                                       }
    property bool focused           : false
    property bool isFocused         : false
    property color highlightColor   : CommonConstants.Transparent
    property color backgroundColor  : CommonConstants.Transparent

    signal buttonCompleted(var model, int row , int col)
    signal buttonPressed(var model, int row, int col)
    signal buttonReleased(var model, int row, int col)
    signal buttonClicked(var model, int row, int col)
    signal buttonLongClicked(var model, int row, int col)
    signal buttonFocused(var model, int row, int col)
    signal buttonLostFocused(var model, int row, int col)

    onMenuArrChanged: {
        if (!menuArr)
            return
        createModel()
    }

    function setOwnerObj(obj, row, col) {
        menuArr[row][col][menuAttrEnum.Obj] = obj
        setBtnProperty(row, col)
    }

    function setBtnProperty(row, col) {
        var obj = menuArr[row][col][menuAttrEnum.Obj]
        if (!obj)    return

        var text = menuArr[row][col][menuAttrEnum.Name]
        var key = menuArr[row][col][menuAttrEnum.Key]
        var imgN = menuArr[row][col][menuAttrEnum.ImgN]
        var imgP = menuArr[row][col][menuAttrEnum.ImgP]


        obj.width = listView.cellWidth
        obj.height = listView.cellHeight
        obj.box.colorN = CommonConstants.Transparent
        obj.box.colorP = CommonConstants.Transparent

        if (!CommonUtils.isNull(text)) {
            obj.img.sourceN             = Resource.imageUrl(imgN)
            obj.img.sourceP             = Resource.imageUrl(imgP)
            obj.img.opacity             = (!CommonUtils.isNull(key)) ? 1.0 : 0.3
            obj.img.anchors.horizontalCenter = obj.box.horizontalCenter
            obj.txt.text                = text
            obj.txt.width               = obj.width
            obj.txt.anchors.centerIn    = undefined
            obj.txt.horizontalAlignment = Qt.AlignHCenter
            obj.txt.colorN = (!CommonUtils.isNull(key)) ? GlobalConstants.kListTextColor : CommonConstants.Dark + CommonConstants.Gray
            obj.txt.colorP = (!CommonUtils.isNull(key)) ? GlobalConstants.kListTextColor: CommonConstants.Dark + CommonConstants.Gray
        }
        obj.enabledOpacity   = (!CommonUtils.isNull(key)) ? false: true
        obj.enabled    = (!CommonUtils.isNull(key)) ? true : false
    }

    Component {
        id: columnDelegate
        TPushButton {
             width: listView.cellWidth
             height: listView.cellHeight
             property var row : model.row
             property var col : model.col
             property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && tileMenuRoot.focused
             property color outlineColor: CommonConstants.Transparent
             property var componentOnCompleted:{
                 setOwnerObj(this, row, col)
                 tileMenuRoot.buttonCompleted(model, row,col)
                 return null
             }

             onIsFocusedChanged: {
                 if (isFocused) {
                     tileMenuRoot.buttonFocused(model,row, col)
                 } else {
                     tileMenuRoot.buttonLostFocused(model, row,col)
                 }
             }


             onButtonPressed: {
                 tileMenuRoot.buttonPressed(model, row, col)
             }
             onButtonReleased: {
                 tileMenuRoot.buttonReleased(model, row, col)
             }
             onButtonClicked: {
                  tileMenuRoot.buttonClicked(model, row, col)
             }
             onButtonLongClicked: {
                 tileMenuRoot.buttonLongClicked(model, row, col)
             }
             Rectangle {width:parent.width;height:1;color:outlineColor}
             Rectangle {width:1;height:parent.height;color:outlineColor; visible:col}
       }
    }

    function createModel(){
        listModel.clear()
        for (var i = 0; i<gridRow; i++) {
            for(var j=0; j< gridCol; j++) {
                listModel.append(      {
                    row : i,
                    col : j
                })
            }
        }
    }

    ListModel { id: listModel }

    MouseArea {
        width:parent.width
        height:parent.height
    }

    Rectangle {
       id: frame
       width: parent.width
       height: parent.height
       color: backgroundColor
       Column {
           id: tileMenu
           width: parent.width
           height: parent.height
           GridView {
             id: listView
             width: parent.width
             height: parent.height
             cellWidth: parent.width/gridCol
             cellHeight: height/gridRow
             //cacheBuffer: cellHeight * 3
             clip: true
             model: listModel
             currentIndex: -1
             interactive: false
             highlightFollowsCurrentItem: false
             delegate: columnDelegate

             //TMCRSM-343
             property var pressedItem: null
           }
       }
   }
}
