import QtQuick 1.1

FocusScope {
	anchors.fill: parent
	signal inputok(string returntext)
	signal inputabort
	property alias text: dialogText.text
	property alias input: dialogInput
	id: dialogComponent
	z: 1000

	Rectangle {
		color: Qt.rgba(1,1,1,0.5)
		anchors.fill: parent
		MouseArea {
			anchors.fill: parent
		}
		
		Rectangle {
			id: inner
			anchors.centerIn: parent
			width: 300
			height: dialogText.height+buttons.height+dialogInput.height+20
			Text {
			id: dialogText
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			}
			Input {
			focus: true
			id: dialogInput
			anchors.top: dialogText.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.topMargin: 10
			anchors.leftMargin: 10
			anchors.rightMargin: 10
			}
			Row {
				id: buttons
				anchors.top: dialogInput.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.topMargin: 10
				anchors.leftMargin: 10
				anchors.rightMargin: 10
				ButtonA {
					id: dialogButtonCancel
					text: "Abbrechen"
					onClicked: {
						inputabort();
						dialogComponent.destroy()
					}
				}
				
				ButtonA {
					id: dialogButtonOK
					text: "Bestätigen"
					onClicked: {
						if (dialogInput.text.length) {
						inputok(dialogInput.text);
						dialogComponent.destroy()
						} else {
						console.log("No input text!");
						}
					}
				}
			} // end row
		} // end inner rectangle
	} // end outer rectangle
} // end focus scope