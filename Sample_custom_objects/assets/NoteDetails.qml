import bb.cascades 1.0
import bb.system 1.0
import my.library 1.0

PreDestroyablePage {
    id: self
    property variant noteData: null
    property variant noteId: null
    titleBar: TitleBar {
        title: "Note"
    }
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
            topPadding: 35.0
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    minWidth: 150.0
                    Label {
                        text: "Note:"
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.textAlign: TextAlign.Right
                    }
                }
                Container {
                    leftMargin: 20.0
                    Label {
                        id: noteLabel
                        multiline: true
                    }
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    minWidth: 150.0
                    Label {
                        text: "Status:"
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.textAlign: TextAlign.Right
                    }
                }
                Container {
                    leftMargin: 20.0
                    DropDown {
                        id: statusDropDown
                        enabled: true
                        onSelectedIndexChanged: {
                            console.log("SelectedValue was changed to " + statusDropDown.selectedValue);
                            if (noteData.fields["status"] != statusDropDown.selectedValue) {
                                noteData.addFieldValue("status", statusDropDown.selectedValue);
                                networkManager.updateCO(noteData);
                            }
                        }
                        preferredWidth: 300.0
                        Option {
                            text: "New"
                            value: "New"
                        }
                        Option {
                            text: "In Progress"
                            value: "In Progress"
                        }
                        Option {
                            text: "Done"
                            value: "Done"
                        }
                    }
                }
            }
            Label {
                text: "Comments:"
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Fill
            }
            ListView {
                id: commentListView
                function scrollToBottom() {
                    scrollToPosition(1, 0x2);
                }
                dataModel: ArrayDataModel {
                    id: commentsDataModel
                    onItemAdded: {
                        timer.stop()
                        timer.start()
                    }
                }
                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        Label {
                            text: {
                                (ListItem.initialized ? qsTr("#" + (ListItem.indexPath * 1 + 1) + " " + ListItemData) : "")
                            }
                            multiline: true
                        }
                    }
                ]
                // Item type-mapping
                function itemType(data, indexPath) {
                    return 'item'
                }
                attachedObjects: [
                    QTimer {
                        id: timer
                        singleShot: true
                        interval: 100
                        onTimeout: {
                            commentListView.scrollToBottom();
                        }
                    }
                ]
            }
        }
    }
    onNoteDataChanged: {
        console.log("onNoteDataChanged " + noteData)
        if (noteData) {
            self.noteId = qsTr(noteData.id)
            noteLabel.text = noteData.fields["note"]
            for (var i = 0; i < statusDropDown.count(); i ++) {
                if (statusDropDown.at(i).text == noteData.fields["status"]) {
                    statusDropDown.setSelectedOption(statusDropDown.at(i));
                }
            }
            //for smooth comments adding
            for (var idx in noteData.fields["comments"]) {
                //add new
                if (commentsDataModel.size() <= idx) {
                    commentsDataModel.append(noteData.fields["comments"][idx]);
                } else {
                    //replace
                    if (commentsDataModel.data(idx) != noteData.fields["comments"][idx]) {
                        commentsDataModel.replace(idx, noteData.fields["comments"][idx])
                    }
                }
            }
        }
    }
    actions: [
        ActionItem {
            id: addActionItem
            title: "Delete Note"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                deleteDialog.show()
            }
        },
        ActionItem {
            id: addCommentItem
            title: "Add comment"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                newCommentPrompt.show();
            }
        }
    ]
    attachedObjects: [
        SystemDialog {
            id: deleteDialog
            body: "Are you sure you want to delete this note?"
            onFinished: {
                console.log("system dialog button clicked " + buttonSelection().label);
                if (buttonSelection() == confirmButton) {
                    console.log("confirm Button clicked")
                    networkManager.deleteCO(noteData);
                    navigationPane.pop();
                } else if (buttonSelection() == cancelButton) {
                    console.log("cancel Button clicked")
                }
            }
        },
        SystemPrompt {
            id: newCommentPrompt
            title: "New Comment"
            inputField.maximumLength: 50
            onFinished: {
                if (result == SystemUiResult.ConfirmButtonSelection && inputFieldTextEntry().trim() != "") {
                    console.log(inputFieldTextEntry());
                    //                    commentsDataModel.append(inputFieldTextEntry());
                    noteData.addValueToArrayField("comments", inputFieldTextEntry());
                    networkManager.updateCO(noteData);
                }
            }
        }
    ]
}
