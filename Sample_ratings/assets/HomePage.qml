/*
 * HomePage.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import bb.system 1.0
import com.example.Sample_ratings.QBAuth 1.0

NavigationPane {
    id: navigationPane
    backButtonsVisible: false
    peekEnabled: false
    Page {
        Container {
            id: mainContainer
            minHeight: _app.displayHeight()
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
                id: logo
                topPadding: 50
                bottomPadding: 50
                horizontalAlignment: HorizontalAlignment.Center
                ImageView {
                    imageSource: "asset:///images/logo_quickblox.png"
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    text: "QuickBlox Ratings BB10"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.XLarge
                }
            }
            ActivityIndicator {
                id: loadIndicator
                visible: false
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: 300
                running: qbauth.loading
            }
        } // end the main container
    }
    property Page secondPage
    function getSecondPage() {
        if (! secondPage) {
            secondPage = secondPageDefinition.createObject();
        }
        return secondPage;
    }
    attachedObjects: [
        ComponentDefinition {
            id: secondPageDefinition
            source: "RatingsPage.qml"
        },
        SystemToast {
            id: myToast
        },
        QBAuth {
            id: qbauth
            onCompleteLogin: {
                _app.setObjectQBNetwork(qbauth);
                if (! qbauth.unauthorized && ! qbauth.loading) {
                    //containerContent.enabled = true;
                    var page = getSecondPage();
                    console.debug("pushing detail " + page)
                    navigationPane.push(page);
                }
                loadIndicator.visible = qbauth.loading;
                //containerContent.enabled = true;
            }
        }
    ]
    onCreationCompleted: {
        qbauth.setCredentials("", "");
        qbauth.requestSessionWithLogin();
        loadIndicator.visible = qbauth.loading;
        // set DisplayPortrait
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
    }
    onPopTransitionEnded: {
        page.destroy();
        _app.createModel();
                qbauth.setCredentials("", "");
                qbauth.requestSessionWithLogin();
                loadIndicator.visible = qbauth.loading;
    }
}
