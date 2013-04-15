import bb.cascades 1.0
import bb.system 1.0

Dialog {
    id: self
    property bool isLogin: true
    function clear() {
        loginTextField.text = "";
        passwordTextField.text = "";
    }
    function registerAction() {
        networkManager.registerUser(loginTextField.text, passwordTextField.text);
    }
    function loginAction() {
        networkManager.login(loginTextField.text, passwordTextField.text);
    }
    signal actionStarted;
    Container {
        horizontalAlignment: HorizontalAlignment.Center
        topPadding: 200.0
        layout: DockLayout {
        }
        ImageView {
            imageSource: "asset:///images/dialog_bg.png"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        Container {
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {
                }
                ImageView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    imageSource: "asset:///images/topbar.png"
                }
                Label {
                    id: titleLabel
                    text: "Login"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.color: Color.White
                }
            }
            Container {
                leftPadding: 15.0
                rightPadding: 15.0
                topPadding: 15.0
                bottomPadding: 15.0
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    TextField {
                        horizontalAlignment: HorizontalAlignment.Fill
                        id: loginTextField
                        hintText: "Login"
//                        validator: Validator {
//                            mode: ValidationMode.Immediate
//                            errorMessage: "Your username must be 10 characters or fewer."
//                            onValidate: {
//                                if (loginTextField.text.length <= 10) state = ValidationState.Valid; else state = ValidationState.Invalid;
//                            }
//                        }
                    }
                    TextField {
                        horizontalAlignment: HorizontalAlignment.Fill
                        id: passwordTextField
                        hintText: "Password"
                        inputMode: TextFieldInputMode.Password
                    }
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    topMargin: 15.0
                    horizontalAlignment: HorizontalAlignment.Center
                    Button {
                        text: "Cancel"
                        onClicked: self.close()
                    }
                    Button {
                        id: confirmButton
                        text: "Sign In"
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
                                self.actionStarted()
                                //                            loadActivityView.visible = true
                                self.close()
                                if (self.isLogin) {
                                    self.loginAction();
                                } else {
                                    self.registerAction();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    onIsLoginChanged: {
        if (isLogin) {
            titleLabel.text = "Login"
            confirmButton.text = "Sign In"
        } else {
            titleLabel.text = "Registration"
            confirmButton.text = "Sign Up"
        }
    }
    attachedObjects: [
        SystemToast {
            id: myToast
        }
    ]
    function validLogin() {
        var login = loginTextField.text;
        return (/^[a-zA-Z0-9,.]+$/.test(login));
    }
    function validPassword() {
        var password = passwordTextField.text;
        return (/^[a-zA-Z0-9,.]+$/.test(password));
    }
    function validEmpty() {
        var success = false;
        if (loginTextField.text.trim() == "" || passwordTextField.text.trim() == "") {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass8Characters() {
        var success = false;
        if (passwordTextField.text.trim().length < 8) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenPass40Characters() {
        var success = false;
        if (passwordTextField.text.trim().length > 40) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin2Characters() {
        var success = false;
        if (loginTextField.text.trim().length < 2) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
    function lenLogin50Characters() {
        var success = false;
        if (loginTextField.text.trim().length > 50) {
            success = false;
        } else {
            success = true;
        }
        return success;
    }
}
