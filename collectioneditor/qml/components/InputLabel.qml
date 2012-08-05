import QtQuick 1.1

Item {
	id: wrapper
	anchors.left : parent.left
	anchors.right: parent.right
	anchors.margins: 5
	height: input.height
	property alias labelWidth: labelitem.width
	property alias label: labelitem.text
	property alias text: input.text
	property alias maximumLength: input.maximumLength
	property alias acceptableInput: input.acceptableInput
	property alias readOnly: input.readOnly
	signal changed
	Text {
		id: labelitem;
		text: "Label:"
		verticalAlignment: Text.AlignVCenter
		anchors.top : parent.top
		anchors.bottom: parent.bottom
	}
	Input {
		id: input
		anchors.left: labelitem.right
		anchors.right: parent.right
		anchors.leftMargin: 5
		focus: true
		onChanged: wrapper.changed()
	}
}