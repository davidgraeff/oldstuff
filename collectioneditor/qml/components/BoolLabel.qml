import QtQuick 1.1

Item {
	anchors.left : parent.left
	anchors.right: parent.right
	anchors.margins: 5
	height: input.height
	property alias labelWidth: labelitem.width
	property alias label: labelitem.text
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
	ListModel {
		id: model
		ListElement {
			text: "Nicht aktiv"
			value:0
		}
		ListElement {
			text: "Aktiv"
			value:1
		}
	}
	ComboBoxA {
		id: input
		focus: true
		model: model
		anchors.left: labelitem.right
		anchors.right: parent.right
		anchors.leftMargin: 5
		onCurrentIndexChanged: changed()
	}
}