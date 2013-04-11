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
                onTriggered: {
                    var selectedItem = dataModel.data(indexPath);
                    var pageDetail = getSecondPage();
                    var login = dataModel.data(indexPath).login;
                    var email = dataModel.data(indexPath).email;
                    var phone = dataModel.data(indexPath).phone;
                    var website = dataModel.data(indexPath).website;
                    var facebook_id = dataModel.data(indexPath).facebook_id;
                    var twitter_id = dataModel.data(indexPath).twitter_id;
                    var created_at = dataModel.data(indexPath).created_at;

                    if (email == null) {
                        email = "---"
                    }
                    if (phone == null) {
                        phone = "---"
                    }
                    if (website == null) {
                        website = "---"
                    }
                    if (facebook_id == null) {
                        facebook_id = "---"
                    }
                    if (twitter_id == null) {
                        twitter_id = "---"
                    }
                    if (created_at == null) {
                        created_at = "---"
                    }
                    pageDetail.login = login
                    pageDetail.email = email
                    pageDetail.phone = phone
                    pageDetail.website = website
                    pageDetail.facebook = facebook_id
                    pageDetail.twitter = twitter_id
                    pageDetail.createdDate = created_at
                    navigationPane.push(pageDetail);
                }
            }
        }
        onCreationCompleted: {
            listView.visible = true;
            myListModel.load("app/native/assets/users.json");

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
            },
            ComponentDefinition {
                objectName: "detailUsersPage"
                id: detailUsersPage
                source: "DetailUsersPage.qml"
            }
        ]
    }
    property Page secondPage
    function getSecondPage() {
        if (! secondPage) {
            secondPage = detailUsersPage.createObject();
        }
        return secondPage;
    }
}
