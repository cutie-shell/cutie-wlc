import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Cutie
import Cutie.CutieWlc

CutieWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Launcher")

    function addApp(data) {
        launcherApps.append(data)
    }

        GridView {
            id: launchAppGrid
            anchors.fill: parent
            anchors.topMargin: 20
            model: launcherApps
            cellWidth: width / Math.floor(width / 85)
            cellHeight: cellWidth

            property real tempContentY: 0
            property bool refreshing: false


            onAtYBeginningChanged: {
                if(atYBeginning){
                    tempContentY = contentY
                }
            }

            onContentYChanged: {
                if(atYBeginning){
                    if(Math.abs(tempContentY - contentY) > 30){
                        if(refreshing){
                            return;
                        } else {
                            refreshing = true               
                        }
                    }
                }
            }

            onMovementEnded: {
                if (refreshing) {
                    launcherApps.clear();
                    settings.loadAppList();
                    refreshing = false     
                }
            }

            delegate: Item {
                Button {
                    id: appIconButton
                    width: launchAppGrid.cellWidth
                    height: width
                    icon.name: model["Desktop Entry/Icon"]
                    icon.source: "file://" + model["Desktop Entry/Icon"]
                    icon.color: "transparent"
                    icon.height: width / 2
                    icon.width: height / 2
                    background: Rectangle {
                        color: "transparent"
                    }

                    onClicked: {
                        CutieShell.execApp(model["Desktop Entry/Exec"]);
                        launcherContainer.state = "closed"
                        if (root.state === "homeScreen")
                            wallpaperBlur.opacity = 0;
                    }
                }
                Text {
                    anchors.bottom: appIconButton.bottom
                    anchors.horizontalCenter: appIconButton.horizontalCenter
                    text: model["Desktop Entry/Name"]
                    font.pixelSize: 12
                    clip: true
                    font.family: "Lato"
                    color: Atmosphere.textColor
                    width: 2 * appIconButton.width / 3
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

    ListModel { id: launcherApps }


}