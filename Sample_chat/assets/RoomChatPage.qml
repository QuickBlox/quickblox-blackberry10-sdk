import bb.cascades 1.0
import bb.system 1.0

PreDestroyablePage {
    property alias roomId: chatContainer.userId
    property alias roomName: chatTitle.title
    havePopAction: true
    havePushAction: true
    titleBar: TitleBar {
        id: chatTitle
    } //TitleBar
    ChatContainer {
        id: chatContainer
        onMessageSendAction: {
            var result = _chat.sendRoomMessage(chatContainer.userId, text);
            console.log("message to room sended " + result);
            if (!result) {
                networkErrorToast.show();
            }
        }
    }
    function newMessages(messages) {
        chatContainer.chatDataModel.append(messages)
    }
    function preDestroyAction() {
        console.log("room chat preDestroyAction");
        // commented for disable fast entering room bug
//        _chat.leaveRoom(roomId);
    }
    function postPushAction() {
        chatContainer.restartTimer();
    }
    attachedObjects: [
        SystemToast {
            id: networkErrorToast
            body: "Please, check your internet connection"
        }
    ]
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: {
                navigationPane.backButtonsVisible = false;
                navigationPane.pop();
            }
        }
    }
}
