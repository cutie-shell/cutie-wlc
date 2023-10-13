import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import Cutie

Rectangle {
    id: settingSheet
    width: Screen.width
    height: Screen.height
    color: Atmosphere.secondaryAlphaColor
    opacity: 0
    y: -Screen.height

    Behavior on opacity {
        PropertyAnimation { duration: 300 }
    }

    property bool isPoweroffPressed: false
    property string weatherIcon: ""
    
    onOpacityChanged: {
        isPoweroffPressed = false;
    }

    onIsPoweroffPressedChanged: {
        for (let i = 0; i < settingsModel.count; i++) {
            let btn = settingsModel.get(i)
            if (btn.bText == "Power Off") {
                if (isPoweroffPressed) {
                    btn.tText = "Tap Again";
                } else {
                    btn.tText = "";
                }
            }
        }
    }

    function modemDataChangeHandler(n) {
        return (data) => {
            for (let i = 0; i < settingsModel.count; i++) {
                let btn = settingsModel.get(i)
                if (btn.tText == "Cellular " + (n + 1).toString()) {
                    if (!data.Online || !data.Powered) {
                        btn.bText = qsTr("Offline");
                        btn.icon = "icons/network-cellular-offline.svg"
                    }
                }
            }
        }
    }

    function modemNetDataChangeHandler(n) {
        return (netData) => {
            for (let i = 0; i < settingsModel.count; i++) {
                let btn = settingsModel.get(i)
                if (btn.tText == "Cellular " + (n + 1).toString()) {
                    if (netData.Status === "unregistered"
                        || netData.Status === "denied") {
                        btn.bText = qsTr("Offline");
                        btn.icon = "icons/network-cellular-offline.svg"
                    } else if (netData.Status === "searching") {
                        btn.bText = qsTr("Searching");
                        btn.icon = "icons/network-cellular-no-route.svg"
                    } else {
                        btn.bText = netData.Name;
                        if (netData.Strength > 80) {
                            btn.icon = "icons/network-cellular-signal-excellent.svg"
                        } else if (netData.Strength > 50) {
                            btn.icon = "icons/network-cellular-signal-good.svg"
                        } else if (netData.Strength > 30) {
                            btn.icon = "icons/network-cellular-signal-ok.svg"
                        } else if (netData.Strength > 10) {
                            btn.icon = "icons/network-cellular-signal-low.svg"
                        } else {
                            btn.icon = "icons/network-cellular-signal-none.svg"
                        }
                    }
                }
            }
        }
    }

    function wirelessDataChangeHandler(wData) {
        for (let i = 0; i < settingsModel.count; i++) {
            let btn = settingsModel.get(i)
            if (btn.tText == qsTr("WiFi")) {
                btn.bText = CutieWifiSettings.activeAccessPoint.data["Ssid"].toString();
                if (wData.Strength > 80) {
                    btn.icon = "icons/network-wireless-signal-excellent-symbolic.svg"
                } else if (wData.Strength > 50) {
                    btn.icon = "icons/network-wireless-signal-good-symbolic.svg"
                } else if (wData.Strength > 30) {
                    btn.icon = "icons/network-wireless-signal-ok-symbolic.svg"
                } else if (wData.Strength > 10) {
                    btn.icon = "icons/network-wireless-signal-low-symbolic.svg"
                } else {
                    btn.icon = "icons/network-wireless-signal-none-symbolic.svg"
                }
            }
        }
    }

    function wirelessActiveAccessPointHandler(activeAccessPoint) {
        if (activeAccessPoint) {
            let wData = CutieWifiSettings.activeAccessPoint.data;
            wirelessDataChangeHandler(wData);
            CutieWifiSettings.activeAccessPoint.dataChanged.connect(wirelessDataChangeHandler);
        } else {
            for (let i = 0; i < settingsModel.count; i++) {
                let btn = settingsModel.get(i)
                if (btn.tText == qsTr("WiFi")) {
                    btn.bText = qsTr("Offline");
                    btn.icon = "icons/network-wireless-offline.svg";
                }
            }
        }
    }

    function wirelessEnabledChangedHandler(wirelessEnabled) {
        if (!wirelessEnabled) {
            for (let i = 0; i < settingsModel.count; i++) {
                let btn = settingsModel.get(i)
                if (btn.tText == qsTr("WiFi")) {
                    btn.bText = qsTr("Disabled");
                    btn.icon = "icons/network-wireless-offline.svg";
                }
            }
        }
    }

    Component.onCompleted: {
        if (CutieWifiSettings.wirelessEnabled) {
            if (CutieWifiSettings.activeAccessPoint) {
                let wData = CutieWifiSettings.activeAccessPoint.data;

                wirelessDataChangeHandler(wData);
                CutieWifiSettings.activeAccessPoint.dataChanged.connect(wirelessDataChangeHandler);
            } else {
                wirelessActiveAccessPointHandler(null);
            }
        } else {
            wirelessEnabledChanged(false);
        }

        CutieWifiSettings.activeAccessPointChanged.connect(wirelessActiveAccessPointHandler);
        CutieWifiSettings.wirelessEnabledChanged.connect(wirelessEnabledChangedHandler);

        let modems = CutieModemSettings.modems;
        for (let n = 0; n < modems.length; n++) {
            let data = modems[n].data;
            if (!data.Online || !data.Powered) {
                settingsModel.append({
                    tText: qsTr("Cellular ") + (n + 1).toString(),
                    bText: qsTr("Offline"),
                    icon: "icons/network-cellular-offline.svg"
                });

                CutieModemSettings.modems[n].dataChanged.connect(modemDataChangeHandler(n));
                CutieModemSettings.modems[n].netDataChanged.connect(modemNetDataChangeHandler(n));

                CutieModemSettings.modems[n].setProp("Online", true);
                continue;
            }
            let netData = modems[n].netData;
            let icon;
            if (netData.Strength > 80) {
                icon = "icons/network-cellular-signal-excellent.svg"
            } else if (netData.Strength > 50) {
                icon = "icons/network-cellular-signal-good.svg"
            } else if (netData.Strength > 30) {
                icon = "icons/network-cellular-signal-ok.svg"
            } else if (netData.Strength > 10) {
                icon = "icons/network-cellular-signal-low.svg"
            } else {
                icon = "icons/network-cellular-signal-none.svg"
            }
            settingsModel.append({
                tText: qsTr("Cellular ") + (n + 1).toString(),
                bText: netData.Name,
                icon: icon
            });

            CutieModemSettings.modems[n].dataChanged.connect(modemDataChangeHandler(n));
            CutieModemSettings.modems[n].netDataChanged.connect(modemNetDataChangeHandler(n));
        }
    }

    function setSettingContainerY(y) {
        settingContainer.y = y;
    }

    function setSettingContainerState(state) {
        settingContainer.state = state;
    }

    Rectangle {
        id: tst
        x: 0
        y: parent.height - 10
        height: 10
        width: parent.width
        color: "red"

        MouseArea {
            drag.target: parent; drag.axis: Drag.YAxis; drag.minimumY: - 10; drag.maximumY: Screen.height - 10
            enabled: settingsState.state != "closed"
            anchors.fill: parent
            propagateComposedEvents: true

            onPressed: {
                settingsState.state = "closing";
                settingContainer.state = "closing";
            }

            onReleased: {
                if (parent.y < Screen.height - 2 * parent.height) {
                    settingsState.state = "closed"
                    settingContainer.state = "closed"
                }
                else {
                    settingsState.state = "opened"
                    settingContainer.state = "opened"
                }
                parent.y = parent.parent.height - 10
            }

            onPositionChanged: {
                if (drag.active) {
                    settingSheet.opacity = 1/2 + parent.y / Screen.height / 2
                    settingContainer.y = parent.y - Screen.height
                }
            }
        }
    }

    Item {
        id: settingContainer
        y: 0
        height: parent.height
        width: parent.width

        state: "closed"

        states: [
            State {
                name: "opened"
                PropertyChanges { target: settingContainer; y: 0 }
                PropertyChanges { target: tst; y: Screen.height - 10}
                PropertyChanges { target: settingsState; height: Screen.height }
            },
            State {
                name: "closed"
                PropertyChanges { target: settingContainer; y: -Screen.height }
                PropertyChanges { target: settingsState; height: 30 }
            },
            State {
                name: "opening"
                PropertyChanges { target: settingContainer; y: -Screen.height }
                PropertyChanges { target: settingsState; height: Screen.height }
            },
            State {
                name: "closing"
                PropertyChanges { target: settingContainer; y: 0 }
                PropertyChanges { target: settingsState; height: Screen.height }
            }
        ]

        Behavior on y {
            PropertyAnimation { duration: 300 }
        }

        Rectangle {
            height: 160
            color: Atmosphere.primaryAlphaColor
            radius: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.leftMargin: 20
            y: 35
            clip: true

            Text {
                id: text2
                x: 20
                y: 20
                text: qsTr("Atmosphere")
                font.pixelSize: 24
                font.family: "Lato"
                font.weight: Font.Black
                color: Atmosphere.textColor
                transitions: Transition {
                    ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                }
            }

            ListView {
                anchors.fill: parent
                anchors.topMargin: 64
                model: Atmosphere.atmosphereList
                orientation: Qt.Horizontal
                clip: false
                spacing: -20
                delegate: Item {
                    width: 100
                    height: 100
                    Image {
                        x: 20
                        width: 60
                        height: 80
                        source: "file:/" + modelData.path + "/wallpaper.jpg"
                        fillMode: Image.PreserveAspectCrop

                        Text {
                            anchors.centerIn: parent
                            text: modelData.name
                            font.pixelSize: 14
                            font.bold: false
                            color: (modelData.variant == "dark") ? "#FFFFFF" : "#000000"
                            font.family: "Lato"
                        }

                        MouseArea{
                            anchors.fill: parent
                            onClicked:{
                                Atmosphere.path = modelData.path;
                                atmosphereTimer.start();
                            }
                        }

                        Timer {
                            id: atmosphereTimer
                            interval: 500
                            repeat: false
                            onTriggered: {
                            }
                        }
                    }
                }
            }
        }


        ListModel {
            id: settingsModel

            ListElement {
                bText: ""
                tText: "Power Off"
                icon: "icons/system-shutdown-symbolic.svg"
                clickHandler: function (self) {
//                    if (isPoweroffPressed) {
//                        quicksettings.execApp("systemctl poweroff");
//                    }
//                    isPoweroffPressed = !isPoweroffPressed
                }
            }

            ListElement {
                bText: ""
                tText: qsTr("WiFi")
                icon: "icons/network-wireless-offline.svg"
            }

        }

        GridView {
            id: widgetGrid
            anchors.fill: parent
            anchors.topMargin: 215
            anchors.bottomMargin: 100
            anchors.leftMargin: 20
            model: settingsModel
            cellWidth: width / Math.floor(width / 100)
            cellHeight: cellWidth
            clip: true

            delegate: Item {
                width: widgetGrid.cellWidth
                height: widgetGrid.cellWidth
                Rectangle {
                    id: settingBg
                    width: parent.width - 20
                    height: parent.width - 20
                    color: Atmosphere.secondaryAlphaColor
                    radius: 10

                    Text {
                        id: topText
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 14
                        text: tText
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignHCenter
                        font.family: "Lato"
                        font.bold: false
                        color: Atmosphere.textColor
                        transitions: Transition {
                            ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                        }
                    }

                    Text {
                        id: bottomText
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 14
                        text: bText
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignHCenter
                        font.family: "Lato"
                        font.bold: false
                        color: Atmosphere.textColor
                        transitions: Transition {
                            ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                        }
                    }

                    Image {
                        anchors.fill: parent
                        anchors.margins: parent.width / 3
                        source: icon
                        sourceSize.height: 128
                        sourceSize.width: 128
                        fillMode: Image.PreserveAspectFit
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: clickHandler(this)
                    }
                }
            }
        }

        Item {
            id: brightness
            width: parent.width - 10
            height: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 10
            anchors.leftMargin: 10
            anchors.bottomMargin: 50

            Rectangle
            {
                id: maskRect1
                width: parent.height
                height: width
                visible: false
                state: Atmosphere.variant
                color: Atmosphere.textColor
                transitions: Transition {
                    ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                }
            }

            Image {
                id: volumeMuted1
                width: parent.height / 2
                height: width
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                source: "icons/gpm-brightness-lcd-disabled.svg"
                anchors.leftMargin: 7
                sourceSize.height: height*2
                sourceSize.width: width*2
                visible: false
            }

            OpacityMask {
                anchors.fill: volumeMuted1
                source: maskRect1
                maskSource: volumeMuted1
            }

            Image {
                id: volumeHigh1
                width: parent.height / 2
                height: width
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                source: "icons/gpm-brightness-lcd"
                anchors.rightMargin: 7
                sourceSize.height: height*2
                sourceSize.width: width*2
                visible: false
            }

            OpacityMask {
                anchors.fill: volumeHigh1
                source: maskRect1
                maskSource: volumeHigh1
            }

            Rectangle {
                id: volumeBarTrack1
                height: 2
                radius: 1
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: volumeMuted1.right
                anchors.right: volumeHigh1.left
                anchors.rightMargin: 25
                anchors.leftMargin: 25
                color: Atmosphere.primaryAlphaColor
                transitions: Transition {
                    ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                }
            }

            Rectangle {
                id: volumeBarThumb1
                x: (screenBrightness * (volumeBarTrack1.width - volumeBarThumb1.width) / 100) + volumeBarTrack1.x
                y: volumeBarTrack1.y - height/2
                width: height
                height: parent.height / 2
                radius: width / 2
                MouseArea {
                    anchors.fill: parent
                    drag.axis: Drag.XAxis
                    drag.maximumX: volumeBarTrack1.x - width + volumeBarTrack1.width
                    drag.target: volumeBarThumb1
                    drag.minimumX: volumeBarTrack1.x
                }
                onXChanged: {
                    var fullrange = volumeBarTrack1.width - volumeBarThumb1.width;
                    var vol = 100*(volumeBarThumb1.x - volumeBarTrack1.x)/fullrange;
                    quicksettings.StoreBrightness(vol);
                    let maxB = quicksettings.GetMaxBrightness();
                    quicksettings.SetBrightness(maxB / 6 + maxB * vol / 120);
                }
                state: Atmosphere.variant
                color: Atmosphere.textColor
                transitions: Transition {
                    ColorAnimation { properties: "color"; duration: 500; easing.type: Easing.InOutQuad }
                }
            }
        }
    }
}
