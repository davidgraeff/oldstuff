import QtQuick 1.1
import "qmlprivate.js" as P
import "/qml/components"

FocusScope {
	id: container
	anchors.fill: parent
	property variant collectionobject: null
	property variant serviceobject: null
	property bool isChanged
	property bool isLoading: false
	property Component componentInputLabel: Qt.createComponent("/qml/components/InputLabel.qml");
	property Component componentIntLabel: Qt.createComponent("/qml/components/IntLabel.qml");
	property Component componentDoubleLabel: Qt.createComponent("/qml/components/DoubleLabel.qml");
	property Component componentBoolLabel: Qt.createComponent("/qml/components/BoolLabel.qml");
	property Component componentEnumLabel: Qt.createComponent("/qml/components/EnumLabel.qml");
	property Component componentMultipleSelectLabel: Qt.createComponent("/qml/components/MultipleSelectLabel.qml");

	Column {
		spacing: 5
		anchors.fill: parent
		
		Toolbar {
			id: toolbar
			textLeft:"Zurück"
			textRight:container.serviceobject ? ((container.isChanged)?"Speichern":"Keine Änderung") : "Erstellen"
			buttonRight.enabled: container.isChanged && selectedPlugin.currentIndex!=-1 && selectedType.currentIndex!=-1 && selectedAction.currentIndex!=-1
			onButtonLeftClicked: {screen.state="ServicesView";}
			onButtonRightClicked: {
				if (isChanged) {
					isChanged = false;
					saveService();
				}
			}
		}
		
		EnumLabel {
			id: selectedPlugin;
			label: "Plugin:"
			labelWidth: 70
			z: 12
			model: selectedPluginModel
			width: parent.width;
			KeyNavigation.tab: selectedType
			KeyNavigation.backtab: btnRemove
			onCurrentIndexChanged: {
				if (container.isLoading)
					return;
				container.isChanged = true;
				
				selectedTypeModel.clear();
				selectedActionModel.clear();
				freeParameterItems();
				
				// get current item
				var o = selectedPluginModel.get(selectedPlugin.currentIndex);
				if (!o)
					return;

				// go through all remaining schemas and collect all possible types ("action", "condition" etc)
				// and insert them into the next comboBox model
				var schemas = o.data;
				var z = schemas.count;
				var dataMap = {};
				for (var i=0;i<z;++i) {
					var o = schemas.get(i);
					if (dataMap[o.targettype] == null)
						dataMap[o.targettype] = [];
					dataMap[o.targettype].push(o);
				}
				var data = []; z=0;
				for (var o in dataMap) {
					data[z++] = o;
				}
				var selectedEntry = -1;
				container.isLoading = true;
				for (var i=0;i<data.length;++i) {
					if (container.serviceobject && container.serviceobject.type_ == data[i])
						selectedEntry = i;
					selectedTypeModel.append({"text":data[i], "data":dataMap[data[i]]})
				}
				container.isLoading = false;
				// set current index for selectedType
				selectedType.currentIndex = -1;
				selectedType.currentIndex = selectedEntry;
			}
		}
		EnumLabel {
			id: selectedType;
			label: "Typ:"
			labelWidth: 70
			z: 11
			model: selectedTypeModel
			width: parent.width;
			KeyNavigation.tab: selectedAction
			KeyNavigation.backtab: selectedPlugin
			onCurrentIndexChanged: {
				if (container.isLoading)
					return;
				container.isChanged = true;
				
				selectedActionModel.clear();
				freeParameterItems();
				
				// get current item
				var o = selectedTypeModel.get(selectedType.currentIndex);
				if (!o)
					return;
				
				// go through all schemas and collect all possible actions which fit to the selected plugin and type
				// and insert them into the action-combobox-model
				var schemas = o.data;
				if (schemas && schemas.count) {
					actiondelay.opacity = (schemas.get(i).targettype == "action")?1:0;
				}
				var z = schemas.count;
				var dataMap = {};
				for (var i=0;i<z;++i) {
					var o = schemas.get(i);
					if (dataMap[o.member_] == null)
						dataMap[o.member_] = [];
					dataMap[o.member_].push(o);
				}
				var data = []; z=0;
				for (var o in dataMap) {
					data[z++] = o;
				}
				var selectedEntry = -1;
				container.isLoading = true;
				for (var i=0;i<data.length;++i) {
					if (container.serviceobject && container.serviceobject.member_ == data[i])
						selectedEntry = i;
					selectedActionModel.append({"text":data[i], "data":dataMap[data[i]]})
				}
				container.isLoading = false;
				
				// set current index for selectedAction
				selectedAction.currentIndex = -1;
				selectedAction.currentIndex = selectedEntry;
			}
		}
		EnumLabel {
			id: selectedAction;
			label: "Aktion:"
			labelWidth: 70
			z: 10
			model: selectedActionModel
			width: parent.width;
			KeyNavigation.tab: parameters
			KeyNavigation.backtab: selectedType
			onCurrentIndexChanged: {
				if (container.isLoading)
					return;
				container.isChanged = true;
				
				freeParameterItems();

				var o = selectedActionModel.get(selectedAction.currentIndex);
				if (!o)
					return;

				// o.data contains only one element that is the schema associated with
				// the selected plugin+type+action
				createParameterItems(o.data.get(0));
			}
		}
		
		InputLabel {
			id: instanceid
			label: "Instance ID"
			labelWidth: 70
			text: "0"
			onTextChanged: container.isChanged = true;
		}
		
		IntLabel {
			id: actiondelay;
			label: "Verzögerung:"
			labelWidth: 70
			text: "0"
			opacity: 0
			onTextChanged: container.isChanged = true;
		}
		
		Column {
			id: parameters
			spacing: 5
			anchors.left: parent.left
			anchors.right: parent.right
			height: childrenRect.height
		}
		
	}
	
	Item {
		anchors.left : parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: childrenRect.height
		ButtonA {
			id: btnExecute
			text: "Ausführen"
			anchors.right: btnRemove.left
			KeyNavigation.tab: btnRemove
			KeyNavigation.backtab: toolbar
			onClicked: {
				if (!container.serviceobject || container.serviceobject.type_!="action")
					return;
				var o = qmlhelper.deepcopy(container.serviceobject);
				o.type_="execute";
				networkController.write(o);
			}
		}
		
		ButtonA {
			id: btnRemove
			text: "Löschen"
			anchors.right: parent.right
			KeyNavigation.tab: selectedPlugin
			KeyNavigation.backtab: btnExecute
			onClicked: {
				database.removeDocument(container.serviceobject.type_, container.serviceobject._id);
				screen.state="ServicesView";
			}
		}
	}
	

	ListModel {
		id: selectedPluginModel
	}
	ListModel {
		id: selectedTypeModel
	}
	ListModel {
		id: selectedActionModel
	}
	
	Connections {
		target: actioncontroller
		onNotification: { // parameter: plugin: string, value: map
			var o = (P.priv("ServiceView").parameterNotification && P.priv("ServiceView").parameterNotification[value.id])
				? P.priv("ServiceView").parameterNotification[value.id]:null;
			if (o && value[o.textproperty]) {
				o.paramobj.text = value[o.textproperty];
				console.log("notify ok:", value[o.textproperty]);
			}
		}
	}
	
	function createParameterItems(schema) {
		var parameterItemZ = 10;
		for (var paramname in schema.parameters) {
			--parameterItemZ;
			var param = schema.parameters[paramname];
			var name = param.name?param.name:paramname;
			var obj;
			if (param.type == "string") {
				obj = componentInputLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"text": container.serviceobject?container.serviceobject[paramname]:"",
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "date") {
				obj = componentInputLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"text": container.serviceobject?container.serviceobject[paramname]:"",
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "time") {
				obj = componentInputLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"text": container.serviceobject?container.serviceobject[paramname]:"",
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "integer") {
				obj = componentIntLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"text": container.serviceobject?container.serviceobject[paramname]:"",
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "double") {
				obj = componentDoubleLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"text": container.serviceobject?container.serviceobject[paramname]:"",
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "boolean") {
				if (componentBoolLabel.status != Component.Ready) {
					console.log("Component bool not ready"); continue;
				}
				var obj = componentBoolLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"currentIndex": container.serviceobject?container.serviceobject[paramname]:-1,
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "enum") {
				obj = componentEnumLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"currentIndex": container.serviceobject?container.serviceobject[paramname]:-1,
					"model": param.data,
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "multienum") {
				obj = componentMultipleSelectLabel.createObject(parameters, {"label": name+":", "labelWidth": 70,
					"z": parameterItemZ
				});
				obj.setData(param.data, container.serviceobject?container.serviceobject[paramname]:[]);
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else if (param.type == "modelenum") {
				// bind enum to notification-model
				if (!param.model)
					continue;
				
				var m = actioncontroller.getModel(param.model);
				if (!m) {
					console.log("modelenum model not found!", param.model);
					continue;
				}

				obj = componentEnumLabel.createObject(parameters, {"label": name +":", "labelWidth": 70,
					"model": m,
					"currentIndex":m.indexOf(container.serviceobject?container.serviceobject[paramname]:""),
					"z": parameterItemZ
				});
				obj.changed.connect( function() {container.isChanged = true;} )
				P.priv("ServiceView").parameterItems[paramname] = obj;
			} else {
				obj = null;
				console.log("Type not supported!", param.type);
			}
			// bind text to notifications
			if (param.notification) {
				var paramNotiObj = {};
				paramNotiObj.indexproperty = param.indexproperty?param.indexproperty:paramname;
				paramNotiObj.textproperty = param.textproperty?param.textproperty:paramname;
				paramNotiObj.paramobj = obj;
				P.priv("ServiceView").parameterNotification[param.notification] = paramNotiObj;
			}
		}
	}
	
	function freeParameterItems() {
		var parameterItems = P.priv("ServiceView").parameterItems;
		if (parameterItems) {
			//console.log("REMOVE OLD PARAM OBJECTS", parameterItems.length);
			for (var paramname in parameterItems) {
				parameterItems[paramname].destroy(0)
			}
		}
		P.priv("ServiceView").parameterItems = {};
		P.priv("ServiceView").parameterNotification = {};
	}
	
	function loadView(collobj, serviceobj) {
		container.collectionobject = collobj;
		container.serviceobject = serviceobj;
		// tidy up
		selectedPluginModel.clear();
		selectedTypeModel.clear();
		selectedActionModel.clear();
		freeParameterItems();
		
		componentInputLabel.statusChanged.connect(loadIfComponentsReady);
		componentIntLabel.statusChanged.connect(loadIfComponentsReady);
		componentDoubleLabel.statusChanged.connect(loadIfComponentsReady);
		componentBoolLabel.statusChanged.connect(loadIfComponentsReady);
		componentEnumLabel.statusChanged.connect(loadIfComponentsReady);
		componentMultipleSelectLabel.statusChanged.connect(loadIfComponentsReady);
		loadIfComponentsReady();
	}
	
	function loadIfComponentsReady() {
		if (componentInputLabel.status!=Component.Ready ||
			componentIntLabel.status!=Component.Ready ||
			componentDoubleLabel.status!=Component.Ready ||
			componentBoolLabel.status!=Component.Ready ||
			componentEnumLabel.status!=Component.Ready ||
			componentMultipleSelectLabel.status!=Component.Ready
		) {
			console.log("Error loading component:", componentInputLabel.errorString()+
				componentIntLabel.errorString()+componentDoubleLabel.errorString()+
				componentBoolLabel.errorString()+componentEnumLabel.errorString()+
				componentMultipleSelectLabel.errorString()
			);
			return;
		}

		// fill plugin model
		container.isLoading = true;
		var z = schemaModel.rowCount();
		var pluginsMap = {};
		for (var i=0;i<z;++i) {
			var o = schemaModel.getValueByIndex(i);
			if (pluginsMap[o.plugin_] == null)
				pluginsMap[o.plugin_] = [];
			pluginsMap[o.plugin_].push(o);
		}
		var plugins = []; z=0;
		for (var o in pluginsMap) {
			plugins[z++] = o;
		}
		var selectedEntry = -1;
		for (var i=0;i<plugins.length;++i) {
			if (container.serviceobject && container.serviceobject.plugin_ == plugins[i])
				selectedEntry = i;
 			selectedPluginModel.append({"text":plugins[i], "data": pluginsMap[plugins[i]]})
		}
		
		if (container.serviceobject && container.serviceobject.instanceid_)
			instanceid.text = container.serviceobject.instanceid_;
		
		container.isLoading = false;
		// set current index for selectedPlugin
		selectedPlugin.currentIndex = -1;
		selectedPlugin.currentIndex = selectedEntry;
		container.isChanged = false;
	}
	
	function saveService() {
		// get schema
		var schema = selectedActionModel.get(selectedAction.currentIndex).data.get(0);
		
		var o = {"type_":schema.targettype,
		"plugin_":schema.plugin_,
		"instanceid_":instanceid.text,
		"member_":schema.member_,
		"collection_":collectionobject._id};
		var o_types = {};
		
		// add delay property for action service
		if (schema.targettype == "action") {
			o["delay_"] = parseInt(actiondelay.text);
		}
		// parameters
		for (var paramname in schema.parameters) {
			var param = schema.parameters[paramname];
			var obj = P.priv("ServiceView").parameterItems[paramname];
			
			if (!obj) {
				console.log("No object for", paramname);
				continue;
			}
			
			if (param.type == "string") {
				o[paramname] = obj.text;
				o_types[paramname] = "QString";
				
			} else if (param.type == "date") {
				o[paramname] = obj.text;
				o_types[paramname] = "QString";
				
			} else if (param.type == "time") {
				o[paramname] = obj.text;
				o_types[paramname] = "QString";
				
			} else if (param.type == "integer") {
				o[paramname] = parseInt(obj.text);
				o_types[paramname] = "int";
				
			} else if (param.type == "double") {
				o[paramname] = parseFloat(obj.text);
				o_types[paramname] = "double";
				
			} else if (param.type == "boolean") {
				o[paramname] = obj.currentIndex?true:false;
				o_types[paramname] = "bool";
				
			} else if (param.type == "enum") {
				o[paramname] = obj.currentIndex;
				o_types[paramname] = "int";
				
			} else if (param.type == "multienum") {
				o[paramname] = obj.getSelectedIndices();
				o_types[paramname] = "QVariantList";
				
			} else if (param.type == "modelenum") {
				o[paramname] = obj.model.get(obj.currentIndex)[param.indexproperty];
				o_types[paramname] = "QString";
			} else {
				console.log("Type not supported!", param.type, paramname);
			}
			
		}

// 			console.debug(JSON.stringify(o));
// 			console.debug(JSON.stringify(container.serviceobject));
		
		if (container.serviceobject) { // alter existing object and save it
			o._id = container.serviceobject._id;
			database.changeDocument(o, false, o_types);
		} else { // create new collection object and save it
			console.debug(JSON.stringify(o));
			database.changeDocument(o, true, o_types);
			screen.state="ServicesView";
		}
	}
}