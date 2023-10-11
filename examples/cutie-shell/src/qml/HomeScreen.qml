import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import Cutie

Item {
    id: homeScreen
    anchors.fill: parent
    opacity: 0
    enabled: root.state == "homeScreen"
}
