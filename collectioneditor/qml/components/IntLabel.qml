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
	property alias min: validator.bottom
	property alias max: validator.top
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
		focus: true
		anchors.left: labelitem.right
		anchors.right: parent.right
		anchors.leftMargin: 5
		onTextChanged: {
			if (input.acceptableInput) {
				wrapper.changed()
				input.color = Qt.rgba(0,0,0,1)
			} else {
				input.color = Qt.rgba(1,0,0,1)
			}
		}
		validator: IntValidator{id: validator}
	}
}