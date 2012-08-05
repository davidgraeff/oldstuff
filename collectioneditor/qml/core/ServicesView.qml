import QtQuick 1.1
import "qmlprivate.js" as P
import "/qml/components"

FocusScope {
    id: container
    property variant collectionobject: null
    property bool objectIsLoading: false

	Toolbar {
		id: toolbar
		textLeft:"Zurück"
		textRight:"Neuer Service"
		onButtonLeftClicked: {screen.state="CollectionView";}
		onButtonRightClicked: {
			serviceView.loadView(container.collectionobject, null)
			screen.state="ServiceView";
		}
	}
	
	ListView {
		id: services
		model: servicesModel
		delegate: servicesDelegate
		anchors.topMargin: toolbar.height
		anchors.fill: parent
		section.property: "type"
		section.criteria: ViewSection.FullString
		section.delegate: sectionDelegate
	}
	
	
	Item {
		anchors.left : parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: childrenRect.height
		ButtonA {
			id: btnMark
			text: "Markieren"
			anchors.right: btnCopy.left
			KeyNavigation.tab: btnCopy
			KeyNavigation.backtab: btnPaste
			onClicked: { btnMark.permanentlyPressed = !btnMark.permanentlyPressed }
		}
		ButtonA {
			id: btnCopy
			text: "Kopieren"
			enabled: btnMark.permanentlyPressed
			anchors.right: btnPaste.left
			KeyNavigation.tab: btnPaste
			KeyNavigation.backtab: btnMark
			onClicked: {
				var elements = [];
				var z = servicesModel.rowCount();
				for (var i = 0;i<z;++i) {
					if (servicesModel.getSelected(i))
						elements.push(servicesModel.getValue(i));
				}
				P.priv("copyServices").elements = elements;
				P.priv("copyServices").fromid = container.collectionobject._id;
				P.priv("copyServices").fromname = container.collectionobject.name;
				console.log("Copy "+elements.length+" from "+ container.collectionobject.name);
			}
		}
		ButtonA {
			id: btnPaste
			text: "Einfügen"
			anchors.right: parent.right
			KeyNavigation.tab: btnMark
			KeyNavigation.backtab: btnCopy
			onClicked: {
				var elements = P.priv("copyServices").elements;
				console.log("Paste "+elements.length+" from "+ P.priv("copyServices").fromname);
				var z = elements.length;
				for (var i = 0;i<z;++i) {
					delete elements[i]._id;
					delete elements[i]._rev;
					elements[i].collection_ = container.collectionobject._id;
					database.requestAdd(elements[i]);
				}
			}
		}
	}
	
	///////////////////////////////////////////////////////////////
	function loadView(collectionobj) {
		container.objectIsLoading = true;
		container.collectionobject = collectionobj;
		container.objectIsLoading = false;
		if (container.collectionobject && container.collectionobject._id)
			servicesModel.requestDataOfCollection(container.collectionobject._id);
		btnPaste.enabled = (P.priv("copyServices").elements!=null && P.priv("copyServices").elements.length);
	}
	Component {
		id: sectionDelegate
		Rectangle {
			color: "#9bf"
			border.color: "darkgrey"
			width: parent.width
			height: sectionText.height
			Text {
				id: sectionText
				font.pixelSize: 22
				text: section
			}
		}
	}
	Component {
		id: servicesDelegate

		Rectangle {
			property alias backgroundcolor: rectBackground.color

	//       backgroundcolor: mainScreen.currentIndex == index ? "#9bf" : "lightgrey"
	//       
			height: itemDate.height + itemText.height + 20
			width: parent.width

			id: rectBackground
			color: btnMark.permanentlyPressed && model.selected ? "#9bf" : "lightgrey"
			border.color: "darkgrey"
			border.width: 2

			Item {
				width: parent.width
				Text {
					id: itemDate
					font.pixelSize: 22
					text: model.name
				}

				Text {
					id: itemText
					text: model.description
					anchors.margins: 10
					anchors.top: itemDate.bottom
					width: parent.width
					font.pixelSize: 16
					wrapMode: Text.Wrap
				}
			}
		
			MouseArea {
				id: mouseRegion
				anchors.fill: parent
				onPressed: rectBackground.state = 'Pressed'
				onReleased: rectBackground.state = 'Default'
				onClicked: {
					if (btnMark.permanentlyPressed) {
						servicesModel.setSelected(index, !model.selected);
					} else {
						serviceView.loadView(container.collectionobject, servicesModel.getValue(index))
						screen.state="ServiceView";
					}
				}
			}

			states:[
			State {
				name: "Pressed"
				when: mouseRegion.pressed == true
				PropertyChanges {
					target: itemText
					style: Text.Sunken
					color: "white"
				}
			}
			]
		}
	}

}