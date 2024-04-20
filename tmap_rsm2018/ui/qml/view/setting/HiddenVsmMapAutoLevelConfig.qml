import QtQuick 2.1
import HiddenSettingConstants 1.0
import QtQuick.Controls 1.3
import "../../component"
import "../../singleton"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


Item {
    id: rootItem
    width: 460
    height: 250
    visible: false

    signal loaded()
    signal defaultClicked()
    signal applyClicked(var data)
    signal quitClicked()

    property variant item1: undefined
    property variant item2: undefined
    property variant item3: undefined
    property variant customConfig: null
    property int viewMode: 1 // 0: far, 1:normal 2: near
    property variant viewInitConfig: {
        0: {
            low:    [11.0, 14.0],
            middle: [10.0, 22.0],
            high:   [9.0, 25.0]
        },
        1: {
            low:    [11.5, 12.0],
            middle: [10.5, 18.0],
            high:   [9.5, 23.5]
            },
        2: {
            low:    [12.0, 10.0],
            middle: [11.0, 14.0],
            high:   [10.0, 22.0]
        }
    }

    property variant viewCacheConfig: {
        0: {
            low:    [11.0, 14.0],
            middle: [10.0, 22.0],
            high:   [9.0, 25.0]
        },
        1: {
            low:    [11.5, 12.0],
            middle: [10.5, 18.0],
            high:   [9.5, 23.5]
            },
        2: {
            low:    [12.0, 10.0],
            middle: [11.0, 14.0],
            high:   [10.0, 22.0]
        }
    }

    function load() {
        lowSpeedMapLevelConfig.group1Loader.sourceComponent    = compDriveLevelMapConfig
        middleSpeedMapLevelConfig.group2Loader.sourceComponent = compDriveLevelMapConfig
        highSpeedMapLevelConfig.group3Loader.sourceComponent   = compDriveLevelMapConfig
        visible = true
    }
    function unload() {
        lowSpeedMapLevelConfig.group1Loader.sourceComponent = null
        middleSpeedMapLevelConfig.group2Loader.sourceComponent = null
        highSpeedMapLevelConfig.group3Loader.sourceComponent = null
        visible = false
    }

    function resetToDefault(viewmode) {
            var config = viewInitConfig[viewmode]
            lowSpeedMapLevelConfig.group1Loader.item.setConfig(    config["low"][0],    config["low"][1] )
            middleSpeedMapLevelConfig.group2Loader.item.setConfig( config["middle"][0], config["middle"][1])
            highSpeedMapLevelConfig.group3Loader.item.setConfig(   config["high"][0],    config["high"][1])
    }
    function setViewCacheConfig(viewmode) {
        var config = viewCacheConfig[viewMode]
        lowSpeedMapLevelConfig.group1Loader.item.setConfig(    config["low"][0],    config["low"][1] )
        middleSpeedMapLevelConfig.group2Loader.item.setConfig( config["middle"][0], config["middle"][1])
        highSpeedMapLevelConfig.group3Loader.item.setConfig(   config["high"][0],    config["high"][1])
    }


    // set config saved on DB
    function setCustomConfig(config) {
        customConfig = config
    }


    function itemLoads() {
        if (!CommonUtils.isNull(item1) &&
            !CommonUtils.isNull(item2) &&
            !CommonUtils.isNull(item3)
            ) {
                loaded()
        }
    }

    function apply() {
        var data = {}
        data[HiddenSettingConstants.VsmAutoLevelMap]          = 1
        data[HiddenSettingConstants.VsmAutoLevelLowLevel]     = lowSpeedMapLevelConfig.group1Loader.item.levelItem.value
        data[HiddenSettingConstants.VsmAutoLevelMiddleLevel]  = middleSpeedMapLevelConfig.group2Loader.item.levelItem.value
        data[HiddenSettingConstants.VsmAutoLevelHighLevel]    = highSpeedMapLevelConfig.group3Loader.item.levelItem.value
        data[HiddenSettingConstants.VsmAutoLevelLowTilt]      = lowSpeedMapLevelConfig.group1Loader.item.tiltItem.value
        data[HiddenSettingConstants.VsmAutoLevelMiddleTilt]   = middleSpeedMapLevelConfig.group2Loader.item.tiltItem.value
        data[HiddenSettingConstants.VsmAutoLevelHighTilt]     = highSpeedMapLevelConfig.group3Loader.item.tiltItem.value

        customConfig = data

        applyClicked(data)

        // cache at current view mode
        viewCacheConfig[rootItem.viewMode] = {
            low:    [lowSpeedMapLevelConfig.group1Loader.item.levelItem.value, lowSpeedMapLevelConfig.group1Loader.item.tiltItem.value ],
            middle: [middleSpeedMapLevelConfig.group2Loader.item.levelItem.value,middleSpeedMapLevelConfig.group2Loader.item.tiltItem.value],
            high:   [highSpeedMapLevelConfig.group3Loader.item.levelItem.value, highSpeedMapLevelConfig.group3Loader.item.tiltItem.value]
        }
    }

    function quit()
    {
        unload()
        quitClicked()
    }

    function showRestoreButton(show) {
          btnRestore.visible = show
    }

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: 0.5
        radius: 4.0
    }


    Column {
        x: 20
        y: 20
        spacing: 2
        width: parent.width-20
        Item {
            id: lowSpeedMapLevelConfig
            width: parent.width * 0.9
            height: 60
            property alias group1Loader: loader1
            Loader {
                id: loader1
                onLoaded: {
                    item.width = parent.width
                    item.height = parent.height
                    item.groupName  = " ~ 40km/h"
                    if (!CommonUtils.isNull(customConfig)) {
                        item.setConfig(
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelLowLevel]),
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelLowTilt]))
                    }
                    rootItem.item1 = item
                    rootItem.itemLoads()
                }
            }

            Connections {
                target: loader1.item
                onLevelValueChangedByUi: {
                    apply()
                }
                onTiltValueChangedByUi: {
                    apply()
                }
            }

        }
        Item {
            id: middleSpeedMapLevelConfig
            width: parent.width * 0.9
            height: 60
            property alias group2Loader: loader2
            Loader {
                id: loader2
                onLoaded: {
                    item.width = parent.width
                    item.height = parent.height
                    item.groupName  = "40~80km/h"
                    if (!CommonUtils.isNull(customConfig)) {
                        item.setConfig(
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelMiddleLevel]),
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelMiddleTilt]))
                    }
                    rootItem.item2 = item
                    rootItem.itemLoads()
                }
            }

            Connections {
                target: loader2.item
                onLevelValueChangedByUi: {
                    apply()
                }
                onTiltValueChangedByUi: {
                    apply()
                }
            }
        }

        Item {
            id: highSpeedMapLevelConfig
            width: parent.width * 0.9
            height: 60
            property alias group3Loader: loader3
            Loader {
                id: loader3
                onLoaded: {
                    item.width = parent.width
                    item.height = parent.height
                    item.groupName  = "80km/h ~"
                    if (!CommonUtils.isNull(customConfig)) {
                        item.setConfig(
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelHighLevel]),
                                    parseFloat(customConfig[HiddenSettingConstants.VsmAutoLevelHighTilt]))
                    }
                    rootItem.item3 = item
                    rootItem.itemLoads()
                }
            }

            Connections {
                target: loader3.item
                onLevelValueChangedByUi: {
                    apply()
                }
                onTiltValueChangedByUi: {
                    apply()
                }
            }
        }

        // restore to default
        Row {
            id: defaultOptions
            TPushButton {
                width: 150
                height: 25
                label: qsTr("초기화")
                normalBackgroundColor: GlobalConstants.kNormalColor
                pressedBackgroundColor: MultiSenseCoordinator.kAmbientColor
                onButtonClicked: {
                    resetToDefault(rootItem.viewMode)
                    apply()
                }
            }

            ExclusiveGroup { id: radiogroup }
             RadioButton {
                 id: normal
                 width: 25
                 height: 25
                 exclusiveGroup: radiogroup
                 onClicked: {
                     rootItem.viewMode = 0
                     setViewCacheConfig(0)
                     apply()
                 }
                 Component.onCompleted: {
                      checked = CommonUtils.isEqual(rootItem.viewMode, 0)
                 }
             }
             Text {
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                    color: "white"
                    text: "멀리"
             }

             // spacing
             Item {
                 width: 40
                 height: 1
             }


             RadioButton {
                 id: near
                 width: 25
                 height: 25
                 exclusiveGroup: radiogroup
                 onClicked: {
                     rootItem.viewMode = 1
                     setViewCacheConfig(1)
                     apply()
                 }

                Component.onCompleted: {
                    checked = CommonUtils.isEqual(rootItem.viewMode,  1)
                }
             }
             Text {
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                    color: "white"
                    text: "보통"
                }

             // spacing
             Item {
                 width: 40
                 height: 1
             }

             RadioButton {
                 width: 25
                 height: 25
                 exclusiveGroup: radiogroup
                 onClicked: {
                     rootItem.viewMode = 2
                     setViewCacheConfig(2)
                     apply()
                 }
                 Component.onCompleted: {
                    checked = CommonUtils.isEqual(rootItem.viewMode, 2)
                 }

             }
             Text {
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14 }
                    color: "white"
                    text: "가까히"
             }
        }

        // apply button
        // TPushButton {
        //     width: 150
        //     height: 25
        //     label: qsTr("적용하기")
        //     normalBackgroundColor: GlobalConstants.kNormalColor
        //     pressedBackgroundColor: MultiSenseCoordinator.kAmbientColor
        //     onButtonClicked: {
        //         apply()
        //     }
        // } // apply button

        // move back button
        TPushButton {
            id: btnRestore
            width: 150
            height: 25
            label: qsTr("돌아가기")
            normalBackgroundColor: GlobalConstants.kNormalColor
            pressedBackgroundColor: MultiSenseCoordinator.kAmbientColor
            onButtonClicked: {
                rootItem.quit()
            }
        }

    } // columns


    Component {
        id: compDriveLevelMapConfig
        Item {
            id: driveLevelMapConfigItem
            width: 450
            height: 60
            property string groupName  // ~ 40km/h, 40~80kmg/h  , 80km/h ~
            property var levelItem
            property var tiltItem

            signal levelValueChangedByUi(real level)
            signal tiltValueChangedByUi(real tilt)

            function setConfig(level, tilt) {
                if (!CommonUtils.isNull(levelItem))
                    levelItem.value = level
                if (!CommonUtils.isNull(tiltItem))
                    tiltItem.value = tilt
            }

            Row {
                width:parent.width
                height: 60
                spacing:1
                Text {
                    width: 120
                    height: parent.height
                    font { family: GlobalConstants. kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize; }
                    color:"#dddddd"
                    text: groupName
                }
                Text {
                    width: 40
                    height: parent.height
                    font { family: GlobalConstants. kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize;}
                    color: "#dddddd"
                    text: qsTr("레벨 ")
                }
                Item {
                    id: levelInputer
                    width: (parent.width - 230 - 5) * 0.5
                    height: parent.height
                    Loader {
                        id: levelInputLoader
                        sourceComponent: TPickerButton {}
                        onLoaded: {
                            driveLevelMapConfigItem.levelItem = item
                            item.step = 0.5
                            item.height = 50
                        }
                    }

                    Connections {
                        target: levelInputLoader.item
                        onValueChangedByUi: {
                              driveLevelMapConfigItem.levelValueChangedByUi(val)
                        }
                    }
                }
                // spacing
                Item {
                    width: 30
                    height: parent.height
                }

                Text {
                    width: 40
                    height: parent.height
                    font { family: GlobalConstants. kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize;}
                    color:"#dddddd"
                    text: "틸트 "
                }
                Item {
                    id: tiltInputer
                    width: (parent.width - 230 - 5) * 0.5
                    height: parent.height
                    Loader {
                        id: tiltInputLoader
                        sourceComponent: TPickerButton {}
                        onLoaded: {
                            driveLevelMapConfigItem.tiltItem = item
                            item.step = 1
                            item.unitLabel = "°"
                            item.height = 50
                        }
                    }

                    Connections {
                        target: tiltInputLoader.item
                        onValueChangedByUi: {
                              driveLevelMapConfigItem.tiltValueChangedByUi(val)
                        }
                    }
                }
            }
        }
    }

} // speed map level config group
