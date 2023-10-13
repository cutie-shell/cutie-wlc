/*
    Copyright (C) 2019-2020 Ping-Hsun "penk" Chen
    Copyright (C) 2020 Chouaib Hamrouche
    Contact: hello@cutiepi.io
    This file is part of CutiePi shell of the CutiePi project.
    CutiePi shell is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    CutiePi shell is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with CutiePi shell. If not, see <https://www.gnu.org/licenses/>.
*/

import QtQuick
import QtMultimedia
import QtQuick.Window
import Cutie
import CutieShell

Item {
    id: comp
    objectName: "comp"
    width: Screen.width
    height: Screen.height
    visible: true
    WaylandScreen { id: screen }

    CutieShell {
        id: cShell
    }

    function addApp(name, exec, icon) {
        launcherApps.append({
                        appName: name,
                        appExec: exec,
                        appIcon: icon
                    })
    }



    /*Connections {
        target: settings
        function onAddApp (name, exec, icon)  { 
            launcherApps.append({
                        appName: name,
                        appExec: exec,
                        appIcon: icon
                    })
        }
    }*/
    
    SoundEffect {
        id: notifSound
        source: "qrc:/sounds/notif.wav"
    }

    function addNotification(title, body, id) {
        notifications.append({title: title, body: body, id: id});
        notifSound.play();
    }

    function delNotification(id) {
        for (let c_i = 0; c_i < notifications.count; c_i++){
            if (notifications.get(c_i).id === id)
                notifications.remove(c_i);
        }
    }

    function lock() {
        screen.lock();
    }

    function volUp() {
        if (CutieVolume.volume < 0.9)
            CutieVolume.volume += 0.1;
        else CutieVolume.volume = 1.0;

        if (CutieVolume.volume > 0.0)
            CutieVolume.muted = false;
        else CutieVolume.muted = true;
    }

    function volDown() {
        if (CutieVolume.volume > 0.1)
            CutieVolume.volume -= 0.1;
        else CutieVolume.volume = 0.0;

        if (CutieVolume.volume > 0.0)
            CutieVolume.muted = false;
        else CutieVolume.muted = true;
    }

    ListModel { id: notifications }
}
