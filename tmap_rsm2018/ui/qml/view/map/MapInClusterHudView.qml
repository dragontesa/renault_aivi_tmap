import QtQuick 2.1
import Constants 1.0
import SceneGraphRendering 1.0
import MapInClusterHudViewEvent 1.0
import ViewRegistry 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: viewRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property int mapMoveMode: Renderer.Navigation
    property variant parentWindow

    Component.onCompleted: {
    }

    Component.onDestruction: {        
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {
        viewRoot.sendEvent(MapInClusterHudViewEvent.Mediated, {renderer: renderer})
    }

    onActivated: {
    }

    Item {
        id: rendererWrap
        anchors.fill: parent

        Renderer {
            id: renderer
            objectName: "renderer"
            anchors {
                fill: parent
            }

            moveMode: viewRoot.mapMoveMode
            viewMode: Renderer.HeadUp
            downloadRouteTile: false
            theme: Renderer.ThemeCluster
            clusterPoi: true
            showBuilding: false
            //showRoute: false
            skyState: Renderer.SkyCluster
            positionIcon: Renderer.PositionIconNone
            showRoute: true
            showRouteObject: Renderer.RouteObjectRouteLine | Renderer.RouteObjectRouteLineTBT

            Component.onCompleted: {                
                renderer.setViewLevel(11,false)
            }

            Component.onDestruction: {                
                viewRoot.sendEvent( MapInClusterHudViewEvent.RendererOnDestruction, renderer )
            }
		}
    }
}

