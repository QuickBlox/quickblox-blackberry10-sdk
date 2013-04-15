import bb.cascades 1.0
import bb.system 1.0
import com.rim.example.custom 1.0

PreDestroyablePage {
    Container {
        layout: DockLayout {
        }
        preferredWidth: _app.displayWidth()
        preferredHeight: _app.displayHeight()
        ImageView {
            imageSource: "asset:///images/imagesbackground.jpeg"
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
        }
        Container {
            preferredWidth: _app.displayWidth() * 0.8
            horizontalAlignment: HorizontalAlignment.Center
            topPadding: 50.0
            Label {
                text: "Note:"
            }
            TextArea {
                id: noteTextArea
                preferredHeight: 200.0
                horizontalAlignment: HorizontalAlignment.Fill
                maximumLength: 50
            }
            Label {
                text: "Comment:"
            }
            TextArea {
                id: commentTextArea
                preferredHeight: 200.0
                horizontalAlignment: HorizontalAlignment.Fill
                maximumLength: 50
            }
            Button {
                text: "Add Note"
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    if (noteTextArea.text.trim() == "" || commentTextArea.text.trim() == "") {
                        myToast.body = "Please, fill both Note & Comment fields"
                        myToast.show()
                    } else {
                        networkManager.addCO(_app.getNote(noteTextArea.text, commentTextArea.text));
                        navigationPane.pop();
                    }
                }
            }
        }
    }
    attachedObjects: [
        SystemToast {
            id: myToast
            body: "Error"
        }
    ]
}
