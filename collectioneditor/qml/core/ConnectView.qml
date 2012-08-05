import QtQuick 1.1
import "/qml/components"

FocusScope {
	id: container
	anchors.fill: parent
	property bool isChanged
	property bool isLoading: false
	
	Panel {
		title: "Verbindungseinstellung"
		anchors.centerIn: parent
		width: 300
		height: childrenRect.height
		Column {
			y: 20
			spacing: 5
			anchors.left: parent.left
			anchors.right: parent.right
			
			Text {
				id: statetext
				anchors.horizontalCenter: parent.horizontalCenter
				text: "State: unknown";
				font.pixelSize: 14
				opacity: 0.7
			}

			InputLabel {
				id: "host"
				label: "Host"
				labelWidth: 120
				focus: true
				text: "192.168.1.2"
			}
			
			IntLabel {
				id: "portServer"
				labelWidth: 120
				label: "Server Port"
				text:"3101"
			}
			
			Item {
				height: 20
			}
			
			ButtonA {
				id: btnConnect
				text: "Neu verbinden"
				anchors.right: parent.right
				onClicked: connect()
			}
		}
	}
	
	
	Connections  {
		target: database
		onStateChanged: checkStates()
	}
	
	Connections  {
		target: networkController
		onStateChanged: checkStates()
	}
	
	states:
	State {
		name: "invalidinput"
		when: host.text=="" || portServer.text=="" || portDatabase.text==""
		PropertyChanges { target: btnConnect ; enabled: false ; }
	}
	transitions:
	Transition {
		NumberAnimation { target: btnConnect; property: "opacity"; duration: 200 }
	}
	
	function checkStates() {
		// we tried to connect but one of db or server is not responding
		var state = database.state + networkController.state;
		btnConnect.enabled = (state%2==0);
		if (state==4) {
			database.requestCollections();
			database.requestSchemas();
			networkController.requestAllProperties();
			screen.state="CollectionsView";
			return;
		}
		
		statetext.text = "State: "
		if (database.state==0)
			statetext.text += "Database off"
		else if (database.state==1)
			statetext.text += "Database sync"
		else if (database.state==2)
			statetext.text += "Database on"
			
		statetext.text += ", "
		
		if (networkController.state==0)
			statetext.text += "Server off"
		else if (networkController.state==1)
			statetext.text += "Server sync"
		else if (networkController.state==2)
			statetext.text += "Server on"

	}
	
	function loadView() {
		btnConnect.enabled = true;
		var db = openDatabaseSync("roomcontrol.collectioneditor", "1.0", "Connection settings", 1000000);
		db.transaction(function(tx) {
			tx.executeSql('CREATE TABLE IF NOT EXISTS Configuration(key TEXT PRIMARY KEY, value TEXT, UNIQUE(key))');
			var rs = tx.executeSql('SELECT * FROM Configuration WHERE key = "host"');
			if (rs.rows.length)
				host.text = rs.rows.item(0).value;
			var rs = tx.executeSql('SELECT * FROM Configuration WHERE key = "portServer"');
			if (rs.rows.length)
				portServer.text = rs.rows.item(0).value;
		});
		checkStates()
	}
	
	function connect() {
		//btnConnect.enabled = false;
		var db = openDatabaseSync("roomcontrol.collectioneditor", "1.0", "Connection settings", 1000000);
		db.transaction(function(tx) {
			tx.executeSql('DELETE FROM Configuration WHERE key = "host"');
			tx.executeSql('DELETE FROM Configuration WHERE key = "portServer"');
			tx.executeSql('INSERT INTO Configuration VALUES("host","'+host.text+'")');
			tx.executeSql('INSERT INTO Configuration VALUES("portServer","'+portServer.text+'")');
		});
		networkController.connectToServer(host.text, portServer.text);
		database.connectToDatabase(host.text, false);
	}

	Component.onCompleted: loadView()
}