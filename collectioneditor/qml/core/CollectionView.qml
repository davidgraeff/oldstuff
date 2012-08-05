import QtQuick 1.1
import "/qml/components"

FocusScope {
	id: container
	anchors.fill: parent
	property bool isChanged
	property variant collectionobject: null

	Column {
		spacing: 5
		width: parent.width

		Toolbar {
			id: toolbar
			textLeft:"Zurück"
			textRight:container.collectionobject ? ((container.isChanged)?"Speichern":"Services") : "Erstellen"
			onButtonLeftClicked: {screen.state="CollectionsView";}
			onButtonRightClicked: {
			if (isChanged) {
				if (collectionname.text.length==0)
					return;
				
				var categories = [];
				for (var i=0;i <tagsModel.count;++i) {
					categories.push(tagsModel.get(i).text);
				}
				
				isChanged = false;
				if (container.collectionobject) { // alter existing object and save it
					var o = container.collectionobject;
					o.enabled = collectionenabled.currentIndex==1?true:false;
					o.name = collectionname.text
					o.categories = categories;
					database.changeDocument(o);
				} else { // create new collection object and save it
					var o = {"type_":"collection",
					"enabled":collectionenabled.currentIndex==1?true:false,
					"name":collectionname.text,
					"categories":categories};
					
					database.changeDocument(o, true);
					screen.state="CollectionsView";
				}
			} else if (!container.collectionobject) {
				
			} else {
				servicesView.loadView(container.collectionobject);
				screen.state="ServicesView";
			}
			}
		}
		
		InputLabel {
			id: collectionname
			label: "Name:"
			onChanged: container.isChanged = true
		}
		
		BoolLabel {
			id: collectionenabled
			label: "Aktiv:"
			onCurrentIndexChanged: container.isChanged = true
		}
		
		Flow {
		Repeater {
			id: collectionTags
			model: tagsModel
			width: parent.width
// 			height: 40
			delegate: collectionTagsDelegate
		}
		}
	}

	Item {
		anchors.left : parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: childrenRect.height
		
		ButtonA {
			id: btnAddTag
			text: "Neuer Tag"
			anchors.right: btnExecute.left
			onClicked: {
				function inputText(tagname) {
					tagsModel.append({"text":tagname})
					container.isChanged = true
					collectionname.focus = true;
				}
				function inputAbort() {
					collectionname.focus = true;
				}
				showInputDialog(container, "Tagnamen eingeben", inputText, inputAbort);
			}
		}
	
		ButtonA {
			id: btnExecute
			text: "Ausführen"
			anchors.right: btnRemove.left
			KeyNavigation.tab: btnRemove
			KeyNavigation.backtab: btnAddTag
			onClicked: {
				if (!container.collectionobject)
					return;
				var o = {"type_":"execute","plugin_":"server","member_":"runcollection","collectionid_":container.collectionobject._id};
				networkController.write(o);
			}
		}
		
		ButtonA {
			id: btnRemove
			text: "Löschen"
			anchors.right: parent.right
			KeyNavigation.tab: toolbar
			KeyNavigation.backtab: btnExecute
			onClicked: {
				database.removeDocument(container.collectionobject.type_, container.collectionobject._id);
				screen.state="CollectionsView";
			}
		}
	}
	
	Timer {
		property int index;
		index: -1
		onIndexChanged: {
			finishTimer.start();
		}
		id: finishTimer
		interval: 300; running: false; repeat: false
		onTriggered: { tagsModel.remove(index);index=-1;console.log("removed", index)}
	}

	Component {
		id: collectionTagsDelegate
		ButtonA {
			id: listitem
			text: modelData
			onClicked: {
				container.isChanged = true
				listitem.state = "remove"
			}
			Component.onCompleted: {
				listitem.opacity = 0;
				listitem.state = "add"
			}
// 			GridView.onRemove: SequentialAnimation {
// 				PropertyAction { target: listitem; property: "GridView.delayRemove"; value: true }
// 				NumberAnimation { target: listitem; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
// 				PropertyAction { target: listitem; property: "GridView.delayRemove"; value: false }
// 			}
// 			GridView.onAdd: SequentialAnimation {
// 				PropertyAction { target: listitem; property: "scale"; value: 0 }
// 				NumberAnimation { target: listitem; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
// 			}


			transitions: [
				Transition {
					to: "remove"
					//QTBUG: QTBUG-13927, animationn running is not updated in transitions
					// Therefore use a SequentialAnimation+Timer
					SequentialAnimation {
						NumberAnimation {
							target: listitem
							properties: "opacity"
							duration: 250
							to:0
						}
						ScriptAction { script: finishTimer.index = index }
					}
				},
				Transition {
					to: "add"
					NumberAnimation {
						target: listitem
						properties: "opacity"
						duration: 250
						to:1.0
					}
				}
			]
		}
	}

	function loadView(collectionobj) {
		container.collectionobject = collectionobj;
		if (container.collectionobject) {
			collectionname.text = container.collectionobject.name
			collectionenabled.currentIndex = container.collectionobject.enabled?1:0
			tagsModel.clear()
			for(var i=0; i < container.collectionobject.categories.length; ++i)
			tagsModel.append({"text":container.collectionobject.categories[i]})
			btnRemove.enabled = true;
			btnExecute.enabled = true;
		} else {
			collectionname.text = ""
			collectionenabled.currentIndex = 0
			tagsModel.clear()
			btnRemove.enabled = false;
			btnExecute.enabled = false;
		}
		container.isChanged = false;
	}

	ListModel {
			id: tagsModel
	}
}