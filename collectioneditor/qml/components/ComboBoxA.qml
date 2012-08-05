import QtQuick 1.1

Item {
    id: container
    property string defaulttext: "--Nichts--"
    property alias model: view.model
    property string roleText: "text"
	property alias currentIndex: view.currentIndex
	property int __modelCount: view.model?( view.model.count?view.model.count:(view.model.length?view.model.length:0) ) : 0;
	property int __visibleItems: Math.min(8, container.__modelCount)
	height: btnText.height + 10
	width: btnText.width + 20
	z: popup.opacity ? 10 : 5

    BorderImage {
        id: buttonImage
        border { top: 11; bottom: 40; left: 38; right: 38; }
        source: "/qml/images/button.png"
        width: container.width; height: container.height
    }
    BorderImage {
        id: pressed
        opacity: 0
        border { top: 11; bottom: 40; left: 38; right: 38; }
        source: "/qml/images/button_pressed.png"
        width: container.width; height: container.height
    }
    MouseArea {
        id: mouseRegion
        anchors.fill: buttonImage
        onClicked: {
			if (container.__modelCount == 0) {
				popup.opacity = 0
				return;
			}
			popup.opacity = popup.opacity ? 0 : 1
		}
    }
    Text {
        id: btnText
        color: if(container.keyUsing){"#D0D0D0";} else {"#FFFFFF";}
        anchors.centerIn: buttonImage; font.bold: true
        style: Text.Raised; styleColor: "black"
        font.pixelSize: 16
        text: view.currentIndex==-1 ? container.defaulttext : view.currentItem.text
    }
    Rectangle {
		id: popup
		clip: true
		color: "#D0D0D0"
		opacity: 0
		x: 5
		width: container.width - 10
		y: container.height + 5
		height: container.height * container.__visibleItems
// 		radius: 5
// 		Image {
// 			id: background
// 			anchors.fill: parent
// 			source: "/qml/images/stripes.png"
// 			fillMode: Image.Tile
// 		}

		ListView {
			id: view
			interactive: container.__visibleItems < container.__modelCount
			boundsBehavior: Flickable.StopAtBounds
			snapMode: ListView.SnapToItem
			cacheBuffer: 10
			anchors.fill: parent
			delegate: Component {
				Rectangle {
					color: ListView.isCurrentItem ? "lightsteelblue" : "#FFFFFF"
					property alias text: textitem.text
					height: container.height
					width: popup.width
					//radius: 5
					Text {
						anchors.fill: parent
						anchors.margins: 5
						id:textitem
						text: typeof(modelData)!="undefined"?modelData:model[container.roleText]
					}
					MouseArea {
						id: mouseRegion
						anchors.fill: parent
						onClicked: {
							popup.opacity = 0
							view.currentIndex = index
						}
					}
				}
			}
			focus: true
		}
	}
    states: [
        State {
            name: "Pressed"
            when: mouseRegion.pressed == true
            PropertyChanges { target: pressed; opacity: 1 }
        },
        State {
            name: "Focused"
            when: container.activeFocus == true
            PropertyChanges { target: btnText; color: "#FFFFFF" }
        }
    ]
    transitions: Transition {
        ColorAnimation { target: btnText; }
        NumberAnimation { target: popup; properties: "opacity"; duration: 300; easing.type: Easing.Linear }
    }
}
