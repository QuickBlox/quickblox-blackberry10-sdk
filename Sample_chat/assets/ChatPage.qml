import bb.cascades 1.0
import bb.system 1.0

PreDestroyablePage {
    property alias userId: chatContainer.userId
    property alias chatName: chatTitle.title
    havePushAction: true
    titleBar: TitleBar {
        id: chatTitle
    } //TitleBar
    ChatContainer {
        id: chatContainer
        onMessageSendAction: {
            var result = _chat.sendMessage(chatContainer.userId , text);
            console.log("message to user sended " + result);
            if (!result) {
                networkErrorToast.show();
            }
        }
    }
    function newMessages(messages)
    {
        chatContainer.chatDataModel.append(messages)
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
