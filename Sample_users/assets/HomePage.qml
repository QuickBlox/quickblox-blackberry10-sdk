/*
 * HomePage.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import bb.system 1.0
import com.example.Sample_users.QBNetwork 1.0

NavigationPane {
    id: navigationPane
    //backButtonsVisible: false
    Page {
        Container {
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
                id: containerContent
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
                        text: "QuickBlox Users BB10"
                        textStyle.color: Color.Black
                        textStyle.fontSize: FontSize.XLarge
                    }
                }
                Container {
                    id: containerInput
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
                                text: "default"
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
                                text: "11111111"
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
                        id: buttonRegister
                        topMargin: 50
                        bottomMargin: 0.0
                        preferredHeight: 70
                        preferredWidth: 300
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        layout: DockLayout {
                        }
                        ImageButton {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            defaultImageSource: "asset:///images/smbutton1.png"
                            pressedImageSource: "asset:///images/smbutton1.png"
                            onClicked: {
                                if (! validEmpty()) {
                                    myToast.body = "Error: login or password cannot be blank";
                                    myToast.show();
                                } else if (! validSpace()) {
                                    myToast.body = "Error: incorrect login or password";
                                    myToast.show();
                                } else if (! validLogin()) {
                                    myToast.body = "Error: login should contain alphanumeric and punctuation characters only";
                                    myToast.show();
                                } else if (! validPassword()) {
                                    myToast.body = "Error: password should contain alphanumeric and punctuation characters only";
                                    myToast.show();
                                } else if (! lenPass8Characters()) {
                                    myToast.body = "Error: password must be minimum 8 characters";
                                    myToast.show();
                                } else if (! lenPass40Characters()) {
                                    myToast.body = "Error: your password must not exceed 40 characters";
                                    myToast.show();
                                } else if (! lenLogin2Characters()) {
                                    myToast.body = "Error: your login must have at least 2 characters";
                                    myToast.show();
                                } else if (! lenLogin50Characters()) {
                                    myToast.body = "Error: your login must not exceed 50 characters";
                                    myToast.show();
                                } else {
                                    qbnetwork.setCredentials(textFieldLogin.text, textFieldPassword.text);
                                    qbnetwork.requestSessionRegister();
                                    loadIndicator.visible = qbnetwork.loading;
                                    containerContent.enabled = false;
                                }
                            }
                        }
                    } // end container for button register
                    // container for button login
                    Container {
                        id: buttonLogin
                        preferredHeight: 70
                        preferredWidth: 300
                        topMargin: 0.0
                        bottomMargin: 0.0
                        layout: DockLayout {
                        }
                        ImageButton {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            defaultImageSource: "asset:///images/logregbtn1.png"
                            pressedImageSource: "asset:///images/logregbtn1.png"
                            onClicked: {
                                if (! validEmpty()) {
                                    myToast.body = "Error: login or password cannot be blank";
                                    myToast.show();
                                } else {
                                    qbnetwork.setCredentials(textFieldLogin.text, textFieldPassword.text);
                                    qbnetwork.requestSessionWithLogin();
                                    loadIndicator.visible = qbnetwork.loading;
                                    containerContent.enabled = false;
                                }
                            }
                        }
                    } // end container for button login
                } // end container for buttons
            } //
            ActivityIndicator {
                id: loadIndicator
                visible: false
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: 300
                running: qbnetwork.loading
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
    function validSpace() {
        if (textFieldLogin.text == " " || textFieldPassword.text == " ") {
            return false;
        } else {
            return true;
        }
    }
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
        if (textFieldLogin.text == "" || textFieldPassword.text == "") {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass8Characters() {
        var success = false;
        if (textFieldPassword.text.length < 8) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass40Characters() {
        var success = false;
        if (textFieldPassword.text.length > 40) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin2Characters() {
        var success = false;
        if (textFieldLogin.text.length < 2) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin50Characters() {
        var success = false;
        if (textFieldLogin.text.length > 50) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    attachedObjects: [
        ComponentDefinition {
            id: secondPageDefinition
            source: "UsersPage.qml"
        },
        SystemToast {
            id: myToast
        },
        QBNetwork {
            id: qbnetwork
            onCompleteLogin: {
                if (! qbnetwork.unauthorized) {
                    containerContent.enabled = true;
                    var page = getSecondPage();
                    console.debug("pushing detail " + page)
                    navigationPane.push(page);
                }
                _app.setObjectQBNetwork(qbnetwork);
                loadIndicator.visible = qbnetwork.loading;
                containerContent.enabled = true;
            }
        }
    ]
    onCreationCompleted: {
        console.log("------------ NavigationPane - onCreationCompleted()");
        // set DisplayPortrait
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
    }
    onPopTransitionEnded: {
        textFieldPassword.text = ""
        textFieldLogin.text = ""
        page.destroy();
    }
}
