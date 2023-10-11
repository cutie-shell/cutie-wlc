import QtQuick

Item {
	id: settingsState

	width: Screen.width
	height: Screen.height
	y: settingSheet.y

	state: "closed" 
	states: [
		State {
			name: "opened"
			PropertyChanges { target: settingSheet; y: 0; opacity: 1 }
			PropertyChanges { target: setting; opacity: 1 }
		},
		State {
			name: "closed"
			PropertyChanges { target: settingSheet; y: -Screen.height + 30; opacity: 0 }
			PropertyChanges { target: setting; opacity: 1 }
		},
		State {
			name: "opening"
			PropertyChanges { target: settingSheet; y: 0 }
			PropertyChanges { target: setting; opacity: 0 }
		},
		State {
			name: "closing"
			PropertyChanges { target: settingSheet; y: 0 }
			PropertyChanges { target: setting; opacity: 0 }
		}
	]

	transitions: [
		Transition {
			to: "*"
			NumberAnimation { target: settingSheet; properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad; }
		},
		Transition {
			to: "opening"
			ParallelAnimation {
				NumberAnimation { target: setting; properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad; }
				SequentialAnimation {
					NumberAnimation { target: setting; properties: "anchors.topMargin"; duration: 300; easing.type: Easing.InOutQuad; to: -20 }
				}
			}
		},
		Transition {
			to: "closing"
			ParallelAnimation {
				NumberAnimation { target: setting; properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad; }
				SequentialAnimation {
					NumberAnimation { target: setting; properties: "anchors.topMargin"; duration: 300; easing.type: Easing.InOutQuad; to: -20 }
				}
			}
		},
		Transition {
			to: "opened"
			ParallelAnimation {
				NumberAnimation { target: setting; properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad; }
				SequentialAnimation {
					NumberAnimation { target: setting; properties: "anchors.topMargin"; duration: 600; easing.type: Easing.InOutQuad; to: 0 }
				}
			}
		},
		Transition {
			to: "closed"
			ParallelAnimation {
				NumberAnimation { target: setting; properties: "opacity"; duration: 800; easing.type: Easing.InOutQuad; }
				SequentialAnimation {
					NumberAnimation { target: setting; properties: "anchors.topMargin"; duration: 600; easing.type: Easing.InOutQuad; to: 0 }
				}
			}
		}
	]
	SettingSheet { id: settingSheet }
	StatusArea { id: setting }
}
