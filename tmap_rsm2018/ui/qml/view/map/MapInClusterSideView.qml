import QtQuick 2.1
import Constants 1.0
import SceneGraphRendering 1.0
import MapInClusterSideViewEvent 1.0
import ViewRegistry 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: viewRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property variant parentWindow

    property bool isNaviOrAndo: true
    property bool isOnline: false
    property bool showTrafficInfo: true // from controller
    property bool enableTrafficInfo: false // when control mode
    property bool showTrafficInfoOnMap: false
    property bool showTrafficInfoOnRouteLine: false
    property bool showTrafficInfoOnRouteLineForce: false // 경로 요약 화면, 경로 상세 화면에서 설정
    property bool isUnderControl: false

    Component.onCompleted: {
    }

    Component.onDestruction: {             
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {
        viewRoot.sendEvent(MapInClusterSideViewEvent.Mediated, {renderer: renderer, layout: layout})
    }

    onActivated: {
    }

    Item {
        id: rendererWrap
        anchors.fill: parent

        onWidthChanged: {
        }

        Renderer {
            id: renderer
            objectName: "renderer"
            anchors {
                fill: parent
            }

            moveMode: isUnderControl ? Renderer.Normal : Renderer.Navigation
            viewMode: Renderer.BirdView
            downloadRouteTile: false
            theme: Renderer.ThemeCluster
            clusterPoi: true
            showLandmark: false
            showBuilding: false
            skyState: Renderer.SkyCluster
            trafficInfo: (enableTrafficInfo || (showTrafficInfoOnMap && isNaviOrAndo)) && isOnline
            trafficInfoOnRoute: (enableTrafficInfo || showTrafficInfoOnRouteLine || showTrafficInfoOnRouteLineForce) && isOnline
            showRouteObject: Renderer.RouteObjectDefault & ~Renderer.RouteObjectMainRoadName

            Component.onCompleted: {
            }

            Component.onDestruction: {
                viewRoot.sendEvent( MapInClusterSideViewEvent.RendererOnDestruction, renderer )
            }
		}
    }
}

