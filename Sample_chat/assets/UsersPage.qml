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

PreDestroyablePage {
    id: thePage
    function usersReceived(usersData, firstTime) {
        if (firstTime) {
            myListModel.load(usersData);
            thePage.connect();
        }
        else {
            myListModel.update(usersData);
        }
    }
    function loadingFinished() {
        myIndicator.running = false;
        listView.visible = true;
        _chat.requestAllRooms();
        userUpdateTimer.start();
    }
    havePopAction: true
    function preDestroyAction() {
        console.log("users preDestroyAction");
        thePage.disconnect();
    }
    attachedObjects: [
        QTimer {
            id: userUpdateTimer
            interval: 30000 //30 sec
            onTimeout: {
                console.log("user update timer is on fire");
                networkManager.requestUsers();
            }
        }
    ]
    actions: [
        ActionItem {
            id: logoutActionItem
            title: "Logout"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
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
            ActivityIndicator {
                id: myIndicator
                visible: true
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: 1000
                running: true
            }
            Container {
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                TextField {
                    horizontalAlignment: HorizontalAlignment.Fill
                    inputMode: TextFieldInputMode.Text
                    onTextChanged: {
                        myListModel.setFilter(text.trim());
                    }
                }
                ListView {
                    id: listView
                    visible: false
                    objectName: "listView"
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    function scrollToTop() {
                        scrollToPosition(0, 0x2);
                    }
                    dataModel: MyListModel {
                        id: myListModel
                        onNewMessageForUser: {
                            if (listView.chatPage && listView.chatPage.userId == userId) {
                                listView.chatPage.newMessages(chatMessage);
                            } else {
                                myListModel.setUnreadedMessageCount(userId, 1);
                            }
                        }
                        onNewMessageForRoom: {
                            if (listView.roomChatPage && listView.roomChatPage.roomId == roomId) {
                                listView.roomChatPage.newMessages(chatMessage);
                            }
                        }
                        onItemAdded: {
                            timer.stop()
                            timer.start()
                        }
                    }
                    property real itemsWidth: _app.displayWidth()
                    listItemComponents: [
                        ListItemComponent {
                            type: "header"
                            Header {
                                title: ListItemData
                            }
                        },
                        ListItemComponent {
                            type: "user"
                            Container {
                                id: itemContainer
                                preferredWidth: (ListItem.initialized ? ListItem.view.itemsWidth : 0)
                                background: {
                                    getBackground()
                                }
                                layout: DockLayout {
                                }
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.LeftToRight
                                    }
                                    ImageView {
                                        imageSource: "asset:///images/user.png"
                                    }
                                    Label {
                                        text: qsTr(ListItemData.login)
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                                Container {
                                    horizontalAlignment: HorizontalAlignment.Right
                                    verticalAlignment: VerticalAlignment.Center
                                    rightPadding: 150.0
                                    ImageView {
                                        imageSource: ListItemData.online ? "asset:///images/onLine.png" : "asset:///images/offLine.png"
                                    }
                                }
                                function getBackground() {
//                                    console.log("getBackground " + ((ListItemData.unreadedCount == 0) ? "#00000000" : "#80FF5C00"));
                                    return (ListItemData.unreadedCount == 0) ? Color.create("#00000000") : Color.create("#80FF5C00")
                                }
                                function setHighlight(highlighted, blank) {
//                                    console.log("highlighted " + highlighted)
                                    if (highlighted) {
                                        background = Color.Blue;
                                    } else {
                                        if (blank) background = Color.create("00000000");
                                        else background = getBackground();
                                    }
                                }
                                onTouch: {
                                    if (event.isDown()) {
                                        setHighlight(true, false);
                                    } else if (event.isUp()) {
                                        setHighlight(false, true);
                                    } else if (event.isCancel()) {
                                        setHighlight(false, false);
                                    }
                                }
                            }
                        },
                        ListItemComponent {
                            type: "room"
                            StandardListItem {
                                title: qsTr(ListItemData.name)
                                imageSource: "asset:///images/room.png"
                            }
                        }
                    ]
                    onTriggered: {
                        var chosenItem = dataModel.data(indexPath);
                        if (dataModel.itemType(indexPath) == "user") {
                            myListModel.setUnreadedMessageCount(chosenItem.id, 0);
                            var cPage = getChatPage();
                            cPage.newMessages(chosenItem.chat);
                            cPage.userId = chosenItem.id
                            cPage.chatName = chosenItem.login
                            navigationPane.push(cPage);
                        } else if (dataModel.itemType(indexPath) == "room") {
                            var rPage = getRoomChatPage();
                            rPage.newMessages(chosenItem.chat);
                            rPage.roomId = chosenItem.id
                            rPage.roomName = chosenItem.name
                            navigationPane.push(rPage);
                            _chat.joinToRoom(chosenItem.id);
                        }
                    }
                    property Page chatPage
                    function getChatPage() {
                        if (! chatPage) {
                            chatPage = chatPageDefinition.createObject();
                        }
                        return chatPage;
                    }
                    property Page roomChatPage
                    function getRoomChatPage() {
                        if (! roomChatPage) {
                            roomChatPage = roomChatPageDefinition.createObject();
                        }
                        return roomChatPage;
                    }
                    attachedObjects: [
                        QTimer {
                            id: timer
                            singleShot: true
                            interval: 100
                            onTimeout: {
                                listView.scrollToTop();
                            }
                        }, //QTimer
                        ComponentDefinition {
                            id: chatPageDefinition
                            source: "ChatPage.qml"
                        },
                        ComponentDefinition {
                            id: roomChatPageDefinition
                            source: "RoomChatPage.qml"
                        }
                    ]
                }
            }
        }
    }
    function newMessageAction(message) {
        myListModel.newChatMessage(message)
    }
    function newRoomMessageAction(message) {
        myListModel.newRoomMessage(message);
    }
    function newRoom(roomId, roomName) {
        myListModel.newRoom(roomId, roomName)
    }
    function presenceReceived(user) {
        myListModel.presenceReceived(user)
    }
    property bool isLogged: false
    function loggedIn() {
        if (!isLogged) {
            isLogged = true
        	myListModel.loggedIn(_chat.currentUserId());
        }
    }
    function connect() {
        _chat.messageSended.connect(thePage.newMessageAction);
        _chat.messageReceived.connect(thePage.newMessageAction);
        _chat.roomAdded.connect(thePage.newRoom);
        _chat.roomMessageReceived.connect(thePage.newRoomMessageAction);
        _chat.presenceReceived.connect(thePage.presenceReceived);
        _chat.loggedIn.connect(thePage.loggedIn);
    }
    function disconnect() {
        _chat.messageSended.disconnect(thePage.newMessageAction);
        _chat.messageReceived.disconnect(thePage.newMessageAction);
        _chat.roomAdded.disconnect(thePage.newRoom);
        _chat.roomMessageReceived.disconnect(thePage.newRoomMessageAction);
        _chat.presenceReceived.disconnect(thePage.presenceReceived);
        _chat.loggedIn.disconnect(thePage.loggedIn);
    }
}
