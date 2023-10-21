import QtQuick
import QtQuick.Controls
import Cutie
import CutieIM

Item {
    id: mainView
    anchors.fill: parent
    anchors.margins: 5
    focus: false
    
    InputMethodManagerV2{
          id: inputMgr
          objectName: "inputMgr"
    }

    property double rowSpacing:     0.01 * width  // horizontal spacing between keyboard
    property double columnSpacing:  0.02 * height // vertical   spacing between keyboard
    property bool   shift:          false
    property bool   symbols:        false
    property bool   ctrl:           false
    property double columns:        10
    property double rows:           5

    MouseArea {anchors.fill: parent} // don't allow touches to pass to MouseAreas underneath
    
    Loader {
        id: kbdLayout
        source: "layouts/en_US.qml"
    }

    Rectangle {
        anchors.fill: parent
        anchors.bottom: parent.bottom
        color: "transparent"

        Flickable {
            id: view
            anchors.fill: parent

            topMargin: 5

            contentWidth: keyboard.width
            contentHeight: keyboard.height+1

            onContentYChanged: {
                //if(contentY < -keyboard.height/3)
                //    Qt.quit()
            }

            onMovementEnded: {
                if(contentY != -5)
                    contentY = -5
            }

            Item { // keys
                id: keyboard

                width: mainView.width
                height: mainView.height
                
                Column {
                    spacing: columnSpacing

                    Item {
                        width: mainView.width
                        height: keyboard.height * 0.65 / rows

                        PageIndicator {
                            id: numberIndicator
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: numberSwipe.top
                            count: numberSwipe.count
                            currentIndex: numberSwipe.currentIndex

                            delegate: Rectangle {
                                implicitWidth: mainView.width/numberSwipe.count
                                implicitHeight: 2
                                color: index === numberSwipe.currentIndex ? Atmosphere.accentColor : Atmosphere.secondaryColor

                                required property int index
                            }
                        }

                        SwipeView {
                            id: numberSwipe
                            anchors.fill: parent
                            Row { // 1234567890
                                spacing: rowSpacing
                                opacity: SwipeView.isCurrentItem ? 1 : 0
                                Behavior on opacity {
                                    OpacityAnimator {
                                        duration: 300
                                    }
                                }
                                width: childrenRect.width
                                Repeater {
                                    model: shift ? kbdLayout.item.row1_model_shift : kbdLayout.item.row1_model
                                    
                                    delegate: KeyButton {
                                        text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                            shift && capitalization ? displayText.toUpperCase() : displayText
                                        width: keyWidth * keyboard.width / columns - rowSpacing
                                        height: keyboard.height * 0.65 / rows - columnSpacing
                                        checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                        onClicked: {
                                            mainView.clicked(displayText)
                                        }
                                    }
                                }
                            }

                            Row { // 1234567890
                                spacing: rowSpacing
                                opacity: SwipeView.isCurrentItem ? 1 : 0
                                Behavior on opacity {
                                    OpacityAnimator {
                                        duration: 300
                                    }
                                }
                                width: childrenRect.width
                                Repeater {
                                    model: kbdLayout.item.row1B_model
                                    
                                    delegate: KeyButton {
                                        text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                            shift && capitalization ? displayText.toUpperCase() : displayText
                                        width: keyWidth * keyboard.width / columns - rowSpacing
                                        height: keyboard.height * 0.65 / rows - columnSpacing
                                        checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                        onClicked: {
                                            mainView.clicked(displayText)
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    Item {
                        width: mainView.width
                        height: keyboard.height * 1.05 / rows * 3 - columnSpacing
                        anchors.topMargin: columnSpacing

                        PageIndicator {
                            id: symbolIndicator
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: symbolSwipe.top
                            count: symbolSwipe.count
                            currentIndex: symbolSwipe.currentIndex

                            delegate: Rectangle {
                                implicitWidth: mainView.width/symbolSwipe.count
                                implicitHeight: 2
                                color: index === symbolSwipe.currentIndex ? Atmosphere.accentColor : Atmosphere.secondaryColor

                                required property int index
                            }
                        }

                        SwipeView {
                            id: symbolSwipe
                            anchors.fill: parent

                            Column {
                                opacity: SwipeView.isCurrentItem ? 1 : 0
                                spacing: columnSpacing

                                Behavior on opacity {
                                    OpacityAnimator {
                                        duration: 300
                                    }
                                }

                                Row { // qwertyuiop
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row2_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05/ rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                                
                                Row { // asdfghjkl
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row3_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05/ rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                                
                                Row { // zxcvbnm
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row4_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05 / rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                            }

                            Column {
                                opacity: SwipeView.isCurrentItem ? 1 : 0
                                spacing: columnSpacing

                                Behavior on opacity {
                                    OpacityAnimator {
                                        duration: 300
                                    }
                                }

                                Row { // qwertyuiop
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row2B_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05/ rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                                
                                Row { // asdfghjkl
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row3B_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05/ rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                                
                                Row { // zxcvbnm
                                    spacing: rowSpacing
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: childrenRect.width
                                    Repeater {
                                        model: kbdLayout.item.row4B_model
                                        
                                        delegate: KeyButton {
                                            text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                                shift && capitalization ? displayText.toUpperCase() : displayText
                                            width: keyWidth * keyboard.width / columns - rowSpacing
                                            height: keyboard.height * 1.05 / rows - columnSpacing
                                            checked: displayText == "\u21E7" && mainView.shift || 
                                                    displayText == "Ctrl" && mainView.ctrl
                                            onClicked: {
                                                mainView.clicked(displayText)
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    }
                    
                    Row {
                        spacing: rowSpacing
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: childrenRect.width
                        Repeater {
                            model: shift ? kbdLayout.item.row5_model_shift : kbdLayout.item.row5_model
                            
                            delegate: KeyButton {
                                text: keyCode == "SPACE" ? kbdLayout.item.layout : 
                                    shift && capitalization ? displayText.toUpperCase() : displayText
                                width: keyWidth * keyboard.width / columns - rowSpacing
                                height: keyboard.height * 1.05 / rows - columnSpacing
                                checked: displayText == "\u21E7" && mainView.shift || 
                                        displayText == "Ctrl" && mainView.ctrl
                                onClicked: {
                                    mainView.clicked(displayText)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    signal clicked(string text)
    onClicked: {
        var modifier = Qt.NoModifier;
        if(text == '\u2190') { //ENTER
            vkbd.send_key(text, 1);
            vkbd.send_key(text, 0);
        }
        else if(text == Qt.ShiftModifier)
            shift = !shift
        else if(text == Qt.ControlModifier)
            ctrl = !ctrl
        else {
            if(shift)
                modifier = Qt.ShiftModifier
            if(ctrl)
                modifier = Qt.ControlModifier
            vkbd.send_key(text, 1, modifier);
            vkbd.send_key(text, 0, modifier);
            shift = false
            ctrl = false
        }
    }
}
