import QtQuick 2.1
import QtGraphicalEffects 1.0
import HighwayTbtViewEvent 1.0
import Csw 1.0
import "../../component"
import "../../view"

View {
    id: root
    property var listView
    property int maxListNum: 8
    property int bgWidth: 303
    property int progressWidth: 22    
    property int panelWidth: bgWidth + progressWidth    
    property int panelHeight: 86
    property color nameTextColor: 'white'
    property font nameTextFont: Qt.font({
        pixelSize: Resource.pixelToDp(29)
    });
    property int nameTextX: 12
    property int nameTextY: 13
    property int nameTextWidth: 214
    property int nameTextHeight: 26
    property font tollTextFont: Qt.font({
        pixelSize: Resource.pixelToDp(25)
    });
    property int tollTextX: 12
    property int tollTextY: 45
    property int tollTextWidth: 116
    property int tollTextHeight: 24
    property color tollTextColor: '#93c0fc'
    property string tollText: qsTr('요금') + Tr.d
    property string wonText: qsTr('원') + Tr.d

    property int popIconX: 229 + 13
    property int popIconY: 5
    property int popIconWidth: 40
    property int popIconHeight: 40
    property int arrowImageX: 150
    property int arrowImageY: 45
    property int stationX: 12
    property int stationBottomMargin: 17
    property int stationBrandIconLeftMargin: 4
    property int stationBrandIconSpacing: 5
    property int stationBrandIconWidth: 24
    property int stationBrandIconHeight: 24
    property int stationOilIconWidth: 20
    property int stationOilIconHeight: 20
    property int stationOilIconLeftMargin: 5
    property font stationPriceTextFont: Qt.font({
        pixelSize: Resource.pixelToDp(25)
    });
    property int stationOilPriceTextWidth: 48
    property int stationOilPriceTextHeight: 24
    property int arrowIconWidth: 40
    property int arrowIconHeight: 24
    property int distanceY: 45
    property int distanceWidth: 134
    property int distanceHeight: 24
    property int distanceRightMargin: progressWidth + 24
    property int listTopMargin: 26
    property int textVCenterOffset: 2

    width: panelWidth
    height: parent ? parent.height : undefined
    hasFocus: false
    visible: false
    property bool isBuildTarget: false
    property var csws: [destPanelCsw]
    property bool listWrapVisible: false
    property int cswFocusTopMargin: -3
    property int cswFocusRightMargin: 25
    property bool onlyHasFinalTbt: false

    property int buttonProgressHeight: 112
    property int buttonProgressMoreHeight: 112
    property int buttonProgressEndHeight: 47
    property int buttonProgressHeightOnFinalTbt: 64

    property string evFastChargerIconPath: Resource.imageUrl( 'highway', 'highway_evstation_fast.png' )
    property string evSlowChargerIconPath: Resource.imageUrl( 'highway', 'highway_evstation_slow.png' )

    property variant lastListWrapVisibleByUser: undefined

    property string onlineOrOffline: "offline"
    property string dayOrNight: "day"    

    ListModel {
        id: buttonModel
        ListElement {
            type: 'button'
            noPrice: false
            noData: false
            name: ''
            popIcon: ''
            toll: ''
            distanceLength: 0
            distance: [
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                },
                ListElement {
                    img: ''
                }
            ]
            station: ''
            oilAndPriceLength: 0
            oilAndPrice: [
                //[TMCRSM-207 leejaeho] 초기설정을 '정보없음'으로 표시
                ListElement {
                    icon: ''
                    price: qsTr('정보없음')
                },
                ListElement {
                    icon: ''
                    price: ''
                },
                ListElement {
                    icon: ''
                    price: ''
                }
            ]
            isEvRapidCharger:false
            isEvSlowCharger:false
            progress: 'tbt_progress_null'
        }
    }

    ListModel {
        id: listModel
    }

    property var lastButtonModel
    property var lastDistanceImages
    property var lastListModel

    function reset() {
        visible = false
        lastButtonModel = null
        lastDistanceImages = null
        lastListModel = null
    }

    function setButtonModel( model ) {
        visible = true
        //console.debug( "item: " + contentLoader.item + " #")
        lastButtonModel = model
        if ( contentLoader.item )
            contentLoader.item.setButtonModel( model )
    }

    function setButtonModelDistance( distanceImages ) {
        visible = true
        //console.debug( "item: " + contentLoader.item + " #")
        lastDistanceImages = distanceImages
        if ( contentLoader.item )
            contentLoader.item.setButtonModelDistance( distanceImages )
    }

    function setListModel( models ) {
        visible = true
        //console.debug( "item: " + contentLoader.item + " #")
        onlyHasFinalTbt = models.length === 0
        lastListModel = models
        if ( contentLoader.item )
            contentLoader.item.setListModel( models )
    }

    function setListModelDistance( index, distanceImages ) {        
        visible = true
        //console.debug( "item: " + contentLoader.item + " #")
        if ( contentLoader.item )
            contentLoader.item.setListModelDistance( index, distanceImages )
    }

    function retranslateUi() {
        evFastChargerIconPath = Resource.imageUrl( 'highway', 'highway_evstation_fast.png' )
        evSlowChargerIconPath = Resource.imageUrl( 'highway', 'highway_evstation_slow.png' )
    }

    Image {
        id: cswFocusMark
        source: Resource.imageUrl( 'highway', 'map_highway_tbt_csw_f.png' )
        visible: false
        z: 99
        anchors.top: parent.top
        anchors.topMargin: cswFocusTopMargin
        anchors.right: parent.right
        anchors.rightMargin: cswFocusRightMargin
    }

    Csw {
        id: destPanelCsw
        objectName: 'destPanelCsw'
        section: Csw.Section_Map
        sequence: Csw.Sequence_HightwayTbtDestPanel
        deletable: false
        enable: root.visible

        onFocused: {
            console.log('[CSW][HighwayTbtView]onFocused')
            cswFocusMark.visible = true
        }
        onLostFocusd: {
            console.log('[CSW][HighwayTbtView]HighwayTbtView onLostFocusd')
            cswFocusMark.visible = false
        }
        onSelected: {
            console.log('[CSW][HighwayTbtView]onSelected')

            //listWrap.visible = !listWrap.visible
            listWrapVisible = !listWrapVisible
            lastListWrapVisibleByUser = listWrapVisible
        }
    }

    //5초 후에는 스크롤 위치 자동 원복
    Timer {
        id: scrollTimer
        interval: 0
        running: false
        repeat: false
        onTriggered: {
            if (root.listView) {
                root.listView.positionViewAtEnd()
            }
        }
    }
    function scrollTouched() {
        scrollTimer.stop()
        scrollTimer.interval = 5000
        scrollTimer.start()
    }

    onMediated: {
        console.log('[HighwayTbtView]lastListWrapVisibleByUser: ' + lastListWrapVisibleByUser)
        sendEvent( HighwayTbtViewEvent.Mediated )
        sendEvent(HighwayTbtViewEvent.RegistCsw, csws)
    }

    onVisibleChanged: {
        sendEvent( HighwayTbtViewEvent.VisibleChanged, {visible: visible} )
    }

    Loader {
        id: contentLoader
        visible: parent.visible
        sourceComponent: contentLoader.visible ? contentComp : null
        onLoaded: {
            if ( lastButtonModel ) item.setButtonModel(lastButtonModel)
            if ( lastListModel ) item.setListModel(lastListModel)
        }
    }

    Component {
        id: contentComp
        Item {
            id: contentWrap
            anchors.fill: parent

            function reset() {
                visible = false
                buttonWrap.visible = false
               // listWrap.visible = false
                listWrapVisible = false

                var buttonModelSetProperty = buttonModel.setProperty
                var buttonModelGet = buttonModel.get

                buttonModelSetProperty( 0, 'noPrice', false )
                buttonModelSetProperty( 0, 'noData', false )
                buttonModelSetProperty( 0, 'name', '' )
                buttonModelSetProperty( 0, 'popIcon', '' )
                buttonModelSetProperty( 0, 'toll', '' )

                buttonModelSetProperty( 0, 'distanceLength', 0 )
                for ( var i = 0; i < 5; i++ ) {
                    buttonModelGet( 0 ).distance.setProperty( i, 'img', '' )
                }

                buttonModelSetProperty( 0, 'station', '' )
                buttonModelSetProperty( 0, 'oilAndPriceLength', 0 )

                for ( i = 0; i < 3; i++ ) {
                    buttonModelGet( 0 ).oilAndPrice.setProperty( i, 'icon', '' )
                    //[TMCRSM-207 leejaeho] 초기설정을 '정보없음'으로 표시
                    buttonModelGet( 0 ).oilAndPrice.setProperty( i, 'price', qsTr('정보없음') )
                }

                buttonModelSetProperty( 0, 'isEvRapidCharger', false )
                buttonModelSetProperty( 0, 'isEvSlowCharger', false )

                buttonModelSetProperty( 0, 'progress', 'tbt_progress_null' )

                listModel.clear()
            }

            function setButtonModel( model ) {
                buttonWrap.visible = true
                var buttonModelSetProperty = buttonModel.setProperty
                var buttonModelGet = buttonModel.get

                buttonModelSetProperty( 0, 'noPrice', false )
                buttonModelSetProperty( 0, 'noData', false )
                buttonModelSetProperty( 0, 'name', model.name )
                buttonModelSetProperty( 0, 'popIcon', model.popIcon )
                buttonModelSetProperty( 0, 'toll', model.toll )
                buttonModelSetProperty( 0, 'distanceLength', model.distanceLength )

                if ( buttonModelGet( 0 ).distance.count !== model.distanceLength ) {
                    buttonModelGet( 0 ).distance.clear()
                    for ( var i = 0; i < model.distanceLength; i++ ) {
                        buttonModelGet( 0 ).distance.append( {"img": model.distance[i]} )
                    }
                }
                else {
                    for ( i = 0; i < model.distanceLength; i++ ) {
                        buttonModelGet( 0 ).distance.setProperty( i, 'img', model.distance[i] )
                    }
                }

                buttonModelSetProperty( 0, 'station', model.station )
                buttonModelSetProperty( 0, 'oilAndPriceLength', model.oilAndPriceLength )

                if ( buttonModelGet( 0 ).oilAndPrice.count !== model.oilAndPriceLength ) {
                    buttonModelGet( 0 ).oilAndPrice.clear()
                    for ( i = 0; i < model.oilAndPriceLength; i++ ) {
                        buttonModelGet( 0 ).oilAndPrice.append( {'icon': model.oilAndPrice[i].icon,'price': model.oilAndPrice[i].price} )
                    }
                }
                else {
                    for ( i = 0; i < model.oilAndPriceLength; i++ ) {
                        buttonModelGet( 0 ).oilAndPrice.setProperty( i, 'icon', model.oilAndPrice[i].icon )
                        buttonModelGet( 0 ).oilAndPrice.setProperty( i, 'price', model.oilAndPrice[i].price )
                    }
                }

                buttonModelSetProperty( 0, 'isEvRapidCharger', false )
                buttonModelSetProperty( 0, 'isEvSlowCharger', false )

                buttonModelSetProperty( 0, 'progress', model.progress )
            }

            function setButtonModelDistance( distanceImages ) {
                var buttonModelGet = buttonModel.get
                buttonModel.setProperty( 0, 'distanceLength', distanceImages.length )

                if ( buttonModelGet( 0 ).distance.count != distanceImages.length ) {
                    buttonModelGet( 0 ).distance.clear()
                    for ( var i = 0; i < distanceImages.length; i++ ) {
                        buttonModelGet( 0 ).distance.append( {"img": distanceImages[i]} )
                    }
                }
                else {
                    for ( var i = 0; i < distanceImages.length; i++ ) {
                        buttonModelGet( 0 ).distance.setProperty( i, 'img', distanceImages[i] )
                    }
                }
            }

            function setListModel( models ) {
                console.log('[HighwayTbtView][setListModel]lastListWrapVisibleByUser: ' + lastListWrapVisibleByUser)
                if (lastListWrapVisibleByUser !== undefined) {
                    listWrapVisible = lastListWrapVisibleByUser
                }
                else {
                    listWrapVisible = true
                }

                listWrap.listModelCount = models.length

                //console.log( "listModelCount: " + listWrap.listModelCount )
                if (listWrap.listModelCount == 0) {
                    console.log( "[HighwayTbtView]listModelCount: " + listWrap.listModelCount )
                    //listWrapVisible = false
                }

                if ( models.length != listModel.count ) {
                    listModel.clear()
                    var listModelAppend = listModel.append
                    for ( var i = 0; i < models.length; i++ ) {
                        listModelAppend( {
                            'type': 'list',
                            'noPrice': false,
                            'noData': false,
                            'name': '',
                            'popIcon': '',
                            'toll': '',
                            'distanceLength': 0,
                            'distance': [{img:''},{img:''},{img:''},{img:''},{img:''}],
                            'station': '',
                            'oilAndPriceLength': 0,
                            //[TMCRSM-207 leejaeho] 초기설정을 '정보없음'으로 표시
                            'oilAndPrice': [{icon:'',price:qsTr('정보없음')},{icon:'',price:''},{icon:'',price:''}],
                            'isEvRapidCharger': false,
                            'isEvSlowCharger': false,
                            'progress': ''
                        })
                    }
                }

                var listModelSetProperty = listModel.setProperty
                var listModelGet = listModel.get

                for ( var i = 0; i < models.length; i++ ) {
                    var model = models[i]
                    var index = listModel.count - i - 1;
                    listModelSetProperty( index, 'noPrice', model.noPrice )
                    listModelSetProperty( index, 'noData', model.noData )
                    listModelSetProperty( index, 'name', model.name )
                    listModelSetProperty( index, 'popIcon', model.popIcon )
                    listModelSetProperty( index, 'toll', model.toll )
                    listModelSetProperty( index, 'distanceLength', model.distanceLength )

                    if ( listModelGet( index ).distance.count !== model.distanceLength ) {
                        listModelGet( index ).distance.clear()
                        for ( var j = 0; j < model.distanceLength; j++ ) {
                            listModelGet( index ).distance.append( {'img': model.distance[j]} )
                        }
                    }
                    else {
                        for ( j = 0; j < model.distanceLength; j++ ) {
                            listModelGet( index ).distance.setProperty( j, 'img', model.distance[j] )
                        }
                    }

                    listModelSetProperty( index, 'station', model.station )
                    listModelSetProperty( index, 'oilAndPriceLength', model.oilAndPriceLength )
                    //QmlAppHelper.debug( 'oilAndPriceLength: ' + model.oilAndPriceLength )

                    if ( listModelGet( index ).oilAndPrice.count !== model.oilAndPriceLength ) {
                        listModelGet( index ).oilAndPrice.clear()
                        for ( j = 0; j < model.oilAndPriceLength; j++ ) {
                            listModelGet( index ).oilAndPrice.append( {'icon': model.oilAndPrice[j].icon,'price': model.oilAndPrice[j].price} )
                        }
                    }
                    else {
                        for ( j = 0; j < model.oilAndPriceLength; j++ ) {
                            listModelGet( index ).oilAndPrice.setProperty( j, 'icon', model.oilAndPrice[j].icon )
                            listModelGet( index ).oilAndPrice.setProperty( j, 'price', model.oilAndPrice[j].price )
                            //QmlAppHelper.debug( '[' + index + '][' + j + ']' + 'icon: ' + model.oilAndPrice[j].icon + ' price: ' + model.oilAndPrice[j].price )
                        }
                    }

                    listModelSetProperty( index, 'isEvRapidCharger', model.isEvRapidCharger )
                    listModelSetProperty( index, 'isEvSlowCharger', model.isEvSlowCharger )

                    listModel.setProperty( index, 'progress', model.progress )
                }

                listView.positionViewAtEnd()
            }

            function setListModelDistance( index, distanceImages ) {                                
                var index2 = listModel.count - index - 1;
                var listModelGet = listModel.get
                listModel.setProperty( index2, 'distanceLength', distanceImages.length )
                var name = listModelGet( index2 ).name

                if ( listModelGet( index2 ).distance.count != distanceImages.length ) {
                    listModelGet( index2 ).distance.clear()
                    for ( var i = 0; i < distanceImages.length; i++ ) {
                        listModelGet( index2 ).distance.append( {"img": distanceImages[i]} )
                    }
                }
                else {
                    for ( var i = 0; i < distanceImages.length; i++ ) {
                        listModelGet( index2 ).distance.setProperty( i, 'img', distanceImages[i] )
                    }
                }
            }

            Component.onCompleted: {
                listView.positionViewAtEnd()
            }

            Component {
                id: panelComp
                Item {
                    id: panel
                    width: panelWidth
                    height: panelHeight
                    z: index

                    Image {
                        id: bgImg
                        source: type == 'button' ? Resource.imageUrl( 'highway', 'highway_pop_bg1.png' ) : Resource.imageUrl( 'highway', 'highway_pop_bg2.png' )
                    }

                    MarqueeText {
                        id: marqueeText
                        x: nameTextX
                        y: nameTextY
                        width: nameTextWidth
                        font: nameTextFont
                        textColor: nameTextColor
                        text: name

                        layer.enabled: scrolling
                        layer.samplerName: "textSource"
                        layer.effect: ShaderEffect {
                            property var colorSource: gradientRect;
                            fragmentShader: "
                                        uniform lowp sampler2D colorSource;
                                        uniform lowp sampler2D textSource;
                                        uniform lowp float qt_Opacity;
                                        varying highp vec2 qt_TexCoord0;
                                        void main() {
                                            gl_FragColor =
                                                texture2D(colorSource, qt_TexCoord0).a
                                                * texture2D(textSource, qt_TexCoord0)
                                                * qt_Opacity;
                                        }
                                    "
                        }

                        LinearGradient {
                            id: gradientRect
                            anchors.fill: marqueeText
                            gradient: Gradient {
                                GradientStop { position: 0; color: Qt.rgba(0,0,0,0) }
                                GradientStop { position: 16/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                                GradientStop { position: (gradientRect.width-16)/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                                GradientStop { position: 1; color: Qt.rgba(0,0,0,0) }
                            }
                            start: Qt.point(0, 0)
                            end: Qt.point(gradientRect.width, 0)
                            visible: false
                            layer.enabled: true
                            layer.smooth: true
                        }
                    }

                    Image {
                        id: popIconImg
                        x: popIconX
                        y: popIconY
                        source: popIcon ? Resource.imageUrl( 'highway', popIcon ) : ''
                    }
                    Item {
                        id: tollWrap
                        x: tollTextX
                        y: tollTextY
                        width: tollTextWidth
                        height: tollTextHeight
                        visible: toll ? true : false
                        Text {
                            y: textVCenterOffset
                            font: tollTextFont
                            color: tollTextColor
                            text: toll ? root.tollText + ' ' + toll + root.wonText : ''
                        }
                    }
                    Item {
                        id: distanceWrap
                        y: distanceY
                        anchors.right: parent.right
                        anchors.rightMargin: distanceRightMargin
                        width: distanceRow.width
                        height: distanceHeight
                        Row {
                            id: distanceRow
                            anchors.right: parent.right
                            width: childrenRect.width
                            Repeater {
                                id: distanceNumberRepeater
                                model: distance
                                delegate: Image {
                                    id: distanceNumberImage
                                    visible: index < distanceLength ? true : false
                                    source: img ? Resource.imageUrl( 'highway', img ) : ''
                                }
                                onModelChanged: {
                                    //console.log( '[3624]' + name + ' ' + distanceLength + ' ' + model.length)
                                }
                            }
                        }
                    }

                    //Rectangle {
                    Item {
                        id: evChargerWrap
                        //color: 'yellow'
                        anchors.left: parent.left
                        anchors.leftMargin: stationX
                        anchors.right: distanceWrap.left
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: stationBottomMargin
                        height: stationBrandIconHeight
                        visible: (isEvRapidCharger | isEvSlowCharger)

                        Image {
                            id: evChargerIcon
                            anchors.bottom: parent.bottom
                            source: Resource.imageUrl( 'highway', 'highway_evstation.png' )
                        }

                        Image {
                            id: evFastChargerIcon
                            anchors.left: evChargerIcon.right
                            anchors.leftMargin: 8
                            anchors.bottom: parent.bottom
                            source: root.evFastChargerIconPath
                            visible: isEvRapidCharger
                        }

                        Image {
                            id: evSlowChargerIcon
                            anchors.left: evFastChargerIcon.right
                            anchors.leftMargin: 4
                            anchors.bottom: parent.bottom
                            source: root.evSlowChargerIconPath
                            visible: isEvSlowCharger
                        }
                    }

                    Item {
                    //Rectangle {
                        //color: 'blue'
                        id: stationWrap
                        anchors.left: parent.left
                        anchors.leftMargin: stationX
                        anchors.right: distanceWrap.left
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: stationBottomMargin
                        height: stationBrandIconHeight
                        visible: !(evChargerWrap.visible || noData) && station

                        Image {
                            id: stationBrandIcon
                            anchors.bottom: parent.bottom
                            source: station ? Resource.imageUrl( 'highway', station ) : ''
                        }

                        // 18.4.4 hskim TMCRSM-542
                        Item {
                        //Rectangle {
                            //color: 'red'
                            id: oilAndPriceNoDataWrap
                            anchors.left: stationBrandIcon.right
                            anchors.leftMargin: stationBrandIconLeftMargin
                            anchors.right: parent.right
                            height: stationBrandIconHeight
                            visible: noPrice ? true : false

                            Repeater {
                                model: oilAndPrice
                                Item {
                                //Rectangle {
                                    //color: 'red'
                                    id: stationOilAndPriceWrap
                                    width: childrenRect.width
                                    height: stationBrandIconHeight
                                    visible: true
                                    Image {
                                        id: stationOilIcon
                                        anchors.verticalCenter: parent.verticalCenter
                                        source: icon ? Resource.imageUrl( 'highway', icon ) : ''
                                    }
                                    Text {
                                        anchors.left: stationOilIcon.right
                                        anchors.leftMargin: stationOilIconLeftMargin
                                        height: stationBrandIconHeight + textVCenterOffset
                                        verticalAlignment: Text.AlignVCenter
                                        font: stationPriceTextFont
                                        color: nameTextColor
                                        text:  index == 0 ? qsTr('정보없음') + Tr.d : ''
                                    }
                                }
                            }
                        }

                        Row {
                            anchors.left: stationBrandIcon.right
                            anchors.leftMargin: stationBrandIconLeftMargin
                            spacing: stationBrandIconSpacing
                            visible: noPrice ? false : true
                            Repeater {
                                model: oilAndPrice
                                Item {
                                //Rectangle {
                                    //color: 'red'
                                    id: stationOilAndPriceWrap
                                    width: childrenRect.width
                                    height: stationBrandIconHeight
                                    //[TMCRSM-207 leejaeho] 비통신 상황 및 정보를 받아올 수 없는 경우에도 화면 표시
                                    visible: true//index < oilAndPriceLength ? true : false
                                    Image {
                                        id: stationOilIcon
                                        anchors.verticalCenter: parent.verticalCenter
                                        source: icon ? Resource.imageUrl( 'highway', icon ) : ''
                                    }
                                    Text {
                                        anchors.left: stationOilIcon.right
                                        anchors.leftMargin: stationOilIconLeftMargin
                                        height: stationBrandIconHeight + textVCenterOffset
                                        verticalAlignment: Text.AlignVCenter
                                        font: stationPriceTextFont
                                        color: nameTextColor
                                        text:  price
                                    }
                                }
                            }
                        }
                    }
                    Item {
                        id: progressWrap
                        width: progressWidth
                        height: parent.height
                        anchors.left: bgImg.right

                        Image {
                            id: barImg
                            anchors.top: dotImg.top
                            anchors.topMargin: 11
                            property bool isEnd: type == 'button' ? false : index == listView.count - 1
                            visible: listWrapVisible
                            height: {
                                var ret = buttonProgressHeight
                                if ( type == 'button' ) {
                                    if ( root.onlyHasFinalTbt ) {
                                        ret = buttonProgressHeightOnFinalTbt
                                    }
                                }
                                else {
                                    if ( isEnd ) {
                                        ret = buttonProgressEndHeight
                                    }
                                    else {
                                        //[ljaeho] tbt스크롤시 dotImg를 덮는문제 발생하여 높이 지정함.
                                        ret = panelHeight-7
                                    }
                                }

                                return ret;
                            }

                            fillMode: Image.TileVertically
                            source: {
                                var ret = ''
                                if ( type == 'button' ) {
                                    if ( listView.more ) {
                                        ret = Resource.imageUrl( 'highway', 'highway_progress_more.png' )
                                    }
                                    else {
                                        ret = model.progress ? Resource.imageUrl( 'tbt', onlineOrOffline, dayOrNight, model.progress + '.png' ) : ''
                                    }
                                    if ( root.onlyHasFinalTbt && root.listWrapVisible ) {
                                        ret = model.progress ? Resource.imageUrl( 'highway', onlineOrOffline, dayOrNight, model.progress + '_highway_end.png' ) : ''
                                    }
                                }
                                else {
                                    if ( isEnd ) {
                                        ret = model.progress ? Resource.imageUrl( 'tbt', onlineOrOffline, dayOrNight, model.progress + '_end.png' ) : ''
                                    }
                                    else {
                                        ret = model.progress ? Resource.imageUrl( 'tbt', onlineOrOffline, dayOrNight, model.progress + '.png' ) : ''
                                    }
                                }

                                return ret
                            }
                        }

                        Image {
                            id: dotImg
                            y: 28
                            source: Resource.imageUrl( 'tbt', 'tbt_progress_dot.png' )
                            z: 1
                        }
                    }
                }
            }

            Item {
                id: buttonWrap
                width: panelWidth
                height: panelHeight
                visible: false
                ListView {
                    id: buttonView
                    anchors.fill: parent
                    model: buttonModel
                    interactive: false
                    boundsBehavior: Flickable.StopAtBounds
                    delegate: panelComp
                }
                TMouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //listWrap.visible = !listWrap.visible
                        listWrapVisible = !listWrapVisible
                        lastListWrapVisibleByUser = listWrapVisible
                        destPanelCsw.forceLostFocus()
                    }
                }
                Component.onCompleted: {
                }
                Component.onDestruction: {
                }
            }

            Item {
                id: arrowWrap
                anchors.top: buttonWrap.bottom
                width: bgWidth
                height: 26
                Image {
                    id: arrowImage
                    width: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: -5
                    property string arrowIcon: listWrapVisible ? 'highway_arw_up.png' : 'highway_arw_down.png'
                    source: arrowIcon ? Resource.imageUrl( 'highway', arrowIcon ) : ''
                    TMouseArea {
                        anchors.fill: parent
                        onClicked: {
                            listWrapVisible = !listWrapVisible
                            lastListWrapVisibleByUser = listWrapVisible
                        }
                    }
                }
            }

            Item {
                id: listWrap
                anchors.top: buttonWrap.bottom
                anchors.topMargin: listTopMargin
                width: panelWidth
                property int listModelCount: 0
                height: panelHeight * (listModelCount > 4 ? 4 : listModelCount)
                z: 1

                visible: listWrapVisible// false

                onVisibleChanged: {
                    sendEvent(HighwayTbtViewEvent.ListVisibleChanged, visible)
                }

                ListView {
                    id: listView
                    anchors.fill: parent
                    model: listModel
                    interactive: true
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds
                    cacheBuffer: panelHeight * 3
                    delegate: panelComp

                    property bool more: false

                    function updateMore() {
                        var stillMore = false
                        if ( count > 1 ) {
                            if ( contentY >= 0 ) {
                                var firstVisibleIdx = indexAt( 1, contentY )
                                if ( firstVisibleIdx > 0 ) {
                                    stillMore = true
                                }
                            }
                        }
                        more = stillMore
                    }

                    Component.onCompleted: {
                        root.listView = listView
                        positionViewAtEnd()
                    }

                    onContentYChanged: {
                        updateMore()
                    }

                    onMovementEnded: {
                        scrollTouched()
                    }
                }
            }
        }
    }
}
