import QtQuick 1.1
import "/qml/components"

Rectangle {
	SystemPalette { id: systemPalette; colorGroup: SystemPalette.Active }
	color: systemPalette.window
	id: screen;
	anchors.fill: parent

	state:"ConnectView"

	ConnectView {
		id: connectView
		anchors.fill: parent
		opacity: 0
	}
	CollectionsView {
		id: collectionsView
		anchors.fill: parent
		opacity: 0
	}
	CollectionView {
		id: collectionView
		anchors.fill: parent
		opacity: 0
	}
	ServicesView {
		id: servicesView
		anchors.fill: parent
		opacity: 0
	}
	ServiceView {
		id: serviceView
		anchors.fill: parent
		opacity: 0
	}

	states: [
		State {
			name: "ConnectView";
			PropertyChanges { target: connectView; opacity: 1}
			PropertyChanges { target: collectionsView; opacity: 0}
			PropertyChanges { target: collectionView; opacity: 0}
			PropertyChanges { target: servicesView; opacity: 0}
			PropertyChanges { target: serviceView; opacity: 0}
			PropertyChanges { target: connectView; focus: true}
		},
		State {
			name: "CollectionsView";
			PropertyChanges { target: collectionsView; opacity: 1}
			PropertyChanges { target: collectionView; opacity: 0}
			PropertyChanges { target: collectionsView; focus: true}
		},
		State {
			name: "CollectionView";
			PropertyChanges { target: collectionsView; opacity: 0}
			PropertyChanges { target: collectionView; opacity: 1}
			PropertyChanges { target: servicesView; opacity: 0}
			PropertyChanges { target: collectionView; focus: true}
		},
		State {
			name: "ServicesView";
			PropertyChanges { target: servicesView; opacity: 1}
			PropertyChanges { target: collectionView; opacity: 0}
			PropertyChanges { target: serviceView; opacity: 0}
		},
		State {
			name: "ServiceView";
		PropertyChanges { target: serviceView; opacity: 1}
			PropertyChanges { target: servicesView; opacity: 0}
		}
	]
	transitions: [
		Transition {
			//NumberAnimation { properties: "x, y"; duration: 500; easing.type: Easing.InOutQuad }
			NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.Linear }
		}
	]

	function showInputDialog(parentWindow, text, callback, callbackabort) {
		var component = Qt.createComponent("/qml/components/InputDialog.qml");
		if (component.status == Component.Ready) {
			var d = component.createObject(parentWindow, {"text": text})
			d.inputok.connect(callback)
			d.inputabort.connect(callbackabort)
			d.text = text;
			d.focus = true
		} else
			console.log("failed", component.errorString());
	}
}

