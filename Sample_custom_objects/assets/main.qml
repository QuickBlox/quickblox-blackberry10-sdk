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

NavigationPane {
    id: navigationPane
    property bool mainPageisShowed: true
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
                    text: "QuickBlox Custom Objects BB10"
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
    }
    property Page coMainPage
    function getCoMainPage() {
        if (! coMainPage) {
            coMainPage = coMainPageDefinition.createObject();
        }
        return coMainPage;
    }
    attachedObjects: [
        ComponentDefinition {
            id: coMainPageDefinition
            source: "COMain.qml"
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
            }
            onSessionEstablished: {
                console.log("onSessionEstablished");
            }
            onLoggedIn: {
                console.log("onLoggedIn");
                requestCO("Note");
            }
            onLoggedOut: {
                console.log("onLoggedOut");
            }
            onUsersReceived: {
                console.log("onUsersReceived");
            }
            onCoReceived: {
                console.log("onCoReceived");
                console.log(coData);
                var page = navigationPane.getCoMainPage();
                page.coDataModel.init(coData);
                navigationPane.push(page);
            }
            onCoAdded: {
                console.log("onCoAdded");
                console.log(coData);
                var page = navigationPane.getCoMainPage();
                page.coDataModel.addCO(coData, "Note");
            }
            onCoDeleted: {
                console.log("onCoDeleted");
                console.log(id);
                var page = navigationPane.getCoMainPage();
                page.coDataModel.deleteCO(id);
            }
            onCoUpdated: {
                console.log("onCoUpdated");
                console.log(coData);
                var page = navigationPane.getCoMainPage();
                page.coDataModel.addCO(coData, "Note");
            }
        }
    ]
    onCreationCompleted: {
        console.log("------------ NavigationPane - onCreationCompleted()");
        // set DisplayPortrait
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
        networkManager.login("emmaemma", "emmaemma");
    }
    onPopTransitionEnded: {
        console.log("onPopTransitionEnded")
        if (page.haveAction) page.preDestroyAction()
        page.destroy();
    }
    onPushTransitionEnded: {
        console.log("onPushTransitionEnded")
        if (top == coMainPage && mainPageisShowed) {
            mainPageisShowed = false
            remove(mainPage)
            backButtonsVisible = true;
            peekEnabled = true;
        }
    }
    onNavigateToTransitionEnded: {
        console.log("onNavigateToTransitionEnded");
    }
    backButtonsVisible: false
    peekEnabled: false
}
