import QtQuick 1.1

Item {
	id: container
	anchors.left : parent.left
	anchors.right: parent.right
	anchors.margins: 5
	height: 20
// 	height: labelitem.height*visibleItems
	property alias labelWidth: labelitem.width
	property alias label: labelitem.text
	property int visibleItems: Math.min(5, enumModel.count)
	signal changed
	function setData(data, selectedIndices) {
		enumModel.clear();
		for (var i=0;i< data.length;++i) {
			enumModel.append({"text":data[i],"active":selectedIndices.indexOf(i)!=-1})
		}
		var elementsPerRow = Math.floor(view.width / (view.cellWidth+10)); // abrunden
		var rows = Math.ceil(data.length / elementsPerRow); // aufrunden
		container.height = rows * view.cellHeight;
	}
	function getSelectedIndices() {
		var selected = [];
		for (var i=0;i< data.length;++i) {
			if (enumModel.get(i).active)
				selected.push(i);
		}
		return selected;
	}
	Text {
		id: labelitem;
		text: "Label:"
		verticalAlignment: Text.AlignVCenter
		anchors.top : parent.top
	}
	ListModel {
			id: enumModel
	}

	GridView {
		id: view
		cellHeight: labelitem.height+10
		cellWidth: 100
		model: enumModel
		anchors.left: labelitem.right
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.leftMargin: 5
		onCurrentIndexChanged: changed()
		interactive: false
		boundsBehavior: Flickable.StopAtBounds
		snapMode: ListView.SnapToItem
		cacheBuffer: 10
		delegate: Component {
			Rectangle {
				color: model.active ? "lightsteelblue" : "#FFFFFF"
				property alias text: textitem.text
				//radius: 5
				height: GridView.view.cellHeight
				width: GridView.view.cellWidth
				Text {
					anchors.fill: parent
					anchors.margins: 5
					id:textitem
					text: model.text
				}
				MouseArea {
					id: mouseRegion
					anchors.fill: parent
					onClicked: {
						enumModel.setProperty(index, "active", !model.active);
					}
				}
			}
		}
		focus: true
	}
}