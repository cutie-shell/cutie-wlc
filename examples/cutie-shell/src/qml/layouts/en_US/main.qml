/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2020 Ping-Hsun "penk" Chen 
** 
** GNU General Public License Usage
** This file may be used under the terms of the GNU 
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles

KeyboardLayout {
    inputMode: InputEngine.Latin
    keyWeight: 160
    KeyboardRow {
        Key {
            key: Qt.Key_Q
            text: "q"
        }
        Key {
            key: Qt.Key_W
            text: "w"
        }
        Key {
            key: Qt.Key_E
            text: "e"
            alternativeKeys: "êeëèé"
        }
        Key {
            key: Qt.Key_R
            text: "r"
            alternativeKeys: "ŕrř"
        }
        Key {
            key: Qt.Key_T
            text: "t"
            alternativeKeys: "ţtŧť"
        }
        Key {
            key: Qt.Key_Y
            text: "y"
            alternativeKeys: "ÿyýŷ"
        }
        Key {
            key: Qt.Key_U
            text: "u"
            alternativeKeys: "űūũûüuùú"
        }
        Key {
            key: Qt.Key_I
            text: "i"
            alternativeKeys: "îïīĩiìí"
        }
        Key {
            key: Qt.Key_O
            text: "o"
            alternativeKeys: "œøõôöòóo"
        }
        Key {
            key: Qt.Key_P
            text: "p"
        }
        BackspaceKey {}
    }
    KeyboardRow {
        TabKey {} 
        Key {
            key: Qt.Key_A
            text: "a"
            alternativeKeys: "aäåãâàá"
        }
        Key {
            key: Qt.Key_S
            text: "s"
            alternativeKeys: "šsşś"
        }
        Key {
            key: Qt.Key_D
            text: "d"
            alternativeKeys: "dđď"
        }
        Key {
            key: Qt.Key_F
            text: "f"
        }
        Key {
            key: Qt.Key_G
            text: "g"
            alternativeKeys: "ġgģĝğ"
        }
        Key {
            key: Qt.Key_H
            text: "h"
        }
        Key {
            key: Qt.Key_J
            text: "j"
        }
        Key {
            key: Qt.Key_K
            text: "k"
        }
        Key {
            key: Qt.Key_L
            text: "l"
            alternativeKeys: "ĺŀłļľl"
        }
        EnterKey {
            weight: 283
        }
    }
    KeyboardRow {
        keyWeight: 156
        ShiftKey {
            //weight: 204
        }
        Key {
            key: Qt.Key_Z
            text: "z"
            alternativeKeys: "zžż"
        }
        Key {
            key: Qt.Key_X
            text: "x"
        }
        Key {
            key: Qt.Key_C
            text: "c"
            alternativeKeys: "çcċčć"
        }
        Key {
            key: Qt.Key_V
            text: "v"
        }
        Key {
            key: Qt.Key_B
            text: "b"
        }
        Key {
            key: Qt.Key_N
            text: "n"
            alternativeKeys: "ņńnň"
        }
        Key {
            key: Qt.Key_M
            text: "m"
        }

        Key {
            text: ","
            alternativeKeys: ",.;'~*"
        }
        Key {
            text: "."
            alternativeKeys: ",.;'~*"
        }
        Key {
            text: "/"
            alternativeKeys: "<>/-|\\"
        }
    }
    KeyboardRow {
        keyWeight: 154

        SymbolModeKey {
            weight: 217
            text: ""
            noKeyEvent: true
        }
        ChangeLanguageKey {
            weight: 154
        }
        WSpaceKey {
            weight: 800
        }
        HideKeyboardKey {
            weight: 130
        }
    }
}
