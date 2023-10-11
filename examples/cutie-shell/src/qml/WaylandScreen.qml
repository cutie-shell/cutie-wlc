import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtSensors

import Cutie

Item{
    anchors.fill: parent
    id: compositor
    property variant formatDateTimeString: "HH:mm"
    property variant batteryPercentage: "56"
    property variant simPercentage: "27"
    property variant queue: []
    property bool batteryCharging: false

    property real pitch: 0.0
    property real roll: 0.0
    readonly property double radians_to_degrees: 180 / Math.PI
    property variant orientation: 0
    property variant sensorEnabled: true 

    property real unlockBrightness: 0.5

    property int drawerMargin: 5
    property string nextAtmospherePath: "/usr/share/atmospheres/city/"
    property color atmosphereForeground: "#ffffff"

    function lock() {
        // if (screenLockState.state == "closed") {
        //     openBl.start();
        // } else {
        //     unlockBrightness = settings.GetBrightness() / settings.GetMaxBrightness();
        //     closeBl.start();
        // }
    }

    NumberAnimation { 
        id: closeBl
        target: settings
        property: "brightness"
        to: 0 
        duration: 200

        onFinished: {
            //screenLockState.state = "closed";
        }
    }

    NumberAnimation { 
        id: openBl
        target: settings
        property: "brightness"
        to: settings.GetMaxBrightness() * unlockBrightness
        duration: 200

        onFinished: {
            //screenLockState.state = "locked";
        }
    }   

    Item {
        id: root
        property string oldState: "homeScreen"
        property string tmpState: "homeScreen"

        onStateChanged: {
            oldState = tmpState;
            tmpState = state;
        }

        state: "homeScreen" 
        states: [
            State{
                name: "homeScreen"
                PropertyChanges { target: wallpaperBlur; opacity: 0 }
                PropertyChanges { target: homeScreen; opacity: 1 }
                PropertyChanges { target: notificationScreen; opacity: 0 }
                PropertyChanges { target: mainViewContainer; opacity: 1 }
            },
            State {
                name: "notificationScreen"
                PropertyChanges { target: wallpaperBlur; opacity: 1 }
                PropertyChanges { target: homeScreen; opacity: 0 }
                PropertyChanges { target: notificationScreen; opacity: 1 }
                PropertyChanges { target: mainViewContainer; opacity: 1 }
            }
        ]

        transitions: [
           Transition {
                to: "*"
                NumberAnimation { target: notificationScreen; properties: "opacity"; duration: 300; easing.type: Easing.InOutQuad; }
                NumberAnimation { target: homeScreen; properties: "opacity"; duration: 300; easing.type: Easing.InOutQuad; }
           }

        ]
    }

    Item {
        id: screenLockState
        state: "opened" 
        states: [
            State{
                name: "closed"
                PropertyChanges { target: lockscreen; opacity: 1; y: 0 }
                PropertyChanges { target: shellContainer; visible: false; opacity: 0 }
            },
            State {
                name: "locked"
                PropertyChanges { target: lockscreen; opacity: 1; y: 0 }
                PropertyChanges { target: shellContainer; visible: true; opacity: 0 }
            },
            State {
                name: "opened"
                PropertyChanges { target: lockscreen; opacity: 0; y: -view.height }
                PropertyChanges { target: shellContainer; visible: true; opacity: 1 }
            }
        ]

        transitions: [
           Transition {
                to: "*"
                NumberAnimation { target: lockscreen; properties: "opacity"; duration: 200; easing.type: Easing.InOutQuad; }
           }

        ]
    }

    Item {
        visible: true
        anchors.fill: parent
        Item {
            id: view
            anchors.fill: parent

            Item {
                id: content 
                anchors.fill: parent

                Item {
                    id: realWallpaper
                    anchors.fill: parent
                    Image {
                        id: wallpaper
                        anchors.fill: parent
                        source: "file:/" + atmospherePath + "/wallpaper.jpg"
                        fillMode: Image.PreserveAspectCrop
                    }

                    Image {
                        id: nextWallpaper
                        anchors.fill: parent
                        source: "file:/" + nextAtmospherePath + "/wallpaper.jpg"
                        fillMode: Image.PreserveAspectCrop
                        opacity: 0
                        state: "normal"
                        states: [
                            State {
                                name: "changing"
                                PropertyChanges { target: nextWallpaper; opacity: 1 }
                            },
                            State {
                                name: "normal"
                                PropertyChanges { target: nextWallpaper; opacity: 0 }
                            }
                        ]

                        transitions: Transition {
                            to: "normal"

                            NumberAnimation {
                                target: nextWallpaper
                                properties: "opacity"
                                easing.type: Easing.InOutQuad
                                duration: 500
                            }
                        }
                    }
                }

                Item {
                    id: shellContainer
                    anchors.fill: parent

                    Behavior on opacity {
                        PropertyAnimation { duration: 300 }
                    }

                    FastBlur {
                        id: wallpaperBlur
                        anchors.fill: parent
                        source: realWallpaper
                        radius: 70

                        Behavior on opacity {
                            PropertyAnimation { duration: 300 }
                        }
                    }

                    Rectangle {
                        color: Atmosphere.secondaryAlphaColor
                        anchors.fill: wallpaperBlur
                        opacity: wallpaperBlur.opacity / 3
                    }

                    Item {
                        id: mainViewContainer
                        anchors.fill: parent

                        Behavior on opacity {
                            PropertyAnimation { duration: 300 }
                        }

                        HomeScreen { id: homeScreen }
                        NotificationScreen { id: notificationScreen }

                        ScreenSwipe { id: screenSwipe }
                    }
                }

                LockScreen { id: lockscreen }
            }
        }
    }
}
