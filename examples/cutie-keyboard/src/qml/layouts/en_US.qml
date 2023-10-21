import QtQuick

Item {
	property var row1_model: row1
	property var row1_model_shift: row1_shift
	property var row1B_model: row1B
	property var row2_model: row2
	property var row2B_model: row2B 
	property var row3_model: row3 
	property var row3B_model: row3B 
	property var row4_model: row4
	property var row4B_model: row4B
	property var row5_model: row5
	property var row5_model_shift: row5_shift

	property var layout: 'English US'

	ListModel {
		id: row1
		ListElement{displayText: '1'; keyWidth: 1; keyCode: Qt.Key_1; capitalization: false;}
		ListElement{displayText: '2'; keyWidth: 1; keyCode: Qt.Key_2; capitalization: false;}
		ListElement{displayText: '3'; keyWidth: 1; keyCode: Qt.Key_3; capitalization: false;}
		ListElement{displayText: '4'; keyWidth: 1; keyCode: Qt.Key_4; capitalization: false;}
		ListElement{displayText: '5'; keyWidth: 1; keyCode: Qt.Key_5; capitalization: false;}
		ListElement{displayText: '6'; keyWidth: 1; keyCode: Qt.Key_6; capitalization: false;}
		ListElement{displayText: '7'; keyWidth: 1; keyCode: Qt.Key_7; capitalization: false;}
		ListElement{displayText: '8'; keyWidth: 1; keyCode: Qt.Key_8; capitalization: false;}
		ListElement{displayText: '9'; keyWidth: 1; keyCode: Qt.Key_9; capitalization: false;}
		ListElement{displayText: '0'; keyWidth: 1; keyCode: Qt.Key_0; capitalization: false;}
	}

	ListModel {
		id: row1_shift
		ListElement{displayText: '!'; keyWidth: 1; keyCode: Qt.Key_Exclam; capitalization: false;}
		ListElement{displayText: '@'; keyWidth: 1; keyCode: Qt.Key_At; capitalization: false;}
		ListElement{displayText: '#'; keyWidth: 1; keyCode: Qt.Key_NumberSign; capitalization: false;}
		ListElement{displayText: '$'; keyWidth: 1; keyCode: Qt.Key_Dollar; capitalization: false;}
		ListElement{displayText: '%'; keyWidth: 1; keyCode: Qt.Key_Percent; capitalization: false;}
		ListElement{displayText: '^'; keyWidth: 1; keyCode: Qt.Key_AsciiCircum; capitalization: false;}
		ListElement{displayText: '&'; keyWidth: 1; keyCode: Qt.Key_Ampersand; capitalization: false;}
		ListElement{displayText: '*'; keyWidth: 1; keyCode: Qt.Key_Asterisk; capitalization: false;}
		ListElement{displayText: '('; keyWidth: 1; keyCode: Qt.Key_ParenLeft; capitalization: false;}
		ListElement{displayText: ')'; keyWidth: 1; keyCode: Qt.Key_ParenRight; capitalization: false;}
	}

	ListModel {
		id: row1B
		ListElement{displayText: '\u21E5'; keyWidth: 1.8; keyCode: Qt.Key_Tab; capitalization: false;}
		ListElement{displayText: 'Ctrl'; keyWidth: 1.4; keyCode: Qt.ControlModifier; capitalization: false;}
		ListElement{displayText: 'Alt'; keyWidth: 1.4; keyCode: Qt.AltModifier; capitalization: false;}
		ListElement{displayText: '\u21D1'; keyWidth: 1.35; keyCode: Qt.Key_Up; capitalization: false;}
		ListElement{displayText: '\u21D3'; keyWidth: 1.35; keyCode: Qt.Key_Down; capitalization: false;}
		ListElement{displayText: '\u21D0'; keyWidth: 1.35; keyCode: Qt.Key_Left; capitalization: false;}
		ListElement{displayText: '\u21D2'; keyWidth: 1.35; keyCode: Qt.Key_Right; capitalization: false;}
	}

	ListModel {
		id: row2
		ListElement{displayText: 'q'; keyWidth: 1; keyCode: Qt.Key_Q; capitalization: true;}
		ListElement{displayText: 'w'; keyWidth: 1; keyCode: Qt.Key_W; capitalization: true;}
		ListElement{displayText: 'e'; keyWidth: 1; keyCode: Qt.Key_E; capitalization: true;}
		ListElement{displayText: 'r'; keyWidth: 1; keyCode: Qt.Key_R; capitalization: true;}
		ListElement{displayText: 't'; keyWidth: 1; keyCode: Qt.Key_T; capitalization: true;}
		ListElement{displayText: 'y'; keyWidth: 1; keyCode: Qt.Key_Y; capitalization: true;}
		ListElement{displayText: 'u'; keyWidth: 1; keyCode: Qt.Key_U; capitalization: true;}
		ListElement{displayText: 'i'; keyWidth: 1; keyCode: Qt.Key_I; capitalization: true;}
		ListElement{displayText: 'o'; keyWidth: 1; keyCode: Qt.Key_O; capitalization: true;}
		ListElement{displayText: 'p'; keyWidth: 1; keyCode: Qt.Key_P; capitalization: true;}
	}

	ListModel {
		id: row2B
		ListElement{displayText: '*'; keyWidth: 1; keyCode: Qt.Key_Asterisk; capitalization: false;}
		ListElement{displayText: '-'; keyWidth: 1; keyCode: Qt.Key_Minus; capitalization: false;}
		ListElement{displayText: '+'; keyWidth: 1; keyCode: Qt.Key_Plus; capitalization: false;}
		ListElement{displayText: '"'; keyWidth: 1; keyCode: Qt.Key_QuoteDbl; capitalization: false;}
		ListElement{displayText: '<'; keyWidth: 1; keyCode: Qt.Key_Less; capitalization: false;}
		ListElement{displayText: '>'; keyWidth: 1; keyCode: Qt.Key_Greater; capitalization: false;}
		ListElement{displayText: "'"; keyWidth: 1; keyCode: Qt.Key_Apostrophe; capitalization: false;}
		ListElement{displayText: ':'; keyWidth: 1; keyCode: Qt.Key_Colon; capitalization: false;}
		ListElement{displayText: ';'; keyWidth: 1; keyCode: Qt.Key_Semicolon; capitalization: false;}
		ListElement{displayText: '~'; keyWidth: 1; keyCode: Qt.Key_AsciiTilde; capitalization: false;}
	}

	ListModel {
		id: row3
		ListElement{displayText: 'a'; keyWidth: 1; keyCode: Qt.Key_A; capitalization: true;}
		ListElement{displayText: 's'; keyWidth: 1; keyCode: Qt.Key_S; capitalization: true;}
		ListElement{displayText: 'd'; keyWidth: 1; keyCode: Qt.Key_D; capitalization: true;}
		ListElement{displayText: 'f'; keyWidth: 1; keyCode: Qt.Key_F; capitalization: true;}
		ListElement{displayText: 'g'; keyWidth: 1; keyCode: Qt.Key_G; capitalization: true;}
		ListElement{displayText: 'h'; keyWidth: 1; keyCode: Qt.Key_H; capitalization: true;}
		ListElement{displayText: 'j'; keyWidth: 1; keyCode: Qt.Key_J; capitalization: true;}
		ListElement{displayText: 'k'; keyWidth: 1; keyCode: Qt.Key_K; capitalization: true;}
		ListElement{displayText: 'l'; keyWidth: 1; keyCode: Qt.Key_L; capitalization: true;}
	}

	ListModel {
		id: row3B
		ListElement{displayText: '='; keyWidth: 1; keyCode: Qt.Key_Equal; capitalization: false;}
		ListElement{displayText: '$'; keyWidth: 1; keyCode: Qt.Key_Dollar; capitalization: false;}
		ListElement{displayText: '€'; keyWidth: 1; keyCode: 0x20AC; capitalization: false;}
		ListElement{displayText: '£'; keyWidth: 1; keyCode: 0xC2; capitalization: false;}
		ListElement{displayText: '₵'; keyWidth: 1; keyCode: 0xA2; capitalization: false;}
		ListElement{displayText: '¥'; keyWidth: 1; keyCode: 0xA5; capitalization: false;}
		ListElement{displayText: '§'; keyWidth: 1; keyCode: Qt.Key_section; capitalization: false;}
		ListElement{displayText: '['; keyWidth: 1; keyCode: Qt.Key_BracketLeft; capitalization: false;}
		ListElement{displayText: ']'; keyWidth: 1; keyCode: Qt.Key_BracketRight; capitalization: false;}
	}

	ListModel {
		id: row4
		ListElement{displayText: '\u21E7'; keyWidth: 1.5; keyCode: Qt.ShiftModifier; capitalization: false;}
		ListElement{displayText: 'z'; keyWidth: 1; keyCode: Qt.Key_Z; capitalization: true;}
		ListElement{displayText: 'x'; keyWidth: 1; keyCode: Qt.Key_X; capitalization: true;}
		ListElement{displayText: 'c'; keyWidth: 1; keyCode: Qt.Key_C; capitalization: true;}
		ListElement{displayText: 'v'; keyWidth: 1; keyCode: Qt.Key_V; capitalization: true;}
		ListElement{displayText: 'b'; keyWidth: 1; keyCode: Qt.Key_B; capitalization: true;}
		ListElement{displayText: 'n'; keyWidth: 1; keyCode: Qt.Key_N; capitalization: true;}
		ListElement{displayText: 'm'; keyWidth: 1; keyCode: Qt.Key_M; capitalization: true;}
		ListElement{displayText: '\u21E6'; keyWidth: 1.5; keyCode: Qt.Key_Backspace; capitalization: false;}
		
	}

	ListModel {
		id: row4B
		ListElement{displayText: '\u21E7'; keyWidth: 1.5; keyCode: Qt.ShiftModifier; capitalization: false;}
		ListElement{displayText: '_'; keyWidth: 1; keyCode: Qt.Key_Underscore; capitalization: false;}
		ListElement{displayText: '`'; keyWidth: 1; keyCode: Qt.Key_Agrave; capitalization: false;}
		ListElement{displayText: '{'; keyWidth: 1; keyCode: Qt.Key_BraceLeft; capitalization: false;}
		ListElement{displayText: '}'; keyWidth: 1; keyCode: Qt.Key_BraceRight; capitalization: false;}
		ListElement{displayText: '\u2216'; keyWidth: 1; keyCode: Qt.Key_Backslash; capitalization: false;}
		ListElement{displayText: '|'; keyWidth: 1; keyCode: Qt.Key_Bar; capitalization: false;}
		ListElement{displayText: '™'; keyWidth: 1; keyCode: 0x2122; capitalization: false;}
		ListElement{displayText: '\u21E6'; keyWidth: 1.5; keyCode: Qt.Key_Backspace; capitalization: false;}
		
	}

	ListModel {
		id: row5
		ListElement{displayText: '-'; keyWidth: 0.9; keyCode: Qt.Key_Minus; capitalization: false;}
		ListElement{displayText: ','; keyWidth: 0.9; keyCode: Qt.Key_Comma; capitalization: false;}
		ListElement{displayText: 'English'; keyWidth: 4.7; keyCode: Qt.Key_Space; capitalization: false;}
		ListElement{displayText: '.'; keyWidth: 0.9; keyCode: Qt.Key_Period; capitalization: false;}
		ListElement{displayText: '/'; keyWidth: 0.9; keyCode: Qt.Key_Slash; capitalization: false;}
		ListElement{displayText: '\u21B5'; keyWidth: 1.7; keyCode: Qt.Key_Enter; capitalization: false;}
	}

	ListModel {
		id: row5_shift
		ListElement{displayText: '_'; keyWidth: 0.9; keyCode: Qt.Key_Underscore; capitalization: false;}
		ListElement{displayText: '<'; keyWidth: 0.9; keyCode: Qt.Key_Comma; capitalization: false;}
		ListElement{displayText: 'English'; keyWidth: 4.7; keyCode: Qt.Key_Space; capitalization: false;}
		ListElement{displayText: '>'; keyWidth: 0.9; keyCode: Qt.Key_Period; capitalization: false;}
		ListElement{displayText: '?'; keyWidth: 0.9; keyCode: Qt.Key_Question; capitalization: false;}
		ListElement{displayText: '\u21B5'; keyWidth: 1.7; keyCode: Qt.Key_Enter; capitalization: false;}
	}
}
