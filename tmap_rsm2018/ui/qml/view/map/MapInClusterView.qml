import QtQuick 2.1
import Constants 1.0
import SceneGraphRendering 1.0
import MapInClusterViewEvent 1.0
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
    property bool rendererPause: false

    Component.onCompleted: {
    }

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {        
        viewRoot.sendEvent( MapInClusterViewEvent.Mediated );
    }

    onActivated: {
    }

    function createRenderer( pause ) {
        viewRoot.rendererPause = pause
        rendererLoader.sourceComponent = rendererComp
    }

    Loader {
        id: rendererLoader
        anchors.fill: parent
    }

    Component {
        id: rendererComp
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
                //trafficInfo: (enableTrafficInfo || (showTrafficInfoOnMap && isNaviOrAndo)) && isOnline
                trafficInfo: false
                trafficInfoOnRoute: (enableTrafficInfo || showTrafficInfoOnRouteLine || showTrafficInfoOnRouteLineForce) && isOnline
                showRouteObject: Renderer.RouteObjectDefault & ~Renderer.RouteObjectMainRoadName

                Component.onCompleted: {
                    console.log( "[Cluster][Renderer]onCompleted #1" );
                    if ( viewRoot.rendererPause ) {
                        console.log( "[Cluster][Renderer]onCompleted #2" );
                        renderer.visible = false
                        renderer.onPause()
                    }

                    viewRoot.sendEvent( MapInClusterViewEvent.RendererCreated, {renderer: renderer, layout: layout} );
                }

                Component.onDestruction: {
                    viewRoot.sendEvent( MapInClusterViewEvent.RendererOnDestruction, renderer )
                }

                onViewLevelChanged: {
                    console.log( "[Cluster]viewLevel: " + viewLevel)
                }
            }
        }
    }
}

