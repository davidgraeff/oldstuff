import QtQuick 1.1

Rectangle {
	width: 135
	height: 137
	property alias title: titletext.text
	gradient: Gradient {
		GradientStop {
			position: 0
			color: "#ffffff"
		}
		
		GradientStop {
			position: 1
			color: "#000000"
		}
	}
	Text {
		id: titletext
		anchors.horizontalCenter: parent.horizontalCenter
		font.pixelSize: 16
		height: 20
	}
}