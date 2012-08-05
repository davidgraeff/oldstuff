import QtQuick 1.1
Rectangle {
    id: toolbar
    color: systemPalette.window
    height: 40
    z: 100
    anchors.margins: 5
    width: parent.width
    property alias textLeft: buttonLeft.text
    property alias textRight: buttonRight.text
    property alias buttonRight: buttonRight
    property alias buttonLeft: buttonLeft
    property int buttonWidth: 150
    signal buttonLeftClicked
    signal buttonRightClicked
    //BorderImage { source: "/qml/images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }
    ButtonA {
		id: buttonLeft
		text:"Left"
		//anchors.left: parent.left; anchors.leftMargin: 5; y: 3; width: parent.width/2-15; height: parent.height - 6
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		width: buttonWidth
		onClicked: buttonLeftClicked()
    }
    ButtonA {
		id: buttonRight
		text:"Right"
		//anchors.left: parent.left; anchors.leftMargin: 5; y: 3; width: parent.width/2-15; height: parent.height - 6
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		width: buttonWidth
		onClicked: buttonRightClicked()
    }
}