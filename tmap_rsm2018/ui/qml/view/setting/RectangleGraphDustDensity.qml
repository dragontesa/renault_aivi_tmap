import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

Item {
    id: widgetRoot
    width: parent.width
    height: parent.height
    property var layout     : ({ l:{margin:  0,width: 0,height: 0, textArr:([]), textWidth:0, textHeight:0, fontSize:0},
                                 m:{margin:  0,width: 0,height: 0, },
                                 b:{margin:  0,width: 0,height: 0, textArr:([]), textWidth:0, textHeight:0, fontSize:0}
                               })
    property var result     : ({pm10Value:0,pm10Grade1h:0,pm10Grade:0,pm10Value24:0,
                                pm25Value:0,pm25Grade1h:0,pm25Grade:1,pm25Value24:0,
                                pm10List:[{pm10Value:0,dataTime:"0000-00-00 00:00"},],
                                no2Grade:0,no2Value:0.0,
                                o3Grade:0,o3Value:0.0,
                                khaiGrade:0,khaiValue:0,
                                so2Grade:0,so2Value:0.0,
                                coGrade:0,coValue:0.0,
                                dataTime:"0000-00-00 00:00",mangName:"",stationName:"",
                                })
    property var baseDate   : ({})
    property var levelRgbArr: ([])
    //라인
    Column {
        width: parent.width
        height: parent.height
        Repeater {
            model: layout.l.textArr.length
            delegate:
                Item {width:parent.width;height:layout.l.height
                    Rectangle {width: parent.width;height:1; anchors.top: parent.top; color: GlobalConstants.kListTextColor; opacity: (!CommonUtils.isEqual(index , layout.l.textArr.length)) ? 0.1 : 0.5}
                }
        }
    }
    //Left 텍스트
    Column {
        width: layout.l.width
        height: parent.height
        Repeater {
            model: layout.l.textArr.length
            delegate:
                Item {width: layout.l.width;height: layout.l.height
                    Text {
                        width: layout.l.textWidth
                        height: layout.l.textHeight
                        text: CommonUtils.isEqual(index, (layout.l.textArr.length-1)) ? CommonConstants.Empty : layout.l.textArr[layout.l.textArr.length - (index+1)]
                        color: GlobalConstants.kListTextColor
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(layout.l.fontSize)}
                    }
                }
        }
    }
    //그래프
    Column {
        width:parent.width
        height:parent.height
        Item {width:parent.width
            height: ((layout.l.textArr.length -1) * layout.l.height)
            Row {
                width: parent.width
                height: parent.height
                Item {width:layout.l.textWidth+layout.m.margin-5;height:parent.height}
                RectangleGraph {
                    width: layout.m.width
                    height: parent.height                    
                    result: widgetRoot.result
                    levelRgbArr: widgetRoot.levelRgbArr
                    baseDate: widgetRoot.baseDate
                    sliceWidth: layout.b.width
                }
            }
        }
        Item {width: parent.width;height:layout.b.margin}
        Row {
            width: parent.width
            height: layout.b.textHeight
            Item {width:layout.l.width;height:parent.height}
            Repeater {
                model: layout.b.textArr.length
                delegate:
                    Row {
                        width: layout.b.width
                        height: parent.height
                        Item {width:layout.b.textWidth;height:parent.height
                            Text {
                                color: GlobalConstants.kListTextColor
                                text: layout.b.textArr[index]
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(layout.b.fontSize)}
                                anchors.centerIn: parent
                            }
                        }
                        Item {width:layout.b.width-layout.b.textWidth;height:parent.height}
                    }
            }
        }
        Item {width:parent.width;height:layout.b.height-(layout.b.margin+layout.b.textHeight)}
    }//Column
}//Item


