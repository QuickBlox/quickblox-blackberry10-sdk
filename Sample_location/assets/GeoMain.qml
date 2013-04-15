import bb.cascades 1.0
import com.rim.example.custom 1.0
import my.library 1.0
import bb.system 1.0

TabbedPane {
    id: self
    property alias geoDataModel: geoDataModel
    showTabsOnActionBar: true
    activeTab: mapTab
    Tab {
        id: mapTab
        title: "Map"
        Page {
            actions: [
                ActionItem {
                    title: "Check In"
                    onTriggered: {
                        self.checkIn()
                    }
                }
            ]
            Container {
                id: map_controller
                background: Color.White
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                layout: DockLayout {
                }
                Container {
                    WebView {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        id: webViewMap
                        url: Qt.resolvedUrl("maps/map.html")
                        settings.zoomToFitEnabled: true
                        onMessageReceived: {
                            console.log("onMessageReceived");
                            console.log(message);
                            // Ensure that we don't double click
                            if (webViewMap.enabled) {
                                //                            messageReceivedFromWebview(message.data);
                                webViewMap.enabled = false;
                            }
                        }
                        onLoadingChanged: {
                            if (loadRequest.status == WebLoadStatus.Succeeded) {

                                //                            statusLabel.setText("Load finished.")
                            }
                        }
                    }
                    Container {
                        id: zoomContainer
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Bottom
                        topPadding: 15.0
                        bottomPadding: 15.0
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        Button {
                            text: "Zoom In"
                            id: zoomInButton
                            onClicked: {
                                mapController.zoomIn();
                            }
                        }
                        Button {
                            text: "Zoom Out"
                            id: zoomOutButton
                            onClicked: {
                                mapController.zoomOut();
                            }
                        }
                    }
                }
                Container {
                    id: loadActivityView
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    visible: false
                    layout: DockLayout {
                    }
                    ActivityIndicator {
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        running: true
                        preferredWidth: 200.0
                        preferredHeight: 200.0
                    }
                }
            }
        }
    }
    Tab {
        id: latestGeoDataTab
        title: "Latest checkins"
        Page {
            Container {
                ListView {
                    property real itemsWidth: _app.displayWidth()
                    id: geoListView
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    dataModel: {
                        geoDataModel
                    }
                    function scrollToTop() {
                        scrollToPosition(0, 0x2);
                    }
                    listItemComponents: [
                        ListItemComponent {
                            type: "geoItem"
                            StandardListItem {
                                preferredWidth: (ListItem.initialized ? ListItem.view.itemsWidth : 0)
                                imageSource: "asset:///images/user.png"
                                title: ListItemData.user.login
                                description: ListItemData.status
                            }
                        }
                    ]
                    // Item type-mapping
                    function itemType(data, indexPath) {
                        return 'geoItem'
                    }
                    attachedObjects: [
                        QTimer {
                            id: timer
                            singleShot: true
                            interval: 100
                            onTimeout: {
                                geoListView.scrollToTop();
                            }
                        }
                    ]
                }
            }
        }
    }
    property bool isLoggedIn: false
    function checkIn() {
        if (! isLoggedIn) {
            console.log("login\register dialog action")
            checkInDialog.show()
        } else {
            console.log("checkin action")
            //checkin
            statusPrompt.show();
        }
    }
    function loggedIn(userData) {
        myToast.body = "Authentication successful."
        myToast.show()
        loadActivityView.visible = false
        geoDataModel.updateCurrentUser(userData);
        isLoggedIn = true
    }
    function registered() {
//        myToast.body = "Registration was successfull.\nPlease now sign in."
//        myToast.show()
//        loadActivityView.visible = false
		//TODO little hacky
        signDialog.loginAction()
    }
    function error(info) {
        loadActivityView.visible = false
        //TODO little hack for this, we show login\registration dialog, when we have not interner connection error
        if (info.indexOf("internet") < 0) {
        	signDialog.open()
        }
    }
    function successfullCheckin() {
        loadActivityView.visible = false
        myToast.body = qsTr("Check in was successfull!\nYour coordinates:\nLatitude: " + geolocator.latitude + "\nLongitude: " + geolocator.longitude) 
        myToast.show()
    }
    property variant pin_data: ""
    attachedObjects: [
        SystemToast {
            id: myToast
        },
        SystemPrompt {
            id: statusPrompt
            title: "Please enter your message"
            inputField.maximumLength: 50
            onFinished: {
                if (result == SystemUiResult.ConfirmButtonSelection && inputFieldTextEntry().trim() != "") {
                    console.log(inputFieldTextEntry());
                    loadActivityView.visible = true
                    networkManager.createGeoData(geolocator.latitude, geolocator.longitude, inputFieldTextEntry());
                }
            }
        },
        SystemDialog {
            id: checkInDialog
            title: "You must first be authorized"
            cancelButton.label: "Registration"
            confirmButton.label: "Cancel"
            customButton.label: "Login"
            onFinished: {
                console.log("system dialog button clicked " + buttonSelection().label);
                if (buttonSelection().label == "Registration") {
                    //                if (buttonSelection() == confirmButton) { - it's not work always :(
                    //Sign Up
                    console.log("Sign Up clicked")
                    signDialog.isLogin = false
                    signDialog.clear();
                    signDialog.open()
                } else if (buttonSelection().label == "Login") {
                    //Sign In
                    console.log("Sign In clicked")
                    signDialog.isLogin = true
                    signDialog.clear();
                    signDialog.open()
                }
            }
        },
		AuthDialog {
		    id: signDialog
		    onActionStarted: {
                loadActivityView.visible = true
            }
            onOpenedChanged: {
                zoomContainer.visible = !opened
            }
        },
        GeoDataModel {
            id: geoDataModel
            grouping: ItemGrouping.None
            sortedAscending: false
            sortingKeys: [
                "createdAt"
            ]
            function pinsUpdateAction(indexPath) {
                mapController.setPinsClickable(false);
                //restart pins adding timer
                pinsTimer.stop();
                pinsTimer.start();
                //add new item data to pin_data string
                var itemData = data(indexPath);
                var lat = itemData.latitude;
                var lot = itemData.longitude;
                var status = itemData.user.login + ": " + itemData.status;
                if (self.pin_data) {
                    self.pin_data += ","
                }
                var lat = itemData.latitude;
                var lot = itemData.longitude
                var status = itemData.user.login + ": " + itemData.status;
                self.pin_data += '{ ' + '"latitude" : ' + lat + ", " + '"longitude" : ' + lot + ", " + '"pinId" : ' + '"' + status + '"' + ' }';
            }
            onItemAdded: {
                //restart  scroll to top timer
                timer.stop()
                timer.start()
                
                pinsUpdateAction(indexPath);
            }
            onItemUpdated: {
                pinsUpdateAction(indexPath);
            }
        },
        MapController {
            id: mapController
        },
        Geolocator {
            id: geolocator
            property bool alreadyUpdated: false
            onPositionUpdated: {
                console.log("onPositionUpdated latitude=" + latitude + " longitude=" + longitude);
                geoTimer.start()
                //                if (! alreadyUpdated) {
                //                    alreadyUpdated = true;
                //                    mapController.setCenter(latitude, longitude, 7);
                //                }
            }
            onPositionUpdatedTimeout: {
                console.log("onPositionUpdatedTimeout");
            }
        },
        QTimer {
            id: geoTimer
            interval: 1000
            singleShot: true
            onTimeout: {
                //                console.log("geoTimer is on fire latitude=" + geolocator.latitude + " longitude=" + geolocator.longitude);
                mapController.setCenter(geolocator.latitude, geolocator.longitude, 9);
            }
        },
        QTimer {
            id: pinsTimer
            interval: 300
            singleShot: true
            onTimeout: {
                console.log("pinsTimer is on fire");
                if (self.pin_data) {
                    var out = '{"pins": [' + self.pin_data + ']}';
                    self.pin_data = "";
                    mapController.setMapPins(out);
                    mapController.setPinsClickable(true);
                    //                    geoTimer.start()
                }
            }
        }
    ]
    onCreationCompleted: {
        webViewMap.enabled = true; // ensure pins can be clicked
        mapController.setWebView(webViewMap);
        mapController.setPinsClickable(true);
    }
}
