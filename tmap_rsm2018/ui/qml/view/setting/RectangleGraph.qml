import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

//그래프
Canvas {
    id: canvas
    width                   : parent.width
    height                  : parent.height
    anchors.bottom          : parent.bottom
    antialiasing            : true

    property int sliceWidth : 0
    property var request    : ({Background:0,OutlineDot:1,DotPoint:2})
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
    property var strokeStyle: levelRgbArr[levelEnum.None]
    property var fillStyle  : levelRgbArr[levelEnum.None]
    property int   lineWidth: 0
    property bool  fill     : true
    property bool  stroke   : true
    property real  alpha    : 0.15

    property real xGap      : 18.33
    property int  div       : 1
    property int xOffset    : 5
    property int yOffset    : 5

    property real maxValue  : 180.0
    property real perValue  : (1.0/maxValue)

    function ctxPaint(seq, lineWidth, alpha, color) {
        var ctx = getContext("2d")
        ctx.save();
        if (CommonUtils.isEqual(seq , request.Background)) {
            ctx.clearRect(0,0,canvas.width, canvas.height)
        }
        ctx.strokeStyle = color
        ctx.fillStyle = color
        ctx.lineWidth = lineWidth
        ctx.globalAlpha = alpha
        ctx.lineJoin = "round"

        ctx.beginPath()
            var len = result.pm10List.length
            var x,y,i,j,k
            x=y=i=j=k=0
            if (CommonUtils.isEqual(seq , request.Background)) {
                ctx.moveTo(xOffset, height)
            }
            div = Math.round(len/(timeArr.length-1))
            xGap = sliceWidth/3
            var startX = null
            var endX = null
            var nextPm10Value = false
            var m = true
            for(i=len-1; i>=0; i--) {
                var data = result.pm10List[i].dataTime
                switch(seq) {
                    case request.Background:                        
                        if (i-1>= 0) {
                            if (result.pm10List[i-1].pm10Value > 0) {
                                nextPm10Value = true
                            }
                        }
                        if (result.pm10List[i].pm10Value > 0 ) {
                            x = xOffset + (j*xGap)
                            y = height - result.pm10List[i].pm10Value
                            if (m) {
                                m = false
                                ctx.moveTo(x, height)
                            }

                            if (nextPm10Value) {
                                ctx.lineTo(x, y)
                            } else {
                                startX = xOffset + ((j-1)*xGap + (xGap/1.2))
                                ctx.moveTo(startX, height)
                                ctx.lineTo(startX, y)
                                endX = xOffset + ((j+1)*xGap - (xGap/1.2))
                                ctx.lineTo(endX, y)
                                ctx.lineTo(endX, height)
                            }

                        } else {
                          nextPm10Value= false
                          if (!m) {
                              ctx.lineTo(x, height)
                          }
                          m = true
                        }
                        j++
                        break
                    case request.OutlineDot:
                        if (i-1>= 0) {
                            if (result.pm10List[i-1].pm10Value > 0) {
                                nextPm10Value = true
                            }
                        }

                        if (result.pm10List[i].pm10Value > 0 ) {
                            x = xOffset + (j*xGap)
                            y = height - result.pm10List[i].pm10Value
                            if (m ) {
                                m = false
                                ctx.moveTo(x, y)
                            }

                            if (nextPm10Value) {
                                ctx.lineTo(x, y)
                            } else {
                                startX = xOffset + ((j-1)*xGap + (xGap/1.2))
                                ctx.moveTo(startX, y)
                                endX = xOffset + ((j+1)*xGap - (xGap/1.2))
                                ctx.lineTo(endX, y)
                            }

                            if (CommonUtils.isEqual(j%div ,0)) {
                                ctx.ellipse(j*xGap,y-yOffset,10,10)
                            }
                            ctx.moveTo(x, y)
                        } else {
                          nextPm10Value = false
                          m = true
                        }
                        j++
                        break
                    case request.DotPoint:
                        y = height-result.pm10List[i].pm10Value
                        if (CommonUtils.isEqual(i ,0) && result.pm10List[i].pm10Value > 0) {
                            ctx.ellipse(j*xGap,y-yOffset,10,10)
                            break
                        }
                        j++
                        break
                }
            }
            if (CommonUtils.isEqual(seq , request.Background)) {
                ctx.lineTo(x, height)
            }
        ctx.closePath()

        if (canvas.fill)
            ctx.fill()
        if (canvas.stroke)
            ctx.stroke()
        ctx.restore()
    }

    onPaint: {
        var len = result.pm10List.length
        for(var i=0; i< len; i++) {
            result.pm10List[i].pm10Value = (result.pm10List[i].pm10Value * perValue) * canvas.height
        }
        ctxPaint(request.Background ,0 ,0.15 ,levelRgbArr[levelEnum.None])
        ctxPaint(request.OutlineDot ,0 ,1.0  ,levelRgbArr[levelEnum.None])
        ctxPaint(request.DotPoint   ,0 ,1.0  ,levelRgbArr[result.pm10Grade1h])
    }
}
