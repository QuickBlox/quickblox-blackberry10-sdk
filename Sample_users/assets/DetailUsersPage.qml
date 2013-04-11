/*
 * DetailUsersPage.qml
 * 
 * Created on: 21.02.2013
 * Author: Sergey Fedunets
 * Company: Injoit Ltd
 */

import bb.cascades 1.0
import com.rim.example.custom 1.0
import my.library 1.0
import bb.system 1.0

Page {
    id: detailUsersPage
    property alias login: labelLogin.text
    property alias email: labelEmail.text
    property alias phone: labelPhone.text
    property alias website: labelWebsite.text
    property alias facebook: labelFacebook.text
    property alias twitter: labelTwitter.text
    property alias createdDate: labelCreatedDate.text
    actions: [
        ActionItem {
            title: "Logout"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _app.singout();
                navigationPane.pop();
                navigationPane.pop();
            }
        }
    ]
    Container {
        layout: DockLayout {
        }
        ImageView {
            imageSource: "asset:///images/bg.jpg"
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
        }
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Top
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            topPadding: 50
            Container {
                preferredWidth: 720
                layout: DockLayout {
                }
                ImageView {
                    imageSource: "asset:///images/user_big.png"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            // container for Login
            Container {
                id: containerLogin
                preferredWidth: 720
                bottomMargin: 20
                topPadding: 50
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
                    Label {
                        id: labelLogin
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: ""
                        multiline: true
                    }
                }
            } // end container for Login
            // container for email
            Container {
                id: containerEmail
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Email"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelEmail
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sempla@sample.com"
                        multiline: true
                    }
                }
            } // end container for email
            // container for Phone
            Container {
                id: containerPhone
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Phone"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelPhone
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sample"
                        multiline: true
                    }
                }
            } // end container for Phone
            // container for Website
            Container {
                id: containerWebsite
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Website"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelWebsite
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sample"
                        multiline: true
                    }
                }
            } // end container for Website
            // container for Facebook
            Container {
                id: containerFacebook
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Facebook"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelFacebook
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sample"
                        multiline: true
                    }
                }
            } // end container for Facebook
            // container for Twitter
            Container {
                id: containerTwitter
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Twitter"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelTwitter
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sample"
                        multiline: true
                    }
                }
            } // end container for Twitter
            // container for CreatedDate
            Container {
                id: containerCreatedDate
                preferredWidth: 720
                bottomMargin: 20
                layout: DockLayout {
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    text: "Created"
                    textStyle.color: Color.Black
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    preferredWidth: 525
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    layout: DockLayout {
                    }
                    Label {
                        id: labelCreatedDate
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        preferredWidth: 500
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontWeight: FontWeight.Bold
                        text: "sample"
                        multiline: true
                    }
                }
            } // end container for CreatedDate
        }
        onCreationCompleted: {
            // this slot is called when declarative scene is created
            // write post creation initialization here
            console.log("Page - onCreationCompleted()")

            // enable layout to adapt to the device rotation
            // don't forget to enable screen rotation in bar-bescriptor.xml (Application->Orientation->Auto-orient)
            OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
        }
        attachedObjects: [
            SystemToast {
                id: myToast
            }
        ]
    }
}
