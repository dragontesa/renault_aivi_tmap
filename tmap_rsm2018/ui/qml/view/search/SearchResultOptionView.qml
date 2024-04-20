import QtQuick 2.1
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import SettingConstants 1.0
import SearchResultViewEvent 1.0
import SearchResultOptionViewEvent 1.0
import SearchResultDetailViewEvent 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils



PopupView {
    id: searchRoot

    property var caller         : null
    property var owner          : null
    property var relay          : null
    property string searchType  : CommonConstants.Empty

    //언어 관련
    property var __lang         : null
    property bool needInit      : true


    //ID
    property var region         : null

    //Enum
    property var menuEnum       : ({})

    property var menuArr        : ([])


    function allocateMenuEnum(){
        if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
        }
        var i = 0
        CommonUtils.insertEnum(menuEnum , "Region"    , i++, [CommonConstants.Name] , [qsTr("지역 설정")])

        CommonUtils.fetchArr(menuEnum , CommonConstants.Name , menuArr)
    }


    function init(){
        if (!needInit)
            return
        needInit = false

        allocateMenuEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main
    }

    function closeView() {
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultOptionView)
        sendEvent(SearchResultOptionViewEvent.CloseView, data)
    }

    onMediated: {
        sendEvent(SearchResultOptionViewEvent.RequestLang)
        init()
    }

    onActivated: {
         init()
         sendEvent(SearchResultOptionViewEvent.ResetCsw)
         sendEvent(SearchResultOptionViewEvent.ModalChanged, {modal: true})
    }

    onDeactivated: {
        sendEvent(SearchResultOptionViewEvent.ModalChanged, {modal: false})
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        searchRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }


      //main
      Loader {
           id: loader
           width: parent.width
           height: parent.height
      }

      Component {
          id: main
          Item {
              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
                  width = parent.width
                  height = parent.height
              }
              MouseArea {
                   width: parent.width
                   height: parent.height
                   onPressed: {
                       closeView()
                   }
              }
              Rectangle {
                  x: parent.width - (width+5)
                  y: parent.height - (height+5)
                  width: 200
                  height: 30 + 10
                  radius: 15
                  color: CommonConstants.White
                  Column {
                      width: parent.width - 10
                      height: parent.height - 10
                      anchors.centerIn: parent 
                      TPushButton {
                          width: parent.width
                          height: 30
                          txt.font.family: GlobalConstants.kFontFamily
                          txt.font.pixelSize: Resource.pixelToDp(GlobalConstants.kListSubTextFontSize)
                          label: menuArr[menuEnum.Region]
                          enabled: true
                          Component.onCompleted: { searchRoot.region = this
                              frame.colorN = Qt.binding(function(){ return CommonConstants.White})
                              frame.colorP = Qt.binding(function(){ return CommonConstants.White})
                              txt.colorN = Qt.binding(function(){ return enabled ? CommonConstants.Black : CommonConstants.Gray})
                              txt.colorP = Qt.binding(function(){ return enabled ? CommonConstants.Blue : CommonConstants.Gray})
                          }
                          onButtonClicked:  {
                              if (!enabled)
                                  return

                              var data = ({})
                              data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.RegionSearchView)
                              data[CommonConstants.Caller     ] = caller
                              data[CommonConstants.Relay      ] = owner
                              data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.SearchResultOptionView)
                              data[SearchConstants.SearchType ] = searchType
                              sendEvent(SearchResultOptionViewEvent.CreateView, data)

                              closeView()
                          }
                      }
                  }
              }
          }
      }

}
