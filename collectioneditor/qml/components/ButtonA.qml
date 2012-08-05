import QtQuick 1.1

Item {
    id: container

    signal clicked

    property string text
    property bool keyUsing: false
    property bool permanentlyPressed: false

	height: btnText.height + 10
	width: btnText.width + 20
	opacity: container.enabled?1:0.3

    BorderImage {
        id: buttonImage
        border { top: 20; bottom: 40; left: 38; right: 38; }
        source: "/qml/images/button.png"
        width: container.width; height: container.height
    }
    BorderImage {
        id: pressed
        opacity: 0
        border { top: 20; bottom: 40; left: 38; right: 38; }
        source: "/qml/images/button_pressed.png"
        width: container.width; height: container.height
    }
    MouseArea {
        id: mouseRegion
        anchors.fill: buttonImage
        onClicked: { container.clicked(); }
    }
    Text {
        id: btnText
        color: if(container.keyUsing){"#D0D0D0";} else {"#FFFFFF";}
        anchors.centerIn: buttonImage; font.bold: true
        text: container.text; style: Text.Raised; styleColor: "black"
        font.pixelSize: 16
    }
    states: [
        State {
            name: "Pressed"
            when: mouseRegion.pressed == true || container.permanentlyPressed == true
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
    }
}
