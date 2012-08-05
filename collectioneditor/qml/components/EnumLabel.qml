import QtQuick 1.1

Item {
	anchors.left : parent.left
	anchors.right: parent.right
	anchors.margins: 5
	height: input.height
	property alias labelWidth: labelitem.width
	property alias label: labelitem.text
	property alias model: input.model
	property alias roleText: input.roleText
	property alias combobox: input
	property alias currentIndex: input.currentIndex
	signal changed
	Text {
		id: labelitem;
		text: "Label:"
		verticalAlignment: Text.AlignVCenter
		anchors.top : parent.top
		anchors.bottom: parent.bottom
	}
	ComboBoxA {
		id: input
		focus: true
		anchors.left: labelitem.right
		anchors.right: parent.right
		anchors.leftMargin: 5
		onCurrentIndexChanged: changed()
	}
}