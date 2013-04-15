/*
 * main.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import bb.system 1.0
import com.rim.example.custom 1.0

Page {
    id: mainPage
    Container {
        background: Color.Black
        layout: DockLayout {
        }
        horizontalAlignment: HorizontalAlignment.Fill
        ImageView {
            imageSource: "asset:///images/bg.jpg"
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
        }
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ImageView {
                imageSource: "asset:///images/logo_quickblox.png"
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                text: "QuickBlox Location BB10"
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.color: Color.Black
                textStyle.fontSize: FontSize.Medium
            }
            ActivityIndicator {
                running: true
                preferredWidth: 200.0
                preferredHeight: 200.0
                horizontalAlignment: HorizontalAlignment.Center
            }
        }
        Container {
            topPadding: 25
            bottomPadding: 25
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
            Label {
                text: "v1.0.3"
            }
        }
    } // end the main container
    attachedObjects: [
        Sheet {
            id: mySheet
            content: GeoMain {
                id: geoMainPage
            }
            peekEnabled: false
        },
        SystemToast {
            id: myToast
            body: "Error"
        },
        QBNetwork {
            id: networkManager
            onError: {
                console.log("onError " + info);
                myToast.body = info;
                myToast.show();
                geoMainPage.error(info);
            }
            onSessionEstablished: {
                console.log("onSessionEstablished");
            }
            onLoggedIn: {
                console.log("onLoggedIn");
                console.log(userData);
                geoMainPage.loggedIn(userData);
            }
            onLoggedOut: {
                console.log("onLoggedOut");
            }
            onRegistered: {
                console.log("onRegistered");
                geoMainPage.registered();
            }
            onUsersReceived: {
                console.log("onUsersReceived");
            }
            onGeoDataReceived: {
                console.log("onGeoDataReceived");
                console.log(geoData);
                geoMainPage.geoDataModel.init(geoData);
                mySheet.open()
            }
            onGeoDataCreated: {
                console.log("onGeoDataCreated");
                console.log(geoData);
                geoMainPage.geoDataModel.itemReceived(geoData);
                geoMainPage.successfullCheckin();
            }
        }
    ]
    onCreationCompleted: {
        console.log("------------ NavigationPane - onCreationCompleted()");
        // set DisplayPortrait
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
        networkManager.getGeoData();
    }
}
