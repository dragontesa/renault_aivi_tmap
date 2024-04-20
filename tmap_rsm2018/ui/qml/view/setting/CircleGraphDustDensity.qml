import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

Image {
  property var layout: ({
      t:{margin:0,width:0,height:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
      m:{margin:0,width:0,height:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
      b:{margin:0,width:0,height:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
  })
  width:parent.width
  height:parent.height
  source: Resource.imageUrl(layout.t.image)
  Column {
      width:parent.width
      height:parent.height
      Item {width:parent.width;height:layout.t.margin}
      Item {width:parent.width;height:layout.t.height
          Text {
              text: layout.t.text
              color: GlobalConstants.kListTextColor
              anchors.verticalCenter: parent.verticalCenter
              anchors.horizontalCenter: parent.horizontalCenter
              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(20)}
          }
      }
      Item {width:parent.width;height:layout.m.margin}
      Image {
          width:parent.width
          height: layout.m.height
          source: Resource.imageUrl(layout.m.image)
      }
      Item {width:parent.width;height:layout.b.margin}
      Item {width:parent.width;height:layout.b.height
          Loader {
               width: parent.width
               height:parent.height
               Component.onCompleted: {
                   sourceComponent = CommonUtils.isNull(layout.b.text) ?  bottomFail : bottomSuccess
               }
          }
      }
  }

  Component {
      id: bottomSuccess
      Item {
          onParentChanged: {
              if (CommonUtils.isNull(parent))
                  return
              width = parent.width
              height = parent.height
          }

          Row {
              width:parent.width
              height:parent.height
              anchors.centerIn: parent
              Component.onCompleted: CommonUtils.applyWidth(this)
              visible: CommonUtils.isNull(layout.b.text) ? false : true
              Repeater {
                  model: CommonUtils.isNull(layout.b.text) ? 0: layout.b.text.length
                  delegate:
                      Image {
                          width:20
                          height: parent.height
                          source: CommonUtils.isNull(layout.b.text) ? CommonConstants.Empty : Resource.imageUrl("setting/wid_aqi_num_"+layout.b.text[index]+".png")
                      }
              }
              Item {width:4;height:parent.height}
              Image {
                  width: 48
                  source:  Resource.imageUrl(layout.b.image)
                  anchors.bottom: parent.bottom
              }
          }
      }
  }

  Component {
      id: bottomFail
      Item {
          onParentChanged: {
              if (CommonUtils.isNull(parent))
                  return
              width = parent.width
              height = parent.height
          }
          Image {
            source: Resource.imageUrl("setting/wid_aqi_num_error.png")
            anchors.centerIn: parent
          }
      }
  }

}


