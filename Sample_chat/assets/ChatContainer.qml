import bb.cascades 1.0
import com.rim.example.custom 1.0
import my.library 1.0

Container {
    id: self
    property alias chatDataModel: chatDataModel
    property variant userId
    signal messageSendAction(string text);
    function restartTimer() {
        timer.stop()
        timer.start()
    }
    ListView {
        property real itemsWidth: _app.displayWidth()
        property variant currentUserId: _chat.currentUserId()
        id: chatListView
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        focusPolicy: FocusPolicy.Touch
        function scrollToBottom() {
            scrollToPosition(1, 0x2);
        }
        dataModel: ArrayDataModel {
            id: chatDataModel
            onItemAdded: {
                self.restartTimer();
            }
        }
        listItemComponents: [
            ListItemComponent {
                type: "chatItem"
                ChatItem {
                }
            }
        ]
        // Item type-mapping
        function itemType(data, indexPath) {
            return 'chatItem'
        }
        attachedObjects: [
            LayoutUpdateHandler {
                id: layoutHandler
                onLayoutFrameChanged: {
                    console.log("Layout Frame: [" + layoutFrame.x + ", " + layoutFrame.y + ", " + layoutFrame.width + ", " + layoutFrame.height + "]");
                    chatListView.scrollToBottom();
                }
            },
            QTimer {
                id: timer
                singleShot: true
                interval: 100
                onTimeout: {
                    console.log("scroll to bottom timer is on fire")
                    chatListView.scrollToBottom();
                }
            } //QTimer
        ]
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Bottom
        layout: StackLayout {
            orientation: LayoutOrientation.RightToLeft
        }
        ImageButton {
            onClicked: {
                if (inputTextField.text.trim().length > 0) {
                    self.messageSendAction(inputTextField.text.trim())
                }
                inputTextField.text = ""
                inputTextField.requestFocus();
            }
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            defaultImageSource: "asset:///images/sendButton.png"
        }
        TextField {
            bottomMargin: 10.0
            id: inputTextField
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            focusRetentionPolicyFlags: FocusRetentionPolicy.LoseOnScroll | FocusRetentionPolicy.LoseToFocusable
        }
    }
}
