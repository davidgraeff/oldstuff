import QtQuick 1.1
import "/qml/components"

FocusScope {
	Toolbar {
		id: toolbar
		textLeft:"Trennen"
		textRight:"Neues Profil"
		onButtonLeftClicked: {
			database.disconnectFromHost();
			networkController.disconnectFromHost();
		}
		onButtonRightClicked: {
			collectionView.loadView(null)
			screen.state="CollectionView"
		}
	}

	GridView {
		z: 1
		id: collectionsViewGrid;
		model: collectionModelProxy;
		delegate: collectionsDelegate;
		cacheBuffer: 100;
		anchors.fill: parent
		cellHeight: 60
		cellWidth: 235
		anchors.topMargin: toolbar.height
		anchors.bottomMargin: collectionTags.height
		anchors.leftMargin: parent.width % collectionsViewGrid.cellWidth / 2
	}

	Rectangle {
		color: systemPalette.window
		height: 50;
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		z: 10
		
		BorderImage { source: "/qml/images/titlebar.sci"; anchors.fill: parent}
		ListView {
			id: collectionTags
			model: collectiontagModel
			delegate: collectionTagsDelegate
			orientation: ListView.Horizontal
			anchors.fill: parent
		}
	}
	
	Component {
		id: collectionsDelegate
		Rectangle {
			property alias backgroundcolor: delegateItem.color

	//       backgroundcolor: mainScreen.currentIndex == index ? "#9bf" : "lightgrey"
	//       
			id: delegateItem
			height: delegateItem.GridView.view.cellHeight
			width: delegateItem.GridView.view.cellWidth

			color: "lightgrey"
			border.color: "darkgrey"
			border.width: 2

			Text {
				id: itemDate
				anchors.centerIn: parent
				font.pixelSize: 22
				text: model.name ? model.name : "unknown"
			}
		
			MouseArea {
				id: mouseRegion
				anchors.fill: parent
				onPressed: delegateItem.color="white"
				onReleased: delegateItem.color="lightgrey"
				onCanceled: delegateItem.color="lightgrey"
				onClicked: {
					collectionView.loadView(model.value)
					screen.state = "CollectionView"
				}
			}

			ListView.onAdd: SequentialAnimation {
				PropertyAction { target: delegateItem; property: "height"; value: 0 }
				NumberAnimation { target: delegateItem; property: "height"; to: delegateItem.GridView.view.cellHeight; duration: 250; easing.type: Easing.InOutQuad }
			}

			ListView.onRemove: SequentialAnimation {
				PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: true }
				NumberAnimation { target: delegateItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }

				// Make sure delayRemove is set back to false so that the item can be destroyed
				PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: false }
			}
		}
	}

	Component {
		id: collectionTagsDelegate
		ButtonA {
			id: listitem
			text: model.name + "(" + model.collectioncount + ")"
			permanentlyPressed: model.checked
			width: 100
			z: 100
			height: 40
			y: 5
			onClicked: { collectiontagModel.toggleChecked(index); }
		}
	}
}