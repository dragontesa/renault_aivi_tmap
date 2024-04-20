import QtQuick 2.1
import TrafficBarViewEvent 1.0
import TrafficBarViewType 1.0
import QuickTrafficBarGraph 1.0
import LayoutConstants 1.0
import "../../component"
import "../../view"


View {
    id: root
    readonly property int trafficBarWidth: 20 //18
    property int          trafficBarHeight: 430 //428  TMCRSM-4128: updated by day night
    readonly property int progressBarInnerMargin: 2 // margin top,bottom each 1
    readonly property int trafficConditionBarWidth: 16
    property int          trafficConditionBarHeight: trafficBarHeight - progressBarInnerMargin // height for traffic condition area to be painted, trafficbar height - top,bottom inner margin
    property bool visualable: (parent != undefined)?parent.visible:true // 교통정보 업데이트만 할 지 화면에 그리기까지 할지(기본값)
    property int  onlineState: 0 // 0: offline, 1: online
    property int  skyState: 0 // 0: day, 1: night
    property Item progressBar: progressImg

    onVisualableChanged: {
        console.log("tb---> tb "+this+" visualable<> changed to "+visualable)
        if (trafficBar !== undefined)
           trafficBar.visualable = visualable
    }

    onOnlineStateChanged: {
        if (trafficBar !== undefined) {
            trafficBar.updateTrafficGuidance();
            trafficBar.updateLeftDistance();
            trafficBar.updateTrafficGraph()
        }
    }

    onSkyStateChanged: {
        console.log("tb---> skystat changed to "+skyState);
        if (trafficBar !== undefined) {
            trafficBar.updateTrafficGuidance();
            trafficBar.updateLeftDistance();
            trafficBar.updateTrafficGraph()
        }
    }

    layout: QtObject {

        function updateLayout( resolution ) {
            if ( resolution === LayoutConstants.L820x1024 ) {
                trafficBarHeight = 430 // 428
                viewType = TrafficBarViewType.App_L820x1024

            } else if (resolution === LayoutConstants.L800x480) {
                trafficBarHeight = 300
            }


            // update bar, cavata layout by resolution
            updateByViewtype();
        }
    }

    readonly property double tbarOpacity: 1
    property string passedRoadColor: (skyState == 0)?"#bebebe":"#aeaeae" // "#979797"//"#565656"/*"#979797"*/
    // TMCRSM-3896 change color regarding day or night for each on/offline, 0:offline, 1:online
    readonly property var tconditionColors: {
       0: [
             [ "#188BF4", "#03BC6F", "#fb9a0d", "#f21616" ], // day
             [ "#00B4FF", "#13BC62", "#fb9a0d", "#f21616" ]  // night
        ],
       1: [
             [ "#7b7c82", "#03BC6F", "#fb9a0d", "#f21616" ], // day
             [ "#7b7c82", "#13BC62", "#fb9a0d", "#f21616" ]  // night
        ]
    }

    property int viewType: TrafficBarViewType.App_L800x480
    onViewTypeChanged: {
        // update bar, cavata layout by view type
        updateByViewtype();        
    }

    visible: false
    width: 20 //42 TMCRSM-4128: updated by day night, left margin 13 | bar 16 | right margin 13 | right edge margin 10
    height: trafficBarHeight

    readonly property var guideEnum : {
        "AccDistance" : 0,
        "Cong" : 1
    }

    property var trafficBar

    property double leftDistance: 0
    onLeftDistanceChanged: {
        if (trafficBar !== undefined)
            trafficBar.leftDistance = leftDistance
    }
    property double totalDistance: 0
    onTotalDistanceChanged: {
        if (trafficBar !== undefined)
            trafficBar.totalDistance = totalDistance
    }

    property var trafficGuideList
    onTrafficGuideListChanged: {
        if (trafficBar !== undefined) {
            trafficBar.trafficGuideList = trafficGuideList
//            trafficBar.visible = trafficGuideList.length > 0 // trafficbar's visibility spec has changed, that is, trafficbar should be visible while RG regardless local or server.
        }
    }

    function updateByViewtype() {
        switch (viewType) {
        case TrafficBarViewType.App_L800x480:
            trafficBarHeight = 300
            break

        case TrafficBarViewType.App_L820x1024:
        case TrafficBarViewType.Widget_XXXL:
            trafficBarHeight = 430 //428
            break

        case TrafficBarViewType.Widget_XXL:
        case TrafficBarViewType.Widget_XL:
        case TrafficBarViewType.Widget_L:
            trafficBarHeight = 292
            break

        case TrafficBarViewType.Widget_M:
        case TrafficBarViewType.Widget_S:
            console.log("not supported!!!")
            break
        }

        // init bar, cavata layout
        progressImg.visible = true
        trafficConditionBarHeight = trafficBarHeight - progressBarInnerMargin // just to break the qml binding
    }

    onMediated: {
        sendEvent( TrafficBarViewEvent.Mediated)
    }

    Image {
        id: progressImg
        width: 20 //42
        height: trafficBarHeight
        anchors.fill: parent
//        TMCRSM-4128: updated by day night
        source: (skyState == 0) ? Resource.imageUrl('drive_progress_bg_day.png') : Resource.imageUrl('drive_progress_bg_night.png')
        visible: false
        opacity: tbarOpacity
       Component.onCompleted: {
           console.log("tb---> progress img:"+source.toString());
       }
    }

    Loader {
        anchors.top: progressBar.top
        anchors.topMargin: 1
        anchors.horizontalCenter: progressBar.horizontalCenter
        anchors.horizontalCenterOffset: 0
        sourceComponent: trafficBarComp
        onLoaded: {
            item.objectName = "trafficBar"
            trafficBar = item
        }
    }


    Component {
        id: trafficBarComp
        Item {
            property real totalDistance: 0.0  // 목적지까지 총 거리
            property real leftDistance: 0.0 // 목적지까지 남은 거리
            property real firstBarSize: 0.0 // 현재 구간 초기 바길이
            property real firstDistance: 0.0 // 현재 구간 초기 거리
            property real conditionBarSize: 0 // 교통상황 구간 BAR 크기
            property var trafficGuideList       // 교통정보
            property variant barsamples: []     // 교통정보 그래프 샘플
            property bool visualable: false // 교통데이터 업데이트하고 화면 그리기 (기본값) , 20191002 fix 가젯의 교통정보바에서 카바타위치 벗어남, TMCRSM-3897
            readonly property real barMinimum: 20.0  // 2020.3.6, old: cavata height(58)/2 (top inner margin 14 + cavata's half 15) + cavata top edge(2) + minimal space for user(4)
                                                                                   // 원인: 가젯에서는 뷰가 아니라서 RG,재탐색 정보 업데이트를 하지 않음으로.
            property bool needUpdateTrafficGraph: false
            property bool needUpdateTrafficResized: false
            property bool smoothMoveToNextArea: false // 자연스러운 구간 이동

            width: trafficConditionBarWidth
            height: conditionBarSize

            function updateTrafficGuidance()
            {
               // 남은거리가 없거나, 안내종료 등 상황
                if (totalDistance == 0 || trafficGuideList.length == 0)
                {
                    barsamples = []
                    firstBarSize = 0.0
                    firstDistance = 0.0
                    return;
                }

                // 20191002. lckxx fix TMCRSM-3897, RG 남은거리와 재탐색 후 교통정보의 싱크는 보장되지 않으므로 유효성 검사 필요
                // RG데이터가 오지 않는 경우(정지상태,맵매칭X,재탐색 중) UI 오류 발생 방지
                if (totalDistance < leftDistance)
                    leftDistance = totalDistance

                 // 길안내 초기에 음영지역, 지하 등에서 맵매칭을 하지 않는 경우 RG 정보가 없는 경우
                if (leftDistance <= 0 || conditionBarSize <= 0) {
                    leftDistance     = totalDistance;
                    conditionBarSize = trafficConditionBarHeight
                    firstBarSize     = conditionBarSize
                    firstDistance    = leftDistance
                }
                else {
                    // 교통정보 업데이트 시점에 바길이 계산  2019.12.30 fix TMCRSM-4438 TMCRSM-4434
                    conditionBarSize = (trafficConditionBarHeight * (leftDistance/totalDistance)).toFixed(3)
                }

                // 통신/비통신에 따른 표시 2019.12.19
                if (root.onlineState == 0) {
                    barsamples = []
                    var ba = [3]
                    ba[0] = totalDistance
                    ba[1] = conditionBarSize
                    ba[2] = tconditionColors[0][root.skyState][0]
                    barsamples[0] = ba
                    firstBarSize  = conditionBarSize
                    firstDistance = leftDistance
                    needUpdateTrafficGraph = true;
                    return
                }

                var ld     = leftDistance
                var passed = totalDistance - ld;
                // 재탐색 후 RG 남은 거리 정보가 보장 안되거나, 경로가 다시 계산 된 경우
                if (passed < 0)
                    passed = 0;
                var lastDistance = 0;
                var cnt = ( barsamples !== null ) ? barsamples.length: 0;
                var conditionChanged = true;
                if (cnt > 0) {
                    lastDistance = barsamples[0][0] // distance
                    conditionChanged = lastDistance - passed <= 0;
                }

                // 지나간 거리를 제외한 교통상황 구간의 시작 위치
                var lo = 0;
                var hi = trafficGuideList.length-1;
                var half = 0;
                while (lo <= hi)
                {
                    half = parseInt((lo+hi)/2);
                    var di = trafficGuideList[half][guideEnum.AccDistance];
                    if (lo == hi && di <= passed) {
                        half += 1;
                        break;
                    }

                    if (di <= passed)
                        lo = half+1;
                    else
                        hi = half-1;
                }

                // 중복된 교통상태 구간을 하나로
                var guideSectionList = [];
                var sectionCnt = 1;
                var flg = trafficGuideList[half][guideEnum.Cong];
                guideSectionList[0] = trafficGuideList[half];
                // console.log("tb---> tg="+trafficGuideList.length+", cong 0: "+flg);
                for (var i=half;i<trafficGuideList.length;i++)
                {
                    if (flg != trafficGuideList[i][guideEnum.Cong]) {
                        guideSectionList[sectionCnt++] = trafficGuideList[i];
                        flg = trafficGuideList[i][guideEnum.Cong];
                    }
                    else {
                        guideSectionList[sectionCnt-1][guideEnum.AccDistance] = trafficGuideList[i][guideEnum.AccDistance];
                        // TMCRSM-3659, 시뮬레이션 모드에서 대안 경로를 변경하여 다시 시작 할 때 바길이가 길어진다(거리가 더 짧은 대안경로 변경시)
                        //  원인: 대안경로 변경 후 재탐색이 되야 하나 이전 경로에 대한 트래픽정보를 읽어오고 있다
                        //  예외 처리, (경로 거리가 더 짧을 경우는 길이에 영향 없음)
                        if (guideSectionList[sectionCnt-1][guideEnum.AccDistance] > totalDistance) {
                            guideSectionList[sectionCnt-1][guideEnum.AccDistance] = totalDistance;
                            break;
                        }
                    }
                }

                // 2020.1.22 통신상태이나 남은 구간 1개(도착 지점)에서 더이상 교통정보 업데이트가 안일어나고, 
                // 마지막 업데이트 이후 경로 거리에 차이가 발생하는 경우 !
                if (sectionCnt == 1) {
                    barsamples = []
                    var ba2 = [3]
                    ba2[0] = totalDistance
                    ba2[1] = conditionBarSize
                    ba2[2] = tconditionColors[1][root.skyState][guideSectionList[0][guideEnum.Cong]%4]
                    barsamples[0] = ba2
                    firstBarSize  = conditionBarSize
                    firstDistance = leftDistance
                    needUpdateTrafficGraph = true;
                    // console.log("tb---> up tg just 1, t="+totalDistance+", l="+leftDistance+" ="+conditionBarSize);
                    return
                }

                // 남은 거리 교통상황 구간 반영
                // 현 위치 구간의 정보 구간이 카바타보다 작아서 가려지는 이슈 TMCRSM-3022 수정 2019-4-25
                // 현위치에 가중치 적용
                var barSize = conditionBarSize;
                var curDistance = guideSectionList[0][guideEnum.AccDistance];
                var barmin = barMinimum;
                var x1 = curDistance-passed; // first traffic section's distance
                var pv = barSize*x1/ld; // 측정값
                var sv = pv + (pv<barmin)*(barmin-pv*(ld-x1)/ld); // 기대값 :  x1이 0에 가까워 지면 sv는 임계값 barMinimum에 이른다
                var a  = x1 // 가중치 계수  x-√(barSize-sv) <= a <= x
                //  2020.2.19 fix 기대값 오류, 주로 목적지 근처에 도달 할 때까지 총 경로 거리에 큰 변화가 없을 경우 발생,
                //  대부분은 재탐색에 의해 총 경로거리는 남은 거리에 맞게 줄어든다
                if (barSize > sv)
                    a = x1-Math.sqrt(barSize-sv) + Math.sqrt(barSize-sv)*(ld-x1)/ld;

                var w  = ((x1-a)*(x1-a)+sv)/pv;  // <- pv*w(x) = (x-a)^2+sv
                 // 가중치 필요 없는 경우는 그대로 진행
                if (pv >= sv)
                    w = 1;
                // 재탐색 또는 경로이탈 등 현재 구간이 늘거나 남은거리가 늘어난 경우
                else if ((lastDistance > 0 && lastDistance<curDistance) ||
                         (lastDistance-curDistance> 0)) {
                }
                // 동일한 구간에서 이동 중일 때6
                else if (!conditionChanged && passed > 0) {
                }

                var mv = pv*w;
                // 2020.2.19 기대값 오류
                if (mv > barSize)
                    mv = pv
                firstBarSize  = mv
                firstDistance = x1
                smoothMoveToNextArea = false

                // console.log("tb---> tg h="+half+", c="+sectionCnt+", d0="+curDistance+", mv="+mv+" x1="+x1+", pv="+pv+" sv="+sv+" a="+a+" w="+w)

                var bar = [3]
                bar[0] = curDistance;
                bar[1] = mv;
                bar[2] = tconditionColors[onlineState][skyState][guideSectionList[0][guideEnum.Cong]%4]
                barsamples = []
                barsamples[0] = bar
                passed  += x1;
                ld      -= x1
                barSize -= mv
                for (var j=1;j<sectionCnt;j++)
                {
                    var guide = guideSectionList[j];
                    var distance = guide[guideEnum.AccDistance]-passed;
                    var pvj  = barSize*distance/ld;
                    var cong = guide[guideEnum.Cong]
                    var b  = [3]
                    b[0]   = guide[guideEnum.AccDistance];
                    b[1]   = pvj;
                    b[2]   = tconditionColors[onlineState][skyState][cong%4];
                    barsamples[j] = b;
                    passed += distance
                }
                needUpdateTrafficGraph = true;
            }

            function updateLeftDistance()
            {
                if (totalDistance <= 0) { conditionBarSize = 0; return; }
                if (leftDistance == 0) return; // 안내 중 RG에서 0으로 오는 경우 빈번
                if (totalDistance < leftDistance) // 20191002. lckxx fix  TMCRSM-3897, RG 데이터와 재탐색 후 경로 정보의 싱크는 보장안되며,
                    leftDistance = totalDistance // RG데이터가 올라오지 않을 경우(정지상태) 업데이트 될 때까지 UI 오류가 발생 한다.

                var total     = totalDistance
                var ld        = leftDistance
                var bar0      = firstBarSize
                var d0        = firstDistance
                var barSize   = (trafficConditionBarHeight * (ld/total)).toFixed(3)
                var cavataPos = (progressBar.y + parseFloat(barSize)).toFixed(3)
                conditionBarSize = barSize

                // cavata 위치 업데이트
                // DONT'T APPLY WEIGHT WHEN TURN INTO NEXT AREA
                var ovlay  = 4.0  // cavata 와 bar가 겹치는 길이, (cavata의 round 상단에서 배경바를 완전히 가리는 round가 끝나는 지점)
                if (bar0 < 4.0) {
                    ovlay = bar0/2;
                }

                var offset = 14.0 + ovlay // cavata top inner margin(14) + cavata top edge
                // 가젯에서, 카바타 tall(58) 중 -offset 이 바 밑에 있기 때문에, 교통정보바와 zone3의 간격 20을 넘지 않도록 보정. 이때, 투명한 bottom inner margin(14)는 제외
                if (barSize-offset+26 >= trafficConditionBarHeight+20) {
                    offset += barSize-offset+26-trafficConditionBarHeight-20 + 1.5
                }

                cavataItem.y = cavataPos - offset

                if (barsamples.length > 0 && bar0 > 0) {
                    var passed  = total - ld;
                    var d1      = barsamples[0][0] - passed
                    if (d1 > 0) { // 현재 구간 이동 중
                        // 가중치 고려한 카바타 위치 조정
                        var bar1 = bar0*d1/d0
                        var y0   = cavataPos-bar1

                        // 모의 주행으로 next, prev 조작 시 잘못된 남은 거리나 경로 거리 정보가 들어오는 경우 오류 방지
                        if (y0 < 0) {
                            return;
                        }

                        // smooth cavata overlay offset, bar의 70% 지점까지 증가
                        if (smoothMoveToNextArea == true && d1/d0 > 0.3)
                            ovlay = ovlay*(1-d1/d0)
                        else
                            smoothMoveToNextArea = false

                        var y1   = y0+bar1-ovlay*d1/d0-14.0

//                        console.log("tb-- bar1/bar0="+bar1+"/"+bar0+", d1/d0="+d1/d0+", offset="+offset+", ov="+ovlay+", y1="+y1);
                        // 비통신에서는 현 구간과 barSize는 같아야 하나, 
                        // 재탐색이 발생하지 않는 동안, 총 거리가 변경될 수 있어 변경 사항을 반영해준다.
                        if (onlineState == 0 || barsamples.length == 1) {
                            if (barSize !== bar1.toFixed(3)) {
                                barsamples[0][0] = totalDistance
                                barsamples[0][1] = barSize
                                firstBarSize = barSize
                                firstDistance = ld
                                needUpdateTrafficResized = true
                                return
                            }
                        }
                        cavataItem.y = y1
                        barsamples[0][1] = bar1
                        // resize all bars after first
                        passed  += d1
                        ld      -= d1
                        barSize -= bar1
                        for (var j=1;j<barsamples.length;j++)
                        {
                            var b    = barsamples[j]
                            var di   = b[0]-passed;
                            var pv   = barSize*di/ld;
                            barsamples[j][1] = pv
                            passed += di
                        }

                        needUpdateTrafficResized = true
                    }
                    else {
                        // OPT1: NO WEIGHT, SMOOTH MOVE WHEN TURN INTO NEXT AREA (자연스러운 이동에 선호)
                        // // 2020.3.6 turn into next area without recalcuating whole traffic guidance
                         if (barsamples.length > 1) {
                             // arrange samples to have second bar as head
                             var samples = []
                             for (var k=0;k<barsamples.length-1;k++) {
                                 samples[k] = barsamples[k+1]
                             }
                             firstBarSize = barsamples[1][1]
                             firstDistance = barsamples[1][0]-barsamples[0][0]
//                             console.log("tb--- next b0="+firstBarSize+", d0="+firstDistance+", d="+(firstDistance-barsamples[1][0]+passed));
                             barsamples = samples
                             needUpdateTrafficGraph = true;
                             smoothMoveToNextArea = true
                             updateLeftDistance(); // 20200324 이동거리가 있으므로 반영한다,
                         }

                        // OPT2: APPLY WEIGHT WHEN TURN INTO NEXT AREA (짧은 구간이 많을 경우에 선호함)
//                        updateTrafficGuidance()
                    }
                }
            }

            function updateTrafficGraph()
            {
                if (needUpdateTrafficGraph) {
                    bargraph.trafficBarSamples = barsamples
                    needUpdateTrafficGraph = false
                }
                if (needUpdateTrafficResized) {
                    bargraph.resizeTrafficBar(barsamples);
                    needUpdateTrafficResized = false
                }
            }

            onLeftDistanceChanged: {
                if (visualable) {
                    updateLeftDistance()
                    updateTrafficGraph()
                }
            }

            onTrafficGuideListChanged: {
                // 2019.12.30 fix TMCRSM-4438 TMCRSM-4434
                // 경유지 변경 등의 화면 변화를 동반하는 재탐색의 경우 visualable이 false이므로 이 상태를 따라가지 않아야 한다.
                // 남은거리와 전체 바 정보가 갱신이 안되어 회색바, 주행선과 차이 등의 이슈가 발생됨.
//                if (visualable) {
                   leftDistance = totalDistance; // 2019.09.27 TMCRSM-3899 수정, 교통정보는 경로변경, 재탐색 되면 받아오기 때문에 이전 경로에 대한 남은 거리는 초기화.
                   updateTrafficGuidance()
                   updateLeftDistance()
                   updateTrafficGraph()
//                }
            }

            onVisualableChanged: {
                if (visualable) {
                    updateTrafficGuidance()
                    updateLeftDistance()
                    updateTrafficGraph()
                }
            }

            TrafficBarGraph {
                id: bargraph
                anchors.fill: parent
                trafficBarSamples: null
            }

            // fixed TMCRSM-3655 (IR key 593007) in simulation case some time cavata could be covered by traffic condition graph, 
            // Statical layer order like 'Loader-Image' seems to get broken while repainting on item instantiated by Loader, It seems QtQuick's bug. 
            Image {
                id: cavataItem
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: 0
                y: progressBar.y+parent.conditionBarSize - sourceSize.height/2
                source: Resource.imageUrl('drive_progress_cavata.png')
                visible: parent.visualable
            }
        }
    }
}
