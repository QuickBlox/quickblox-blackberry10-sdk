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
    Page {
        id: mainPage
        Container {
            minHeight: {
                _app.displayHeight()
            }
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
                layout: StackLayout {
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
                        text: "QuickBlox Chat BB10"
                        textStyle.color: Color.Black
                        textStyle.fontSize: FontSize.XLarge
                    }
                }
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    layout: StackLayout {
                    }
                    leftPadding: 20
                    rightPadding: 20
                    bottomMargin: 20
                    // container for login
                    Container {
                        id: containerLogin
                        preferredWidth: 720
                        bottomMargin: 20
                        layout: DockLayout {
                        }
                        Label {
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Left
                            text: "Login"
                            textStyle.color: Color.Black
                            textStyle.fontSize: FontSize.Large
                        }
                        Container {
                            preferredWidth: 525
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Right
                            layout: DockLayout {
                            }
                            TextField {
                                id: textFieldLogin
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                preferredWidth: 500
                            }
                        }
                    } // end container for login
                    // container for password
                    Container {
                        id: containerPassword
                        preferredWidth: 720
                        bottomMargin: 20
                        layout: DockLayout {
                        }
                        Label {
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Left
                            text: "Password"
                            textStyle.color: Color.Black
                            textStyle.fontSize: FontSize.Large
                        }
                        Container {
                            preferredWidth: 525
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Right
                            layout: DockLayout {
                            }
                            TextField {
                                id: textFieldPassword
                                inputMode: TextFieldInputMode.Password
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                preferredWidth: 500
                            }
                        }
                    } // end container for password
                }
                // container for buttons
                Container {
                    id: containerButtons
                    topMargin: 50
                    bottomMargin: 0.0
                    leftPadding: 20
                    rightPadding: 20
                    verticalAlignment: VerticalAlignment.Bottom
                    horizontalAlignment: HorizontalAlignment.Center
                    layout: StackLayout {
                        orientation: LayoutOrientation.BottomToTop
                    }
                    // container for button register
                    Container {
                        topMargin: 50
                        bottomMargin: 0.0
                        preferredHeight: 70
                        preferredWidth: 300
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {
                        }
                        ImageButton {
                            id: buttonRegister
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            defaultImageSource: "asset:///images/smbutton1.png"
                            pressedImageSource: "asset:///images/smbutton1.png"
                            onClicked: {
                                if (! validEmpty()) {
                                    myToast.body = "Error: login or password cannot be blank!";
                                    myToast.show();
                                } else if (! validLogin()) {
                                    myToast.body = "Error: login should contain alphanumeric and punctuation characters only!";
                                    myToast.show();
                                } else if (! validPassword()) {
                                    myToast.body = "Error: password should contain alphanumeric and punctuation characters only!";
                                    myToast.show();
                                } else if (! lenPass8Characters()) {
                                    myToast.body = "Error: password must be minimum 8 characters!";
                                    myToast.show();
                                } else if (! lenPass40Characters()) {
                                    myToast.body = "Error: your password must not exceed 40 characters!";
                                    myToast.show();
                                } else if (! lenLogin2Characters()) {
                                    myToast.body = "Error: your login must have at least 2 characters!";
                                    myToast.show();
                                } else if (! lenLogin50Characters()) {
                                    myToast.body = "Error: your login must not exceed 50 characters!";
                                    myToast.show();
                                } else {
                                    buttonRegister.enabled = false;
                                    networkManager.registerUser(textFieldLogin.text, textFieldPassword.text);
                                }
                            }
                        }
                    } // end container for button register
                    // container for button login
                    Container {
                        preferredHeight: 70
                        preferredWidth: 300
                        topMargin: 0.0
                        bottomMargin: 0.0
                        layout: DockLayout {
                        }
                        ImageButton {
                            id: buttonLogin
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            defaultImageSource: "asset:///images/logregbtn1.png"
                            pressedImageSource: "asset:///images/logregbtn1.png"
                            onClicked: {
                                if (! validEmpty()) {
                                    myToast.body = "Error: login or password cannot be blank!";
                                    myToast.show();
                                } else {
                                    networkManager.login(textFieldLogin.text, textFieldPassword.text);
                                }
                            }
                        }
                    } // end container for button login
                } // end container for buttons
            } //
            Container {
                id: activityView
                visible: false
                background: Color.create("#30FFFFFF")
                preferredHeight: {
                    _app.displayHeight()
                }
                preferredWidth: {
                    _app.displayWidth()
                }
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {
                }
                ActivityIndicator {
                    running: true
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: 200.0
                    preferredHeight: 200.0
                }
            }
            Container {
                topPadding: 25
                bottomPadding: 25
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                Label {
                    text: "v1.0.12"
                }
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
            source: "UsersPage.qml"
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
                buttonRegister.enabled = true;
            }
            onSessionEstablished: {
                console.log("onSessionEstablished");
            }
            onLoggedIn: {
                console.log("onLoggedIn");
                console.log("user id " + userId());
                // show detail page when logged in
                var upage = navigationPane.getSecondPage();
                console.debug("pushing detail " + upage)
                navigationPane.push(upage);
                networkManager.requestUsers();
            }
            onLoggedOut: {
                console.log("onLoggedOut");
                textFieldLogin.text = ""
                textFieldPassword.text = ""
                initialUsers = true
                _chat.logout();
            }
            onRegistered: {
                console.log("onRegistered");
                myToast.body = "User was successfully registered";
                myToast.show();
                buttonRegister.enabled = true;
                networkManager.login(textFieldLogin.text, textFieldPassword.text);
            }
            property bool initialUsers: true
            onUsersReceived: {
                console.log("onUsersReceived");
                navigationPane.getSecondPage().usersReceived(usersData, initialUsers)
                if (initialUsers) {
                    initialUsers = false
                    _chat.loggedIn.connect(navigationPane.onChatLoggedIn);
                	_chat.login(networkManager.userId(), networkManager.userPassword());
                }
            }
            onLoadActionChanged: {
                activityView.visible = loadAction;
            }
        }
    ]
    function onChatLoggedIn() {
        _chat.loggedIn.disconnect(navigationPane.onChatLoggedIn);
        navigationPane.getSecondPage().loadingFinished();
    }
    onCreationCompleted: {
        console.log("------------ NavigationPane - onCreationCompleted()");
        // set DisplayPortrait
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
    }
    onPopTransitionEnded: {
        console.log("onPopTransitionEnded page " + page)
        buttonLogin.enabled = true;
        buttonRegister.enabled = true;
        if (page.havePopAction) page.preDestroyAction()
        page.destroy();
        if (navigationPane.top == mainPage && page == secondPage) networkManager.logout();
    }
    onPushTransitionEnded: {
        if (page.havePushAction) page.postPushAction()
    }
    onTopChanged: {
        if (page == secondPage || page == mainPage) {
            navigationPane.backButtonsVisible = false;
//            navigationPane.peekEnabled = false;
        } else {
            navigationPane.backButtonsVisible = true;
//            navigationPane.peekEnabled = true;
        }
    }
    backButtonsVisible: false
    peekEnabled: false    
    function validLogin() {
        var login = textFieldLogin.text;
        return (/^[a-zA-Z0-9,.]+$/.test(login));
    }
    function validPassword() {
        var password = textFieldPassword.text;
        return (/^[a-zA-Z0-9,.]+$/.test(password));
    }
    function validEmpty() {
        var success = false;
        if (textFieldLogin.text.trim() == "" || textFieldPassword.text.trim() == "") {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass8Characters() {
        var success = false;
        if (textFieldPassword.text.trim().length < 8) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass40Characters() {
        var success = false;
        if (textFieldPassword.text.trim().length > 40) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin2Characters() {
        var success = false;
        if (textFieldLogin.text.trim().length < 2) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin50Characters() {
        var success = false;
        if (textFieldLogin.text.trim().length > 50) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
}
