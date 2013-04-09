/*
 * UsersPage.qml
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
    id: theUserPage
    actions: [
        ActionItem {
            title: "Logout"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _app.singout();
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
        // container for show users
        Container {
            id: buttonShow
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Top
            layout: DockLayout {
            }
            ImageButton {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
                defaultImageSource: "asset:///images/showuserbtn.png"
                pressedImageSource: "asset:///images/showuserbtn.png"
                onClicked: {
                    visible = false;
                    myListModel.load("app/native/assets/users.json")
                    listView.visible = true
                }
            }
        } // end container for show users
        Container {
            layout: DockLayout {
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ListView {
                id: listView
                visible: false
                objectName: "listView"
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                dataModel: MyListModel {
                    id: myListModel
                    objectName: "myListModel"
                }
                property int activeItem: -1
                listItemComponents: [
                    // define delegates for different item types here
                    ListItemComponent {
                        // StandardListItem is a convivience component for lists with default cascades look and feel
                        StandardListItem {
                            title: ListItemData.login
                            imageSource: "asset:///images/user.png"
                        }
                    }
                ]
                onActivationChanged: {
                    console.log("onActivationChanged, active: " + active)
                    if (active) listView.activeItem = indexPath[0]
                    listView.select(active, true);
                }
            }
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
